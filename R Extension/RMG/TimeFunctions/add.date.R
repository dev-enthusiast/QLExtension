# Advance a date given a certain rule.
# source("H:/user/R/RMG/TimeFunctions/add.date.R")
# add.date(as.Date("2007-03-05"), "-1 e")
#
# Written by Adrian Dragulescu on 20-Jun-2005
# Added type = "b" and type = "x" by Anatoley Zheleznyak 06-Mar-2007

add.date <- function(date, rule){
  require(RODBC)
  PossibleTypes <- c("b","d","w","y","x","f","e","m","fri")
  aux    <- strsplit(rule, " ")[[1]]
  amount <- as.numeric(aux[1])
  type   <- aux[2]
  m <- as.numeric(format(date, "%m"))
  y <- as.numeric(format(date, "%Y"))
  d <- as.numeric(format(date, "%d"))
  weekend <- c("Saturday","Sunday")
  weekdaytypes <- c("mon","tue","wed","thu","fri")
  if (!(type%in%PossibleTypes)){stop("Please select a valid rule\n")}

  #----------------------------------------- Business days ----------
  if (type=="b"){  
    datePricingMin <- as.Date("2006-08-02")
    datePricing <- date 
    if(datePricing < datePricingMin) { datePricing <- datePricingMin }  
    if(datePricing > (as.Date(Sys.time(), "%b-%m-%Y") - 1)) { datePricing = as.Date(Sys.time(), "%b-%m-%Y") - 1 }
    dsnPath = "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/CPSPROD.dsn"
    conString <- paste( dsnPath, ";UID=stratdatread;PWD=stratdatread;", sep="")
    con = odbcDriverConnect(conString)
    nDim <- 0
    while( nDim == 0 ) {  
      query <- paste("SELECT CALENDAR_DATE, HOLIDAY, WEEKEND ", 
      "FROM foitsox.SDB_Holiday_Calendar ", 
      "WHERE calendar='CPS-BAL' ",     
      "AND pricing_Date = TO_DATE('", format(datePricing, "%m/%d/%Y"),"','MM/DD/YYYY')")
      data <- sqlQuery(con, query)
      nDim <- dim(data)[1]   
      datePricing <- datePricing -1 
    }
    odbcCloseAll()
    indB <- intersect(which(data$HOLIDAY == "N"), which(data$WEEKEND == "N"))
    indNow <- which(as.Date(data$CALENDAR_DATE) == date)
    if (amount > 0) {
      indForw <- indB[which(indB > indNow)]
      newDate <- data$CALENDAR_DATE[indForw[amount]]
    } else {
      indBack <- indB[which(indB < indNow)]
      newDate <- data$CALENDAR_DATE[indBack[(length(indBack) + 1 + amount)]]
    }
    newDate <- format(newDate, "%Y-%m-%d")
  }              
  #----------------------------------------- Julian (Calendar) Days ---
  if (type=="d"){newDate <- date + amount}
  
  #----------------------------------------- Weeks --------------------
  if (type=="w"){newDate <- date + 7*amount}
  
  #----------------------------------------- Months --------------------
  if (type=="m"){
    newDate <- seq(date, by=paste(amount, "month"), length=2)[2]
  }
  #----------------------------------------- Years --------------------
  if (type=="y"){
    newDate <- seq(date, by=paste(amount, "year"), length=2)[2] 
  }
  
  #----------------------------------------- First of the month -------
  if (type=="f"){
    newDate <- seq(as.Date(ISOdate(y,m,1)), by=paste(amount, "month"),
               length=2)[2]
  }
  #----------------------------------------- End of month -------------
  if (type=="e"){
    newDate <- seq(as.Date(ISOdate(y,m,1)), by=paste(amount+1, "month"),
               length=2)[2] - 1
  }
  #----------------------------------------- Last business day of the month
  if (type=="x"){
    dateX <-  seq(as.Date(ISOdate(y,m,1)), by=paste( (amount+1), "month"), length=2)[2]
    newDate <- add.date(dateX, "-1 b")
  }
#-----------------------------------------------------------------------#
return(as.Date(newDate))
}


 	### Number of "???days" ahead
# 		if(any(rule==weekdaytypes)){
# 		Day = as.character(weekdays(z))
# 		dddDay = tolower(substr(Day,1,3))
#		     for(k in 1:K){
# 			while(dddDay[k]!=rule){
# 			z[k] = z[k] + 1
# 			Day[k] = as.character(weekdays(z[k]))
# 			dddDay[k] = tolower(substr(Day[k],1,3))
# 			}
#		     }
# 		    newDate = z + 7*(Amount-1)
# 		}
