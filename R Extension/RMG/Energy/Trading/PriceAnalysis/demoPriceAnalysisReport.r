rm(list = ls(all=TRUE))
options <- NULL
options$curve.power <- c("COMMOD PWX 5X16 PHYSICAL", "COMMOD PWJ 5X16 WEST PHYSICAL", "COMMOD PWY 5X16 NYC PHYSICAL",
  "COMMOD PWO 5X16 SC PHYSICAL", "COMMOD PWM 6X16 PHYSICAL", "COMMOD PWC 6X16 NP15 PHYSICAL", 
  "COMMOD PWP 6X16 SP15 PHYSICAL", "COMMOD PWP 6X16 PHYSICAL")
options$curve.powerShortNames <- c("NEPOOL", "PJM WEST", "NY ZONE J", "ERCOT SC", "MID COL", "CA NP15", "CA SP15", 
  "PALO VER")  
options$curve.NG <- c("COMMOD NG EXCHANGE", "COMMOD NG TETZM3 PHYSICAL", "COMMOD NG DOMSP PHYSICAL",
  "COMMOD NG TRAZN6 NY PHYSICAL", "COMMOD NG HSCBEA PHYSICAL", "COMMOD NG MALIN PHYSICAL", 
  "COMMOD NG PGECG PHYSICAL", "COMMOD NG SOBCAL PHYSICAL", "COMMOD NG ELPSJB PHYSICAL") 
options$curve.NG.ShortNames <- c("NYMEX", "TETCO M3", "DOMINIAN SOUTH", 
  "TRANSCO Z6 (NY)", "HOUS SHIP CHAN", "MALIN",
  "PGE CITY GATE", "SOCAL BORDER", "SAN JUAN BASIN")   
options$HR.Power2NG <- c("COMMOD NG TETZM3 PHYSICAL", "COMMOD NG DOMSP PHYSICAL",
  "COMMOD NG TRAZN6 NY PHYSICAL", "COMMOD NG HSCBEA PHYSICAL", "COMMOD NG MALIN PHYSICAL", 
  "COMMOD NG PGECG PHYSICAL", "COMMOD NG SOBCAL PHYSICAL", "COMMOD NG ELPSJB PHYSICAL")  
source("H:/user/R/RMG/TimeFunctions/add.date.R")  
options$pricing.dt.end    <- as.Date("2007-03-16")  
options$pricing.dt.start  <-  add.date(options$pricing.dt.end, "-4 b")   
options$contract.dt.start <- as.Date(paste(format(options$pricing.dt.end, "%Y-%m"), "-01", sep = ""))
options$contract.dt.end   <- as.Date("2010-12-01") 
options$dirOut <- paste("S:/All/Risk/Reports/PriceChangeReport/", as.character(options$pricing.dt.end), "/", sep = "")
dir.create(options$dirOut,  showWarnings = FALSE)
source("H:/user/R/RMG/Energy/Trading/PriceAnalysis/mainPriceAnalysisReportA.R")
mainPriceAnalysisReportA(options) 