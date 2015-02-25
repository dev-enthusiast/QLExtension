# Keep some utilities for dealing with (my) ENERGY positions
#
# .change_in_positions
# .etis_for_delivpt                  <- get all etis for a delivpt
# .format_one_pfolio
# .format_secdb_deltas               <- good one to use
# .get_booked_load_mwh               <- how much load do we currently have booked, for executives
# .get_booked_mcc_locations          <- return MCC locations where I have FTRs
# .get_positions_delivpt             <- by month, bucket, delivery point, from the R Archive
# .get_positions_book                <- by month, bucket, delivery point, from prism
# .get_zonal_positions_SecDb         <- from SecDb, at commod locations
#
# .read_R_position_archive
# .mwh_to_mw                         <- go from mwh to mw
#

# See also lib.positions.move
#

#################################################################
# @param books, a list of books
# aggregation refers to book aggregation only
#
.change_in_positions <- function(books, day1, day2, aggregate=FALSE)
{
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.compare.df.R")
  
  QQ1 <- .get_zonal_positions_SecDb(day1, books, aggregate=aggregate)
  colnames(QQ1)[which(colnames(QQ1)=="portfolioBook")] <- "book"
  #QQ1 <- QQ1[,c("book", "curve", "month", "value")]
  QQ1$value <- round(QQ1$value)
  
  QQ2 <- .get_zonal_positions_SecDb(day2, books, aggregate=aggregate)
  colnames(QQ2)[which(colnames(QQ2)=="portfolioBook")] <- "book"
  #QQ2 <- QQ2[,c("book", "curve", "month", "value")]
  QQ2$value <- round(QQ2$value)

  res <- .change_data_frames(QQ1, QQ2)
  res$year <- as.numeric(format(res$month, "%Y"))
  
  res
}


#################################################################
# from = "R_ARCHIVE" or maybe "PRISM" (TODO)
# delivpt = "SEABROOK.SBRK"
# searches for  "SEABROOK.SBRK",  "SEABROOK.SBRK DA",
#               "SEABROOK.SBRK RT",  "MCC SEABROOK.SBRK DA" 
#
.etis_for_delivpt <- function(delivpt,
   asOfDate=as.Date(secdb.dateRuleApply(Sys.Date(), "-1b")),
   books=secdb.getBooksFromPortfolio("PM NorthEast Portfolio"),
   from="R_ARCHIVE")
{
  if (from == "R_ARCHIVE") {
    QQ <- .read_R_position_archive(asOfDate)
    QQ <- subset(QQ, portfolio %in% c("NEPOOL ACCRUAL PORTFOLIO", "NEPOOL MTM HEDGES OF ACCRUAL"))
    QQ <- subset(QQ, mkt == "PWX" & leg.type != "ELEC ATTRITION PRICER")
    QQ <- subset(QQ, service != "UCAP")
    if (unique(QQ$service) != "ENERGY")
      stop("Please check!")

    QQ <- subset(QQ, trading.book %in% books)

    locs <- c(delivpt, paste(delivpt, "DA"), paste(delivpt, "RT"), paste("MCC", delivpt))
    QQ <- subset(QQ, delivery.point %in% locs)

    qq <- unique(QQ[, c("eti", "trading.book", "delivery.point", "counterparty",
                        "trade.start.date", "trade.end.date")])    
  } else {
    stop("Not implemented from=", from)
  }

  qq
}



#################################################################
# format the output of one portfolio ignore UCAP, ISOFEE, VLRCST 
# Where x is the list returned from from secdb.readDeltas
# Output is a data.frame month, CT_PEAK, CT_OFFPEAK, etc.
#
# used in the daily positions report, should be DEPRECATED and 
# use .format_secdb_deltas!
#
.format_one_pfolio <- function(x, months=NULL, mw=TRUE, offpeak=TRUE)
{
  rLog("DEPRECATED!  Use lib_positions.R/.format_secdb_deltas!!!")
  x <- x[sapply(x, length) > 0]  # there are some empty positions
  qq <- ldply(x, I)
  colnames(qq)[1:2] <- c("curvename", "contract.dt")

  if (!is.null(months))
    qq <- subset(qq, contract.dt %in% months)
  
  qq <- cbind(qq, PM:::classifyCurveName(toupper(qq$curvename)))
  qq <- subset(qq, market == "PWX")
  qq <- subset(qq, !grepl("UCAP", qq$curvename))  # take out some UCAP curves
  qq <- subset(qq, !(location %in% c("ISOFEE", "VLRCST")))

  if (offpeak) {
    qq$bucket <- qq$pkoffpk
    qq$pkoffpk <- NULL
    # aggregate the 2x16H and 7x8 into OFFPEAK
    qq <- cast(qq, contract.dt + location + bucket ~ ., sum)  
    qq$bucket <- gsub("^PEAK", "ONPEAK", qq$bucket)
  }

  if (mw) {
      HRS <- subset(NEPOOL$HRS, contract.dt %in% months)
      names(HRS)[which(names(HRS)=="class.type")] <- "bucket"
      qq <- merge(qq, HRS, by=c("contract.dt", "bucket"))
      qq$mw <- qq[,"(all)"]/qq$hours
  }

  qq$location <- gsub("^BOS$", "NEMA", qq$location)
  qq$location <- gsub("^MAHUB$", "Hub", qq$location)

  colnames(qq)[ncol(qq)] <- "value"
  
  qq
}


#################################################################
# format the output of secdb.readDeltas
# ignore UCAP, ISOFEE, VLRCST 
#  
# return a data.frame with zonal positions, buckets (5x16, 2x16H, 7x8)
# good for PxQ calculations, etc.
#
.format_secdb_deltas <- function(x)
{
  x <- x[sapply(x, length) > 0]  # there are some empty positions
  qq <- ldply(x, I)
  colnames(qq)[1:2] <- c("curvename", "contract.dt")
  
  qq <- cbind(qq, PM:::classifyCurveName(toupper(qq$curvename)))
  qq <- subset(qq, market == "PWX")
  qq <- subset(qq, !grepl("UCAP", qq$curvename))  # take out some UCAP curves
  qq <- subset(qq, !(location %in% c("ISOFEE", "VLRCST")))

  qq$location <- gsub("^BOSTON", "NEMA", qq$location)
  qq$location <- gsub("^MAHUB$", "HUB", qq$location)
  qq$location <- toupper(qq$location)

  ind <- which(qq$bucket == "2X16" & qq$location == "HUB")   # <-- PWX only
  if (length(ind) > 0)
    qq[ind,"bucket"] <- "2X16H"
  
  colnames(qq)[which(colnames(qq) == "contract.dt")] <- "month"
  
  qq[,c("market", "bucket", "location", "month", "value")]
}


########################################################################
# get all ccg, cne loads 
#
.get_booked_load_mwh <- function(asOfDate=as.Date(secdb.dateRuleApply(Sys.Date(), "-1b")))
{
  
  pfolio <- "NEPOOL Load Portfolio"
  res <- PM:::secdb.readDeltas2(asOfDate, pfolio, aggregate=TRUE)
  res <- cbind(res,
    PM:::classifyCurveName(res$curve, by=c("bucket", "location", "market"))[,-1])
  res <- subset(res, market=="PWX")
  res <- subset(res, !(location %in% c("UCAP", "VLRCST", "ROP", "ISOFEE")))
  res <- res[!grepl(" UCAP ", res$curve), ]
  res$company <- "CCG"
  ccg <- res


  pfolio <- "CNE NewEngland Load Portfolio"
  res <- PM:::secdb.readDeltas2(asOfDate, pfolio, aggregate=TRUE)
  res <- cbind(res,
    PM:::classifyCurveName(res$curve, by=c("bucket", "location", "market"))[,-1])
  res <- subset(res, market=="PWX")
  res <- subset(res, !(location %in% c("UCAP", "VLRCST", "ROP", "ISOFEE")))
  res <- res[!grepl(" UCAP ", res$curve), ]
  res$company <- "CNE"
  cne <- res

    
  rbind(ccg, cne)
}



########################################################################
# get all the mcc locations where I have positions
#
.get_booked_mcc_locations <- function(asOfDate, books=c("NESTAT", "NPFTRMT1"))
{
  QQ <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
    fix=TRUE, service="ENERGY")

  aux <- unique(QQ$delivery.point)
  aux <- aux[grep("^MCC ", aux)]
  sort(aux)
}




########################################################################
# POWER only for now, by delivery.point, MCC and all the rest 
# prompt+ months
#
.get_positions_book <- function(books, asOfDate, agg.book=TRUE)
{
  QQ <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
    fix=TRUE, service="ENERGY")
  QQ <- subset(QQ, mkt %in% c("PWX", "PWY") & contract.dt >= nextMonth())
  
  if (agg.book) {
    aux <- cast(QQ, contract.dt + bucket + delivery.point ~ ., sum,
                value="delta")
    names(aux)[ncol(aux)] <- "value"
  } else {
    aux <- cast(QQ, trading.book + contract.dt + bucket + delivery.point ~ .,
                sum, value="delta")
    names(aux)[ncol(aux)] <- "value"
  }

  names(aux)[which(names(aux)=="delivery.point")] <- "location"
  names(aux)[which(names(aux)=="contract.dt")] <- "month"

  aux
}


#################################################################
# get the positions by delivery point from ARCHIVE
# my positions mostly ... 
#
.get_positions_archive <- function(asOfDate=Sys.Date()-1,
   books=secdb.getBooksFromPortfolio("PM NorthEast Portfolio"),
   agg.book=TRUE)
{
  QQ <- .read_R_position_archive(asOfDate)
  if (asOfDate < as.Date("2013-07-25")) {   
    QQ <- subset(QQ, portfolio %in% c("NEPOOL ACCRUAL PORTFOLIO",
                                     "NEPOOL MTM HEDGES OF ACCRUAL"))
  }
  QQ <- subset(QQ, trading.book %in% books)
  QQ <- subset(QQ, mkt %in% c("PWY","PWX") & leg.type != "ELEC ATTRITION PRICER")
  QQ <- subset(QQ, service != "UCAP")
  QQ <- subset(QQ, contract.dt >= nextMonth())

  # fix this when the positions got rebooked!
  ind <- which(QQ$delivery.point == "KLN DA")
  if (length(ind) > 0) QQ[ind, "delivery.point"] <- "CT DA"
  ind <- which(QQ$delivery.point == "EDGAR.EDGR DA")
  if (length(ind) > 0) QQ[ind, "delivery.point"] <- "EDGAR.40327 DA"
  ind <- which(QQ$delivery.point == "HUB")
  if (length(ind) > 0) QQ[ind, "delivery.point"] <- "HUB DA"

  if (unique(QQ$service) != "ENERGY") {
    head(QQ)
    stop("Please check!")
  }
  
  if (agg.book) {
    qq <- cast(QQ, contract.dt + bucket + mkt + delivery.point ~ ., sum,
                value="delta")
    names(qq)[ncol(qq)] <- "value"
  } else {
    qq <- cast(QQ, trading.book + contract.dt + bucket + mkt + delivery.point ~ .,
                sum, value="delta")
    names(qq)[ncol(qq)] <- "value"
    colnames(qq)[which(colnames(qq)=="trading.book")] <- "book"
  }
  qq$value <- round(qq$value)
  colnames(qq)[which(colnames(qq)=="contract.dt")] <- "month"
  colnames(qq)[which(colnames(qq)=="delivery.point")] <- "location"
  
  qq
}



#################################################################
# get positions by delivery point, aggregate DA, RT, MCC to DA!
# location!!
# Use this only for Power
#
# @param from "R_ARCHIVE" or "PRISM"
#
.get_positions_delivpt <- function(asOfDate=Sys.Date()-1,
   books=secdb.getBooksFromPortfolio("PM NorthEast Portfolio"),
   from="R_ARCHIVE", markets=c("PWX"))
{
  if (from == "R_ARCHIVE") {
    QQ <- .read_R_position_archive(asOfDate)
    QQ <- subset(QQ, trading.book %in% books)
    QQ <- subset(QQ, mkt %in% markets)
    QQ <- subset(QQ, leg.type != "ELEC ATTRITION PRICER")
    QQ <- subset(QQ, service  != "UCAP")
    ## if (unique(QQ$service) != "ENERGY")
    ##   stop("Please check!")
    
    #aux <- subset(QQ, delivery.point == "NGRID NEG")   <-- What is this solar deal ?!  
    qq <- getPositionsDeliveryPoint(QQ, subset=TRUE, buckets="", mw=FALSE)
    qq$value <- round(qq$value)
    colnames(qq)[which(colnames(qq)=="contract.dt")] <- "month"

    qq$location <- paste(qq$delivery.point, "DA")  
  } else if (from == "PRISM") {
    source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
    qq <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
                                          fix=TRUE)
    qq$location <- qq$delivery.point
  } else {
    stop("Not implemented from=", from)
  }

  
  qq$delivery.point <- NULL
  qq
}



#################################################################
# Read the SecDb positions by zone
# 
.get_zonal_positions_SecDb <- function(asOfDate, portfolioBooks,
   aggregate=TRUE)
{
  res <- PM:::secdb.readDeltas2(asOfDate, portfolioBooks, aggregate=aggregate)
  if (!aggregate)
    res <- do.call(rbind, res)
  
  res <- cbind(res,
    PM:::classifyCurveName(res$curve,
      by=c("bucket", "location", "market", "service"))[,-1])


  res
}




#################################################################
# read the archive file with positions
#
.read_R_position_archive <- function(asOfDate, books=NULL)
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
  filename <- paste(DIR, "positionsAAD_", format(asOfDate), ".RData", sep="")

  if (file.exists(filename)) {
    load(filename)
  } else {
    rLog("Cannot find day", format(asOfDate), "in the archive directory!")
    QQ <- NULL
  }

  if (!is.null(books))
    QQ <- subset(QQ, trading.book %in% books)

  
  QQ
}




#################################################################
# 
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(SecDb)
  require(PM)
  #require(DivideAndConquer)

  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")

  
  #===================================================
  # look at day on day changes in positions 
  day1 <- as.Date("2013-07-30")
  day2 <- as.Date("2013-07-31")
  books <- PM:::secdb.getBooksFromPortfolio(portfolio="Nepool Owned Gen Portfolio")
  res <- .change_in_positions(books, day1, day2, from="R_ARCHIVE")


  require(SecDb)
  asOfDate <- as.Date("2012-07-09")
  
  x <-  secdb.readDeltas2(asOfDate, "pm northeast portfolio") # use PM::secdb.readDeltas2
  x[grep(" CT ", names(x))]


  asOfDate <- as.Date("2012-11-23")
  QQ <- .read_R_position_archive(asOfDate)  # everything

  # positions by location, month, bucket
  QQ <- .get_positions_delivpt(as.Date(secdb.dateRuleApply(Sys.Date(), "-1b")),
                               books="FORERIVM")

  # all the trades for a delivery point
  .etis_for_delivpt( "SEABROOK.SBRK" )
  


  #=======================================================================
  asOfDate <- as.Date(Sys.Date()-1)
  books <- PM:::secdb.getBooksFromPortfolio(portfolio="CNE NewEngland Load Portfolio")
  cne <- .get_zonal_positions_SecDb(asOfDate, books, aggregate=TRUE)
  cne <- subset(cne, service == "ENERGY" & market == "PWX")
  cne <- cast(cne, month ~ bucket, sum, fill=0, value="value") 

  books <- PM:::secdb.getBooksFromPortfolio(portfolio="Nepool Load Portfolio")
  ccg <- .get_zonal_positions_SecDb(asOfDate, books, aggregate=TRUE)
  ccg <- subset(ccg, service == "ENERGY" & market == "PWX")
  ccg <- cast(ccg, month ~ bucket, sum, fill=0, value="value") 

  
  
  #=======================================================================
  #=======================================================================
  # get NEMA positions breakdown 3/21/2014
  # 
  asOfDate <- as.Date("2014-03-20")
  
  QQ <- .get_zonal_positions_SecDb(asOfDate,  "PM NorthEast Portfolio")
  subset(QQ, location %in% c("NEMA", "BOSTON"))
  AA <- subset(QQ, service == "ENERGY" & location %in% c("NEMA", "BOSTON") & market == "PWX")
  head(AA)
  AA$id <- "All position"
  
  LL <- .get_booked_load_mwh()
  head(LL)
  unique(LL$location)
  LL$id <- paste(LL$company, "load")
  LL <- subset(LL, location=="BOSTON")
  #LL <- cast(LL, company + month ~ bucket, sum, subset=location=="BOSTON")

  #GG <- .get_zonal_positions_SecDb(asOfDate, "Nepool Gen Portfolio")
  GG <- .get_zonal_positions_SecDb(asOfDate, "Nepool Owned Gen Portfolio")
  GG <- subset(GG, service == "ENERGY" & location %in% c("NEMA", "BOSTON") & market == "PWX")
  head(GG)
  GG$id <- "Generation"
  
  MY <- .get_zonal_positions_SecDb(asOfDate, "MYSTC89M")
  MY <- subset(MY, service == "ENERGY" & location %in% c("NEMA", "BOSTON") & market == "PWX")
  head(MY)
  MY$id <- "Mystic89"

  FTR <- .get_zonal_positions_SecDb(asOfDate, "NPFTRMT1")
  FTR <- subset(FTR, service == "ENERGY" & location %in% c("BOSTON") & market == "PWX")
  head(FTR)
  FTR$id <- "FTRs"

  OTC <- .get_zonal_positions_SecDb(asOfDate, "NEPPWRBS")
  OTC <- subset(OTC, service == "ENERGY" & location %in% c("BOSTON") & market == "PWX")
  head(OTC)
  OTC$id <- "OTC"
  
  res <- rbind(AA[, c("id", "month", "bucket", "location", "value")],
               LL[, c("id", "month", "bucket", "location", "value")],
               GG[, c("id", "month", "bucket", "location", "value")],
               MY[, c("id", "month", "bucket", "location", "value")],
               FTR[, c("id", "month", "bucket", "location", "value")],
               OTC[, c("id", "month", "bucket", "location", "value")])
  hrs <- ntplot.bucketHours(c("2X16H", "5X16", "7X8"), min(res$month), max(res$month),
                            region="NEPOOL", period="Month")
  colnames(hrs)[2:3] <- c("month", "hours")
  res <- merge(res, hrs, by=c("month", "bucket"))
  colnames(res)[which(colnames(res)=="value")] <- "mwh"
  res$mw <- res$mwh / res$hours
  res$id <- factor(res$id, levels=c("Mystic89", "Generation", "CCG load", "CNE load", "OTC",
                             "FTRs", "All position"))
  
  cast(res, month ~ id + bucket, I, fill=0, value="mw")
  
  
  



























  
  #===================================================
  # get my zonal positions from SecDb
  # for Brad on 3/26/2012
  asOfDate <- as.Date("2012-12-31")
  portfolioBooks <- c("NEPPWRBS", "NPFTRMT1", "NESTAT", "CONGIBT", "CONGIBT2")
  QQ <- .get_zonal_positions_SecDb(asOfDate, portfolioBooks)
  write.csv(QQ, file="C:/temp/Adrian_zonalpositions_2012-12-31.csv", row.names=FALSE)

  
  asOfDate <- as.Date("2013-03-25")
  portfolioBooks <- "PM NorthEast Portfolio"
  QQ <- .get_zonal_positions_SecDb(asOfDate, portfolioBooks, energyOnly=TRUE)

  
  curvenames <- unique(QQ$curve)
  res <- secdb.readDeltas2(asOfDate, portfolioBooks, energyOnly=TRUE)
  unique(res$curve)
 
  #===================================================
  # get all the load booked in SecDb, ccg + cne 
  LL <- .get_booked_load_mwh()
  head(LL)
  unique(LL$location)
  
  aux <- cast(LL, company + month ~ bucket, sum)
  aux <- cast(LL, company + month ~ bucket, sum, subset=location=="BOSTON")
  write.csv(aux, file="c:/temp/load.csv", row.names=FALSE)

  
  
  
  
  
}







