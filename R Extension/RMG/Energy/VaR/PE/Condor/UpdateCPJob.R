################################################################################
# This calculates which counterparties failed and regenerates the counterparty
# condor file for resubmission.
#

################################################################################
# External Libraries
#
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/sendemail.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
source("H:/user/R/RMG/Energy/VaR/PE/PE.R")
source("H:/user/R/RMG/Energy/VaR/PE/PEUtils.R")
source("H:/user/R/RMG/Energy/VaR/PE/Condor/PECondorUtils.R")
source("H:/user/R/RMG/Energy/VaR/PE/Condor/CheckPEStatus.R")


################################################################################
# File Namespace
#
UpdateCPJob = new.env(hash=TRUE)



################################################################################
UpdateCPJob$main <- function()
{
  # GET PE RUN.NAMES (COUNTERPARTY) LIST
  filename <- paste(PE$SRC_DIR, "schedule.overnight.PE.xls", sep="")
  con  <- odbcConnectExcel(filename)
  data <- sqlFetch(con, "PE_RUN_OVERNIGHT")
  odbcCloseAll()
  runNames  <- toupper(sort(as.character(unique(data$RUN_NAME))))
  runNamesD <- gsub(" ", ".", runNames)
  
  asOfDate = overnightUtils.getAsOfDate()
  completed.runs = CheckPEStatus$main(asOfDate)
  
  notFinished.runs <- runNames[which(!(runNamesD %in%completed.runs))]
  cat(length(notFinished.runs), "Unfinished:\n")
  print(notFinished.runs)
  
  peSubmitFile = PECondorUtils$createCounterpartyJobFile(notFinished.runs)
  
  total.no.runs = length(runNames)

  if ( length(notFinished.runs) != 0 )
  {
    subject = paste( length(notFinished.runs), "of",
      total.no.runs, "PE runs have not finished for", asOfDate )
      
    body = paste("These", length(notFinished.runs),
      "runs did not finish:\n")
    body = append(body, capture.output(notFinished.runs))
    
    toList <- c("john.scillieri", "adrian.dragulescu", "jing.bai", "haibin.sun")
    toList <- paste(paste(toList, "@constellation.com", sep=""),
                    collapse=", ")
    sendEmail(from="PE-OVERNIGHT-DO-NOT-REPLY@constellation.com",
              to=toList, subject, body)
  }
  
}


################################ MAIN EXECUTION ################################
.start = Sys.time()
UpdateCPJob$main()
Sys.time() - .start


################################################################################
#                                OLD STUFF
################################################################################
##   unfinished = data.frame(cparty=unfinishedList)
##   unfinished = merge(unfinished, allPositions)
##   unfinished$F0 = NULL
##   unfinished$position = NULL
##   unfinished$curve.name = NULL
##   unfinished = unique(unfinished)

##   currentMonth = as.Date(format(Sys.Date(), "%Y-%m-01"))

##   notExpiring = subset(unfinished, contract.dt != currentMonth)

##   allCounterparties = as.character(unique(unfinished$cparty))
##   activeCounterparties = as.character(unique(notExpiring$cparty))

##   expiringCounterparties = setdiff(allCounterparties, activeCounterparties)

##   cat(length(expiringCounterparties), "Expiring So Not Simulated:\n")
##   print(expiringCounterparties)
