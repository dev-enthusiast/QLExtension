# Define general settings for the overnight VaR run
#
#
# Written by Adrian Dragulescu on 27-Nov-2006

set.parms.overnight <- function(options){

  require(RODBC); require(reshape); require(xtable)

  options$calc$no.hdays <- 70
  options$run$is.overnight <- TRUE
  options$run$useLivePositions <- FALSE
  if (options$keep.log){options$keep.log <- TRUE}
  
  options$source.dir         <- "H:/user/R/RMG/Energy/VaR/rmsys/"
  options$save$datastore.dir <- "S:/All/Risk/Data/VaR/"
  options$save$reports.dir   <- "S:/All/Risk/Data/VaR/Reports/"
  options$save$do.IVaR.file  <- 1 
  
  if (length(options$asOfDate)==0){options$asOfDate <- Sys.Date()-1}
  con.string <- paste("FileDSN=H:/user/R/RMG/Utilities/DSN/CPSPROD.dsn",  
       ";UID=stratdatread;PWD=stratdatread;", sep="")  
  options$connections$CPSPROD <- odbcDriverConnect(con.string)
  con.string <- paste("FileDSN=H:/user/R/RMG/Utilities/DSN/VCTRLP.dsn",  
       ";UID=vcentral_read;PWD=vcentral_read;", sep="")  
  options$connections$VCTRLP <- odbcDriverConnect(con.string)
  con.string <- paste("FileDSN=H:/user/R/RMG/Utilities/DSN/RMSYSP.dsn",  
       ";UID=rmsys_read;PWD=rmsys_read;", sep="")  
  options$connections$RMSYSP <- odbcDriverConnect(con.string)
 
  source(paste(options$source.dir,"Overnight/prepare.overnight.VaR.R",sep=""))
  source(paste(options$source.dir,"Overnight/get.VCentral.positions.R",sep=""))
  source(paste(options$source.dir,"Overnight/get.RMSys.positions.R",sep=""))
  source(paste(options$source.dir,"Overnight/aggregate.all.market.states.R",sep=""))
  source(paste(options$source.dir,"Overnight/get.historical.prices.R",sep=""))
  source(paste(options$source.dir,"Overnight/get.corrected.marks.R",sep=""))
  
  source(paste(options$source.dir,"main.run.VaR.R",sep=""))
  source(paste(options$source.dir,"adjust.bidweek.R",sep=""))
  source(paste(options$source.dir,"make.class.Factors.R",sep=""))
  source(paste(options$source.dir,"utilities.VaR.R",sep=""))
  source(paste(options$source.dir,"core.calc.VaR.R",sep=""))
  source(paste(options$source.dir,"make.VaR.report.R",sep=""))
  source(paste(options$source.dir,"make.pdf.R",sep=""))

  source("H:/user/R/RMG/Utilities/write.xls.R")

  #------------------------------------------Find the days this run happens----
  query <- paste("select pricing_date, calendar_date, holiday from ",
    "foitsox.sdb_holiday_calendar where calendar = 'CPS-BAL' ",
    "and calendar_date >= to_date('", options$asOfDate-400,"', 'yyyy-mm-dd') ", 
    "and calendar_date <= pricing_date + 33 and pricing_date in ( ",
    "select max(pricing_date) from foitsox.sdb_holiday_calendar)", sep="")
  options$calendar <- sqlQuery(options$connections$CPSPROD, query)  
  options$calendar$CALENDAR_DATE <- as.Date(options$calendar$CALENDAR_DATE)
  is.holiday <- subset(options$calendar, CALENDAR_DATE == options$asOfDate)
##   if (is.holiday$HOLIDAY == "Y"){    # weekends are holidays too
##     cat("It's a holiday for CPS-BAL.  Exiting... ")
##     quit(save="no")
##   }
  aux <- subset(options$calendar, HOLIDAY=='N'&CALENDAR_DATE <= options$asOfDate)
  aux <- aux[order(aux$CALENDAR_DATE, decreasing=T), "CALENDAR_DATE"]
  if (options$asOfDate==Sys.Date()){
    options$asOfDate <- as.Date(options$calendar$PRICING_DATE[1])  # get the last date
    aux <- aux[-1]
  }
  options$calc$hdays <- sort(aux[1:(options$calc$no.hdays+1)])
  options$calc$previous.day <- aux[2]
  
  options$save$overnight.dir <- paste(options$save$reports.dir, options$asOfDate, sep="")
  if (file.access(options$save$overnight.dir)!=0){
    dir.create(options$save$overnight.dir, recursive=T)}
  if (options$keep.log){   # start dumping output to the log file ...
    options$run$log.file <- paste(options$save$overnight.dir, "/log.txt", sep="")
    sink(options$run$log.file)
  }
  eom <- seq(as.Date(format(options$asOfDate, "%Y-%m-01")), by="month",
             length.out=2)[2]-1
  aux <- subset(options$calendar, HOLIDAY=='N' & CALENDAR_DATE <= eom)
  options$run$bidweek <- aux$CALENDAR_DATE[(nrow(aux)-4):nrow(aux)]
  options$run$is.bidweek.day <- ifelse(options$asOfDate %in% options$run$bidweek, 
    TRUE, FALSE)
  
  options$save$all.mktData.file <- paste(options$save$datastore.dir,
    "all.mktData.", options$asOfDate, ".RData", sep="")
  options$save$filled.mktData.file <- paste(options$save$datastore.dir,
    "filled.mktData.", options$asOfDate, ".RData", sep="")
  options$save$all.positions.file <- paste(options$save$datastore.dir,
    "all.positions.", options$asOfDate, ".RData", sep="")
  options$save$pHier.file <- paste(options$save$datastore.dir, "pHier.",
    options$asOfDate, ".RData", sep="")
  options$save$curve.info <- paste(options$save$datastore.dir,
    "curve.info.", options$asOfDate, ".RData", sep="")

  #---------------------------------save the nightly options-------------
  filename <- paste(options$save$datastore.dir, "options.overnight.",
    options$asOfDate, ".RData", sep="")
  options.overnight <- options
  save(options.overnight, file=filename)

  return(options)
}


