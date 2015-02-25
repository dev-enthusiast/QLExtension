
#========================================================================
rm(list=ls())
require(RODBC)
source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.discount.factor.R")
data <- NULL
data$buy.sell      <- "BUY"         # from a protection buyer point of view   
data$asOfDate      <- as.Date("2007-03-07")
data$start.date    <- as.Date("2005-01-01")
data$end.date      <- as.Date("2010-01-01")
data$notional      <- 10000000
data$deal.spread   <- 29            # in bps, where you transacted
data$payment.freq  <- "quarterly"   # semiannualy
data$recovery.rate <- 0.40
data$prob.default  <- data.frame(time=c(0, 0.5, 1:5, 7, 10),
   cdf=c(0, 0.0018, 0.0035, 0.0070, 0.0105, 0.0140, 0.0175, 0.0244, 0.0347))

source("H:/user/R/RMG/Finance/FixedIncome/price.CDS.R")
DF <- options <- NULL
res <- price.CDS(data)
res[[1]]

#-------------------------------------------------------------------------
rm(list=ls())
require(RODBC)
source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.discount.factor.R")
quotes <- data.frame(time=c(0.5, 3, 5),
                     cds.spread=c(21, 21, 21)) # in bps
data <- DF <- options <- NULL
source("H:/user/R/RMG/Finance/FixedIncome/price.CDS.R")
source("H:/user/R/RMG/Finance/FixedIncome/lib.credit.toolbox.R")
data$asOfDate <- as.Date("2007-03-16")
calibrate.default.cdf(quotes, data, DF, options)


#========================================================================
rm(list=ls())
source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.discount.factor.R")
data <- NULL
data$buy.sell      <- "BUY"         # from a protection buyer point of view   
data$asOfDate      <- as.Date("2007-03-06")
data$start.date    <- as.Date("2007-03-08")
data$end.date      <- as.Date("2012-03-08")
data$notional      <- 10000000
data$deal.spread   <- 29            # in bps, where you transacted           
data$payment.freq  <- "quarterly"   #semi-annual
data$recovery.rate <- 0.40
data$prob.default  <- data.frame(time=c(0, 0.5, 1:5, 7, 10),
   cdf=c(0, 0.0012, 0.0036, 0.0097, 0.0145, 0.0193, 0.0242, 0.0350, 0.0534))

source("H:/user/R/RMG/Finance/FixedIncome/price.CDS.R")
res <- price.CDS(data)
