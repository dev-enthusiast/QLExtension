

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.stats.R")

startDate <- as.Date("2011-08-01")
endDate   <- Sys.Date()    # today
region    <- "Nepool"      # or "PJM"

.ice_volume_stats(startDate, endDate, region)








