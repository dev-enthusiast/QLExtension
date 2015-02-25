# Send the price of the ultra low sulfur for tomorrow to the 
# schedulers.  This job needs to run in the morning before 10AM.  
#
# http://www.spe.org/industry/reference/unit_conversions.php
#
# Written by AAD on 15-May-2009
#
# .getDieselPrice
# .getFuelOilPrice       not used
# .getFuelOilPrice_V2
# .getHeatingOilPrice    not used
# .getKleenOilPrice
# .get_da_power
# .get_rt_power
#

#################################################################
# 
.getDieselPrice <- function(asOfDate)
{
  out <- "ERROR"
  
  # get the Ultra Low Sulfur Diesel NYH Barge close from Platts
  aux <- tsdb.readCurve("Platts_CrudeNYH1_AATGX00c", Sys.Date()-10, Sys.Date())
  spot <- tail(aux$value,1)/100
 
  if (class(spot) != "try-error") {
    val <- spot*7.2103   
    out <- paste("\nDiesel price for tomorrow: ", round(val,2), " $/MMBTU", sep="")
  }

  return(out)  
}





#################################################################
# Added a $7.28/barrel transportation cost per Steve W. on 10/08/2013   
# https://www.hessenergy.com/dashboard/conversionchart.aspx
#
.getMy7OilPrice <- function(asOfDate)
{
  # we contract for 0.5% FO6 oil which we construct by mixing 0.3% and 0.7%
  oil03 <- tsdb.readCurve("platts_crudenyh2_PUAAE00c", asOfDate-30, asOfDate-1) # No6 0.3% High Pour
  oil03 <- tail(oil03, 1)  # price $/BBL
  oil07 <- tsdb.readCurve("platts_crudenyh2_PUAAH00c", asOfDate-30, asOfDate-1) # No6 0.7% High Pour
  oil07 <- tail(oil07, 1)  # price $/BBL
  
  oil05 <- (oil03$value + oil07$value)/2

  adder <- 13.80        # $/BBL updated the transportation adder per Hansel 7/2/2014
  value <- ((oil05 + adder)/42.0)*6.6225  # 1 BBL = 42 galons, 6.6225 galons FO6 = 1 MMBTU

  
  out <- paste("\nMystic7 oil price for tomorrow: ",
               round(value, 2), " $/MMBTU", sep="")
  return(out)  
}



#################################################################
# 
#
.getFuelOilPrice_V2 <- function(asOfDate)
{
  fo6 <- tsdb.readCurve("fo6_spot_northeast", asOfDate-30, asOfDate-1)
  fo6 <- tail(fo6, 1)  # price $/BBL                                                 
  fo6$value <- (fo6$value/42)/0.151  # 1 BBL = 42 galons, 1 BBL FO6 = 6.6225 MMBTU
  out <- paste("\nFO6 price for tomorrow: ",
               round(fo6$value, 2), " $/MMBTU", sep="")
  return(out)  
}

#################################################################
#
.getHeatingOilPrice <- function(asOfDate)
{
  ho <- tsdb.readCurve("platts_hofut_c_promptmonth", asOfDate-30, asOfDate-1)
  ho <- tail(ho, 1)  # price is in $0.0001/Gal 
  ho$value <- (ho$value/10000)*(42/5.67)   # 1 BBL = 42 galons, 1 BBL = 5.67 MMBTU
  out <- paste("\nHeating Oil (Platts) prompt month for ", ho$date, " = ",
               round(ho$value, 2), sep="")
  return(out)  
}


#################################################################
# http://www.buckeye.com/Search_Tariffs/SearchTariffs.aspx
# http://www.buckeye.com/tariffs/BPL%20F438.3.0_Jetlines.pdf
#
# Current (because we fill the tank)
## A) Tsdb value of Platts_CrudeNYH1_AATGX00c  Ultra Low Sulfur
##      Diesel NYH Barge close from Platts, $/Gal
## B) 0.003  $/Gal for dye treatment
## C) 0.1455 $/Gal for adder in the contract (? what contract)
## D) 7.2103 conversion from HO Gal to MMBTU
## E) 1.088 extra 8.8% for CT gross earnings tax
##  
## So formula for Kleen oil price is (A + B + C) * D * E 
#
#  Before 11/22/2013
## A) Tsdb value of Platts_CrudeNYH1_AATGX00c  Ultra Low Sulfur
##      Diesel NYH Barge close from Platts, $/Gal
## B) 0.03 $/Gal for basis adder in the contract (page 7)
## C) 0.5891 $/Gal from Buckeye tariff
## D) 7.2103 conversion from Gal to MMBTU
## E) 1.10 extra 10% from Kleen supplying the oil
## F) 1.075 extra 7.5% for CT gross earnings tax
##  
## So formula for Kleen oil price is (A + B + C) * D * E * F
#
#
#
.getKleenOilPrice <- function(asOfDate)
{
  # see the spreadsheet in Structured Risk/NEPOOL/Temporary/KLEEN_OIL_PRICE.xlsx
  out <- "ERROR"

  # get the Ultra Low Sulfur Diesel NYH Barge close from Platts
  aux <- tsdb.readCurve("Platts_CrudeNYH1_AATGX00c", Sys.Date()-10, Sys.Date())
  A <- tail(aux$value,1)/100
  D <- 7.2103  # convert to $/MMBTU (from SecDb 1 MMBTU = 7.2103 gal HO)
    
  if (class(A) != "try-error") {
    B <- 0.003   # dye
    C <- 0.1455  # contract
    E <- 1.088   # CT tax
    val <- (A + B + C) * D * E   
    out <- paste("\nKleen oil price for tomorrow: ", round(val,2), " $/MMBTU", sep="")
  }

  return(out)  
}


#################################################################
#
.get_da_power <- function(day)
{
  start.dt <- as.POSIXct(paste(format(day), "01:00:00"))
  end.dt <- as.POSIXct(paste(format(day+1), "00:00:00"))
  hub <- tsdb.readCurve("nepool_smd_da_4000_lmp", start.dt, end.dt)
  
  peak <- round(mean(hub$value[8:23]), 2)
  out  <- paste("\nHub DA onpeak price for ",  format(day), " = ", peak, " $/MWh", sep="")

  offpeak <- round(mean(hub$value[c(1:7,24)]), 2)
  out  <- c(out, paste("\nHub DA offpeak price for ",  format(day), " = ", offpeak, " $/MWh",
                       sep=""))

  out  
}

#################################################################
# get previous day rt prices for nema, ct, fore river
# 
.get_rt_power <- function(asOfDate)
{
  day <- asOfDate - 1
  start.dt <- as.POSIXct(paste(format(day), "01:00:00"))
  end.dt <- as.POSIXct(paste(format(asOfDate), "00:00:00"))
  nema <- tsdb.readCurve("nepool_smd_rt_4008_lmp", start.dt, end.dt)
  if (nrow(nema)==0)
    nema <- tsdb.readCurve("nepool_smd_rt5m_4008_lmp", start.dt, end.dt)
  nema <- round(mean(nema$value), 2)
  out  <- paste("\nNEMA RT price for ",  format(day), " = ", nema, " $/MWh", sep="")

  fr1 <- tsdb.readCurve("nepool_smd_rt_40327_lmp", start.dt, end.dt)
  if (nrow(fr1)==0)
    fr1 <- tsdb.readCurve("nepool_smd_rt5m_40327_lmp", start.dt, end.dt)
  fr1 <- round(mean(fr1$value), 2)
  fr2 <- tsdb.readCurve("nepool_smd_rt_40328_lmp", start.dt, end.dt)
  if (nrow(fr2)==0)
    fr2 <- tsdb.readCurve("nepool_smd_rt5m_40328_lmp", start.dt, end.dt)
  fr2 <- round(mean(fr2$value), 2)
  fr  <- (fr1 + fr2)/2
  
  out  <- c(out, paste("\nFORE RIVER 1,2 RT price for ",  format(day), " = ", fr, " $/MWh",
                       sep=""))

  ct <- tsdb.readCurve("nepool_smd_rt_4004_lmp", start.dt, end.dt)
  if (nrow(ct)==0)
    ct <- tsdb.readCurve("nepool_smd_rt5m_4004_lmp", start.dt, end.dt)
  ct <- round(mean(ct$value), 2)
  out  <- c(out, paste("\nCT RT price for ",  format(day), " = ", ct, " $/MWh",
                       sep=""))

  out  
}


#################################################################
options(width=200)  # make some room for the output
options(stringsAsFactors=FALSE) 
require(methods)   
require(CEGbase)
require(SecDb)


rLog(paste("Start proces at ", Sys.time()))
returnCode <- 0    # succeed = 0

asOfDate <- Sys.Date()
rLog(paste("Running report for", as.character(asOfDate)))
rLog("\n")

out <- c(try(.getDieselPrice(asOfDate)),
  try(.getKleenOilPrice(asOfDate)),
  try(.getMy7OilPrice(asOfDate)),       
  try(.getFuelOilPrice_V2(asOfDate)),
  try(.get_rt_power(asOfDate)),
  try(.get_da_power(asOfDate)),
  paste("\n\n\nPlease contact Adrian Dragulescu for questions regarding",
    "this procmon job."),
  paste("Procmon job:",
    "RMG/R/Reports/Energy/Trading/Congestion/dailyDieselPriceMaine"))

rLog("Email results")
sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
  paste(#getEmailDistro("dailyDieselPriceMaine"),
    "john.waire@constellation.com", "ashutosh.tiwarie@constellation.com",    
    "EastDesk@constellation.com", "pat.moyles@constellation.com", 
    "mark.grear@exeloncorp.com", "robert.pleiss@constellation.com",
    "heather.long@constellation.com", "dave.hansel@constellation.com", 
    "donald.schopp@constellation.com", "andrew.hlasko@constellation.com", sep=", "),
#  "adrian.dragulescu@constellation.com", 
  paste("Nepool delivered fuel prices for ", as.character(asOfDate+1)), out, 
  cc="NECash@constellation.com")

rLog(paste("Done at ", Sys.time()))

returnCode <- 0        # make it succeed.  
if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}





## #################################################################
## # Before 3/15/2013
## .getDieselPrice <- function(asOfDate)
## {
##   out <- "ERROR"

##   #contractDt <- as.Date(format(asOfDate, "%Y-%m-01"))
##   # It stopped getting updated during the cash month! I switch to prompt.
##   contractDt <- as.Date(secdb.dateRuleApply(asOfDate, "+1e"))+1
##   reutersCode <- format.dateMYY(contractDt, -1)
##   contract <- gsub("COMMOD ", "", "COMMOD HO LS NYH CFOB PLTM")
  
##   splitNames <- strsplit(contract, " ")
##   prefix <- sapply(splitNames, "[", 1)
##   suffix <-  lapply( splitNames, "[", -1 )
##   suffixStrings <- sapply( suffix, paste, collapse = " " )
##   contract <- paste(prefix, reutersCode, " ", suffixStrings, sep="")

##   # you can get this value by opening a Swap calculator
##   try(spot <- secdb.getValueType( contract, "Spot" ))

##   if (class(spot) != "try-error"){
##     # the price of Ultra Low Sulfur Diesel has a 0.22 $/gal premium to Low Sulfur Diesel
##     # convert to $/MMBTU by dividing by 0.13869
##     cat("SecDb value: ", spot, "\n")
##     val <- spot/0.139 + 0.22   
##     out <- paste("\nDiesel price for tomorrow: ", round(val,2), " $/MMBTU", sep="")
##   }

##   return(out)  
## }



## #################################################################
## #
## .getFuelOilPrice <- function(asOfDate)
## {
##   out <- "ERROR"
  
##   contractDt <- as.Date(format(asOfDate, "%Y-%m-01"))
##   reutersCode <- format.dateMYY(contractDt, -1)
##   contract <- gsub("COMMOD ", "", "COMMOD FO1 NYH CFOB PLTM")
  
##   splitNames <- strsplit(contract, " ")
##   prefix <- sapply(splitNames, "[", 1)
##   suffix <- lapply(splitNames, "[", -1)
##   suffixStrings <- sapply( suffix, paste, collapse = " " )
##   contract <- paste(prefix, reutersCode, " ", suffixStrings, sep="")

##   # you can get this value by opening a Swap calculator
##   try(spot <- secdb.getValueType( contract, "Spot" ))

##   if (class(spot) != "try-error"){
##     # the SecDb price is in $/barrel.  To convert to deliveded F06 in $/MMBTU
##     # 
##     cat("SecDb value: ", spot, "\n")
##     val <- (spot/42)/0.15 + 0.60   
##     out <- paste("\nFO6 price for tomorrow: ", round(val,2), " $/MMBTU", sep="")
##   }

##   return(out)  
## }
