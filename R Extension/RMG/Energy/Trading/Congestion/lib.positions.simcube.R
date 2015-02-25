# Deal with SimCube positions, it's a Microsoft SQL Server
#
# .fix_simcube_buckets
# get_positions_simcube
# get_positions_simcube_detail
# tieout_book
# get_Intr_vs_Extr_By_Book - new function to fetch the extrinsic value from PDS by book/region
# get_fuel_burn_data - new function to fetch oil burn data for a given book/region from PDS

#######################################################################
# Nip the nonsense!
#
.fix_simcube_buckets <- function( data )
{
  data$bucket <- ifelse(data$TimeBucket == "Peak", "5X16",
                        ifelse(data$TimeBucket == "Offpeak", "7X8",
                               ifelse(data$TimeBucket == "Weekend", "2X16H",
                                      ifelse(data$TimeBucket == "ATC", "7X24",
                                             ifelse(data$TimeBucket == "Wrap", "OFFPEAK", NA)))))


  data
}

#Get Oul Burn data for units
get_fuel_burn_data <- function( asOfDate, region = NULL, books  = NULL ){
    and <- NULL
    if (asOfDate >= Sys.Date())
        stop("You're funny!  You want positions from ", format(asOfDate), "?")

    if ( !is.null(region) ) {
        aux   <- paste(toupper(region), sep="", collapse="', '")
        aux   <- paste(" and s.RegionBU in ('", aux, "')", sep="")
        and <- paste(and, aux, sep="")
    }

    if ( !is.null(books) ) {
        aux   <- paste(toupper(books), sep="", collapse="', '")
        aux   <- paste(" and s.Book in ('", aux, "')", sep="")
        and <- paste(and, aux, sep="")
    }

    query <- paste(
        "select  s.reportdateid, s.ContractMonthId, s.TimeBucket, s.FuelType, s.averagemw, s.MMBTU, s.DeltaMMBTU ",
        " from ReportPosition.dbo.rnfatriskwithidentifier s ",
        " where s.RegionBU='New England' ",
        " and s.glt in ('Generation', 'Transaction') ",
        " and s.reportdateid='", format(asOfDate, "%Y%m%d"), "' ",
        and,   # add extra 'and' clauses
        " and adjustedName='Adjusted' ",
        " and s.FuelType not in ( 'Environmental') ",
        sep="")

    rLog("Querying SimCube...")       # password changes every 30 days ...
    con.string <- paste("FileDSN=",
                        "S:/All/Risk/Software/R/prod/Utilities/DSN/simcuberpt.dsn",
                        ";UID=simcuberpt;PWD=Madness14;", sep="")

    con  <- odbcDriverConnect(con.string)
    if (con == -1) {
        rLog("Cannot connect to DB.  Exiting ...")
        return(NULL)
    }

    data <- sqlQuery(con, query)

    odbcCloseAll()
    rLog("Done")

    data$month <- as.Date(as.character(data$ContractMonthId), "%Y%m%d")
    data$ContractMonthId <- NULL

    ## make a proper bucket column
    data <- .fix_simcube_buckets( data )
    colnames(data) <- tolower(colnames(data))

    data
}

# Function to monitor the PDS Gen Extrinsic value
get_Intr_vs_Extr_By_Book <- function( asOfDate, region = NULL, books= NULL ){

    and <- NULL
    if (asOfDate >= Sys.Date())
        stop("You're funny!  You want positions from ", format(asOfDate), "?")

    if ( !is.null(region) ) {
        aux   <- paste(toupper(region), sep="", collapse="', '")
        aux   <- paste(" and s.RegionBU in ('", aux, "')", sep="")
        and <- paste(and, aux, sep="")
    }

    if ( !is.null(books) ) {
        aux   <- paste(toupper(books), sep="", collapse="', '")
        aux   <- paste(" and s.Book in ('", aux, "')", sep="")
        and <- paste(and, aux, sep="")
    }

    query <- paste(
        "select * ",
        "from ReportPosition.dbo.rnfatriskWithIntrinsic s ",
        "where s.reportdateid='", format(asOfDate, "%Y%m%d"), "' ",
        " and s.AdjustedName = 'UnAdjusted' ",
        and,   # add extra 'and' clauses
#        " and s.TimeBucket in( 'Peak', 'OffPeak', 'Weekend' ) ",
        sep="")

    ## query <- paste("select * from ReportPosition.dbo.rnfatriskWithIdentifier s ",
    ##                "where s.reportdateid='", format(asOfDate, "%Y%m%d"), "' ",
    ##                "and s.book = 'FORERIVM'")


    rLog("Querying SimCube...")       # password changes every 30 days ...
    con.string <- paste("FileDSN=",
                        "S:/All/Risk/Software/R/prod/Utilities/DSN/simcuberpt.dsn",
                        ";UID=simcuberpt;PWD=Madness14;", sep="")

    con  <- odbcDriverConnect(con.string)
    if (con == -1) {
        rLog("Cannot connect to DB.  Exiting ...")
        return(NULL)
    }

    data <- sqlQuery(con, query)

    odbcCloseAll()
    rLog("Done")

    data$month <- as.Date(as.character(data$ContractMonthId), "%Y%m%d")
    data$ContractMonthId <- NULL

    ## make a proper bucket column
    data <- .fix_simcube_buckets( data )
    colnames(data) <- tolower(colnames(data))

    data
}

#######################################################################
# Get positions from simcube aggregated by deliveryPoint, bucket, month.
#
# @param region a vector with names of the regions requested,
#    e.g c("New England", "ERCOT").  If NULL, return all
# @param glt.  If NULL, will get all Generation, Load, Transaction
# @param tradeType.  On the glt="Generation", tradeType="RATABLE GEN" shows you
#    what is there for you to move to PM.    You also have "MANAGED GEN" and "N/A"
# @param bucket=c("Peak", "OffPeak", "Weekend" ) TODO -- not sure if needed
#
get_positions_simcube <- function(asOfDate, region=NULL,
  glt=NULL, books=NULL, tradeType=NULL)
{
  and <- NULL

  if (asOfDate >= Sys.Date())
    stop("You're funny!  You want positions from ", format(asOfDate), "?")

  if ( !is.null(glt) ) {
    aux <- paste(toupper(glt), sep="", collapse="', '")
    aux <- paste(" and s.glt in ('", aux, "')", sep="")
    and <- paste(and, aux, sep="")
  }

  if ( !is.null(tradeType) ) {
    aux <- paste(toupper(tradeType), sep="", collapse="', '")
    aux <- paste(" and s.TradeType in ('", aux, "')", sep="")
    and <- paste(and, aux, sep="")
  }

  if ( !is.null(region) ) {
    aux   <- paste(toupper(region), sep="", collapse="', '")
    aux   <- paste(" and s.RegionBU in ('", aux, "')", sep="")
    and <- paste(and, aux, sep="")
  }

  if ( !is.null(books) ) {
    aux   <- paste(toupper(books), sep="", collapse="', '")
    aux   <- paste(" and s.Book in ('", aux, "')", sep="")
    and <- paste(and, aux, sep="")
  }


  query <- paste(
    "select s.glt, s.gltType, s.regionBU, ",
    "sum(s.MWH) EXPECTED_MWH, sum(s.MMBTU ) EXPECTED_MMBTU, sum(s.deltaMWH) PDS_DELTA, ",
    "sum(s.DeltaMMBTU) Delta_mmbtu, ",
    "s.contractMonthId, s.deliveryPoint, s.TimeBucket, s.FuelType, s.Zone, s.TradeType, adjustedName ",
    "from ReportPosition.dbo.rnfatriskWithIdentifier s ",
    "where s.reportdateid='", format(asOfDate, "%Y%m%d"), "' ",

    and,   # add extra 'and' clauses

    " and s.TimeBucket in( 'Peak', 'OffPeak', 'Weekend' ) ",
    " and adjustedName='Adjusted' ",
    "group by s.glt, s.glttype, s.regionbu, s.contractMonthId, ",
    "adjustedName, s.deliveryPoint, s.TimeBucket, s.FuelType, s.Zone, s.TradeType",
    sep="")

  ## query <- paste("select * from ReportPosition.dbo.rnfatriskWithIdentifier s ",
  ##                "where s.reportdateid='", format(asOfDate, "%Y%m%d"), "' ",
  ##                "and s.book = 'FORERIVM'")


  rLog("Querying SimCube...")       # password changes every 30 days ...
  con.string <- paste("FileDSN=",
    "S:/All/Risk/Software/R/prod/Utilities/DSN/simcuberpt.dsn",
    ";UID=simcuberpt;PWD=Madness14;", sep="")

  con  <- odbcDriverConnect(con.string)
  if (con == -1) {
    rLog("Cannot connect to DB.  Exiting ...")
    return(NULL)
  }

  data <- sqlQuery(con, query)

  odbcCloseAll()
  rLog("Done")

  # make a month column
  data$month <- as.Date(as.character(data$contractMonthId), "%Y%m%d")
  data$contractMonthId <- NULL

  # make a proper bucket column
  data <- .fix_simcube_buckets( data )

  if ("Zone" %in% names(data))
    data$Zone <- toupper(data$Zone)

  if ("deliveryPoint" %in% names(data))
    data$deliveryPoint <- toupper(data$deliveryPoint)

  colnames(data) <- tolower(colnames(data))


  data
}



#######################################################################
# Get positions from simcube by trade, deliveryPoint, bucket, month.
#
# @param region a vector with names of the regions requested,
#    e.g c("New England", "ERCOT").  If NULL, return all
# @param glt.  If NULL, will get all Generation, Load, Transaction
# @param tradeType.  On the glt="Generation", tradeType="RATABLE GEN" shows you
#    what is there for you to move to PM.    You also have "MANAGED GEN" and "N/A"
# @param bucket=c("Peak", "OffPeak", "Weekend" ) TODO -- not sure if needed
#
get_positions_simcube_detail <- function(asOfDate, region=NULL,
  glt=NULL, books=NULL, tradeType=NULL)
{
  and <- NULL

  if (asOfDate >= Sys.Date())
    stop("You're funny!  You want positions from ", format(asOfDate), "?")

  if ( !is.null(glt) ) {
    aux <- paste(toupper(glt), sep="", collapse="', '")
    aux <- paste(" and s.glt in ('", aux, "')", sep="")
    and <- paste(and, aux, sep="")
  }

  if ( !is.null(tradeType) ) {
    aux <- paste(toupper(tradeType), sep="", collapse="', '")
    aux <- paste(" and s.TradeType in ('", aux, "')", sep="")
    and <- paste(and, aux, sep="")
  }

  if ( !is.null(region) ) {
    aux   <- paste(toupper(region), sep="", collapse="', '")
    aux   <- paste(" and s.RegionBU in ('", aux, "')", sep="")
    and <- paste(and, aux, sep="")
  }

  if ( !is.null(books) ) {
    aux   <- paste(toupper(books), sep="", collapse="', '")
    aux   <- paste(" and s.Book in ('", aux, "')", sep="")
    and <- paste(and, aux, sep="")
  }


  query <- paste(
    "select * ",
    "from ReportPosition.dbo.rnfatriskWithIdentifier s ",
    "where s.reportdateid='", format(asOfDate, "%Y%m%d"), "' ",
    and,   # add extra 'and' clauses
    sep="")

  ## query <- paste("select * from ReportPosition.dbo.rnfatriskWithIdentifier s ",
  ##                "where s.reportdateid='", format(asOfDate, "%Y%m%d"), "' ",
  ##                "and s.book = 'NEPGDMOD'")


  rLog("Querying SimCube...")       # password changes every 30 days ...
  con.string <- paste("FileDSN=",
    "S:/All/Risk/Software/R/prod/Utilities/DSN/simcuberpt.dsn",
    ";UID=simcuberpt;PWD=Madness14;", sep="")

  con  <- odbcDriverConnect(con.string)
  if (con == -1) {
    rLog("Cannot connect to DB.  Exiting ...")
    return(NULL)
  }

  data <- sqlQuery(con, query)

  odbcCloseAll()
  rLog("Done")

  # make a month column
  data$month <- as.Date(as.character(data$ContractMonthId), "%Y%m%d")
  data$ContractMonthId <- NULL

  # make a proper bucket column
  data <- .fix_simcube_buckets( data )

  if ("Zone" %in% names(data))
    data$Zone <- toupper(data$Zone)

  if ("deliveryPoint" %in% names(data))
    data$deliveryPoint <- toupper(data$deliveryPoint)

  colnames(data) <- tolower(colnames(data))


  data
}




#######################################################################
# Compare deltas from SecDb vs. simcube for a given book
# @param bookName the name of the book
# @param asOfDate an R Date
#
tieout_book <- function(asOfDate, bookName)
{
  require(SecDb)
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")

  asOfDate <- as.Date("2014-05-27")
  bookName <- "NEPGDMOD"
  #bookName <- "NECIBT"
  SC <- get_positions_simcube(asOfDate, books=bookName)
  head(SC)
  SC.pwr <- cast(SC, month + bucket + zone ~ ., sum, value="pds_delta",
       subset=pds_delta != 0)
  colnames(SC.pwr)[ncol(SC.pwr)] <- "value.pds"
  head(SC.pwr)

  SC.gas <- cast(SC, month + bucket + deliverypoint ~ ., sum, value="delta_mmbtu",
       subset=delta_mmbtu != 0)


  QQ <- .get_zonal_positions_SecDb(asOfDate, bookName)
  colnames(QQ)[which(colnames(QQ)=="location")] <- "zone"
  colnames(QQ)[which(colnames(QQ)=="location")] <- "value.secdb"

  res <- merge(QQ, SC.pwr, all.y=TRUE)


  # again tieout by eti, delivery point
  require(PM)
  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
  asOfDate <- as.Date("2014-06-02")
  bookName <- "NEPGDMOD"

  SC <- get_positions_simcube_detail(asOfDate, books=bookName)
  head(SC)
  SC2 <- subset(SC, month == as.Date("2014-09-01") & !is.na(deltamwh))
  sort(unique(SC2$pdscontract))
  SC2$eti <- gsub("-R$", "", SC2$pdscontract)
  head(SC2)
  sort(unique(SC2$eti))
  SC3 <- cast(SC2, deliverypoint ~ bucket, sum, value="deltamwh", fill=0,
              subset=bucket %in% c("5X16", "2X16H", "7X8") & glttype=="Generation")
  SC3

  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
  COG <- get.positions.from.blackbird( asOfDate=asOfDate, books=bookName)
  head(COG)
  COG2 <- subset(COG, contract.dt == as.Date("2014-09-01") & mkt == "PWX")
  sort(unique(COG2$eti))
  COG3 <- cast(COG2, delivery.point ~ bucket, sum, value="delta", fill=0)
  COG3

  # look at the hub trades
  hub.cog <- cast(COG2, eti + bucket ~ ., sum, fill=0, value="delta",
       subset=delivery.point %in% c("HUB", "HUB DA"))
  colnames(hub.cog)[3] <- "delta.cog"
  head(hub.cog)

  hub.sc <- cast(SC2, eti + bucket ~ ., sum, value="deltamwh", fill=0,
              subset=bucket %in% c("5X16", "2X16H", "7X8") &
                 glttype=="Generation" &
                 deliverypoint == "Hub")
  colnames(hub.sc)[3] <- "delta.simcube"
  head(hub.sc)

  res <- merge(hub.cog, hub.sc, all=TRUE)
  res$diff <- round(res$delta.simcube - res$delta.cog)







  COG <- .get_positions_delivpt(asOfDate=asOfDate, books=bookName)
  cog <- subset(COG, month == as.Date("2014-09-01"))
  cast(cog, location ~ bucket, I, fill=0)




  # again tieout by delivery point
  require(PM)
  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
  asOfDate <- as.Date("2014-05-21")
  bookName <- "NEPGDMOD"

  COG <- .get_positions_delivpt(asOfDate=asOfDate, books=bookName)
  cog <- subset(COG, month == as.Date("2014-09-01"))
  cast(cog, location ~ bucket, I, fill=0)










}



#######################################################################
#######################################################################
#
.test <- function()
{
  require(CEGbase)
  require(reshape)
  require(RODBC)
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.simcube.R")

  asOfDate <- as.Date("2014-06-02")
  region <- c( "New England" )
  qq <- get_positions_simcube(asOfDate, region=region)
  write.csv(qq, file=paste("c:/temp/simcube_nepool_", format(asOfDate), ".csv", sep=""),
    row.names=FALSE)
  x <- subset(qq, deliverypoint=="KLEEN.14614")



  # get everything in a region
  asOfDate <- as.Date("2013-12-05")
  region <- c("PJM East", "PJM West") # "New England"
  qq <- get_positions_simcube(asOfDate, region=region)
  write.csv(qq, file=paste("c:/temp/simcube_pjm_", format(asOfDate), ".csv", sep=""),
    row.names=FALSE)


  # use the books
  asOfDate <- as.Date("2013-12-05")
  books <- c("PJEGNRAP",    # corporate position
             "PJEBBRAP")    # PM position
  qq <- get_positions_simcube(asOfDate, books=books)


  # do a tieout
  asOfDate <- as.Date("2014-05-19")
  bookName <- "NEPGDMOD"

  ##fetch PDS Gen extrinsic
  data <- get_Intr_vs_Extr_By_Book(as.Date("2014-12-03"), books="NPKLEEN")
###  head(data)
}




  ## query <- paste(
  ##   "select sum(s.deltaMWH) PDS_DELTA, s.glt, s.gltType, s.regionBU, ",
  ##   "sum(s.MWH) EXPECTED_MWH, sum(s.MMBTU ) EXPECTED_MMBTU, ",
  ##   "s.contractMonthId,s.deliveryPoint,s.TimeBucket,s.FuelType, s.Zone ,adjustedName ",
  ##   "from ReportPosition.dbo.rnfatrisk s ",
  ##   "where s.reportdateid='20131021' ",
  ##   "and s.RegionBU='New England' ",
  ##   "and s.glt='Generation' ",
  ##   "and s.TimeBucket in( 'Peak','OffPeak', 'Weekend' ) ",
  ##   "and adjustedName='Adjusted' ",
  ##   "and s.FuelType not in ( 'Environmental') ",
  ##   "group by s.glt,s.glttype,s.regionbu,s.contractMonthId, ",
  ##   "adjustedName,s.deliveryPoint,s.TimeBucket,s.FuelType,s.Zone",
  ##   sep="")


   # " and s.FuelType not in ( 'Environmental') ",

  ## query <- paste("select sum(s.deltaMWH) PDS_DELTA, s.glt, s.gltType, s.regionBU,",
  ##    "sum(s.MWH) EXPECTED_MWH, sum(s.MMBTU ) EXPECTED_MMBTU, s.TradeType, ",
  ##    "s.contractMonthId,s.deliveryPoint,s.TimeBucket,s.FuelType, s.Zone ,adjustedName ",
  ##    " from ReportPosition.dbo.RnfAtRiskWithIdentifier s  ",
  ##    "where s.reportdateid='20140131' ",
  ##    "and s.RegionBU='PJM East' ",
  ##    #"and s.TradeType = 'RATABLE GEN' ",
  ##    "and s.TimeBucket in( 'Peak','OffPeak', 'Weekend' ) ",
  ##    "and adjustedName='Adjusted' ",
  ##    "and s.FuelType not in ( 'Environmental') ",
  ##    "group by s.glt,s.glttype,s.regionbu,s.contractMonthId, ",
  ##    "adjustedName,s.deliveryPoint,s.TimeBucket,s.FuelType,s.Zone,s.TradeType", sep="")
