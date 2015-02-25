rm(list = ls(all=TRUE))
options <- NULL
options$curve.power <- c("COMMOD PWX 5X16 PHYSICAL", "COMMOD PWJ 5X16 WEST PHYSICAL", "COMMOD PWY 5X16 NYC PHYSICAL",
  "COMMOD PWO 5X16 SC PHYSICAL", "COMMOD PWM 6X16 PHYSICAL", "COMMOD PWC 6X16 NP15 PHYSICAL", 
  "COMMOD PWP 6X16 SP15 PHYSICAL", "COMMOD PWP 6X16 PHYSICAL")
options$curve.powerShortNames <- c("NEPOOL", "PJM WEST", "NY ZONE J", "ERCOT SC", "MID COLUMBIA", "CA NP15", "CA SP15", 
  "PALO VERDE")  
options$curve.NG <- c("COMMOD NG EXCHANGE", "COMMOD NG TETZM3 PHYSICAL", "COMMOD NG DOMSP PHYSICAL",
  "COMMOD NG TRAZN6 NY PHYSICAL", "COMMOD NG HSCBEA PHYSICAL", "COMMOD NG MALIN PHYSICAL", 
  "COMMOD NG PGECG PHYSICAL", "COMMOD NG SOBCAL PHYSICAL", "COMMOD NG ELPSJB PHYSICAL") 
options$curve.NG.ShortNames <- c("NYMEX", "TETCO M3", "DOMINION SOUTH", 
  "TRANSCO Z6 (NY)", "HOUS SHIP CHAN", "MALIN",
  "PGE CITY GATE", "SOCAL BORDER", "SAN JUAN BASIN")   
options$HR.Power2NG <- c("COMMOD NG TETZM3 PHYSICAL", "COMMOD NG DOMSP PHYSICAL",
  "COMMOD NG TRAZN6 NY PHYSICAL", "COMMOD NG HSCBEA PHYSICAL", "COMMOD NG MALIN PHYSICAL", 
  "COMMOD NG PGECG PHYSICAL", "COMMOD NG SOBCAL PHYSICAL", "COMMOD NG ELPSJB PHYSICAL") 
source("H:/user/R/RMG/TimeFunctions/add.date.R")  
today <- as.Date(format(Sys.time(), "%Y-%m-%d"))   
options$pricing.dt.start  <- add.date(today, "-5 b")
options$pricing.dt.end    <- add.date(today, "-1 b") 

if(weekdays(options$pricing.dt.end) == "Thursday") {
#if(weekdays(options$pricing.dt.end) == "Wednesday") {
  options$dayChange <- 4
} else {
  options$dayChange <- 1  
}  
options$contract.dt.start <- as.Date(paste(format(options$pricing.dt.end, "%Y-%m"), "-01", sep = ""))   
options$contract.dt.end   <- as.Date("2010-12-01")   #optional  
options$runtime <- as.numeric(format(Sys.time(), "%H%M"))
#options$runtime <- 1640
if (options$runtime <= 1630) {                                                                   
  options$dirOut <- paste("S:/All/Risk/Reports/PriceChangeReport/", as.character(options$pricing.dt.end), "/", sep = "")
} else {
  options$dirOut <- paste("S:/All/Risk/Reports/PriceChangeReport/", as.character(format(Sys.time(), "%Y-%m-%d")), "Unaudited/", sep = "")
}
dir.create(options$dirOut,  showWarnings = FALSE)
if(file.exists(paste(options$dirOut,"tableHeader.tex", sep = "")) == FALSE) {
  source("H:/user/R/RMG/Energy/Trading/PriceAnalysis/mainPriceAnalysisReportA.R")
  mainPriceAnalysisReportA(options) 
}  