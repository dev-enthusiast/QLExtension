# source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/Reporting/AprimeBatch.r")
# source("H:/user/R/RMG/Utilities/Reporting/AprimeBatch.r")

AprimeBatch <- function()
{
  source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/RLogger.R")
  source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/Reporting/ReducedAprime.r")
  source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/Reporting/getAsOfDate.r")
  
  Date <- getAsOfDate()
  rLog(paste("Running Aprime Batch for", Date))
  
  rLog("Calling Reduced Aprime ...")
  ReducedAprime(Date)

}
