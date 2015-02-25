
setwd("S:/Risk/2006,Q3/Risk.Analytics/Historical.PnL/")
options=NULL
options$fileNameCore <- "Daily_PNL_Estimate"
options$startDate <- "01/01/2006"
options$endDate   <- "06/30/2006"
options$dir <- "S:/Risk/2006,Q3/Risk.Analytics/Historical.PnL/reports/mgmt" # location of PDFfile

source("H:/User/R/RMG/Energy/Trading/Historical.Pnl/main.TXT2CSV.R")
main.TXT2CSV(options)

