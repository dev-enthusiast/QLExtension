# Scrape pipeline websites to notify on critical notices
#
#


#################################################################
#################################################################
options(width=500)  # make some room for the output
require(CEGbase)
require(httr)
require(rJava)
.jinit()
.jaddClassPath("S:/All/Structured Risk/NEPOOL/Software/Utilities/jsoup-1.7.2.jar")

rLog(paste("Start proces at", Sys.time()))
returnCode <- 99    

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pipewatcher.R")
check_new_notices()

.db_trim()

hour <- as.numeric(format(Sys.time(), "%H"))
if (hour >= 18)
  returnCode <- 0           # succeed after 6pm (don't work overtime)

q( status = returnCode )




