#  Main script to drive SOX validation for rmsys.
#
#
#

rm(list=ls())
require(RODBC); require(reshape)
#source("H:/user/R/RMG/Energy/Risk/StressTests/make.shifted.deals.R")
source("H:/user/R/RMG/Finance/BlackScholes/blackscholes.R")
#setwd("H:/user/R/RMG/Energy/Risk/StressTests/")
options=res=NULL
options$asOfDate <- as.Date("2007-01-31") #Sys.Date()
#options$portfolio.id <- "('1','3')"      # gas trading and transport
options$portfolio <- "Gas Overall Portfolio"

source("H:/user/R/RMG/Energy/Systems/rmsys/Validation/get.rmsys.data.R")
res <- get.rmsys.data(options)
vrp <- res[[1]]; dd <- res[[2]]; deals <- res[[3]]; options <- res[[4]];
res <- NULL

load("C:/temp/vrp.RData")
load("C:/temp/dd.RData")
load("C:/temp/deals.RData")

table(dd$INSTRUMENT.TYPE.ID)

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.PHYFWD.R")
options$instrument.type.id <- 1
res <- value.PHYFWD(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for PHYFWD is ", sum(res$diff),".\n"))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.FINSWAP.R")
options$instrument.type.id <- 2
res <- value.FINSWAP(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for FINSWAP is ", sum(res$diff),".\n"))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.FUT.R")
options$instrument.type.id <- 3
res <- value.FUT(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for FUT is ", sum(res$diff),".\n"))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.EXCHOPT.R")
options$instrument.type.id <- 4
res <- value.EXCHOPT(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res$diffDelta <- res$DELTA-res$RMG.DELTA
res$diffGamma <- res$GAMMA-res$RMG.GAMMA
res$diffVega <- res$VEGA-res$RMG.VEGA
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for EXCHOPT is ", sum(res$diff), ".\n"))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.FINOPT.R")
options$instrument.type.id <- 5
res <- value.FINOPT(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res$diffDelta <- res$DELTA-res$RMG.DELTA
res$diffGamma <- res$GAMMA-res$RMG.GAMMA
res$diffVega <- res$VEGA-res$RMG.VEGA
  
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for FINOPT is ", sum(res$diff, na.rm = TRUE)))
res <- res[order(abs(res$res$diffDelta), decreasing=T), ]
cat(paste("Total Delta variance for FINOPT is ", sum(res$diffDelta, na.rm = TRUE)))
res <- res[order(abs(res$diffGamma), decreasing=T), ]
cat(paste("Total Gamma variance for FINOPT is ", sum(res$diffGamma, na.rm = TRUE)))
res <- res[order(abs(res$diffTheta), decreasing=T), ]
cat(paste("Total Theta variance for FINOPT is ", sum(res$diffTheta, na.rm = TRUE)))
res <- res[order(abs(res$diffVega), decreasing=T), ]
cat(paste("Total Vega variance for FINOPT is ", sum(res$diffVega, na.rm = TRUE)))
res <- res[order(abs(res$diffRho), decreasing=T), ]
cat(paste("Total Rho variance for FINOPT is ", sum(res$diffRho, na.rm = TRUE)))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.OPTPREM.R")
options$instrument.type.id <- 6
res <- value.OPTPREM(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for OPTPREM is ", sum(res$diff)))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.DEMCHG.R")
options$instrument.type.id <- 7
res <- value.DEMCHG(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for DEMCHG is ", sum(res$diff)))

load("C:/temp/vrp.RData")
load("C:/temp/dd.RData")
load("C:/temp/deals.RData")

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.TRNSPRT.R")
options$instrument.type.id <- 8
res <- value.TRNSPRT(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res$diffDelta1 <- res$delta1 - res$RMG.delta1
res$diffDelta2 <- res$delta2 - res$RMG.delta2
res$diffGamma1 <- res$gamma1 - res$RMG.gamma1
res$diffGamma2 <- res$gamma2 - res$RMG.gamma2
res$diffVega1 <- res$vega1 - res$RMG.vega1
res$diffVega2 <- res$vega2 - res$RMG.vega2
#res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for TRNSPRT is ", sum(res$diff)))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.SPREADOPT.R")
options$instrument.type.id <- 10
res <- value.SPREADOPT(vrp, dd, deals, options)   
res$diffMTM <- res$MTM-res$RMG.MTM
res$diffDelta1 <- res$delta1 - res$RMG.delta1
res$diffDelta2 <- res$delta2 - res$RMG.delta2
res$diffGamma1 <- res$gamma1 - res$RMG.gamma1
res$diffGamma2 <- res$gamma2 - res$RMG.gamma2
res$diffVega1 <- res$vega1 - res$RMG.vega1
res$diffVega2 <- res$vega2 - res$RMG.vega2
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for SPREADOPT is ", sum(res$diff)))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.FSOPT.R")
options$instrument.type.id <- 12
res <- value.FSOPT(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res$diffDelta <- res$DELTA-res$RMG.DELTA
res$diffGamma <- res$GAMMA-res$RMG.GAMMA
res$diffVega <- res$VEGA-res$RMG.VEGA 
head(res)
res <- res[order(abs(res$diff), decreasing=T), ]
cat(paste("Total variance for FSOPT is ", sum(res$diff)))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.FSOPTPHY.R")
options$instrument.type.id <- 28
res <- value.FSOPTPHY(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res$diffDelta <- res$DELTA-res$RMG.DELTA
res$diffGamma <- res$GAMMA-res$RMG.GAMMA
res$diffVega <- res$VEGA-res$RMG.VEGA 
head(res)
res <- res[order(abs(res$diff), decreasing=T), ]
cat(paste("Total variance for FSOPT is ", sum(res$diff)))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.EXCHPREM.R")
options$instrument.type.id <- 14
res <- value.EXCHPREM(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for EXCHPREM is ", sum(res$diff)))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.DLYSWAP.R")
options$instrument.type.id <- 20
res <- value.DLYSWAP(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for DLYSWAP is ", sum(res$diff)))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.PARKLOAN.R")
options$instrument.type.id <- 21
res <- value.PARKLOAN(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for PARKLOAN is ", sum(res$diff)))

## source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.CURRSWP.R")
## options$instrument.type.id <- 24
## res <- value.CURRSWP(vrp, dd, deals, options)   
## res$diff <- res$MTM-res$RMG.MTM
## res <- res[order(abs(res$diff), decreasing=T), ]
## head(res)
## cat(paste("Total variance for CURRSWP is ", sum(res$diff)))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.CLRSWPDLY.R")
options$instrument.type.id <- 23
res <- value.CLRSWPDLY(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for CLRSWPDLY is ", sum(res$diff)))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.CLRSWAP.R")
options$instrument.type.id <- 22
res <- value.CLRSWAP(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for CLRSWAP is ", sum(res$diff)))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.DLYANNU.R")
options$instrument.type.id <- 25
res <- value.DLYANNU(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for DLYANNU is ", sum(res$diff)))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.PREMDEM.R")
options$instrument.type.id <- 25
res <- value.PREMDEM(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for PREMDEM is ", sum(res$diff)))

source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.PHYGDOPT.R")
options$instrument.type.id <- 26
res <- value.PHYGDOPT(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for PHYGDOPT is ", sum(res$diff)))


















source("H:/user/R/RMG/Energy/Systems/rmsys/Valuation/value.FSOPT.R")
options$instrument.type.id <- 12
res <- value.FSOPT(vrp, dd, deals, options)   
res$diff <- res$MTM-res$RMG.MTM
res <- res[order(abs(res$diff), decreasing=T), ]
head(res)
cat(paste("Total variance for FSOPT is ", sum(res$diff)))






## MM <- data.frame(contract.dt=res$CONTRACT.DATE, type=res$CALL.PUT.FLAG,
##    moneyness=round(res$PRICE/res$STRIKE, 1), difference=res$diff)
## windows(4,8)
## layout(matrix(1:3), 3, 1)
## plot(MM$contract.dt, MM$difference, col="blue", xlab="Maturity", main="EXCHOPT")
## plot(MM$type, MM$difference, col="blue", xlab="Option type")
## plot(MM$moneyness, MM$difference, col="blue", xlab="Moneyness F/K")



