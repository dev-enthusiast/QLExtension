# Small code snippets that shows you how to run the simulations.
#
#
#

rm(list=ls())
asOfDate <- "2007-11-15"
run.name <- "test_AAD_2"       # can be "overnight" if you want the official run
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
FP.options <- ForwardPriceOptions$create(asOfDate=asOfDate,
   isInteractive=TRUE, run.name=run.name)
setLogLevel(RLoggerLevels$LOG_DEBUG)

FP.options$nsim <- 1000

curvenames <- c("COMMOD NG EXCHANGE")
## curvenames <- "COMMOD NG FLOZN3 FERC"  # weird error... probably the pricehash
## curvenames <- c("COMMOD NG FGTZN3 FERC", "COMMOD NG SONZ3CG PHYSICAL",
##                 "COMMOD NG FLOZN3 FERC", "COMMOD NG EFFINGHAM PHYSICAL",
##                 "COMMOD NG DESTIN PETAL PHYSICAL")   # parent has shorter history 

## curvenames <- "COMMOD PWC 6X16 NP15 INDEX"
## curvenames <- "COMMOD FCO API2 INDEX"
## curvenames <- c("COMMOD NOX C09 EXCHANGE", "COMMOD NOX Y09 EXCHANGE")
## curvenames <- "COMMOD PWJ 5X16 WEST PHYSICAL"
## curvenames <- "COMMOD PWX 7X24 ISOFEE PHYSICAL"
## curvenames <- c("COMMOD NG FLOZN3 FERC")  # parent not significant! 

source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/sim.forward.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/util.R")
sim.all(curvenames, FP.options)

source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/reports.sim.R")
reports.sim(curvenames, FP.options, "txt")


curvenames <- c("COMMOD PWJ 5X16 WEST PHYSICAL", "COMMOD NG EXCHANGE",
                "COMMOD WTI EXCHANGE", "COMMOD COL NYMEX PHYSICAL",
                "COMMOD NG DOMSP PHYSICAL")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/plots.sim.R")
for (ind in 1:length(curvenames))
  plot.OneCurve(curvenames[ind], FP.options, "pdf")


source("H:/user/R/RMG/Statistics/estimate.OU.R")

  tmp <- diff.bday(FP.options$sim.days)
  T <- length(which(tmp==1)) + 1


##########################################################################

#---------------------------------find a set of curvenames----------------
path <- paste("S:/All/Risk/Data/VaR/prod/Positions/forc.positions.",
              asOfDate, ".RData", sep="")
load(path)
uCurves <- unique(QQ.all$CURVE_NAME);  rm(QQ.all)
#curvenames <- uCurves[grep("COMMOD NG ", uCurves)]
curvenames <- uCurves[grep("COMMOD NOX ", uCurves)]
curvenames <- curvenames[1:8]
