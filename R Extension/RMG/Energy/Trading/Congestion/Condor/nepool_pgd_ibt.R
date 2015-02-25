# Update pgd calcs to move 
# Jenny's procmon process is power/nepool_pgd_ibt_bookTrds
#
# Steve's trades from NEPGDMD3 -> BGMOD3 
# Adrian's trade from NEPGDIB2 -> CONGIBT3 
#
# Written by AAD on 12-Feb-2009


#################################################################
#################################################################
options(width=500)  # make some room for the output
options(stringsAsFactors=FALSE)

require(methods)   # not sure why this does not load!
require(CEGbase)
require(xlsx)

require(SecDb)
require(PM)
require(reshape)
require(lattice)
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.simcube.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ibt.pgd.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ibt.pgd.nepool.R")


rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

asOfDate <- Sys.Date()

# run only on Fri evening.  we'll change the procmon job description eventually.
# if ( weekdays(asOfDate) != "Friday" ) {

# Only run on these days ...
if (!(asOfDate %in% as.Date(c("2014-09-23", "2014-10-07")))) {
  rLog("For now, stop it!")
  q( status = returnCode )
}

# if you want to skip a date because you know something is wrong with
# positions on that dae, modify the file below
filename <- "S:/All/Structured Risk/NEPOOL/Temporary/IBTs/skip_days_nepool.txt"
if ( .skip_day( asOfDate, filename ) ) {
  rLog("Not moving anything!  Manually skipping day", format(asOfDate))
  q( status = returnCode )
}

#q( status = 0)  # do nothing until I come back from vacation ...

rLog(paste("Running job for", as.character(asOfDate)))
rLog("\n\n")

posDate <- as.Date(secdb.dateRuleApply(asOfDate, "-1b", "CPS-BAL"))
rLog("Pulling positions from ", format(posDate))

res <- try({
  qq <- .get_positions_move_nepool( posDate, monthsFromCash=3 )
  if (TRUE) {  #( .does_move_make_sense( qq$toWT ) ) {
    .to_xlsx( qq, asOfDate, prefix="" )
    .update_calculators( qq )
  
    0
  } else {
    1
  }
})

if (class(res) == "try-error" || res == 1)
  returnCode <- 1                 # FAILED


q( status = returnCode )


