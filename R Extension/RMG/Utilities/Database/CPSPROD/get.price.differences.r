#  This function returns price differences based on specified curves and contract months
#
#       It can extract prices directly from CPSPROD and calcualte price differences, or 
#       extract prices from nightly archive and calculate prices differences. If nightly
#       archive does not exist, it will extract prices from CPSPROD and then calculate
#       price differences.
#
#
# options=NULL
# options$asOfDate   <- as.Date("2007-03-16")   
# options$curve.names <- c("COMMOD NG Exchange",
#                          "COMMOD PWJ 5x16 WEST Physical")
# options$number.of.days <- 71
# options$contract.dt.start <- as.Date("2007-05-01")
# options$contract.dt.end   <- as.Date("2007-12-01")
# options$source.dir <- "H:/user/R/RMG/"
# options$extraction.type <- 2    # 1 direct database extraction, 2 extraction from archive
# options$file.dsn.path <-  "H:/user/R/RMG/Utilities/DSN/"
# options$file.dsn <- "CPSPROD.dsn"
#
# source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.price.differences.R")
# res <- get.price.differences(options)

get.price.differences <- function(options) {

    if (length(options$asOfDate)==0){options$asOfDate <- Sys.Date()-1}
    if (length(options$number.of.days)==0){options$number.of.days <- 71}   # Make 70 day price changes
    if (length(options$source.dir)==0){options$source.dir <- "H:/user/R/RMG/"}
    if (length(options$file.dsn)==0){options$file.dsn <- "CPSPROD.dsn"}
    if (length(options$file.dsn.path)==0){options$file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"}
    if (length(options$extraction.type)==0){options$extraction.type <- 2}  # Load price from archive
  
    source(paste(options$source.dir,"Utilities/Database/CPSPROD/read.prices.from.CPSPROD.R",sep=""))
    source(paste(options$source.dir,"Utilities/Database/CPSPROD/get.holiday.calendar.r",sep=""))
    source(paste(options$source.dir,"Utilities/Packages/load.packages.r",sep=""))
    source(paste(options$source.dir,"Energy/VaR/rmsys/Overnight/get.historical.prices.R",sep="")) 

    # Make sure curve names are in upper case
    options$curve.names <- toupper(options$curve.names)
    
    package.names <- c("reshape")
    load.packages(package.names, options)
    
    # Get 71 business days
    cal <- get.holiday.calendar(options)
    
    if (options$extraction.type == 1) {       # Load prices directly from CPSPROD
        options$pricing.dt.start  <- cal$start.date
        options$pricing.dt.end    <- cal$end.date
        prices <- read.prices.from.CPSPROD(options)
        
        colnames(prices) <- c("pricing_date", "curve_name", "contract_date", "value")
    
        IVaR.Prices <-  cast(prices, curve_name + contract_date ~ pricing_date)
        
        indDates.IVaR  <- grep("X",colnames(IVaR.Prices))
        cnames <- colnames(IVaR.Prices)[indDates.IVaR]
        
        colnames(IVaR.Prices) <- 
           c("curve.name","contract.dt", as.character(as.Date(gsub("X", "", cnames), format("%Y.%m.%d"))))
           
        IVaR.Changes <- IVaR.Prices

    }
    else {     # Load prices from archive. 
        options$calc$hdays <-  cal$calendar
        options$save$datastore.dir <- "S:/All/Risk/Data/VaR/"

        cat("Reading in historical prices ... \n")
        prices <- NULL                         
        for (d in length(cal$calendar):1){
          cat(paste("\t Processing day", cal$calendar[d], " ... "))
          file <- paste(options$save$datastore.dir,"hPrices.",
                        cal$calendar[d],".RData", sep="")
          if (!file.exists(file)){
              connection.string <- paste("FileDSN=", options$file.dsn.path, options$file.dsn, 
                                          ";UID=stratdatread;PWD=stratdatread;", sep="")
              options$connections$CPSPROD <- odbcDriverConnect(connection.string)
              get.one.day.prices(cal$calendar[d], options)      
              odbcCloseAll()
          }
          load(file)
          #hP  <- subset(hP, curve.name %in% options$curve.names)
          gc()
          if (d < length(cal$calendar)){prices <- merge(prices, hP, all=TRUE)}else{prices <- hP} 
 
          cat("Done!\n")
        }
        IVaR.Prices <- IVaR.Changes <- subset(prices,contract.dt >= options$contract.dt.start & 
            contract.dt <= options$contract.dt.end )
                               
    }
    
    # Calculate price differences
    indDates.IVaR  <- grep("-",colnames(IVaR.Prices)) 
    IVaR.Changes[, indDates.IVaR[-1]] <- IVaR.Changes[,indDates.IVaR[-1]]- 
                                         IVaR.Changes[,indDates.IVaR[-length(indDates.IVaR)]] 
                     
    IVaR.Changes <- IVaR.Changes[,-indDates.IVaR[1]]
    return(IVaR.Changes )
}

