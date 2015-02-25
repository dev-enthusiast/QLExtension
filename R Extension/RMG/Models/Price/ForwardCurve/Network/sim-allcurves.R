###############################################
## This is a script to simulate all curves
## and do some checking.
##
## This can be modified to be a demo script.
###############################################

rm(list=ls())
source("H:/user/R/RMG/Utilities/Packages/load.packages.R")
load.packages( "SecDb" )
library(MASS)
library(rstream)
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/util.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/sim.forward.R")
source("H:/user/R/RMG/Statistics/estimate.OU.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/plots.sim.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/reports.sim.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")

## make FP.options
FP.options = ForwardPriceOptions$create()

## simulation for all curves
t0 <- Sys.time()
#load.AllCurves(FP.options=FP.options)
sim.all(FP.options=FP.options)
t1 <- Sys.time() ## 1000 simulations take 3.5 hours
t1-t0

## try simulate just a few curves
curvenames <- c("COMMOD NOX C04 PHYSICAL", "COMMOD FRC AVGTC BCI", "COMMOD FRC PM TC PHYSICAL")
sim.all(curvenames, FP.options)

                
#########################################
## check the simulation results
#########################################
## 1. Check the fuel reference curves:
## single curve plots
curvenames <- c("COMMOD NG EXCHANGE", "COMMOD COL NYMEX PHYSICAL",
                "COMMOD WTI EXCHANGE")
for(i in 1:length(curvenames)) {
  plot.OneCurve(curvenames[i], FP.options, "png")
}
## multiple curves plots
plot.MultiCurve(curvenames, FP.options, "fuel-multicurve", "png")

### report some key statistics
## takes a long time to run report
report.fuel <- reports.sim(curvenames, FP.options)
save(report.fuel, file="report-fuel.rda")


## 2. Check some regional NG curves, these are parents for electricity curves
curvenames <- c("COMMOD NG TENZN6 PHYSICAL", "COMMOD NG TRAZN6 NY PHYSICAL",
                "COMMOD NG DOMSP PHYSICAL")
for(i in 1:length(curvenames)) {
  plot.OneCurve(curvenames[i], FP.options, "png")
}

## multiple curves plots
plot.MultiCurve(curvenames, FP.options, "ng-multicurve", "png", FALSE)

### report 
report.ng <- reports.sim(curvenames, FP.options)
save(report.ng, file="report-ng.rda")

## 3. Check some minor NG curves in different market,
## these are only correlated via their parents

curvenames <- c("COMMOD NG TEN300 PHYSICAL", "COMMOD NG TRZN6 NON PHYSICAL",
                "COMMOD NG COLAPP FERC")
for(i in 1:length(curvenames)) {
  plot.OneCurve(curvenames[i], FP.options, "png")
}

## multiple curves plots
plot.MultiCurve(curvenames, FP.options, "minor-ng-multicurve", "png", FALSE)

### report 
report.minor.ng <- reports.sim(curvenames, FP.options)
save(report.minor.ng, file="report-ng.rda")



## 4. Check some electricity curves
curvenames <- c("COMMOD PWX 5X16 BOS PHYSICAL",
                "COMMOD PWJ 5X16 BGE PHYSICAL",
                "COMMOD PWY 5X16 NYC PHYSICAL")
for(i in 1:length(curvenames)) {
  plot.OneCurve(curvenames[i], FP.options, "png")
}

## multiple curves plots
plot.MultiCurve(curvenames, FP.options, "elec-multicurve", "png", FALSE)

### report 
report.elec <- reports.sim(curvenames, FP.options)
save(report.elec, file="report-elec.rda")


## 5. FRC curves
curvenames <- c("COMMOD FRC AVGTC BCI", "COMMOD FRC PM TC PHYSICAL")
for(i in 1:length(curvenames)) {
  plot.OneCurve(curvenames[i], FP.options, "png")
}

## multiple curves plots
plot.MultiCurve(curvenames, FP.options, "frc-multicurve", "png", FALSE)

### report 
report.frc <- reports.sim(curvenames, FP.options)
save(report.frc, file="report-frc.rda")

### look at heat content
plot.HeatRate("COMMOD PWY 5X16 NYC PHYSICAL", "COMMOD NG TRAZN6 NY PHYSICAL",
                  FP.options, type="png")


plot.HeatRate("COMMOD PWY 5X16 NYC PHYSICAL", "COMMOD NG WAHA PHYSICAL",
                  FP.options, filename="test", type="png")
