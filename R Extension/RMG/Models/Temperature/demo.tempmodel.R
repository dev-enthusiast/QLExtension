# Main driver for the temperature model.
#
#
#

rm(list = ls(all=TRUE))            # clear the memory
save=NULL; options=NULL
source("H:/user/R/RMG/Models/Temperature/tempmodel.Main.R")

options$tsdb.symbol  <- c("temp_clean_bos_dly_max",
                          "temp_clean_bos_dly_min")
options$airport.name <- "BOS"
tempmodel.Main(save, options)

plot.hist.temp(save, options)   #      do the plots


## options$LIM.symbol   <- "BOSTON.LOGAN.MA"
## options$airport.name <- "BOS"
## tempmodel.Main(save, options)



## options$LIM.symbol   <- "BALTIMORE.WASHINGTON.MD"
## options$airport.name <- "BWI"
## tempmodel.Main(save, options)




## options$airport.name <- "ORH"
## tempmodel.Main(save, options)
