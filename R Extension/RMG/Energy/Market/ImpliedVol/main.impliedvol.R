#
#
#


rm(list=ls())
dyn.load("H:/user/R/RMG/Utilities/Database/LIM/Source/RMGxmimR.dll")
source("H:/user/R/RMG/Utilities/Database/LIM/read.LIM.R")
source("H:/user/R/RMG/Finance/BlackScholes/impliedvol.R")

options=NULL
options$asOfDate    <- as.Date("2006-04-28")
options$contract.dt <- as.Date("2007-03-01")
options$strikes <- 4:20   # as character

source("H:/user/R/RMG/Energy/Market/ImpliedVol/read.options.data.LIM.R")
MM <- read.options.data.LIM(options)

unique(MM$Date)
MM <- subset(MM, Date==options$asOfDate)
MM$Date <- as.Date(MM$Date)
MM$Tex  <- as.numeric(options$contract.dt - MM$Date)/365

din <- data.frame(type=MM$type, price=MM$values, S=MM$Price, Tex=MM$Tex,
                  K=MM$strike, r=0)
source("H:/user/R/RMG/Finance/BlackScholes/impliedvol.R")
MM$ivol <- impliedvol(din)$IVol


plot(MM$strike, MM$ivol, type="n", 
  main=paste("ICE, Nat Gas options, ", sep=""), cex.main=1,
  xlab="Strike", ylab="Implied Vol")
abline(v=MM$Price[1], col="gray")
aux <- subset(MM, type=="CALL")
points(aux$strike, aux$ivol, col="blue")
aux <- subset(MM, type=="PUT")
points(aux$strike, aux$ivol, col="red")





