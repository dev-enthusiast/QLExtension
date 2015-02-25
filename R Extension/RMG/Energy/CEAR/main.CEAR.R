#
#
#
# Written by Adrian Dragulescu on 7-Jul-2005


rm(list=ls())
save=NULL; options=NULL
options$xls.inputfile   <- "S:/All/Risk/Data/FTRs/NEPOOL/Jaya/Cal06 FTR Paths 1.xls"
options$tsdb.refresh    <- 1
options$noSims          <- 10
options$region          <- "NEPOOL"
options$fdata$startDate <- ISOdatetime(2005, 11, 1, 0, 0, 0)
options$fdata$endDate   <- ISOdatetime(2005, 11, 30, 23, 0, 0)
save$dir <- "S:/All/Risk/Analysis/Zonal.Risk/NEPOOL/CEAR/Annual.Auction/"

source("H:/user/R/RMG/Energy/CEAR/set.parms.model.R")
aux <- set.parms.model(save, options)
save <- aux[[1]]; options <- aux[[2]]

source("H:/user/R/RMG/Energy/CEAR/main.analysis.CEAR.R")
aux <- main.analysis.CEAR(save, options)




## Q <- aux[[1]]; sim.prices <- aux[[2]]
## save <- aux[[3]]; options <- aux[[4]]

## source("H:/user/R/RMG/Energy/CEAR/payoff.FTR.R")
## aux <- payoff.FTR(sim.prices, Q)
## unit.Payoff <- aux[[1]]; payoff <- aux[[2]]

## source("H:/user/R/RMG/Energy/CEAR/report.CEAR.R")
## report.CEAR(unit.Payoff, payoff, Q, save, options)

## source("H:/user/R/RMG/Energy/CEAR/make.pdf.R")
## make.pdf(save, options)



























options$fdata$startDate <- as.Date("2005-07-01")
options$fdata$endDate   <- as.Date("2005-12-31")
options$noSims   <- 500                # 500 sims & 7 mths = 2 min.  
options$run.name <- "NEPOOL"
options$book <- NULL # "JBFTR"
options$tsdb.refresh <- 1

source("H:/user/R/RMG/Models/Price/BasisRisk/set.parms.model.R")
res   <- set.parms.model(save, options)
fdata <- res[[1]]; save <- res[[2]]; options <- res[[3]]

res <- read.zonal.LMP(save,options)
MM  <- res[[1]]; dMM <- res[[2]]; options <- res[[3]]
MM[1:10,]

sim.spread <- simulate.spreads(dMM, fdata, save, options) 

#main.hist.analysis(MM, sim.spread, save, options)  

Q        <- read.positions(save, options)
risk.mth <- calculate.zonal.risk(fdata, sim.spread, Q, save, options)
(risk.mth[,,1])

source(paste(save$source.dir, "report.risk.R", sep=""))
report.risk(risk.mth, save, options)

source("H:/user/R/RMG/Models/Price/BasisRisk/make.pdf.R")
make.pdf(save, options)















# require(tseries); require(sm)
# ar <- arma(dMM[,4], order=c(1,0))

# smd <- sm.density(dMM[,4:5])


# ind <- which(colnames(dMM) %in% c("year","month","day","date"))
# ind <- (1:length(colnames(dMM)))[-ind]
# X <- dMM[,ind]-dMM[,ind[1]]; X <- X[,-1]
# ar <- arma(X[,4], order=NULL, lag=list(ar=c(1), ma=NULL))
# ar <- arma(X[,4], order=c(1,0))
# summary(ar)

# smd <- sm.density(X[,4:5])



# source("C:/R/Work/MyLibrary/TimeFunctions/bucket.ts.R")
# mMM <- bucket.ts(cbind(year=dMM$year, month=dMM$month, X), "monthly")
# round(mMM,2)

# round(apply(mMM, 2, sd), 2)
# round(apply(X, 2, sd), 2)


# round(apply(mMM, 2, mean), 2)


# stk.dMM <- cbind(stack(X), date=rep(dMM$date, ncol(X)))
# library(Simple)

# simple.violinplot(values ~ ind, data=stk.dMM, col="wheat1")




# round(cor(mMM),2)   # on the monthly spreads
# round(cor(X),2)     # on the daily spreads




# source("C:/R/Work/MyLibrary/TimeFunctions/calculate.season.R")
# aux <- as.Date(paste("1 ",rownames(mMM)),format="%d %b %Y")

# options$shoulder=1
# source("C:/R/Work/MyLibrary/TimeFunctions/calculate.season.R")
# bux <- as.data.frame(cbind(year=format(aux,"%Y"), month=format(aux, "%m")))
# shld <- calculate.season(bux, options)


# stk.mMM <- cbind(stack(mMM), season=rep(shld$season, ncol(mMM)))
# mean.shld <- tapply(stk.mMM$values, list(stk.mMM$ind, stk.mMM$season), mean)
# round(mean.shld, 2)

# plot(mean.shld[,1], pch="1", col="blue", xaxt="n",
#      ylab="Mean of monthly spreads, $",
#      xlab="")
# axis(side=1, 1:ncol(mMM), labels=colnames(mMM))
# points(mean.shld[,2], pch="2", col="red")
# legend(6, max(sd.shld[,1]), pch=c("1","2"), legend=c("Peak Month", "Shoulder Month"),
#        text.col=c("blue", "red"), col=c("blue", "red"), bty="n")



# sd.shld <- tapply(stk.mMM$values, list(stk.mMM$ind, stk.mMM$season), sd)
# round(sd.shld, 2)
# plot(sd.shld[,1], pch="1", col="blue", xaxt="n",
#      ylab="Standard deviation of monthly spreads, $",
#      xlab="")
# axis(side=1, 1:ncol(mMM), labels=colnames(mMM))
# points(sd.shld[,2], pch="2", col="red")
# legend(6, max(sd.shld[,1]), pch=c("1","2"), legend=c("Peak Month", "Shoulder Month"),
#        text.col=c("blue", "red"), col=c("blue", "red"), bty="n")





