# Analyze the risk of an LMP swap.  You have a swap between two LMP locations
#
#
# Written by Adrian Dragulescu on 28-Oct-2004

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Transport/TetcoM3.to.TenZn6/"
options$shortNames   <- c("TetcoM3","TenZn6") # short column names
options$hdata.startDate <- as.Date("2002-01-01")
options$hdata.endDate   <- as.Date("2007-02-05")
options$hdata.symbols   <- c("GasDailyMean_TETM3", "GasDailyMean_TENN6")
options$deal.start   <- as.Date("2007-02-01")
options$deal.end     <- as.Date("2008-01-31")
#options$omit.months  <- c("2002-05", "2003-04", "2002-04")
#options$use.data.from <- as.Date("2002-01-01")
options$tsdb.refresh  <- 0
options$demand.charge <- 0.00
options$fuel.charge   <- 0.00

source("H:/user/R/RMG/Models/Gas/Transport/main.analyze.transport.R")
main.analyze.transport(options,save)

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Transport/Sta65.to.Sta85/"
options$shortNames   <- c("Station65","Station85") # short column names
options$hdata.startDate <- as.Date("2001-01-01")
options$hdata.endDate   <- as.Date("2006-07-31")
options$hdata.symbols   <- c("GasDailyMean_TRS65", "GasDailyMean_TRS85")
options$deal.start   <- as.Date("2006-08-01")
options$deal.end     <- as.Date("2007-07-31")
#options$omit.months  <- c("2002-05", "2003-04", "2002-04")
#options$use.data.from <- as.Date("2002-01-01")
options$tsdb.refresh  <- 0
options$demand.charge <- 0.00
options$fuel.charge   <- 0.00

source("H:/user/R/RMG/Models/Gas/Transport/main.analyze.transport.R")
main.analyze.transport(options,save)

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Transport/DOMSP.to.TRNON/"
options$shortNames   <- c("DomSP","Transco NonNY") # short column names
options$hdata.startDate <- as.Date("2002-01-01")
options$hdata.endDate   <- as.Date("2006-01-31")
options$hdata.symbols   <- c("GasDailyMean_DOMS", "GasDailyMean_TRNON")
options$deal.start   <- as.Date("2006-04-01")
options$deal.end     <- as.Date("2007-03-31")
#options$omit.months  <- c("2002-05", "2003-04", "2002-04")
#options$use.data.from <- as.Date("2002-01-01")
options$tsdb.refresh  <- 1
options$demand.charge <- 0.19
options$fuel.charge   <- 0.01

source("H:/user/R/RMG/Models/Gas/Transport/main.analyze.transport.R")
main.analyze.transport(options,save)



#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Transport/NIAG.to.TRNON/"
options$shortNames   <- c("Niagara","Transco NonNY") # short column names
options$hdata.startDate <- as.Date("2002-01-01")
options$hdata.endDate   <- as.Date("2006-01-31")
options$hdata.symbols   <- c("GasDailyMean_NIAG", "GasDailyMean_TRNON")
options$deal.start   <- as.Date("2006-04-01")
options$deal.end     <- as.Date("2007-03-31")
#options$omit.months  <- c("2002-05", "2003-04", "2002-04")
#options$use.data.from <- as.Date("2002-01-01")
options$tsdb.refresh  <- 1
options$demand.charge <- 0.19
options$fuel.charge   <- 0.01

source("H:/user/R/RMG/Models/Gas/Transport/main.analyze.transport.R")
main.analyze.transport(options,save)

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Transport/TX.to.LA/"
options$shortNames   <- c("NGPLSTX","TexasGasSL") # short column names
options$hdata.startDate <- as.Date("2002-01-01")
options$hdata.endDate   <- as.Date("2005-08-31")
options$hdata.symbols   <- c("GasDailyMean_NGPLS", "GasDailyMean_TGTSL")
options$deal.start   <- as.Date("2006-04-01")
options$deal.end     <- as.Date("2007-03-31")
#options$omit.months  <- c("2002-05", "2003-04", "2002-04")
#options$use.data.from <- as.Date("2002-01-01")
options$tsdb.refresh  <- 1
options$demand.charge <- 0.19
options$fuel.charge   <- 0.01

source("C:/R/Work/Energy/Models/Transport/main.analyze.transport.R")
main.analyze.transport(options,save)

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Transport/Waha-Katy/"
options$shortNames   <- c("Waha","Katy") # short column names
options$hdata.startDate <- as.Date("1998-01-01")
options$hdata.endDate   <- as.Date("2005-01-31")
options$hdata.symbols   <- c("GasDailyMean_Waha", "GasDailyMean_Katy")
options$deal.start   <- as.Date("2005-09-01")
options$deal.end     <- as.Date("2010-08-31")
#options$omit.months  <- c("2002-05", "2003-04", "2002-04")
options$use.data.from <- as.Date("2002-01-01")
options$tsdb.refresh  <- 0
options$demand.charge <- 0.16
options$fuel.charge   <- 0.01

source("C:/R/Work/Energy/Models/Transport/main.analyze.transport.R")
main.analyze.transport(options,save)

