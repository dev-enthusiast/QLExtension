rm(list=ls())
options             <- NULL
options$asOfDate           <- as.Date("2007-07-02")
options$pricingAsOfDate    <- as.Date("2007-07-02") # back-testing if less th asOfDate
options$portfolio   <- "Power Trading Portfolio"
options$books       <- NA
options$trader      <- NA
options$commodities <- NA
#options$months      <- as.Date(c("2006-11-01", "2006-12-01"))
options$regions     <- NA
options$varMapping  <- "Jeff Addison"
#options$varMapping  <- "Joe Kirkpatrick_NY"
options$N  <- 20
options$nHist <- 70
options$nFieldLimit <- 800
options$dirOut <- paste('S:/Risk/Portfolio Management/OptionData/', options$varMapping, "/", 
                        as.character(options$asOfDate), sep = "")
source("H:/user/R/RMG/Energy/Trading/PositionReport/mainPositionReport.R")
mainPositionReport(options)