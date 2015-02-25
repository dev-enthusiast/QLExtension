# Monitor my Bal-report PnL.  
# Runs in the morning after. 
#
#

#################################################################
# the PnL for Cocco
#
.PxQ_PnL <- function(day1, day2)
{
  source("H:/user/R/RMG/Utilities/Database/VCentral/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/PnL/lib.PnL.R")

  aux  <- .pnl_for_cocco(day1, day2, books="NESTAT")
  pnl  <- aux[["fp"]]
  spnl <- aux[["spreads"]]   
  #write.csv(spnl, file="S:/All/junk.csv", row.names=FALSE)

  out <- paste("From", format(day1), "to", format(day2))
  out <- c(out, paste("Total PnL =", dollar(sum(pnl$PnL))))
  
  out <- c(out, "\nBy year:", capture.output(print(
    cast(pnl, . ~ year, function(x){round(sum(x))}, value="PnL")[,-1],
    row.names=FALSE)))
           
  out <- c(out, "\nBy region:", capture.output(print(
    cast(pnl, mkt ~ year, function(x){round(sum(x))}, value="PnL"),
    row.names=FALSE)))

  out <- c(out, "\nBy book, year:", capture.output(print(            
    cast(pnl, book ~ year, function(x){round(sum(x))}, value="PnL"),
    row.names=FALSE)))

  # use the spread view when looking by location
  out <- c(out, "\nBy location, year:", capture.output(print(            
    cast(spnl, location ~ year, function(x){round(sum(x))}, value="PnL"),
    row.names=FALSE)))

  out <- c(out, "\n\nProcmon job: daily_balreport_AAD",
           "Contact: Adrian Dragulescu")
                                       
  sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
    "adrian.dragulescu@constellation.com",
    "Congestion PxQ PnL",
    out)        
  
  out
}



#################################################################
#################################################################
options(width=500)  # make some room for the output
options(stringsAsFactors=FALSE)

require(reshape)
require(CEGbase)
require(xlsx)
require(SecDb)
require(PM)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.bal.report.R")  

rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))

days <- seq(asOfDate-5, asOfDate, by="1 day")
books <- c("NPFTRMT1", "CONGIBT", "CONGIBT2", "CONGIBT3",
           "NEPPWRBS", "NEPBSCST", "NESTAT", "VIRTSTAT")
pnl <- get_bal_report(days, books)   
aux <- do.call(rbind, lapply(mapply(function(x, y){cbind(book=y, x)},
  pnl, names(pnl), SIMPLIFY=FALSE), tail, 1))
aux[,3:ncol(aux)] <- sapply(aux[,3:ncol(aux)], round)
aux$Period <- format(aux$Period, "%m/%d")
sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
    "adrian.dragulescu@constellation.com",
    paste("Bal report for", asOfDate),
    c(paste("Total PnL =", round(sum(aux$PNL)), "\n\n"),
      capture.output(print(aux, row.names=FALSE)),
      "\n\nProcmon job: daily_balreport_AAD"))


prevMarks <- ldply(pnl, function(x) {
  aux <- subset(x[,c("Period", "Prev.Mark")], abs(Prev.Mark) >= 10)
  aux$Prev.Mark <- round(aux$Prev.Mark)
  aux
})


if ((nrow(prevMarks) > 0) && sum(abs(prevMarks$Prev.Mark)) > 10) {
  sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
    "adrian.dragulescu@constellation.com",
    "Prev marks not zero!",
    capture.output(print(prevMarks, row.names=FALSE))) 
}


# do the PxQ PnL
day1 <- as.Date(secdb.dateRuleApply(Sys.Date(), "-2b"))
day2 <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
try( .PxQ_PnL(day1, day2) )

rLog(paste("Done at ", Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}








## out <- "ERROR"
## if (length(QQ) != 0){
##   .archive_bal_pnl(pnl)
##   out <- "UPDATED"
## }


## rLog("Email results")
## sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
##   "adrian.dragulescu@constellation.com",
##   paste("bal archiver for", as.character(asOfDate), out), "")




