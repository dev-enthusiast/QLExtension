# At the end of each month, provide CNE an estimate of reserve margin
# and net regional clearing price.
#
# Written by AAD on 5/30/2012


######################################################
#
.get_actuals <- function(month, org="CNE")
{  
  fname <- tail(.get_report_csvfilenames(org=org, month=month,
    reportName="SD_FCMCLOSTLDTL"), 1)
  res <- .process_report(fname)

  # the info is on 3rd tab
  aux <- res[[3]][,c("Capacity.Zone.Name",
    "Customer.Capacity.Load.Obligation", "Customer.Peak.Contributions")]
   aux$reserveMargin <- 100*as.numeric(aux[,"Customer.Capacity.Load.Obligation"])/as.numeric(aux[,"Customer.Peak.Contributions"])
  
  aux$Net.Regional.Clearing.Price <- res[[3]]$Net.Regional.Clearing.Price

  out <- c(paste("Actuals for ", format(month), " (from SD_FCMCLOSTLDTL report)\n", sep=""),
    capture.output(print(aux, row.names=FALSE)))

  out
}


######################################################
# report comes before the beginning of the month
#
.get_estimates <- function(month, PER=0, org="CNE")
{
  fname <- tail(.get_report_csvfilenames(org=org, month=month,
    reportName="SD_FCMPRECAPREQ"), 1)
  if (length(fname) == 0) {
    stop("Report SD_FCMPRECAPREQ is not yet out" )
  }
  res <- .process_report(fname)

  # the info is on 3rd tab
  rm <-  res[[3]][,c("Capacity.Zone.Name", "Customer.Capacity.Requirement",
                     "Customer.Peak.Contributions")]
  rm$Reserve.Margin <- 100*
    as.numeric(rm$Customer.Capacity.Requirement)/
      as.numeric(rm$Customer.Peak.Contributions)

  mon <- as.numeric(format(month, "%m"))
  fcaMonth <- if (mon <= 5) {
      as.Date(paste(as.numeric(format(month, "%Y"))-1, "-06-01", sep=""))
    } else {
      as.Date(paste(as.numeric(format(month, "%Y")), "-06-01", sep=""))
    }
  fname <- tail(.get_report_csvfilenames(org=org, month=fcaMonth,
    reportName="SP_FCARESULT"), 1)
  res <- .process_report(fname)

  aux <- res[[4]][,c("Modeled.Capacity.Zone.Name", "Capacity.Clearing.Price")]
  names(aux) <- c("Zone", "Capacity.Clearing.Price")
  aux$PER <- PER
  aux$Net.Regional.Clearing.Price <- aux$Capacity.Clearing.Price - PER
 # aux <- aux[1,]
  
  out <- paste("Estimate for ", format(month), "(from SP_FCMPRECAPREQ report)\n",  
    "Reserve Margin\n ",
    paste(capture.output(print(rm, row.names=FALSE)), collapse="\n"), 
    "\n\nNet Regional Clearing Price ($/kW-month)\n",
    paste(capture.output(print(aux, row.names=FALSE)), collapse="\n"),
               sep="")

  out  
}


######################################################
######################################################
#
options(stringsAsFactors=FALSE)
options(width=600)
require(methods)  
require(CEGbase)
require(reshape)


source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")

rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

asOfDate <- Sys.Date()

prevMonth <- seq(as.Date(format(asOfDate, "%Y-%m-01")), by="-1 month",
  length.out=2)[2]
act <- .get_actuals(prevMonth) 

est <- tryCatch(.get_estimates(nextMonth()), error=function(e){
  paste("Estimate for", format(nextMonth()), "is not out yet.")})

if (TRUE) {
  to <- paste(c("adrian.dragulescu@constellation.com", 
    "karen.steinbach@constellation.com", "long.han@constellation.com",
    "kevin.telford@constellation.com"), sep="", collapse=",")
  to <- "adrian.dragulescu@constellation.com"
  from <- "OVERNIGHT_PM_REPORTS@constellation.com"

  out <- c(act, "\n==========================================\n",
    est, sep="")
  
  sendEmail(from, to, "cne capacity estimates", out)
}




rLog(paste("Done at ", Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}


