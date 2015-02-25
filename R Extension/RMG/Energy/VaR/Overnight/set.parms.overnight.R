# Define general settings for the overnight VaR run
#
#
# Written by Adrian Dragulescu on 27-Nov-2006

set.parms.overnight <- function(options){

  source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
  package.names <- c("reshape", "RODBC", "xtable")
  load.packages(package.names)

  if (length(options$calc$no.hdays)==0){options$calc$no.hdays <- 70}
  if (length(options$calc$holding.period.days)==0){
    options$calc$holding.period.days <- 1}
  if (length(options$run$is.overnight)==0){options$run$is.overnight <- TRUE}
  options$run$useLivePositions <- FALSE
  if (length(options$run$force.recalc)==0){options$run$force.recalc <- FALSE}
  
  if (length(options$keep.log)==0){options$keep.log <- FALSE}
  if (length(options$environment)==0){
    options$environment <- list(data="prod", code="prod", reports="prod")
  }  # can be "dev" too.
  
  options$source.dir         <- "H:/user/R/RMG/Energy/VaR/"
  options$save$datastore.dir <- paste("//ceg/cershare/All/Risk/Data/VaR/",
    options$environment$data, "/", sep="")
  options$save$prices.mkt.dir <- paste(options$save$datastore.dir, "Prices/Market/", sep="")
  options$save$vols.mkt.dir   <- paste(options$save$datastore.dir, "Vols/Market/", sep="")
  options$save$reports.dir    <- paste("//ceg/cershare/All/Risk/Reports/VaR/",
    options$environment$reports, "/", sep="")
  options$save$do.IVaR.file   <- 1
  options$calc$do.CVaR <- TRUE
  options$calc$do.incremental.VaR <- TRUE
  options$calc$do.corr.sensitivity <- TRUE
  options$calc$fill.marks <- TRUE
  
  if (length(options$asOfDate)==0){options$asOfDate <- Sys.Date()-1}

  #-------------------------------------------Source the files-----------------
##   files <- dir(paste(options$source.dir,"Overnight/",sep=""), "\\.[rR]$", full.name=T)
##   files <- files[-grep("set.parms.overnight.R", files)]   # remove infinite loop
##   lapply(files, source, max.deparse.length=1)  # source it all
  source(paste(options$source.dir,"Overnight/get.spot.fx.rates.R",sep=""))
  source(paste(options$source.dir,"Overnight/get.curve.info.R",sep=""))
  source(paste(options$source.dir,"Overnight/get.historical.prices.R",sep=""))
  source(paste(options$source.dir,"Overnight/get.price.buddy.curve.R",sep=""))
  source(paste(options$source.dir,"Overnight/get.vol.buddy.curve.R",sep=""))
  source(paste(options$source.dir,"Overnight/fill.mkt.files.R",sep=""))
  source(paste(options$source.dir,"Overnight/fill.one.ts.curve.R",sep=""))
  source(paste(options$source.dir,"Overnight/get.corrected.marks.R",sep=""))
  source(paste(options$source.dir,"Overnight/make.mkt.files.R",sep=""))   
  
  path.dir <- paste(options$source.dir,"Base/",sep="")
  lapply(dir(path.dir, "\\.[rR]$", full.name=T), source)  # source it all

  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.vcv.vcentral.R")
  source("H:/user/R/RMG/Utilities/write.xls.R")
  source("H:/user/R/RMG/Utilities/mkdir.R")

  con.string <- paste("FileDSN=H:/user/R/RMG/Utilities/DSN/CPSPROD.dsn",  
       ";UID=stratdatread;PWD=stratdatread;", sep="")  
  options$connections$CPSPROD <- odbcDriverConnect(con.string)
  con.string <- paste("FileDSN=H:/user/R/RMG/Utilities/DSN/VCTRLP.dsn",  
       ";UID=vcentral_read;PWD=vcentral_read;", sep="")  
  options$connections$VCTRLP <- odbcDriverConnect(con.string)
  con.string <- paste("FileDSN=H:/user/R/RMG/Utilities/DSN/RMSYSP.dsn",  
       ";UID=rmsys_read;PWD=rmsys_read;", sep="")  
  options$connections$RMSYSP <- odbcDriverConnect(con.string)
  #------------------------------------------Find the days this run happens----
  query <- paste("select distinct calendar_date, holiday from ",
    "foitsox.sdb_holiday_calendar where calendar = 'CPS-BAL' ",
    "and calendar_date <= pricing_date + 15 and pricing_date < to_date('",
    options$asOfDate,"', 'yyyy-mm-dd')  ", sep="")
  options$calendar <- sqlQuery(options$connections$CPSPROD, query)  
  options$calendar$CALENDAR_DATE <- as.Date(options$calendar$CALENDAR_DATE)

  aux <- subset(options$calendar, HOLIDAY=='N' & CALENDAR_DATE <= options$asOfDate)
  aux <- aux[order(aux$CALENDAR_DATE, decreasing=T), "CALENDAR_DATE"]
  if (options$asOfDate==Sys.Date()){
    options$asOfDate <- as.Date(aux[1])  # get the last date
  }
  options$calc$hdays <- sort(aux[1:(options$calc$no.hdays+1)])
  options$calc$previous.day <- aux[2]
  
  options$save$overnight.dir <- paste(options$save$reports.dir, options$asOfDate, sep="")
  if (file.access(options$save$overnight.dir)!=0){
    mkdir(options$save$overnight.dir)}

  eom <- seq(as.Date(format(options$asOfDate, "%Y-%m-01")), by="month",
             length.out=2)[2]-1
  aux <- subset(options$calendar, HOLIDAY=='N' & CALENDAR_DATE <= eom)
  options$run$bidweek <- aux$CALENDAR_DATE[(nrow(aux)-4):nrow(aux)]
  options$run$is.bidweek.day <- ifelse(options$asOfDate %in% options$run$bidweek, 
    TRUE, FALSE)
  
  options$save$fx.file <- paste(options$save$datastore.dir, "fx.",
    options$asOfDate, ".RData", sep="")
  options$save$curve.info <- paste(options$save$datastore.dir,
    "curve.info.", options$asOfDate, ".RData", sep="")  

  #---------------------------------email settings-----------------------
  options$email$send <- TRUE
  toList <- c("john.scillieri", "adrian.dragulescu", "michael.zhang", "peng.wu" )
  options$email$show.status.list <- paste(paste(toList, "@constellation.com",
                                               sep=""), collapse=", ")  
  toList <- c("joe.deluzio", toList)
  options$email$on.done.list <- paste(paste(toList, "@constellation.com",
                                               sep=""), collapse=", ")
  
  #---------------------------------save the nightly options-------------
  filename <- paste(options$save$datastore.dir, "options.overnight.",
    options$asOfDate, ".RData", sep="")
  options.overnight <- options
  if (options$run$is.overnight){
    cat("Wrote ovenight options to", filename, "\n")
    save(options.overnight, file=filename)
  }

  return(options)
}


