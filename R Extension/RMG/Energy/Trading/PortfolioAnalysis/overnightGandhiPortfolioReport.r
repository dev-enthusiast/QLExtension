Sys.setenv( portfolio="RMG Mark Orman" )

source("H:/user/R/RMG/Energy/VaR/Overnight/run.one.VaR.R")



rm(list=ls())
options             <- NULL
options$nFieldLimit <- 800
options$listFields <- c("VAR", "VARVEGA", "VEGA", "VARDELTA", "DELTA")
require(reshape)
require(RODBC)
source("H:/user/R/RMG/TimeFunctions/add.date.R")
today <- as.Date(format(Sys.time(), "%Y-%m-%d"))  
#today <- as.Date("2007-07-27")  
options$dates <- add.date(today, "-1 b")
options$portfolio   <- "Mark Orman Netted Portfolio"
source("H:/user/R/RMG/Energy/Trading/PortfolioAnalysis/mainPortfolioReportF1.r")
runtime <- as.numeric(format(Sys.time(), "%H%M"))
if (runtime <= 1900) {
  mainPortfolioReportF1(options)
}
