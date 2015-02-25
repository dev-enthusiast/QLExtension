# Load prices used by R in the overnight VaR. 
# Where QQ is a data.frame with colnames = c("curve.name", "contract.dt")
#
# options$asOfDate   <- as.Date("2007-07-09")
# options$calc$hdays - the days you want to calculate, doesn't need to be business days
#
# Written by Adrian Dragulescu on 11-Jul-2007

.demo.get.prices.from.R <- function(){

  options <- NULL
  options$asOfDate <- as.Date("2007-07-16")
  options$calc$hdays <- seq(options$asOfDate, by="-1 day", length.out=10)
  
  QQ <- expand.grid(curve.name=c("COMMOD NG EXCHANGE", "COMMOD WTI EXCHANGE"),
    contract.dt= seq(as.Date("2008-01-01"), by="month", length.out=12))

  source("H:/user/R/RMG/Utilities/Database/R/get.prices.from.R.R")
  res <- get.prices.from.R(QQ, options)
  return(res)
  
}


get.prices.from.R <- function(QQ, options){

   source("H:/user/R/RMG/Utilities/Packages/load.packages.R")
   load.packages( "SecDb" )
   if (length(options$calc$fill.marks)==0){options$calc$fill.marks <- FALSE}
   
   calendar <- "CPS-BAL"
   skipWeekends <- FALSE
   ldays <- lapply(as.list(as.character(options$calc$hdays)), as.Date)
   aux <- secdb.invoke("AAD: IsHoliday", "IsHoliday", ldays, calendar, skipWeekends)
   options$calc$hdays <- options$calc$hdays[unlist(aux)==0]
   if (length(options$save$prices.mkt.dir)==0){
     options$save$prices.mkt.dir <- "S:/All/Risk/Data/VaR/prod/Prices/Market/"}

#   options$calc$hdays <- sort(options$calc$hdays)       # not necessary
   ind <- which(options$calc$hdays > options$asOfDate)  # if you get carried away
   if (length(ind)>0){options$calc$hdays <- options$calc$hdays[-ind]}
   options$calc$no.hdays <- length(options$calc$hdays)-1
   
   QQ$curve.name <- toupper(QQ$curve.name)
   QQ$vol.type <- QQ$position <- "NA"

   source("H:/user/R/RMG/Energy/VaR/Base/aggregate.run.prices.R")
   hPP <- aggregate.run.prices.mkt(QQ, options)
   hPP$vol.type <- hPP$position <- NULL
   return(hPP)
}
