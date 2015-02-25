# Compare the historical spread with the current marks
#
#
# Written by Adrian Dragulescu on 28-Oct-2004

#-------------------------------------------------------------------------
rm(list=ls())
SP.options <- NULL

SP.options$asOfDate    <- as.Date("2007-12-31")   
SP.options$start.dt    <- as.Date("2005-01-01")
SP.options$end.dt      <- as.Date("2007-12-31")
SP.options$curves[[1]] <- list(shortName       = "GTC",
                               tsdb            = "GTC_DA_Price_GTC",
                               commod.curve    = "COMMOD PWS 5X16 GTC PHYSICAL",
                               bucket          = "5X16",
                               region          = "SERC")
SP.options$curves[[2]] <- list(shortName       = "SOCO",
                               tsdb            = "MWDaily_16HrDly_SouthernInto",
                               commod.curve    = "COMMOD PWS 5X16 SETM PHYSICAL",
                               bucket          = "5X16",
                               region          = "SERC")
names(SP.options$curves) <- SP.options$shortNames

source("H:/user/R/RMG/Energy/Auditors/Historical.Spread/analyze.spread.R")
SP.Analysis$main( SP.options )

#-------------------------------------------------------------------------
rm(list=ls())
SP.options <- NULL

SP.options$asOfDate    <- as.Date("2007-12-31")   
SP.options$start.dt    <- as.Date("2005-01-01")
SP.options$end.dt      <- as.Date("2007-12-31")
SP.options$curves[[1]] <- list(shortName       = "GTC",
                               tsdb            = "GTC_DA_Price_GTC",
                               commod.curve    = "COMMOD PWS 7X8 GTC PHYSICAL",
                               bucket          = "7X8",
                               region          = "SERC")
SP.options$curves[[2]] <- list(shortName       = "SOCO",
                               tsdb            = "MWDaily_opdly_SouthernInto",
                               commod.curve    = "COMMOD PWS 7X8 SETM PHYSICAL",
                               bucket          = "7X8",
                               region          = "SERC")
names(SP.options$curves) <- SP.options$shortNames

source("H:/user/R/RMG/Energy/Auditors/Historical.Spread/analyze.spread.R")
SP.analysis$main( SP.options )

#-------------------------------------------------------------------------
rm(list=ls())
SP.options <- NULL

SP.options$asOfDate    <- as.Date("2007-12-31")   
SP.options$start.dt    <- as.Date("2005-01-01")
SP.options$end.dt      <- as.Date("2007-12-31")
SP.options$curves[[1]] <- list(shortName       = "GTC",
                               tsdb            = "GTC_DA_Price_GTC",
                               commod.curve    = "COMMOD PWS 2X16H GTC PHYSICAL",
                               bucket          = "2X16H",
                               region          = "SERC")
SP.options$curves[[2]] <- list(shortName       = "SOCO",
                               tsdb            = "MWDaily_16HrDly_SouthernInto",
                               commod.curve    = "COMMOD PWS 2X16H SETM PHYSICAL",
                               bucket          = "2X16H",
                               region          = "SERC")
names(SP.options$curves) <- SP.options$shortNames

source("H:/user/R/RMG/Energy/Auditors/Historical.Spread/analyze.spread.R")
SP.analysis$main( SP.options )



#-------------------------------------------------------------------------
rm(list=ls())
SP.options <- NULL

SP.options$shortNames <- c("SOCO", "Entergy") # short column names
SP.options$region     <- NA       # for bucket definitions, set to NA if you don't need to bucket
SP.options$remove.weekends <- TRUE  # remove weekends from historical data 
SP.options$start.dt <- as.Date("2005-01-01")
SP.options$end.dt   <- as.Date("2007-12-31")
SP.options$hdata.symbols   <- c("MWDaily_16HrDly_SouthernInto", "mwdaily_16hrdly_entergyinto")
SP.options$commod.curve    <- c("COMMOD PWS 5X16 SETM PHYSICAL", "COMMOD PWN 5X16 PHYSICAL")
SP.options$asOfDate        <- as.Date("2007-12-31") 

source("H:/user/R/RMG/Energy/Auditors/Historical.Spread/analyze.spread.R")
SP.analysis$main( SP.options )

#-------------------------------------------------------------------------
rm(list=ls())
SP.options <- NULL

SP.options$shortNames <- c("CT", "Hub") # short column names
SP.options$region     <- "NEPOOL"       # for bucket definitions
SP.options$start.dt <- as.Date("2005-01-01")
SP.options$end.dt   <- as.Date("2007-12-31")
SP.options$hdata.symbols   <- c("NEPOOL_SMD_DA_4004_LMP", "NEPOOL_SMD_DA_4000_LMP")
SP.options$commod.curve    <- c("COMMOD PWX 5X16 CT PHYSICAL", "COMMOD PWX 5X16 PHYSICAL")
SP.options$asOfDate        <- as.Date("2007-12-31") 

source("H:/user/R/RMG/Energy/Auditors/Historical.Spread/analyze.spread.R")
SP.analysis$main( SP.options )




## SP.options$deal.start   <- as.Date("2007-01-01")
## SP.options$deal.end     <- as.Date("2007-12-31") 
## SP.options$omit.months  <- c("2003-08")
## SP.options$use.data.from <- as.Date("2003-01-01")
## SP.options$focus.months <- c(1,7,8)
