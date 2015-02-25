rm(list=ls())
require(reshape); require(SecDb); require(MASS)
source("H:/user/R/RMG/Energy/Systems/CER/VLR/utils.R")

options <- NULL
# how many days you should have before extending
options$min.no.overlap.days <- 365
options$aggregate$start.dt  <- as.Date("2006-10-01")
options$aggregate$end.dt    <- as.Date("2007-09-30")
options$aggregate$days <- seq(options$aggregate$start.dt,
        options$aggregate$end.dt, by="day" )

options$output.base.dir <- "S:/All/Risk/Projects/CER.Integration/VLR/"
options$alarm.no.hdays <- 10 # if you have less than 10 points for
# the LT regression, report.

require(RODBC)
dir <- "//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/"
con.str <- paste("FileDSN=",dir,"CERINT.dsn",
        ";UID=cer_pmo_ro;PWD=cerpmoro;", sep="")
options$con <- odbcDriverConnect(con.str)  

BUs = 1:10

data = NULL
for( businessUnit in BUs )
{
    rLog("BU:", businessUnit )
    RR <- get.all.edc.and.rateclasses(businessUnit, options)
    rLog("nrows:", nrow(RR))
    if(!is.null(RR) && nrow(RR)!=0)
    {
        RR$BU = businessUnit
        data = rbind(data, RR)
    }
}

map = data.frame( BU=c(1:10), 
        BUSUNIT_NAME=c("TORONTO", "OHIO-Unused", "METRO NORTH", "GREAT LAKES", 
                "MID ATLANTIC", "NEW ENGLAND", "TEXAS", "ECIMS", 
                "ALBERTA", "CALIFORNIA"))

