# Run the vol calculations 
#
# Last modified by John Scillieri on 01-May-2007

overnightVolJob.main <- function()
{
  rm(list=ls())
  
  source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
  source("H:/user/R/RMG/Utilities/Procmon.R")
  
  asOfDate <- overnightUtils.getAsOfDate()
  options  <- overnightUtils.loadOvernightParameters(asOfDate)
  
  PROCMON_DEPENDENCY_ORACLE = "power/SdbOraExport"
  PROCMON_DEPENDENCY_VOLS = "power/replrpt/BSVolsOracle_done"
  
  while( Procmon$getStatus(PROCMON_DEPENDENCY_ORACLE, asOfDate, exact=TRUE) != "Succeeded" &&
         Procmon$getStatus(PROCMON_DEPENDENCY_VOLS, asOfDate, exact=TRUE) != "Succeeded" )
  {
      rLog(PROCMON_DEPENDENCY_ORACLE, "Status:", 
              Procmon$getStatus(PROCMON_DEPENDENCY_ORACLE, asOfDate, exact=TRUE))
      rLog(PROCMON_DEPENDENCY_VOLS, "Status:", 
              Procmon$getStatus(PROCMON_DEPENDENCY_VOLS, asOfDate, exact=TRUE))
      rLog("Sleeping for 15 mins...")
      Sys.sleep(900)   
  }
  
  get.one.day.vols(options$asOfDate, options)
  cVV <- get.corrected.vols(options)
  correct.hVols.file(cVV, options)
  make.mkt.vol.files(options)
  fill.mkt.vol.files(options)  
}

overnightVolJob.main()

