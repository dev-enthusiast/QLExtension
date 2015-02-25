#-------------------------------------------------------------------------------
# External Libraries
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/write.xls.R")
source("H:/user/R/RMG/Energy/VaR/PE/PE.R")

#-------------------------------------------------------------------------------
# File Namespace
PEUtils = new.env(hash=TRUE)

#-------------------------------------------------------------------------------
# This function create a file of all the positions by counterparty
PEUtils$setParmsRun <- function( run.name )
{
  rLog("Get classifications/books for the run.name ...")
  run <- classif <- NULL
  if (length(run$classification)==0){run$classification <- NULL}

  run.name <- toupper(run.name)
  filename <- paste(PE$SRC_DIR, "schedule.overnight.PE.xls", sep="")
  con  <- odbcConnectExcel(filename)
  all.runs     <- sqlFetch(con, "PE_RUN_OVERNIGHT")
  pe.synthetic <- sqlFetch(con, "PE_SYNTHETIC_RUNS")
  odbcCloseAll()

  all.runs$RUN_NAME <- toupper(as.character(all.runs$RUN_NAME))
  all.runs <- subset(all.runs, RUN_NAME == run.name)
  pe.synthetic$RUN_NAME <- toupper(as.character(pe.synthetic$RUN_NAME))
  this.run <- merge(all.runs, pe.synthetic, all.x=TRUE)
  names(this.run) <- gsub("#", ".", names(this.run))
  
  if (nrow(this.run)==0){
    stop("Cannot find runName in the schedule.overnight.PE.xls.  Exiting.")
  }

  run$run.name <- run.name
  if (!is.null(run$classification)){
    classif <- as.character(as.matrix(this.run[, grep("CLASS", names(this.run))]))
    classif <- tolower(class[!is.na(class)])
  }
  run$classification <- c(run$classification, classif)
  run$filters <- as.character(na.omit(as.character(as.matrix(
    this.run[, grep("FILTER", names(this.run))]))))
  run$specification  <- this.run
  run$run.type <- toupper(as.character(run$specification$RUN_TYPE[1]))

  if (run$run.type == "COUNTERPARTY"){
    run$specification$COUNTERPARTY <- run$specification$RUN_NAME
  }
  
  rLog("Done.")
  return(run)
}

#-------------------------------------------------------------------------------
# This function create a file of all the positions by counterparty
PEUtils$createCounterpartyPositionFile <- function(asOfDate = NULL)
{
  rLog("Creating Position and Price File...")
  
  PE$loadPEOptions()    
  
  datastoreDir = PE$options$save$datastore.dir
 
  if( is.null(asOfDate) )
  {
    asOfDate = PE$options$asOfDate
  }
  
  allPositions = PEUtils$.aggregatePositionsByCounterparty( asOfDate, 
    datastoreDir )
  
  allPositions = PEUtils$.appendCurrentForwardPrice( allPositions, asOfDate,
    datastoreDir )
  
  save(allPositions, file=PE$COUNTERPARTY_POSITION_FILE)
  
  rLog("Counterparty Position & Price File Created.\n")
}

#-------------------------------------------------------------------------------
# QQ is a data frame that contains the cparty column. 
PEUtils$getPositionsByICR <- function( asOfDate, QQ )
{
  rLog("Get ICR by counterparty ... ")
  con.string <- paste("FileDSN=H:/user/R/RMG/Utilities/DSN/RMGDBP.dsn",  
       ";UID=rmgdbuser;PWD=rmgdbpwd_prod;", sep="")  
  con <- odbcDriverConnect(con.string)
  query <- paste("select counterparty, icr", 
    " from counterparty_icr where dataasofdate = to_date('",
    asOfDate, "', 'yyyy-mm-dd')  ", sep="")
  ICR <- sqlQuery(con, query)
  names(ICR) <- c("counterparty", "ICR")
  ICR$counterparty <- as.character(ICR$counterparty)
  ICR$ICR    <- sub("^ *", "", as.character(ICR$ICR))   # stupid table!
  rLog("Done.")
  odbcCloseAll()

  return(merge(QQ, ICR, all.x=T))
}

#-------------------------------------------------------------------------------
# This function loads the current position list for a given run.name as 
# specified in the schedule.overnight.PE.xls file.
PEUtils$getPositionsForRunName <- function( asOfDate, run )
{
  rLog(paste("Get positions for run name = ", run$run.name, sep=""))
  #------------------------------------------------------------------------------
  if (toupper(run$run.type) %in% c("COUNTERPARTY", "SYNTHETIC_COUNTERPARTY") ){
    QQ <- PEUtils$getPositionsForCounterparty( as.character(run$specification$COUNTERPARTY) )
    
  }
  #------------------------------------------------------------------------------
  if (identical(toupper(run$run.type), "PORTFOLIO")){
    vcv.file <- paste(PE$VCV_POSITION_DIR, "vcv.positions.",
      asOfDate, ".RData", sep="")
    load(vcv.file)
    books <- secdb.getValueType(run$specification$PORTFOLIO, "Nodes")$Book
    PB    <- data.frame(BOOK=names(books), weight=as.numeric(books))

    QQ <- merge(QQ.all[,c("BOOK", "CURVE_NAME", "CONTRACT_MONTH", "DELTA",
                          "COUNTERPARTY")], PB, all.y=TRUE)
    colnames(QQ) <- gsub("_", ".", tolower(colnames(QQ)))
    colnames(QQ)[3:4] <- c("contract.dt", "position")
    QQ$contract.dt  <- as.Date(QQ$contract.dt) 

    QQ <- PEUtils$.appendCurrentForwardPrice( QQ, asOfDate)
  }

  ind <- grep("ICR", run$filters)
  if (length(ind)>0){
    QQ <- PEUtils$getPositionsByICR( asOfDate, QQ )
    QQ <- eval(parse(text=paste("subset(QQ,", run$filters[ind], ")", sep="")))
  }

  if (nrow(QQ)==0) stop("No position data.  Exiting.")
  
  QQ <- aggregate(QQ[,c("position", "F0")], list(curve.name=QQ$curve.name,
                                            contract.dt=QQ$contract.dt), sum)

  rLog("Done.")
  return( QQ)
}

#-------------------------------------------------------------------------------
# Move the positions from cash month to prompt month.
PEUtils$moveCashPositions <- function( QQ )
{
  cash.month <- as.Date(format(PE$options$asOfDate, "%Y-%m-01"))
  prompt.month <- seq(cash.month, length.out=2, by="month")[2]

  if (any(QQ$contract.dt==cash.month))
  {
    rLog("Moving the cash positions to the prompt month ...")
    agg.QQ <- QQ[, c("curve.name", "contract.dt", "position")]
    agg.QQ$contract.dt <- ifelse(agg.QQ$contract.dt==cash.month, prompt.month,
                                 agg.QQ$contract.dt)
    class(agg.QQ$contract.dt) <- "Date"
    agg.QQ <- aggregate(agg.QQ[,"position"], list(curve.name=agg.QQ$curve.name,
                        contract.dt=agg.QQ$contract.dt), sum)
    names(agg.QQ)[3] <- "position"   # not sure why the name isn't preserved

    # need to append F0 from the overnight file, cannot use QQ
    agg.QQ <- PEUtils$.appendCurrentForwardPrice( agg.QQ, PE$options$asOfDate,
      PE$options$save$datastore.dir)
    
    QQ <- agg.QQ
    rLog("Done.")
  }

  return( QQ )
}

#-------------------------------------------------------------------------------
# This function loads the current position list for a given counterparty
PEUtils$getPositionsForCounterparty <- function( counterpartyName )
{
  if( !file.exists(PE$COUNTERPARTY_POSITION_FILE) )
  {
    PEUtils$createCounterpartyPositionFile()
  } else
  {
    load(PE$COUNTERPARTY_POSITION_FILE)
  }
  
  return( subset(allPositions, cparty %in% toupper(counterpartyName)) )
}

#-------------------------------------------------------------------------------
# Get pricing dates for the PE calculation.  Go monthly to end of the 3rd
# year, quarterly for the next two years, calendar after that.
PEUtils$getPricingDates <- function(asOfDate, maxDate)
{
  bom       <- as.Date(format(asOfDate, "%Y-%m-01"))  # beginning of the month
  boy       <- as.Date(format(asOfDate, "%Y-%01-01"))  # beginning of the year
  start.qtr <- seq(boy, by="year", length.out=4)[4]
  start.cal <- seq(boy, by="year", length.out=6)[6]
  pricing.dts.qtr <- pricing.dts.cal <- NULL

  next.mo <- seq(bom, by="month", length.out=2)[2]
  pricing.dts.mo <- seq(next.mo, start.qtr, by="month")
  pricing.dts.mo <- pricing.dts.mo[-length(pricing.dts.mo)]

  # go by quarter after the first 3 years, quarterly for the next 2
  if (maxDate >= start.qtr){
    pricing.dts.qtr <- seq(start.qtr, by="3 months", length.out=8)
    pricing.dts.qtr <- pricing.dts.qtr[pricing.dts.qtr <= maxDate]    
  }
  # go by year after the first 5 years
  if (maxDate >= start.cal){
    pricing.dts.cal <- seq(start.cal, maxDate, by="year")
    pricing.dts.cal <- pricing.dts.cal[pricing.dts.cal <= maxDate]    
  }
  return(c(pricing.dts.mo, pricing.dts.qtr, pricing.dts.cal))
}

#-------------------------------------------------------------------------------
# Write the direct MPE (dMPE) and the (inverse) iMPE to a results xls.
PEUtils$write.xls <- function( dMPE, iMPE, QQ, outputDir )
{
  runName  <- strsplit(outputDir, "/")[[1]]
  runName  <- runName[length(runName)]
  file <- paste(outputDir, "results.", runName, ".xls", sep="")
  
  DD <- data.frame(round(t(dMPE), 2))
  names(DD) <- gsub("\\.", "", gsub("X", "p", names(DD)))
  DD <- cbind(contract.dt=colnames(dMPE), DD)  
  write.xls(DD, file, sheetname="direct")
  
  DD <- data.frame(round(t(iMPE), 2))
  names(DD) <- gsub("\\.", "", gsub("X", "p", names(DD)))
  DD <- cbind(contract.dt=colnames(dMPE), DD)
  write.xls(DD, file, sheetname="indirect")

  write.xls(QQ, file, sheetname="positions")  

  rLog("Wrote the results.xls file.")
}

#-------------------------------------------------------------------------------
PEUtils$GetInitialExposures <- function( AsOfDate=Sys.Date()-1 )
{
    library(RODBC)
    conString <- paste('DRIVER=SQL Server;',
                       'DATABASE=ENERGYCREDITMANUAL;', 'SERVER=CECDBP-MSW-01;',
                       'APP=Microsoft Data Access Components;',
                       'UID=ExecSPOnly;', 'PWD=EnergyCredit_ExecSPOnly;', sep='')
    Conn      <- odbcDriverConnect(conString)

    AvailableDates <- sqlQuery(Conn, 'RptFreightTrade')
    ColName        <- names(AvailableDates)
    Idx            <- which(as.Date(AvailableDates[[ColName]])==AsOfDate)
    if( length(Idx)<1 )
        stop(paste('Initial exposures as of',format(AsOfDate,'%Y-%m-%d'),'not available in RAFT.ENERGYCREDITMANUAL'))

    InitialExposures <- sqlQuery(Conn, paste('{call RptFreightTrade (\'',format(AsOfDate,'%Y-%m-%d'),'\')}',sep=''))

    close(Conn)
    
    return( InitialExposures )
}

########################### INTERNAL OBJECTS ###################################


#-------------------------------------------------------------------------------
# Aggregate the positions by counterparty.  Depends on the VCentral job pull.
#
# Written by Adrian Dragulescu on 31-May-2007
PEUtils$.aggregatePositionsByCounterparty <- function(asOfDate, datastoreDir)
{

  filename <- paste(datastoreDir, "Positions/vcv.positions.", 
    asOfDate, ".RData", sep="")
  load(filename)
  QQ.all <- QQ.all[, c("CURVE_NAME", "CONTRACT_MONTH", "DELTA", "COUNTERPARTY")]
  names(QQ.all) <- c("curve.name", "contract.dt", "value", "cparty")
  QQ.all$contract.dt <- as.Date(QQ.all$contract.dt)
  QQ.all$contract.dt <- as.Date(format(QQ.all$contract.dt, "%Y-%m-01"))
  QQ.all$curve.name  <- toupper(as.character(QQ.all$curve.name))

  QQ.all <- cast(QQ.all, cparty + curve.name + contract.dt ~ ., sum)
  names(QQ.all)[4] <- "position"
  QQ.all <- QQ.all[-which(QQ.all$position==0),]
  QQ.all <- data.frame(QQ.all)
  QQ.all$cparty <- as.character(QQ.all$cparty)
  QQ.all$contract.dt <- as.Date(QQ.all$contract.dt)
  QQ.all <- na.omit(QQ.all)   # there are some positions without cparty?!

#  QQ.all <- list(linear=QQ.all)
  return(QQ.all)
}

#-------------------------------------------------------------------------------
PEUtils$.appendCurrentForwardPrice <- function(risk.factors, asOfDate, 
  datastoreDir="S:/All/Risk/Data/VaR/prod/" )
{
  F0 <- unique(risk.factors[, c("curve.name", "contract.dt")])
  F0 <- F0[order(risk.factors$curve.name, risk.factors$contract.dt),]
  F0$curve.name  <- as.character(F0$curve.name)
  F0$contract.dt <- as.Date(F0$contract.dt)
  
  filename <- paste(datastoreDir, "Prices/hPrices.", asOfDate, ".RData", sep="")
  load(filename)
  
  F0 <- merge(F0, hP, all.x=T)
  names(F0)[3] <- "F0"

  risk.factors <- merge(risk.factors, F0, all.x=T)

  return(risk.factors)
}

#-------------------------------------------------------------------------------
#  dir.create has some issues with network drive (on R 2.5.0)
PEUtils$.mkdir <- function(dir){
  shell(paste("mkdir", dir), translate=TRUE, invisible=TRUE)
}

#-------------------------------------------------------------------------------
# make position files for a (interactive) run given a set of books, portfolios,
# etc.  Use the same setup as the overnight VaR.  Check demo.main.run.VaR for an 
# example of the list "run".
# For example: PE$options$run.name <- "JKIRKPATRICK TRADING PORTFOLIO"

PEUtils$.makePositionsFromRunName <- function(PE){
  
  source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
  run.all <- get.portfolio.book(PE$options)
  run <- NULL
  run$specification <- subset(run.all, RUN_NAME == toupper(PE$options$run.name))

  source("H:/user/R/RMG/Energy/VaR/Base/aggregate.run.positions.R")
  QQ <- aggregate.run.positions(run, PE$options)
  QQ <- subset(QQ, vol.type=="NA"); QQ$vol.type <- NULL

  options <- NULL 
  options$calc$hdays <- PE$options$asOfDate

  source("H:/user/R/RMG/Utilities/Database/R/get.marks.from.R.R")
  PP <- get.prices.from.R(QQ[,c("curve.name", "contract.dt")], options)
  
  QQ <- merge(QQ, PP)
  names(QQ)[4] <- "F0"
  
  return(QQ)
}