options <- NULL
options$asOfDate      <- as.Date("2007-06-28")
options$previousDate  <- as.Date("2007-06-27")
options$portfolio <- "POWER Trading Portfolio"
#options$portfolio <- "Mark Orman Netted Portfolio"
#options$portfolio <- "NEPOOL Accrual Portfolio"
options$trader     <- NA
#options$varMapping <- "Jeff Addison"
options$varMapping <- NA
source("H:/user/R/RMG/Energy/Trading/DailyOptionReport/mainOptionReport.R")
res <- mainOptionReport(options)