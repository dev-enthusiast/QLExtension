# Update pgd calcs to move 
# Jenny's procmon process is power/pjm_pgd_ibt_bookTrds ???
#
# This is how you call it with a given date 
#  "S:\All\Risk\Software\R\R-2.15.1\bin\i386\Rscript.exe" pjm_pgd_ibt.R posDate="2014-01-01"
#
# Written by AAD on 12-Dec-2013


#################################################################
# "S:\All\Risk\Software\R\R-2.15.1\bin\i386\Rscript.exe" pjm_pgd_ibt.R posDate="2014-01-01"
.parse_posDate <- function()
{
  cArgs <- commandArgs()
  #print(cArgs)
  #print(paste("No args:", length(cArgs)))
  ind <- grep("posDate=", cArgs)
  
  if ( length(ind) == 1 ) {
    #print(cArgs[ind])
    posDate <- as.Date(gsub("posDate=(.*)", "\\1", cArgs[ind]))  # pass position Date 
  } else {
    posDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b", "NYM"))
  }

  posDate
}


#################################################################
#################################################################
options(width=500)  # make some room for the output
options(stringsAsFactors=FALSE)

require(methods)   
require(CEGbase)
require(xlsx)
require(SecDb)
require(PM)
require(reshape)
require(RODBC)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.simcube.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ibt.pgd.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/PJM/lib.PJM.ibt.R")


rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

posDate <- .parse_posDate()

rLog(paste("Running job on ", as.character(Sys.Date())))
rLog("\n\n")
pricingDate <- Sys.Date()
rLog("Positions date from: ", format(posDate))

res <- try({
  qq <- get_positions_move_pjm( posDate )
  to_xlsx(qq, posDate)
  update_calculators( qq, pricingDate )
  0
})

if (class(res) == "try-error") {
  returnCode <- 1                 # FAILED

  from <- "OVERNIGHT_REPORTS@constellation.com"
  to   <- "michael.flannery@constellation.com,adrian.dragulescu@constellation.com"
  #  to   <- "adrian.dragulescu@constellation.com"
  subject <- "PJM IBT:  Failed to update calculators"
  body <- c(res[1],
            "Procmon job: RMG/R/Reports/Energy/Trading/Congestion/pjm_pgd_ibt",
            "Contact: Adrian Dragulescu")
  sendEmail(from, to, subject, body)
}


q( status = returnCode )


