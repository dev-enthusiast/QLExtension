# Investigate the bidding behavior in the auctions,
# archive prices, etc. 
#
# CapacityAuction        - define the capacity auction object
# CapabilityPeriod       - from a month, get the Capability period
# .analyze_bidsoffers
# .analyze_summary       - from the summary xls files
# .get_fca_obligations   - all the details
#
# .make_position_spreadsheet
#
# .read_archive_FCMMRAHB
# .update_fcm_price_archive
# .update_price_annual
# .update_price_monthly
# 
#
#


################################################################
# Calculate the capability period from a given month
# Returns a string "2012-2013"
#
CapabilityPeriod <- function(month=as.Date("2013-02-01"))
{
  mon  <- as.numeric(format(month, "%m"))
  year <- as.numeric(format(month, "%Y"))
  ifelse(mon >= 6, 
    paste(year, "-", year+1, sep=""), 
    paste(year-1, "-", year, sep=""))
}


################################################################
# Construct a capacity auction
#   capabilityPeriod = "2014-2015"
#   auction - one of "FCA", "MRA", "ARA"
#
CapacityAuction <- function(auction="FCA", capabilityPeriod=NULL,
  month=NULL)
{
  if (!(auction %in% c("FCA", "MRA", "ARA")))
    stop("Wrong name for the capacity auction type!")

  if (auction %in% c("FCA", "ARA") ) {
    year  <- as.numeric( substr(capabilityPeriod, 1, 4) )
    month <- as.Date(paste(year, "-06-01", sep=""))
  }

  if ( is.null(capabilityPeriod) ) {
    capabilityPeriod <- CapabilityPeriod(month)
  }
  
  structure(list(capabilityPeriod=capabilityPeriod, auction=auction,
    month=month), class="CapacityAuction")
}

################################################################
#
#
.analyze_summary <- function()
{
  require(lattice)
  
  FILE_ARCHIVE <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/",
    "fcm_summary.RData", sep="")  
  load(FILE_ARCHIVE)                   # loads FCM data.frame

  # MRA: total cleared demand bids and supply offers
  aux <- FCM[grep("Total", FCM$column),]
  aux <- subset(aux, section=="Capacity Zone" & auction=="MRA")
  aux <- aux[order(aux$column, aux$month),]
  aux$value <- abs(as.numeric( aux$value))   # make them all positive
  aux <- cast(aux, column + month ~ ., sum, fill=0, value="value") # sum over capacity zones
  aux <- subset(aux, column %in% c("Total Demand Bids Cleared (MW)",
      "Total Supply Offers Cleared (MW)"))
  xyplot( `(all)` ~ month, groups=aux$column, data=aux,
    type=c("g", "o"), ylab="Total (MW)",
    auto.key=list(space="top", points=FALSE, lines=TRUE,
      columns=length(unique(aux$column))),     
    main="Monthly Reconfig Auctions")
  
  # MRA: clearing price
  aux <- subset(FCM, column=="Clearing Price ($/kW-month)" &
                auction=="MRA" & section=="Capacity Zone")
  aux <- aux[order(aux$month),]
  aux$value <- as.numeric( aux$value)
  xyplot( value ~ month, data=aux,
    type=c("g", "o"), , ylab="Clearing Price ($/kW-month)",
    main="Monthly Reconfig Auctions" )


  # Monthly bilateral total acquiring approved
  aux <- subset(FCM, section=="Capacity Zone" & auction=="MonthlyBilateral" &
                column == "Total Acquiring Approved (MW)")
  aux <- aux[order(aux$column, aux$month),]
  aux$value <- abs(as.numeric( aux$value))   # make them all positive
  aux <- cast(aux, column + month ~ ., sum, fill=0, value="value") # sum over capacity zones
  xyplot( `(all)` ~ month, groups=aux$column, data=aux,
    type=c("g", "o"), ylab="Total Acquiring Approved (MW)",
    auto.key=list(space="top", points=FALSE, lines=TRUE,
      columns=length(unique(aux$column))),     
    main="Monthly Bilaterals")


  # ARAs
  aux <- FCM[grep("^ARA", FCM$auction), ]
  aux <- subset(aux, section=="Capacity Zone")
  aux <- subset(aux, column %in% c("Total Demand Bids Cleared (MW)",
      "Total Supply Offers Cleared (MW)"))
  aux$value <- abs(as.numeric( aux$value))   # make them all positive
  aux <- cast(aux, auction + column + capabilityPeriod + isBilateral + bilateralPeriod ~ .,
              sum,
              fill=0, value="value") # sum over capacity zones
  aux <- aux[order(aux$capabilityPeriod, aux$auction, aux$column),]
  aux$auction <- as.factor(aux$auction)
  xyplot( `(all)` ~ auction | as.factor(capabilityPeriod), groups=aux$column, data=aux,
    type=c("g", "o"), ylab="Total (MW)",
    auto.key=list(space="top", points=FALSE, lines=TRUE,
      columns=length(unique(aux$column))),     
    main="Annual Reconfig Auctions")
  
  # ARAs bilaterals 
  aux <- FCM[grep("^ARA", FCM$auction), ]
  aux <- subset(aux, section=="Capacity Zone")
  aux <- subset(aux, column %in% c("Total Acquiring Approved (MW)") )
  aux <- aux[order(aux$capabilityPeriod, aux$auction, aux$column),]
  aux$value <- as.numeric( aux$value)   # make them all positive
  
  aux <- cast(aux, capabilityPeriod + auction+ bilateralPeriod ~ .,
              sum) # sum over capacity zones
  colnames(aux)[4] <- "Total Acquiring Approved (MW)"
  aux[,4] <- round(aux[,4])
  print(aux, row.names=FALSE)
  
  
}


################################################################
# for one month only
#
.analyze_bidsoffers <- function(AA)
{
  # construct the cummulative bids/offers for one month
  .oneMonth <- function(aa) {
    cumDB <- subset(aa, Bid.Offer.Type=="Demand_Bid")
    cumDB <- cumDB[order(-cumDB$Price),]
    cumDB$cumMW <- cumsum(cumDB$MW)

    cumSO <- subset(aa, Bid.Offer.Type=="Supply_Offer")
    cumSO <- cumSO[order(cumSO$Price),]
    cumSO$cumMW <- cumsum(cumSO$MW)

    aux <- rbind(
      cumDB[,c("Commitment.Period", "Bid.Offer.Type", "Price", "cumMW")],
      cumSO[,c("Commitment.Period", "Bid.Offer.Type", "Price", "cumMW")])

    aux
  }

  CC <- ddply(AA, .(Commitment.Period), .oneMonth)

  

  logTicks <- function (lim, loc = c(1, 5)) {
    ii <- floor(log10(range(lim))) + c(-1, 2)
    main <- 10^(ii[1]:ii[2])
    r <- as.numeric(outer(loc, main, "*"))
    r[lim[1] <= r & r <= lim[2]]
  }
  
  yscale.components.log10 <- function(lim, ...) {
    ans <- yscale.components.default(lim=lim, ...)
#browser()    
    tick.at <- c(0.01, 0.1, 1, 10) #logTicks(10^lim, loc=0:2)
    tick.at.major <- c(0.01, 0.1, 1, 10) #logTicks(10^lim, loc = 1)
    major <- tick.at %in% tick.at.major
    ans$left$ticks$at <- log(tick.at, 10)
    ans$left$ticks$tck <- ifelse(major, 1.5, 0.75)
    ans$left$labels$at <- log(tick.at, 10)
    ans$left$labels$labels <- as.character(tick.at)
    ans$left$labels$labels[!major] <- ""
    ans$left$labels$check.overlap <- FALSE
    ans
 }  

  
  xyplot(Price ~ cumMW | Commitment.Period,
    groups=Bid.Offer.Type, 
    data=CC,
    xlim=c(0,1000),      
    type=c("g", "b"),
    scales=list(y=list(log=TRUE)),
    yscale.components = yscale.components.log10,
    #subset=Commitment.Period == as.Date("2011-12-01")
    )
}


################################################################
# Get the FCA obligations for one capability period from the
# MIS reports
# 
.get_fca_obligations <- function( capacityAuction,
   company=c("CCG", "CNE", "EXELON") )
{
  if (capacityAuction$auction != "FCA")
    stop("Not an FCA auction!")
   
  rLog("Pulling FCA obligation for", company, capacityAuction$capabilityPeriod)
  fnames <- .get_report_csvfilenames( org=company,
    month=capacityAuction$month, reportName="SP_FCARESULT" )
  fnames <- fnames[order(fnames)]
  fname <- fnames[ length(fnames) ]   # get the last report

  aux <- .process_report( fname )  
  res <- cbind(company=company, aux[[2]])            # Resource Detail
  res$Commitment.Period.Month <- as.Date(res$Commitment.Period.Month, "%m/%d/%Y")

  res
}


################################################################
# Read the SP_MRARESULT report
# keep - index of parts to keep, e.g. c(1,3) to keep parts 1 and 3 
#
.make_position_spreadsheet <- function( )
{
  company <- c("CCG") #, "CNE", "EXELON")
  
  capacityAuctions <- list(CapacityAuction("2013-2014", "FCA"),
                           CapacityAuction("2014-2015", "FCA"),
                           CapacityAuction("2015-2016", "FCA"))

  res <- lapply(capacityAuctions, function(ca, company){
    .get_fca_obligations(ca, company)}, company)
  res <- do.call(rbind, res)

  aux <- cast(res, company + Resource.Name + Commitment.Period.Month +
    Obligation.Type + Payment.Rate ~ .,
    sum, value="Capacity.Supply.Obligation", fill=0)
  colnames(aux)[ncol(aux)] <- "MW"
  
  aux$capabilityPeriod <- CapabilityPeriod(aux$Commitment.Period.Month)
  aux <- aux[order(aux$Resource.Name, aux$Obligation.Type, aux$Commitment.Period.Month),]

  
  #print(aux, row.names=FALSE)   # paste into spreadsheet
  aux
}


################################################################
# update a file with all fcm prices
# run once every week 
#
.update_fcm_price_archive <- function(month=as.Date("2012-07-01"))
{
  fname <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/fcm_prices.RData"
  if (file.exists(fname)) {
    load(fname)                  # loads FCM_PRICES
  }  else {
    FCM_PRICES <- NULL
  }
  
  FCM_PRICES <- .update_price_annual(month, FCM_PRICES)
  
  FCM_PRICES <- .update_price_ara(month, FCM_PRICES)
  
  FCM_PRICES <- .update_price_monthly(month, FCM_PRICES)

  
  save(FCM_PRICES, file=fname)
  rLog("Saved FCM_PRICES to", fname)
  
}


################################################################
# update a file with all fcm prices
# run once every week 
#
.update_price_annual <- function(month=as.Date("2012-01-01"),
   FCM_PRICES=NULL)
{
  currentYear <- as.numeric(format(month, "%Y"))
  years <- 2010:(currentYear+6)

  for (year in years) {
    rLog("Working on year", year)
    capabilityPeriod <- paste(year, "-", year+1, sep="")
    junYear <- as.Date(paste(year, "-06-01", sep=""))
    
    # the FCA
    fname <- .get_report_csvfilenames(org="CCG", month=junYear,
        reportName="SP_FCARESULT")
    missing <- !(fname %in% FCM_PRICES$filename)
    if ( any(missing) ) {
      fname <- fname[missing]
      if ( .is_folder_zipped(dirname(fname[1])) ) {
        rLog("Have to unzip!")
        next
      } else {
        res <- lapply(fname, function(x){
          aux <- .process_report(x)[[4]]
          bux <- melt(aux[,c("Modeled.Capacity.Zone.Name",
            "Capacity.Clearing.Price", "Payment.Rate", "RTEG.Payment.Rate")], 1)
          cbind(auction="FCA", capability.period=capabilityPeriod, bux,
            month=format(junYear), version=.extract_report_version(x), filename=x)
        })
      }
      res <- do.call(rbind, res)
      colnames(res)[3] <- "Capacity.Zone.Name"
      FCM_PRICES <- rbind(FCM_PRICES, res)   # add them to the bunch
    }
  }
  
  
  FCM_PRICES
}


################################################################
# update a file with all fcm prices
# run once every week 
#
.update_price_ara <- function(month=as.Date("2012-01-01"),
   FCM_PRICES=NULL)
{
  currentYear <- as.numeric(format(month, "%Y"))
  years <- 2010:(currentYear+6)

  for (year in years) {
    rLog("Working on year", year)
    capabilityPeriod <- paste(year, "-", year+1, sep="")
    junYear <- as.Date(paste(year, "-06-01", sep=""))
   
    fname <- .get_report_csvfilenames(org="CCG", month=junYear,
      reportName="SP_ARARESULT")
    missing <- !(fname %in% FCM_PRICES$filename)
    if ( any(missing) ) {
      fname <- fname[missing]
      if ( .is_folder_zipped(basename(fname[1])) ) {
        rLog("Have to unzip!")
        next
      } else {
        res <- lapply(fname, function(x){
          aux <- .process_report(x)[[2]]
          bux <- aux[,c("Capacity.Zone.Name",
            "Clearing.Price")]
          colnames(bux)[2] <- "value"
          cbind(auction="ARA", capability.period=capabilityPeriod, bux,
            variable="Capacity.Clearing.Price", month=format(junYear), 
            version=.extract_report_version(x), filename=x)
        })
      }
      res <- do.call(rbind, res)
      FCM_PRICES <- rbind(FCM_PRICES, res)   # add them to the bunch
    }
  }
  
  
  FCM_PRICES
}

################################################################
# update a file with all fcm prices
# run once every week 
#
.update_price_monthly <- function(month=as.Date("2012-01-01"),
   FCM_PRICES=NULL)
{
  months <- format(seq(as.Date("2010-06-01"), month, by="1 month"))

  for (month in months) {
    rLog("Working on month", month)
    year <- as.numeric(format(as.Date(month), "%Y"))
    capabilityPeriod <- paste(year, "-", year+1, sep="")
   
    fname.CCG <- .get_report_csvfilenames(org="CCG", month=as.Date(month),
        reportName="SP_MRARESULT")
    fname.CNE <- .get_report_csvfilenames(org="CNE", month=as.Date(month),
        reportName="SP_MRARESULT")
    fname <- c(fname.CCG, fname.CNE)
    
    missing <- !(fname %in% FCM_PRICES$filename)
    if ( any(missing) ) {
      fname <- fname[missing]
      if ( .is_folder_zipped(dirname(fname[1])) ) {
        rLog("Have to unzip!")
        next
      } else {
        res <- lapply(fname, function(x){
          aux <- .process_report(x)[[2]]
          if (is.null(aux)) {
            NULL
          } else {
            bux <- aux[,c("Capacity.Zone.Name",
              "Clearing.Price")]
            colnames(bux)[2] <- "value"
            cbind(auction="MRA", capability.period=capabilityPeriod, bux,
              variable="Capacity.Clearing.Price", month=month, 
              version=.extract_report_version(x), filename=x)
          }
        })
      }
      res <- do.call(rbind, res)
      FCM_PRICES <- rbind(FCM_PRICES, res)   # add them to the bunch
    }
  }
  
  
  FCM_PRICES
}



################################################################
# monthly auction reconfig archive BIDS/OFFERS
#
.read_archive_FCMMRAHB <- function()
{
  require(DivideAndConquer)

  .readOne <- function(file=file){
    .process_report(file)[[1]][,-1]
  }

  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FCM_Monthly_Reconfig/Raw"
  files <- list.files(DIR, full.name=TRUE, pattern="\\.CSV$")

  res <- finalize(files, .readOne)

  aux <- melt(res, id=1:8)        # get to long format 
  aux <- aux[!is.na(aux$value),]  # get rid of NA's
  aux$segment <- gsub("Segment\\.([[:digit:]]{1})\\..*", "\\1", aux$variable)
  aux$variable <- gsub("Segment\\..*\\.(.*)$", "\\1", aux$variable)

  res <- data.frame(cast(aux, ... ~ variable, I, fill=NA))
  res$Commitment.Period <- as.Date(paste("1", res$Commitment.Period), "%d %B %Y")
  res <- res[order(res$Commitment.Period, res$Masked.Lead.Participant.ID,
                   res$Masked.Resource.ID), ]
  
  
}


################################################################
################################################################
# 
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(reshape)
  require(lattice)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FCM.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Capacity/lib.capacity.auctions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")


  ###################################################################
  .analyze_summary()  # from the summary xls files


  ###################################################################
  #
  company <- c("CNE") #, "CCG", "EXELON")
  
  capacityAuctions <- list(
    CapacityAuction( auction="FCA", capabilityPeriod="2013-2014" ),
    CapacityAuction( auction="FCA", capabilityPeriod="2014-2015" ),
    CapacityAuction( auction="FCA", capabilityPeriod="2015-2016" ))

  obl <- lapply(capacityAuctions, function(ca, company){
    .get_fca_obligations(ca, company)}, company)
  obl <- do.call(rbind, obl)
  
  

  
  ###################################################################
  #
  qq <- .make_position_spreadsheet()
  # print(qq, row.names=FALSE)
  # unique(qq$Resource.Name)   "FORE RIVER-1"
  
  aux <- subset(qq, Resource.Name %in% c("FORE RIVER-1", "MYSTIC 8") &
                capabilityPeriod=="2014-2015")
  aux <- aux[order(aux$Resource.Name, aux$Obligation.Type, aux$Commitment.Period.Month),]
  



  
  ###################################################################
  # load the FCM archive 
  FILE_ARCHIVE <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/",
    "fcm_summary.RData", sep="")
  load(FILE_ARCHIVE)              # loads the FCM data.frame

  ###################################################################
  # select the NY AC ties for Dean
  aux <- subset(FCM, section == "External Interface")
  bux <- ddply(aux, .(filename), function(x) {
    ind <- grep("New York AC", x$value)
    if (length(ind) == 1)
      subset(x, rowIdx == x$rowIdx[ind])
    else {
      browser()
    }
  })
  write.csv(bux, file="C:/Temp/capacity_flows.csv", row.names=FALSE)

  
  cux <- subset(bux[grep("Net Capacity", bux$column), ], isBilateral==FALSE)
  cux <- cux[order(cux$month), ]
  print(cux, row.names=FALSE)

  
  
  
  
  
  # refresh the archive FCMMRAHB from Scala ... 
  AA <- .read_archive_FCMMRAHB()


  aa <- subset(AA, Commitment.Period == as.Date("2010-06-01"))

  


  

}
