# Deal with OATT-Schedule 2 VAR
# Network load pays, Generators get credit
#
# Historical data for mw is at 
#   http://www.iso-ne.com/stlmnts/iso_rto_tariff/schd2/var_anccrt/index.html
# Historical cost info in the spreadsheet from:
#   http://www.iso-ne.com/stlmnts/iso_rto_tariff/rate_dev/index.html 
#
# archive_mis_volt
# calc_pnl_impact
# get_positions_volt
# get_payments_volt
# get_qualified_mvar
# pull_hist_volt
# .pull_marks_volt
# 



################################################################
# archive the MIS reports
#
archive_mis_volt <- function(months=NULL)
{
  if (is.null(months))
    months <- rev(seq(prevMonth(), by="-1 month", length.out=12))
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  reportName <- "SD_VARCCPMT"; tab <- 1

  fnames <- unlist(sapply(months, function(m){
    .get_report_csvfilenames(org="EXGN", month=m, reportName=reportName)
  }))
  AA <- .archive_report(fnames, tab=tab, save=TRUE)

  AA
}


################################################################
# Calculate PnL impact on marks change
# Where qq is a data.frame with columns c("service", "month", "location", "qty")
#
calc_pnl_impact_volt <- function(day1, day2, qq, 
   endDate=as.Date("2017-12-01"), do.format=TRUE)
{
  asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b")) 
  qq <- get_positions_volt(asOfDate)
  qq <- qq[order(qq$month),]
  head(qq)
  
  PP1 <- .pull_marks_volt()
  colnames(PP1)[which(colnames(PP1)=="value")] <- "P1"

  PP2 <- pull_marks_trsch23(day2, day2, endDate, service=service)
  colnames(PP2)[which(colnames(PP2)=="value")] <- "P2"

  PnL <- merge(qq[,c("month", "qty")],  PP1[,c("month", "serviceType", "P1")],
               all.x=TRUE)
  PnL <- merge(PnL, PP2[,c("month", "serviceType", "P2")], all.x=TRUE)
  PnL <- PnL[order(PnL$serviceType, PnL$month),]
  PnL$PnL <- PnL$qty*(PnL$P2 - PnL$P1)
  PnL <- PnL[!is.na(PnL$PnL), ]
  PnL <- PnL[abs(PnL$PnL)>10,]
  PnL <- PnL[order(PnL$serviceType, PnL$month),]
  
  rLog("Total PnL change is", sum(PnL$PnL))

  if (do.format & (nrow(PnL)>0)) {
    PnL$qty <- round(PnL$qty)
    PnL$PnL <- dollar(PnL$PnL,0)
  }
  
  PnL
}


################################################################
# get booked positions for a given portfolio
# MysticJet is booked in the MYSTIC7M book, no VOLT delta
#
get_positions_volt <- function(asOfDate=Sys.Date()-1,
  service="VOLT", from="SECDB")
{
  books <- c("FORERIVM", "MYSTC89M", "MYSTIC7M", "NPKLEEN",
             "XFRMGHM1", "XFRMGHM2", "XFRMGHM3",
             "XMEDWAY1", "XMEDWAY2", "XMEDWAY3",
             "XLSTREET", "XWYMAN4")
  if (from == "SECDB") {

    filter <- function(x){ x$`Service Type` == "VOLT" }
    qq <- ldply(books, function(book){
      aux <- secdb.flatReport(asOfDate=asOfDate, books=book,
        variables=c("Quantity", "Today Value", "Active Contracts",
        "Delivery Point", "Service Type", "Settlement Type"), filter=filter,
        to.df=TRUE)
      if (length(aux)==0) NULL else cbind(book=book, aux)
    })
     
    
  } else {   # PRISM
    rLog("I cannot tieout PRISM with FLAT report anymore for VOLT!")
    rLog("Pull", service, "positions for", portfolio, "as of", format(asOfDate))
    aux <- get.positions.from.blackbird( asOfDate=asOfDate, fix=TRUE,
      service=service, portfolio=portfolio)

    qq <- cast(aux, contract.dt + delivery.point ~ ., sum, fill=NA,
      value="notional")
    names(qq) <- c("month", "location", "qty")

    cbind(service=service, as.data.frame(qq))
  }

  hrs <- ntplot.bucketHours("Flat", startDate=min(qq$month),
    endDate=max(qq$month), period="Month")
  names(hrs)[2:3] <- c("month", "hours")
  qq <- merge(qq, hrs[, c("month", "hours")])
  qq$mw <- qq$Quantity/qq$hours
  
  qq
}


################################################################
# from the historical ISO reports for all the units in the pool
# http://www.iso-ne.com/stlmnts/iso_rto_tariff/schd2/var_anccrt/index.html
#
get_qualified_mvar <- function()
{
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")

  reportName <- "WW_VARCCRATE"; tab <- 2

  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Voltage/Raw/"
  fnames <- list.files(DIR, full.names=TRUE)
  AA <- .archive_report(fnames, tab=tab)
  head(AA)
  #AA <- .archive_report(fnames, tab=tab, save=TRUE)

  
  AA
}



#########################################################################
# get the historical mark
#
pull_hist_volt <- function()
{
  # cost in $/KVAR-year, set for the calendar year
  HH <- data.frame(
    year     = c(2009,    2010,    2011,    2012,    2013),
    value.yr = c(1.15521, 1.16520, 1.17051, 1.14744, 1.17091))

  # add the hours
  endDate <- as.Date(ISOdate(max(HH$year), 12, 1))
  HRS <- ntplot.bucketHours("FLAT", as.Date("2009-01-01"), endDate,
    region="NEPOOL", period="Month")
  colnames(HRS)[2:3] <- c("month", "hours")
  HRS$year <- as.numeric(format(HRS$month, "%Y"))
  
  HH <- merge(HH, HRS, by="year")
  HH$value <- HH$value.yr * 1000/HH$hours * (1/12)  # in $/MWh
  
  HH[, c("month", "value")]
}


#########################################################################
#
#
.pull_marks_volt <- function(asOfDate=Sys.Date(),
   startDate=nextMonth(), endDate=NULL, service="VOLT")
{
  if (is.null(endDate))
    endDate <- as.Date(paste(as.numeric(format(asOfDate, "%Y"))+5, 
       "-12-01", sep=""))
  
  rLog("Pull VOLT marks as of", format(asOfDate))
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  useFutDates <- FALSE
  location    <- " "      # <-- "" does not work!!!
  bucket      <- c("FLAT")
  PP <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType=service, useFutDates)
  
  PP
}






#########################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(zoo)
  require(SecDb)
  require(lattice)
  require(PM)

  #source("H:/user/R/RMG/Utilities/Database/VCentral/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/Nepool/Ancillaries/lib.volt.R")

  archive_mis_volt()
  
  #====================================================================
  # get marks  
  PP <- rbind(data.frame(type="hist", pull_hist_volt()),
              data.frame(type="fwd",  .pull_marks_volt()[, c("month", "value")]))
  xyplot(value ~ month, data=PP, type=c("g","o"), groups=PP$type, 
         ylab="Price, $/MWh",
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=length(unique(PP$type))))

  #pull_marks_volt(endDate=as.Date("2020-12-01"))[, c("month", "value")]
  
  
  #====================================================================
  # get positions
  QQ <- get_positions_volt()

  res <- cast(QQ, month ~ book, I, fill=0, value="mw")
  print(res, row.names=FALSE)  # put into spreadsheet
  

  calc_pnl_impact( as.Date("2012-05-15"), as.Date("2012-05-16"),
     portfolio=portfolio )

  
  #====================================================================
  # get qualified mvar and see how they changed historically for the units
  AA <- archive_mis_volt()
  ptids <- c(unique(AA$Asset.ID), 1691)

  MM <- get_qualified_mvar()
  MM <- subset(MM, Asset.ID %in% ptids)
  aux <- cast(MM, Asset.Name ~ reportDay, I, value="Total.Qualified.VARs", fill=0)
  
    

  #====================================================================
  # refresh archive
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  AA <- archive_mis_volt()

  month <- as.Date("2013-05-01")
  qq <- subset(AA, reportDay == month)
  qq <- qq[, c("reportDay", "Asset.ID", "Asset.Name", "Total.Qualified.VARs",
               "Ownership.Share", "Customer.Share.Capacity.Cost.Payment")]
  print(qq, row.names=FALSE)

  #====================================================================
  # look for Pilgrim/Rise
  MM <- get_qualified_mvar()
  qq <- subset(MM, Asset.ID %in% c(537))  # 1630
  
  
  

}


  ## startDate <- as.Date("2012-07-01")
  ## endDate   <- as.Date("2012-12-01")
  ## serviceType <- "TR SCH2"
  

