# Check daily if the ISO has created a new interface
#
#
# Written by Adrian Dragulescu on 2-Jan-2015



  
############################################################################
############################################################################
options(width=400)  
options(stringsAsFactors=FALSE)  
require(CEGbase)
require(reshape)
options(java.parameters="-Xmx1024m")
require(xlsx)



returnCode <- 0   # 0 means success

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/nepool_monitor_interfaces.R")
rLog("Start at ", as.character(Sys.time()))

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.transmission.R")

newIntf <- try({
  .download_interface_definition()  # download current file

  .find_new_interfaces()
})


if (class(newIntf) == "try-error") {
  returnCode <- 1

  
} else if ( !is.null(newIntf) ) {
  out <- capture.output(print(data.frame(new_interfaces=newIntf)))
  out <- c(out,
    "\nTransmission group specialist, please investigate any new congestion patterns!",        
    "\n\nProcmon job: RMG/R/Reports/Energy/Trading/Congestion/nepool_new_interfaces",
    "Contact: Adrian Dragulescu\n")

  to <- paste(c("necash@constellation.com"), sep="", collapse=",")
  #to <- "adrian.dragulescu@constellation.com"
  from <- "OVERNIGHT_PM_REPORTS@constellation.com"
  sendEmail(from, to, "NEW interface modeled by NEISO", out)

  # make the new RData with the latest interface definition
  .update_rdata_interfaces_file()

  
} else if (is.null(newIntf)) {
  rLog("No new data so no need to archive.  Remove today's file.")
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/SupportDocuments/Interfaces/"
  fname <- paste(DIR, Sys.Date(), "_generic_interface_constraints.xlsx", sep="")
  if (file.exists(fname))
    unlink(fname)
}



rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}






