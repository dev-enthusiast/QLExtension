# Read the IR data from CPSPROD database, where 
# options$asOfDate  <- as.Date("2005-12-14")
# options$currency  <- "USD"                # optional
# options$months    <- array of Dates       # optional, what months you want  
# 
# source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.discount.factor.R")
# get.discount.factor(options)
#
# Written by Adrian Dragulescu on 15-May-2006

get.discount.factor <- function(options){

  require(RODBC)
  source("H:/user/R/RMG/Utilities/Math/extrapolate.R")
  if (length(options$currency)==0){options$currency <- "USD"}
  if (length(options$asOfDate)==0){options$asOfDate <- Sys.Date()-1}
  if (length(options$file.dsn)==0){options$file.dsn <- "CPSPROD.dsn"}
  if (length(options$file.dsn.path)==0){
    options$file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"}
  if (length(options$con$CPSPROD)==0){
    connection.string <- paste("FileDSN=", options$file.dsn.path, options$file.dsn, 
       ";UID=stratdatread;PWD=stratdatread;", sep="")  
    options$con$CPSPROD <- odbcDriverConnect(connection.strign)
  }

  query <- paste("SELECT * from foitsox.sdb_irfx_rate where ",
    "PRICING_DATE=TO_DATE('", format(options$asOfDate, "%m/%d/%Y"),  
    "', 'MM/DD/YYYY') AND RATE_NAME='", options$currency, "'",  sep="")
  data <- sqlQuery(options$con$CPSPROD, query)
#  odbcCloseAll()
  data <- data[,-6]  # remove update time
  data[,"START_DATE"] <- as.Date(data[,"START_DATE"])
  
  if (length(options$months)>0){
    ind  <- which(data[,"START_DATE"] > min(options$months) &
                  data[,"START_DATE"] < max(options$months))
    data <- data[ind,]
  }
  data <- data[order(data[,"START_DATE"]),]
  t.DB <- as.numeric((data[,"START_DATE"]-options$asOfDate)/365)
  Tex  <- as.numeric((options$months-options$asOfDate)/365)
  Rex  <- extrapolate(t.DB, data$RATE, Tex)
  res  <- data.frame("Date"=options$months, IR=Rex, DF=exp(-Rex*Tex))
  
  return(res)
}

