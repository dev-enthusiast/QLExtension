# source("H:/user/R/RMG/TimeFunctions/countBusinessDate.R")
# dateI <- as.Date("2007-01-01") 
# dateF <- as.Date("2007-03-31") 
# busDates <- countBusinessDate(dateI, dateF)
#
# Written by Anatoley Zheleznyak on 06-Mar-2007
 
countBusinessDate <- function(dateI, dateF){
  require(RODBC)
  datePricingMin <- as.Date("2006-08-02")
  datePricing <- dateI 
  if(datePricing < datePricingMin) { datePricing <- datePricingMin }  
  if(datePricing > (as.Date(Sys.time(), "%b-%m-%Y") - 1)) { datePricing = as.Date(Sys.time(), "%b-%m-%Y") - 1 }
  
  file.dsn <- "CPSPROD.dsn"
  file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
  connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
     ";UID=stratdatread;PWD=stratdatread;", sep="")  
  con <- odbcDriverConnect(connection.string)  
  #con  <- odbcConnect("CPSPROD", uid="stratdatread", pwd="stratdatread")
  nDim <- 0
  while( nDim == 0 ) {  
    query <- paste("SELECT CALENDAR_DATE, HOLIDAY, WEEKEND ", 
    "FROM foitsox.SDB_Holiday_Calendar ", 
    "WHERE calendar='CPS-BAL' ", 
    "AND CALENDAR_DATE >= TO_DATE('", format(dateI, "%m/%d/%Y"),"','MM/DD/YYYY')",     
    "AND CALENDAR_DATE <= TO_DATE('", format(dateF, "%m/%d/%Y"),"','MM/DD/YYYY')",     
    "AND pricing_Date = TO_DATE('", format(datePricing, "%m/%d/%Y"),"','MM/DD/YYYY')")
    data <- sqlQuery(con, query)
    nDim <- dim(data)[1]   
    datePricing <- datePricing -1 
  }
  odbcCloseAll()
  indB <- intersect(which(data$HOLIDAY == "N"), which(data$WEEKEND == "N"))
  busDates <- as.Date(data$CALENDAR_DATE[indB])
  return(busDates)              
}

