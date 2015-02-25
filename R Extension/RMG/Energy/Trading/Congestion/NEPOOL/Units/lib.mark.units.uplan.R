# Calculate PnL from the units once nodes get rebooked
#
# tie_uplan_views                       - for My8,9
# compare_uplan_secdb
# get_positions_units
# get_PnL_impact                        - 
# read_uplan_views                      - from Rujun's xlsx
# update_archive_with_uplan_views       - save to the RData archive
#


################################################################
# Take uplan view from day 1, uplan view from day 2, and the current
# secdb mark, and construct the new secdb mark.
# The secdb mark will capture all the nema moves between uplan updates.
# For My8,9 ONLY!
# @param views the current uplan view
# @param prevUpdate, an R Date of the previous uplan view
# @return res the adjusted view
#
adjust_uplan_view <- function(views, prevUpdate)
{
  setLogLevel(3)
  file1 <-  paste("S:/All/Structured Risk/NEPOOL/Marks/GeneratorMarks/",
    "NEPOOL Nodal Basis ", format(prevUpdate), ".xlsx", sep="")
  viewUp1 <- read_uplan_view(file1)

  # load the nodal archive
  DIR <- "S:/All/Structured Risk/NEPOOL/Marks/Marks Automation/"
  filePMview <- paste(DIR, "PM_node_view.RData", sep="")
  load(filePMview)
   
  res <- views
  locations <- c("MYSTIC.MYS8 DA", "MYSTIC.MYS9 DA")
  for (location in locations) {
    # get the cong difference between prevView and current SecDb 
    dCC <- diff_view_congestion( viewUp1[[location]], pmView[[location]] )
    # add the cong difference to new uplan view     
    res[[location]] <- add_to_congestion_view( views[[location]], dCC )
  }

  res
}


################################################################
# Where nv is a NodalView
#
compare_uplan_vs_secdb <- function(nv, NEPOOL)
{
  marksForDate <- Sys.Date()
  asOfDate  <- as.Date(secdb.dateRuleApply(marksForDate, "-0b"))
  startDate <- as.Date(secdb.dateRuleApply(marksForDate, "+1e+1d"))
  endDate   <- as.Date(paste(as.numeric(format(Sys.Date(), "%Y"))+6, "-12-01", sep=""))
  toMonths  <- seq(startDate, endDate, by="1 month")

  ptid <- nv$ptid
  #if (nv$ptid == 40327)  ptid <- 1691

  #source("h:/user/R/RMG/Utilities/Interfaces/PM/R/libMarkNode.R")
  histNodeMark <- PM:::.getHistMark(ptid, component = c("loss%",        # add PM:::
    "congestion", "hubMark", "nodeMark", "loss$"))
  histNodeMark$source <- "Historical"
  
  secDbNodeMark <- PM:::.getSecDbMark(nv$delivPt, asOfDate, startDate, endDate)

  uplanMark <- constructNodeMark(nv$view, asOfDate, startDate, endDate)
  uplanMark$source <- "UPLAN"

  allMarks <- rbind(histNodeMark, secDbNodeMark, uplanMark)
  allMarks$year <- as.numeric(format(allMarks$month, "%Y"))

  allMarks <- subset(allMarks, month >= as.Date("2004-01-01"))
  plotMarks(allMarks, nv$delivPt, main=paste("Hub to", nv$delivPt),
    hist=36, fwd=40, save=FALSE)

  # stack them
  rLog("Historical + Fwd congestion")
  CC <- cast(allMarks, year ~ bucket, function(x){round(mean(x),2)}, fill=NA,
    subset=(source %in% c("Historical", "UPLAN") & component=="congestion"))
  print(CC, row.names=FALSE)

  rLog("Historical + Fwd lossfactor")
  print(cast(allMarks, year ~ bucket, function(x){round(mean(x),3)}, fill=NA,
    subset=(source %in% c("Historical", "UPLAN") & component=="loss%")),
    row.names=FALSE)

  ## print(cast(allMarks, year ~ bucket, function(x){round(mean(x),3)}, fill=NA,
  ##   subset=(source %in% c("Historical", "pmView") & component=="loss$")),
  ##   row.names=FALSE)

  ## print(cast(allMarks, month ~ bucket, function(x){round(mean(x),3)}, fill=NA,
  ##   subset=(source %in% c("Historical", "pmView") & component=="loss$")),
  ##   row.names=FALSE)

  rLog("Total spread")
  aux <- cast(allMarks, month + bucket ~ component, I, fill=NA,
    subset=(component %in% c("nodeMark", "hubMark") & source %in% c("Historical", "UPLAN")))
  aux$spread <- aux$nodeMark - aux$hubMark
  aux$year   <- format(aux$month, "%Y")
  #cast(aux, month ~ bucket, I, fill=NA, value="spread")  # spread by month
  print(cast(aux, year ~ bucket, function(x){round(mean(x, na.rm=TRUE), 2)},
       fill=NA, value="spread"), row.names=FALSE)   # spread by year

  
  rLog("get FTR prices")
  paths <- data.frame(path.no=1:2, source.ptid=4000, sink.ptid=ptid, 
    class.type=c("ONPEAK", "OFFPEAK"))
  CPSP <- FTR.get.CP.for.paths(paths, NEPOOL, add.SP=TRUE,
    panelName=TRUE)
  #tail(FTR.plot(ptid, 4001, bucket="onpeak", hist=365), 12)
  #CPSP <- subset(CPSP, path.no %in% 1:10)
  CPSP[, c(3,4)] <- sapply(CPSP[, c(3,4)], round, 2)
  print(CPSP[grepl("-", CPSP$auction),-1], row.names=FALSE)

  #print(CPSP[grepl("12", CPSP$auction),-1], row.names=FALSE)

  ## hSP <- FTR.get.hSP.for.paths(paths, NEPOOL)
  ## options <- FTR.makeDefaultOptions("F13-1Y", NEPOOL)
  ## sSP <- FTR.simulate.SP(hSP, options)
  ## qPP <- apply(sSP, 1, quantile, probs=0.01*c(1,5,seq(10, 90, 10), 95,99))
  ## colnames(qPP) <- c("ON", "OFF")
  ## print(t(round(qPP, 2)))
  
}


################################################################
# get and construct the marks
# return the SecDb and UPLAN marks
#
get_fwd_marks <- function( asOfDate, views)
{
  # the SecDb marks
  PP1 <- ldply(views, function(nv){
    startDt <- min(nv$view$month)
    endDt   <- max(nv$view$month)    
    PM:::.getSecDbMark(nv$delivPt, asOfDate, startDt, endDt)
  })
  PP1 <- subset(PP1, component=="nodeMark")
  colnames(PP1)[1] <- "location"
  PP1 <- PP1[,c("month", "bucket", "location", "value")]
  
  # the UPLAN marks
  PP2 <- ldply(views, function(nv){
    startDt <- min(nv$view$month)
    endDt   <- max(nv$view$month)        
    constructNodeMark(nv$view, asOfDate, startDt, endDt)
  })
  PP2 <- subset(PP2, component=="nodeMark")
  colnames(PP2)[1] <- "location"
  PP2 <- PP2[,c("month", "bucket", "location", "value")]

  return(list(PP1, PP2))
}


################################################################
# calculate the PnL impact on the units
#
get_positions_units <- function( asOfDate )
{
  books <- c("MYSTC89M", "FORERIVM", "MYSTIC7M")
  
  aux <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
   fix=TRUE, service="ENERGY")
  aux <- subset(aux, delivery.point %in% c("MYSTIC.MYS8 DA", "MYSTIC.MYS9 DA",
    "EDGAR.40327 DA", "MYSTIC.MYS7 DA") & mkt == "PWX" )
  
  qq <- cast(aux, contract.dt + bucket + delivery.point ~ ., sum, fill=NA,
             value="delta")
  names(qq) <- c("month", "bucket", "location", "value")
  qq$location <- gsub("EDGAR.EDGR DA", "EDGAR.40327 DA", qq$location)
  qq <- subset(qq, month >= nextMonth(asOfDate) )
  
  # cast(qq, month ~ location + bucket, I, fill=0)
  
  data.frame(qq)
}


################################################################
# calculate the PnL impact on the units
#
get_PnL_impact <- function(views)
{
  asOfDate <- secdb.dateRuleApply(Sys.Date(), "-1b")
  qq1 <- get_positions_units(asOfDate)
  uLocations <- unique(qq1$location)

  aux <- get_fwd_marks(Sys.Date(), views[uLocations])
  PP1 <- aux[[1]]
  PP2 <- aux[[2]]
  
  # look at the change in prices
  dP <- merge(PP1, PP2, suffixes=c(".1", ".2"),
              by=c("location", "month", "bucket"))
  dP$year <- format(dP$month, "%Y")
  dP$value <- dP$value.2 - dP$value.1
  print(cast(dP, year + bucket ~ location, function(x){round(mean(x),2)}),
        row.names=FALSE)
  
  
  # calc the PnL
  aux <- calc_pnl_pq(PP1, PP2, qq1, qq1,
    groups=c("year", "location"))  # from lib.pnl.R
  gPnL <- aux[[2]]
  print(cast(gPnL, year ~ location, I, fill=0, value="PnL"),
        row.names=FALSE)

  
}





################################################################
# construct the uplan view file, similar to the pmView file
# return a list of view, each view is a data.frame
#
read_uplan_view <- function( filename )
{
  require(xlsx)

  cong <- .read_ExternalData_xlsx_file(filename, sheetName="Congestion")
  lossFactor <- .read_ExternalData_xlsx_file(filename, sheetName="Loss")
  
  #load NEPOOL MAP
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")

  res <- vector("list", length=length(cong))
  names(res) <- names(cong)
  for (i in seq_along(res)) {
    delivPt <- names(res)[i]
    rLog("Working on ", delivPt)
#    browser()
    ptid <- MAP$ptid[grep(gsub(" DA$", "", delivPt), MAP$location)]
    if (is.na(ptid) || length(ptid)!= 1)
      rError("Cannot find the ptid!")
    
    method   <- "UPLAN"
    nodeType <- "GEN"
    lastUpdate <- as.Date(gsub(".* ([[:digit:]]{4}-[[:digit:]]{2}-[[:digit:]]{2}).*",
      "\\1", basename(filename)))
    # if (is.na(lastUpdate) || abs(lastUpdate-Sys.Date()) > 30)
    #  stop("Is lastUpdate", format(lastUpdate))

    x1 <- data.frame(melt(cong[[delivPt]], id=1), component="congestion")
    names(x1)[2] <- "bucket"
    x2 <- data.frame(melt(lossFactor[[delivPt]], id=1), component="loss%")
    names(x2)[2] <- "bucket"
    
    view <- rbind(x1, x2)
    
    res[[i]] <- NodalView(ptid, delivPt, view, method=method,
      lastUpdate=lastUpdate, nodeType=nodeType, comments="Uplan update")
  }

    
  res
}



################################################################
# Run this by hand !!!
# 
update_archive_with_uplan_views <- function()
{
  setLogLevel(3)
  asOfDate <- Sys.Date()
  currentUpdate <- as.Date("2013-08-28")
  filename <- paste("S:/All/Structured Risk/NEPOOL/Marks/GeneratorMarks/",
    "NEPOOL Nodal Basis ", format(currentUpdate), ".xlsx", sep="")

  FTR.load.ISO.env("NEPOOL")
  NEPOOL$CP <- NEPOOL$.loadClearingPrice()

  views <- read_uplan_view(filename)  # the current uplan views
  
  nv <- views[["EDGAR.40327 DA"]]
  res <- compare_uplan_vs_secdb(nv, NEPOOL)

  nv <- views[["MYSTIC.MYS8 DA"]]
  res <- compare_uplan_vs_secdb(nv, NEPOOL)

  nv <- views[["MYSTIC.MYS9 DA"]]
  res <- compare_uplan_vs_secdb(nv, NEPOOL)

  # calculate PnL impact
  get_PnL_impact(views)

  # Make a test submission file to see that everything is OK
  rLog("Loading MAP_DP ... ")
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/MAP_DP.RData")

  X <- lapply(views, function(nv){
    startDt <- min(nv$view$month)
    endDt   <- max(nv$view$month)        
    aux <- constructNodeMark(nv$view, asOfDate, startDt, endDt)
    
    aux <- cbind(delivPt=nv$delivPt, subset(aux, component=="nodeMark"))
    aux$component=NULL
    res <- spreadToParent(aux, asOfDate=asOfDate,
                          maxMonth=as.Date("2018-12-01"),
                          MAP_DP=MAP_DP)
    as.data.frame(cast(res, month ~ bucket, I, fill=NA))
  })
  lapply(X, head)
  #.write_ExternalData_file(X, filePostfix="_AAD")

  
  return(views)
}




################################################################
################################################################
#
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)
  require(PM)
  require(FTR)
  
  #require(lattice)
  require(xlsx)

  source("H:/user/R/RMG/Utilities/Database/VCentral/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Marks/lib.mark.nodes.SecDb.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Units/lib.mark.units.uplan.R")

  views <- update_archive_with_uplan_views()  # <-- need to point to the right file!

  # when you're READY to update the archive do
  update_nodalview_archive(views, do.save=TRUE)


  

  
  
}






  

  # remove EDGAR.EDGR DA, will be marking EDGAR.40327 DA
  # PM:::.rm_from_pmView_file("EDGAR.EDGR DA")
  # PM:::.rm_from_pmView_file("W_MEDWAY.WMJ3 DA")
  









  ## # how much we inflate value if we were to mark to zone
  ## asOfDate <- as.Date("2012-12-14")
  ## qq <- get_positions_units( asOfDate )
  ## locations <- c("MYSTIC.MYS8 DA", "MYSTIC.MYS9 DA")
  ## qq <- subset(qq, location %in% locations)
  
  ## PP1 <- PM:::secdb.getElecPrices(asOfDate, nextMonth(),
  ##   as.Date("2017-12-01"), 
  ##   "COMMOD PWR NEPOOL PHYSICAL", locations, bucket=c("5X16", "7X8", "2X16H"))
  ## PP1 <- PP1[,c("month", "bucket", "location", "value")]
  ## colnames(PP1)[4] <- "P1"
  ## pq1 <- merge(qq, PP1)
  ## pq1$value <- pq1$value * pq1$P1
  ## pq1 <- cast(pq1, month + bucket ~ ., sum, fill=0)
  ## colnames(pq1)[3] <- "value1"
  
  
  ## PP2 <- PM:::secdb.getElecPrices(asOfDate, nextMonth(),
  ##   as.Date("2017-12-01"), "COMMOD PWR NEPOOL PHYSICAL",
  ##   "NEMA DA", bucket=c("5X16", "7X8", "2X16H"))
  ## PP2$value <- PP2$value - 0.10
  ## colnames(PP2)[ncol(PP2)] <- "P2"
  ## qq2 <- cast(qq, month + bucket ~ ., sum)
  ## colnames(qq2)[3] <- "mwh"
  ## pq2 <- merge(qq2, PP2[, c("month", "bucket", "P2")])
  ## pq2$value2 <- pq2$mwh * pq2$P2
  

  ## pq <- merge(pq1, pq2[,c("month", "bucket", "mwh", "value2")])
  ## pq$PnL <- pq$value2 - pq$value1
  ## pq$year <- format(pq$month, "%Y")
  ## cast(pq, year ~ bucket, sum, fill=0, value="PnL")

  ## cast(pq, year ~ ., sum, fill=0, value="PnL")
  
  
  







  ## # add $0.10 to Mystics, all buckets
  ## PP3 <- PP1
  ## ind <- which(PP3$location %in% c("MYSTIC.MYS7 DA",
  ##   "MYSTIC.MYS8 DA", "MYSTIC.MYS9 DA"))
  ## PP3$value[ind] <- PP3$value[ind] + 0.10
  


  ## # read the SecDb marks relative to zone
  ## filename <- paste("S:/All/Structured Risk/NEPOOL/Marks/Sent Files/",
  ##   "NEPOOL Marks Nodal Spreads 09-19-12.csv", sep="")
  ## SS <- .read_ExternalData_csv_file(filename)
  ## SS <- SS[uLocations]   # pick the units
  ## aux <- mapply(function(x, y){ cbind(location=y, x)},
  ##               SS, names(SS), SIMPLIFY=FALSE)
  ## aux <- do.call(rbind, aux)
  ## PP1 <- melt(aux, id=1:2)
  ## names(PP1)[3] <- "bucket"
 
  
  ## # read the UPLAN marks relative to zone
  ## filename <- paste("S:/All/Structured Risk/NEPOOL/Marks/GeneratorMarks/",
  ##   "NEPOOL Nodal Basis 2012-09-04.xlsx", sep="")
  ## SS <- .read_ExternalData_xlsx_file(filename)
  ## names(SS)[which(names(SS)=="EDGAR.40327 DA")] <- "EDGAR.EDGR DA"
  ## SS <- SS[uLocations]   # pick the units
  ## aux <- mapply(function(x, y){ cbind(location=y, x)},
  ##               SS, names(SS), SIMPLIFY=FALSE)
  ## aux <- do.call(rbind, aux)
  ## PP2 <- melt(aux, id=1:2)
  ## names(PP2)[3] <- "bucket"
