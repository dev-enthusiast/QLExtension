rm(list=ls())
options <- NULL
options$DT <- 11
options$percentile <- 5
#options$showPlots <- 0 # for show plots
options$showPlots <- 1 # not show plots
load("S:/Risk/2006,Q3/Risk.Analytics/temp/4Heston.Rdata")
y <- xxx # this is an awkward way to load values (not fluctuations)
source("H:/user/R/RMG/Finance/Heston/HestonModel.R")
xxx <- HestonModel(y, options)