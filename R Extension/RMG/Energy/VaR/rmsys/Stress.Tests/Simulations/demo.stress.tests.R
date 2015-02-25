#
#
#
#

rm(list=ls())
ST.options <- NULL
ST.options$asOfDate   <- as.Date("2007-12-24")
ST.options$pricing.dt <- as.Date("2008-01-07")
ST.options$run$run.name <- "NYTRADE"   # put book, portfolio name, etc.
ST.options$save$datastore.dir <- "//ceg/cershare/All/Risk/Data/VaR/prod/"
ST.options$sim.filehash.dir <- paste("//NAS-MSW-07/rmgapp/SimPrices/Network/",
  ST.options$asOfDate, "/overnight/result/", sep="")



source("H:/user/R/RMG/Energy/Stress.Tests/Simulations/STCalculator.R")
QQ  <- STCalculator$.loadPositions(ST.options)

sPP <- STCalculator$.getSimPrices(ST.options$pricing.dt, QQ, sPP.env, ST.options)
#old.sPP = sPP

ST.options$PnL <- NULL
ST.options$PnL$aggregate <- TRUE
#ST.options$PnL$by.month <- as.Date(c("2008-01-01", "2008-02-01"))
#ST.options$PnL$by.curve <- c("COMMOD PWY 5X16 WEST PHYSICAL", "COMMOD NG EXCHANGE")

PnL <- STCalculator$calc.PnL(QQ, sPP, ST.options)
PnL <- PnL/1000000  # in millions

summary(PnL)

# what to keep
ST.options$keep$curves <- c("COMMOD NG EXCHANGE",
                            "COMMOD PWJ 5X16 WEST PHYSICAL",
                            "COMMOD PWQ 5X16 ONT PHYSICAL",
                            "COMMOD PWX 5X16 PHYSICAL", 
                            "COMMOD PWY 5X16 WEST PHYSICAL",
                            "COMMOD PWY 5X16 NYC PHYSICAL",
                            "COMMOD PWY 5X16 CEN PHYSICAL",
                            "COMMOD PWY 5X16 HUDV PHYSICAL")


sPP.agg <- STCalculator$.prepareGGOBI(sPP, ST.options)
sPP.agg <- data.frame(sPP.agg, PnL=PnL)


g <- ggobi(sPP.agg)

display(g[1], vars="2D Tour")







ind.bad <- which(PnL < -20)

sPP.bad <- sPP[,,ind.bad]





