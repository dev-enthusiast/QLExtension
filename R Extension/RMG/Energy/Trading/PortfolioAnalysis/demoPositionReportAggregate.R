rm(list=ls())
options             <- NULL
options$asOfDate           <- as.Date("2007-02-22")
options$pricingAsOfDate    <- as.Date("2007-02-22") # back-testing if less th asOfDate
options$portfolio   <- "Power Trading Portfolio"
options$books       <- NA
options$trader      <- NA
options$regions     <- NA
options$varMapping  <- "Jeff Addison"
#options$varMapping  <- "Joe Kirkpatrick_NY"
options$N  <- 20
options$nHist <- 70
options$dirOut <- paste('S:/Risk/2006,Q4/Risk.Analytics/', options$varMapping, "/", 
                        as.character(options$asOfDate), sep = "")
options$nWeight <- 10 
options$DT <- 5
options$percentile <- 5 
options$nFieldLimit <- 800
options$showPlots = FALSE # show plots
options$RefineTimeInterval = FALSE                    
source("H:/user/R/RMG/Energy/Trading/PortfolioAnalysis/mainPositionReportAggregate.R")
mainPositionReportAggregate(options)