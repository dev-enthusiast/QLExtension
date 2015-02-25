# Set general options for the run
#
#
# Written by Adrian Dragulescu on 23-Mar-2005

set.parms.model <- function(save, options){

require(Simple); require(xtable); require(chron); require(lattice);
require(RODBC)

options$source.dir <- "H:/user/R/RMG/Energy/CEAR/"
options$buckets    <- c("OFFPEAK", "PEAK", "FLAT")
options$season     <- "season"
options$no.cuts.x  <- 10
options$HS         <- 1               # Hourly Sampling, for tsdbread.R

options$fdata$startDate <- ISOdatetime(2005, 11, 01,  0, 0, 0)
options$fdata$endDate   <- ISOdatetime(2005, 11, 30, 23, 0, 0)
options$task <- "Simulation and History"  # Input from Excel, Simulation Only,
                                          # Historical Only, Both

save$Analysis$plots    <- 1

source(paste(options$source.dir, options$region, ".R", sep=""))
source("H:/user/R/RMG/Utilities/tsdbRead.SecDB.R")
source("H:/user/R/RMG/Utilities/tsdbRead.R")
source("H:/user/R/RMG/Statistics/sample.from.Ndim.R")
source("H:/user/R/RMG/TimeFunctions/select.onpeak.R")
source("H:/user/R/RMG/TimeFunctions/select.offpeak.R")
source("H:/user/R/RMG/TimeFunctions/calculate.season.R")
source("H:/user/R/RMG/Energy/CEAR/simulate.spreads.R")
source("H:/user/R/RMG/Energy/CEAR/quantiles.by.factor.CEAR.R")

return(list(save, options))

}










