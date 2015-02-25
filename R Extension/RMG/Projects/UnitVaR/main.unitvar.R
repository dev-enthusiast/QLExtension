# UnitVar
# written by Adrian Dragulescu on 16-Aug-2005
# Modified by Wang Yu on 02-Feb-2006

rm(list=ls())

library(R.matlab)

# for speed reason
setwd("C:/R/R-2.2.0")

options <- NULL

options$save <- 1
options$savefile<- "S:/All/Risk/Analysis/UnitVaR/Template.csv"

options$AsOfDate <- "02Feb2006"
options$DateParms$StartCon <- "01Mar2006"
options$DateParms$EndCon <- "31Dec2010"
# Find a date 6 months before asOfDate 
# Strictly speaking, 70 business days in NEAR calendar
Diff <- 180*24*60*60
options$DateParms$StartDT <-  format(strptime(options$asOfDate,"%d%b%Y")-Diff,"%d%b%Y") 
options$DateParms$EndDT <- options$asOfDate 

options$DateParms$TInterval <- list(Sum06 = strptime(c("01Apr2006","01Jul2006"),"%d%b%Y"),
                                    Cal07 = strptime(c("01Jan2007","01Dec2007"),"%d%b%Y"))

options$Commods <-  c("NG_EXCHANGE",
                      "NG_TENZN6_PHYS", 
                      "NG_TRAZN6_PHYS",
                      "NG_TRZN6_NON_PHYS",
                      "NG_TEN200_PHYS",
                      "NG_SOBCAL_GASDLY",
                      "NG_ELPPER_PHYS",
                      "NG_SOBCAL_NGI",
                      "NG_NIAG_PHYS",
                      "PWJ_5x16_West_PHYS",
                      "PWX_5x16_PHYS",
                      "PWY_5x16_West_PHYS")
                      
source("H:/user/R/RMG/Projects/UnitVaR/FutCurve.R")
source("H:/user/R/RMG/Projects/UnitVaR/calculate.VaR.R")
                
MatlabServer=Matlab(host="localhost",port=9999)
# MatlabServer=Matlab(host="pcd1695",port=9999,remote=TRUE)
# MatlabServer=Matlab(host="PCD1695.Ceg.Corp.net",port=9999,remote=TRUE)
# MatlabServer=Matlab(host="10.100.106.65",port=9999,remote=TRUE)
if (!open(MatlabServer))
    throw("MatlabServer is not Running: Waited 30 seconds")

UnitVaR <- matrix(data=NA,nrow=length(options$DateParms$TInterval),
                          ncol=length(options$Commods),
                          dimnames = list(names(options$DateParms$TInterval),options$Commods))
                          
                                                                                                       
for (Commod in options$Commods)
    {

            FutInfo <-  FutCurve(MatlabServer,Commod,options$DateParms)                
            FutInfo$pricing.dts  <- strptime(FutInfo$FutDates,"%Y-%m-%d")
            FutInfo$AsOfDate <- strptime(options$AsOfDate, "%d%b%Y")
            FutInfo$exp.dts <- strptime(FutInfo$FutExp, "%Y-%m-%d")
            cols <- list()
            for (index in 1:length(options$DateParms$TInterval))
            {
               cols[[index]] <- which(FutInfo$exp.dts >= options$DateParms$TInterval[[index]][1] &
                                      FutInfo$exp.dts <= options$DateParms$TInterval[[index]][2] )                                 
                                 
            }
           names(cols)<- names(options$DateParms$TInterval)
           
           FutInfo$contract.dts <- cols
           UnitVaR[,Commod] <- calculate.VaR(FutInfo)
     }     

close(MatlabServer) 

write.csv(UnitVaR,file = options$savefile)
   