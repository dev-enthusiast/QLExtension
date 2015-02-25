
rm(list=ls())
options             <- NULL
options$nFieldLimit <- 800
options$listFields <- c("VAR", "DELTA")
#options$listFields <- c("DELTA")
require(reshape)
require(RODBC)
source("H:/user/R/RMG/TimeFunctions/add.date.R")
today <- as.Date(format(Sys.time(), "%Y-%m-%d"))  
#today <- as.Date("2007-08-01") 
options$dates <- add.date(today, "-1 b")
options$dates <- as.Date("2008-02-08")
options$portfolio   <- "Mark Orman Netted Portfolio"
source("H:/user/R/RMG/Energy/Trading/PortfolioAnalysis/mainPortfolioReportUpstreamExcludeCEP.r")
runtime <- as.numeric(format(Sys.time(), "%H%M"))
if (runtime <= 1900) {
  mainPortfolioReportUpstreamExcludeCEP(options)
}