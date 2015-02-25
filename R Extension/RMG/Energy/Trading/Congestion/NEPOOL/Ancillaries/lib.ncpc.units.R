# Report on the ncpc payments by unit
#
# .get_DANCPC_payments
# .get_RTNCPC_payments
#   ..historical_revenues_iso_fca9
# .make_monthly_report
# 

#######################################################################
# From the old reports before hourly settlements starting on 12/3/2014
#
..get_DANCPC_payments_1 <- function(month = currentMonth())
{
  reportName <- "SD_DANCPCPAYMENT"; tab <- 1
  
  org <- if (month >= as.Date("2013-02-01")) { "EXGN" } else { "CCG" }
  
  fnames <- .get_report_csvfilenames(org=org, month=month, reportName=reportName)

  # this files are corrupted from IT from the ForeRiver process
  corrupted <- "SD_DANCPCPAYMENT_000050428_2014110200_20141107205432.CSV"
  fnames <- fnames[!(basename(fnames) %in% corrupted)]
  
  if (is.null(fnames))
    return(NULL)
  
  AA <- .archive_report(fnames, tab=tab, save=TRUE, minReportDay=month-390)

  days <- seq(month, min(nextMonth(month)-1, Sys.Date()), by="1 day")
  
  # load archive 
  AA <- .load_archive(reportName, tab, onlyLatest=TRUE)
  AA <- subset(AA, reportDay %in% days)

  res <- ddply(AA, c("reportDay", "Asset.ID"), function(x){
   # browser()
    #unique(x[,c("Daily.Total.NCPC", "NCPC.Type")])
    cast(x, Asset.ID + NCPC.Type ~ ., sum, value="Customer.Share.NCPC.Payment" )
  })
  colnames(res)[ncol(res)] <- "Daily.Total.NCPC"

  res
}

#######################################################################
# From the new reports after hourly settlements starting on 12/3/2014
#
..get_DANCPC_payments_2 <- function(month = currentMonth())
{
  reportName <- "SD_DANCPCPYMT"; tab <- 2

  org <- "EXGN"

  fnames <- .get_report_csvfilenames(org=org, month=month, reportName=reportName)
  if (is.null(fnames))
    return(NULL)

  AA <- .archive_report(fnames, tab=tab, save=TRUE, minReportDay=month-390)

  days <- seq(month, min(nextMonth(month)-1, Sys.Date()), by="1 day")
  
  # load archive 
  AA <- .load_archive(reportName, tab, onlyLatest=TRUE)
  AA <- subset(AA, reportDay %in% days)

  res <- ddply(AA, c("reportDay", "Asset.ID"), function(x){
    #browser()
    cast(x, Asset.ID + NCPC.Credit.Type ~ ., sum,
         value="Participant.Share.Day.Ahead.NCPC.Credit" )
  })
  colnames(res)[ncol(res)] <- "Daily.Total.NCPC"
  
  res <- subset(res, !(NCPC.Credit.Type == "" & Daily.Total.NCPC == 0))
  colnames(res)[3] <- "NCPC.Type"
  
  res
}


#######################################################################
#
.get_DANCPC_payments <- function(month = currentMonth())
{
  if (month < as.Date("2014-12-01")) {
    res <- ..get_DANCPC_payments_1(month = month)
    
  } else if (month == as.Date("2014-12-01")) {
    res1 <- ..get_DANCPC_payments_1(month = month)
    res2 <- ..get_DANCPC_payments_2(month = month)

    res <- rbind(res1, res2)
    
  } else if (month > as.Date("2014-12-01")) {
     res <- ..get_DANCPC_payments_2(month = month)
  }

  res$NCPC.Type <- toupper(res$NCPC.Type)
  
  res <- cbind(service="DA NCPC", res)

  res
##     service  reportDay Asset.ID NCPC.Type Daily.Total.NCPC
## 1 DA NCPC 2014-12-01     1625  ECONOMIC             0.00
## 2 DA NCPC 2014-12-01    14614  ECONOMIC             0.00
## 3 DA NCPC 2014-12-02     1625  ECONOMIC             0.00
## 4 DA NCPC 2014-12-02    14614  ECONOMIC             0.00

}




#######################################################################
# before 12/3/2014
#
..get_RTNCPC_payments_1 <- function(month = currentMonth())
{
  reportName <- "SD_RTNCPCPMT"; tab <- 1

  org <- if (month >= as.Date("2013-02-01")) { "EXGN" } else { "CCG" }
  
  fnames <- .get_report_csvfilenames(org=org, month=month, reportName=reportName)
  if (is.null(fnames))
    return(NULL)
  
  AA <- .archive_report(fnames, tab=tab, save=TRUE, minReportDay=month-390)

  days <- seq(month, min(nextMonth(month)-1, Sys.Date()), by="1 day")
  
  # load archive 
  AA <- .load_archive(reportName, tab, onlyLatest=TRUE)
  AA <- subset(AA, reportDay %in% days)

  res <- ddply(AA, c("reportDay", "Asset.ID"), function(x){
    #browser()
    cast(x, Asset.ID + NCPC.Type ~ ., sum, value="Customer.Share.NCPC.Payment" )
  })
  colnames(res)[ncol(res)] <- "Daily.Total.NCPC"
  
  res
}



#######################################################################
# After 12/3/2014
#
..get_RTNCPC_payments_2 <- function(month = currentMonth())
{
  reportName <- "SD_RTNCPCPYMT"; tab <- 3

  org <- "EXGN"
  
  fnames <- .get_report_csvfilenames(org=org, month=month, reportName=reportName)
  if (is.null(fnames))
    return(NULL)
  
  AA <- .archive_report(fnames, tab=tab, save=TRUE, minReportDay=month-390)

  days <- seq(month, min(nextMonth(month)-1, Sys.Date()), by="1 day")
  
  # load archive 
  AA <- .load_archive(reportName, tab, onlyLatest=TRUE)
  AA <- subset(AA, reportDay %in% days)

  AA <- subset(AA, Participant.Share.of.Real.Time.NCPC.Credit != 0)
  
  res <- ddply(AA, c("reportDay", "Asset.ID"), function(x){
    #browser()
    cast(x, Asset.ID + NCPC.Dispatch.Credit.Type ~ ., sum,
         value="Participant.Share.of.Real.Time.NCPC.Credit" )
  })
  colnames(res)[ncol(res)] <- "Daily.Total.NCPC"

  colnames(res)[3] <- "NCPC.Type"
  
  res
}



#######################################################################
#
.get_RTNCPC_payments <- function(month = currentMonth())
{
  if (month < as.Date("2014-12-01")) {
    res <- ..get_RTNCPC_payments_1(month = month)
    
  } else if (month == as.Date("2014-12-01")) {
    res1 <- ..get_RTNCPC_payments_1(month = month)
    res2 <- ..get_RTNCPC_payments_2(month = month)

    res <- rbind(res1, res2)
    
  } else if (month > as.Date("2014-12-01")) {
     res <- ..get_RTNCPC_payments_2(month = month)
  }

  res$NCPC.Type <- toupper(res$NCPC.Type)
  
  res <- cbind(service="RT NCPC", res)
  
  res
##   service  reportDay Asset.ID NCPC.Type Daily.Total.NCPC
## 5 RT NCPC 2014-12-01     1625  ECONOMIC          9832.44
## 6 RT NCPC 2014-12-01    14614  ECONOMIC         26949.79
## 7 RT NCPC 2014-12-02     1625  ECONOMIC         29215.46
## 8 RT NCPC 2014-12-02    14614  ECONOMIC         29317.68  
}



#######################################################################
#
.make_monthly_report <- function(month = currentMonth(), do.email=TRUE)
{
  DD <- rbind(.get_DANCPC_payments(month),
              .get_RTNCPC_payments(month))

  if (nrow(DD)==0)
    return(NULL)
  
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/MAP.RData")
  MAP$Asset.ID <- MAP$ptid 
  
  DD <- merge(DD, MAP[,c("Asset.ID", "location")], all.x=TRUE)
  DD <- DD[order(DD$reportDay, DD$Asset.ID), ]

  
  fname <- paste("S:/All/Structured Risk/NEPOOL/Ancillary Services/",
    "Monthly Ancillary Updates/OpRes/ISO_payments/ncpc_units_", format(month),
    ".xlsx", sep="")
  write.xlsx2(DD, file=fname, row.names=FALSE)

  if (do.email) {
    TT <- cast(DD, location ~ service + NCPC.Type, function(x) {
      round(sum(x))
    }, fill=0, value="Daily.Total.NCPC")
    TT <- data.frame(TT)
    TT$Total <- apply(TT[,2:ncol(TT),drop=FALSE], 1, sum)
    TT[nrow(TT)+1,1] <- "Total" 
    TT[nrow(TT), 2:ncol(TT)] <- apply(TT[1:(nrow(TT)-1),-1,drop=FALSE], 2, sum)

    out <-  c(capture.output(print(TT, row.names=FALSE)),
      "\n", paste("Last report day:", max(DD$reportDay)),
      "Report saved to: S:/All/Structured Risk/NEPOOL/Ancillary Services/Monthly Ancillary Updates/OpRes/ISO_payments folder",        
      "\n\nContact: Adrian Dragulescu",
      "Procmon job: RMG/R/Reports/Energy/Trading/Congestion/nepool_ncpc_units_actuals")



    to <- paste(c("NECash@constellation.com", "william.ewing@constellation.com",
                  "Kyle.Rymon@constellation.com", "brian.schuppert@constellation.com",  
      "steven.webster@constellation.com"), sep="", collapse=",")
    #to <- "adrian.dragulescu@constellation.com"
    from <- "OVERNIGHT_PM_REPORTS@constellation.com"
    subject <- paste(format(month, "%b%y"), "NCPC payments for our units")
    sendEmail(from, to, subject, out)
  }
}





#######################################################################
#######################################################################
#
.test <- function()
{
  require(CEGbase)
  require(plyr)
  require(reshape)
  require(xlsx)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")

  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.units.R")

  month <- as.Date("2013-12-01")
  #month <- as.Date("2014-01-01")
  .make_monthly_report(month=month)




  
  reportName <- "SD_DANCPCPAYMENT"; tab <- 1


  months <- seq(as.Date("2013-01-01"), currentMonth(), by="1 month")
  fnames <- unlist(sapply(months, function(m){
    c(.get_report_csvfilenames(org="EXGN", month=m, reportName=reportName))
  }))
  AA <- .archive_report(fnames, tab=tab, save=TRUE)

  #############################################################
  # look at FR RMR revenues, for Les on 7/16
  #
  DIR <- "S:/All/Structured Risk/NEPOOL/Ancillary Services/Monthly Ancillary Updates/OpRes/ISO_payments"
  files <- list.files(DIR, full.names=TRUE)
  res <- ldply(files, function(filename){
    read.xlsx2(filename, sheetIndex=1)
  })
  ncpc <- subset(res, Asset.ID %in% c("1691", "40327", "40328"))
  ncpc$reportDay <- as.Date(as.numeric(ncpc$reportDay), "1900-01-01")
  ncpc$month <- format(ncpc$reportDay, "%Y-%m")
  ncpc$Daily.Total.NCPC <- as.numeric(ncpc$Daily.Total.NCPC)

  
  cast(ncpc, month ~ service + NCPC.Type, sum, fill=0, value="Daily.Total.NCPC")

  
  
  
  

  


}












## #######################################################################
## # Calculate the historical NCPC revenues for My 7,8,9 to have when 
## # talking to the ISO for FCA9
## #
## ..historical_revenues_iso_fca9 <- function()
## {
##   months <- seq(as.Date("2013-01-01"), as.Date("2013-11-01"), by="1 month")

##   source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.units.R")

##   month <- as.Date("2013-01-01")

##   ldply(months, function(month) {
##     #browser()
##     rLog("Working on month ", format(month))
##     .make_monthly_report(month=month, do.email=FALSE)
##   })

##   #--------------------------------------------------------
##   # read them all and do some basic analysis on them
##   require(xlsx)
##   files <- list.files("S:/All/Structured Risk/NEPOOL/Ancillary Services/Monthly Ancillary Updates/OpRes/ISO_payments", full.names=TRUE)

##   res <- ldply(files, function(fname) {
##     rLog("Working on file ", basename(fname))
##     DD <- read.xlsx2(fname, sheetIndex=1)
##     DD$reportDay <- as.Date(as.numeric(DD$reportDay)-25569, origin="1970-01-01")
##     DD$Daily.Total.NCPC <- as.numeric(DD$Daily.Total.NCPC)
##     DD
##   })
##   mys <- subset(res, Asset.ID %in% c(502, 1478, 1616))
##   write.csv(mys, file="C:/temp/hist_ncpc_mystic.csv", row.names=FALSE)
  
##   require(lattice)
##   xyplot(Daily.Total.NCPC ~ reportDay | location, data=mys,
##          groups=mys$service)

##   mys <- mys[order(mys$Asset.ID, mys$reportDay, mys$service),]
##   mys$month <- as.Date(format(mys$reportDay, "%Y-%m-01"))

##   MM <- cast(mys, month + location ~ .,  sum, value="Daily.Total.NCPC",
##        fill=0)
##   MM[,3] <- round(MM[,3])
##   colnames(MM)[3] <- "Total.NCPC"
##   print(MM, row.names=FALSE)
  
  
## }
