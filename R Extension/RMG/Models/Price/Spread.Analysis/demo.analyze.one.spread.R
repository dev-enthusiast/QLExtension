# Analyze the risk of an LMP swap.  You have a swap between two LMP locations
#
#
# Written by Adrian Dragulescu on 28-Oct-2004

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Spread/PJM/BC-APS/"
options$shortNames   <- c("APS","BC") # short column names
options$region <- "PJM"
options$hdata.startDate <- as.Date("2002-04-01")
options$hdata.endDate   <- as.Date("2005-06-30")
options$hdata.symbols   <- c("PJMDAM_APS_ZONE", "PJMDAM_BC_ZONE")
options$deal.start   <- as.Date("2009-01-01")
options$deal.end     <- as.Date("2009-12-31") 
#options$omit.months  <- c("2003-08")
options$use.data.from <- as.Date("2003-01-01")
options$focus.months <- c(1,7,8)
options$polynomial   <- 1 
options$regression   <- "lm"  # or "rlm"
options$tsdb.refresh <- 0

source("H:/user/R/RMG/Models/Price/Spread.Analysis/main.analyze.one.spread.R")
main.analyze.one.spread(options,save)

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Spread/PJM/APS-WEST/"
options$shortNames   <- c("APS","WEST") # short column names
options$region <- "PJM"
options$hdata.startDate <- as.Date("2002-04-01")
options$hdata.endDate   <- as.Date("2005-06-30")
options$hdata.symbols   <- c("PJMDAM_APS_ZONE", "PJMDAM_WESTERN_HUB")
options$deal.start   <- as.Date("2009-01-01")
options$deal.end     <- as.Date("2009-12-31") 
#options$omit.months  <- c("2003-08")
options$use.data.from <- as.Date("2003-01-01")
options$focus.months <- c(1,7,8)
options$polynomial   <- 1 
options$regression   <- "lm"  # or "rlm"
options$tsdb.refresh <- 0

source("C:/R/Work/Energy/Models/Physical/Price/Spread.Analysis/main.analyze.one.spread.R")
main.analyze.one.spread(options,save)

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Spread/NYPP/HQ-WEST/"
options$shortNames   <- c("West","HQ") # short column names
options$region <- "NYPP"
options$hdata.startDate <- as.Date("2001-01-01")
options$hdata.endDate   <- as.Date("2005-05-31")
options$hdata.symbols   <- c("NYPP_DAM_ZONE_LBM_61844", "NYPP_DAM_ZONE_LBM_61752")
options$deal.start   <- as.Date("2006-01-01")
options$deal.end     <- as.Date("2006-12-31") 
#options$omit.months  <- c("2003-08")
options$use.data.from <- as.Date("2003-01-01")
options$focus.months <- c(10,11,12)
options$polynomial   <- 1 
options$regression   <- "lm"  # or "rlm"
options$tsdb.refresh <- 0

source("C:/R/Work/Energy/Models/Physical/Price/Spread.Analysis/main.analyze.one.spread.R")
main.analyze.one.spread(options,save)

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Spread/NEPOOL/Brayton-RI/"
options$shortNames   <- c("Brayton","RI") # short column names
options$region <- "NEPOOL"
options$hdata.startDate <- as.Date("2003-04-01")
options$hdata.endDate   <- as.Date("2005-03-31")
options$hdata.symbols   <- c("NEPOOL_SMD_DA_350_CongComp", "NEPOOL_SMD_DA_4005_CongComp")
options$deal.start   <- as.Date("2005-08-01")
options$deal.end     <- as.Date("2005-08-31") 
options$omit.months  <- c("2003-08")
#options$use.data.from <- as.Date("2002-04-01")
options$focus.months <- c(8)
options$polynomial   <- 2 
options$regression   <- "lm"  # or "rlm"
options$tsdb.refresh <- 0

source("C:/R/Work/Energy/Models/Price/Spread.Analysis/main.analyze.one.spread.R")
main.analyze.one.spread(options,save)

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Spread/NEPOOL/SEMA-RI/"
options$shortNames   <- c("SEMA","RI") # short column names
options$region <- "NEPOOL"
options$hdata.startDate <- as.Date("2003-04-01")
options$hdata.endDate   <- as.Date("2005-03-31")
options$hdata.symbols   <- c("NEPOOL_SMD_DA_4006_CongComp", "NEPOOL_SMD_DA_4005_CongComp")
options$deal.start   <- as.Date("2005-08-01")
options$deal.end     <- as.Date("2005-08-31") 
#options$omit.months  <- c("2002-05", "2003-04", "2002-04")
#options$use.data.from <- as.Date("2002-04-01")
options$focus.months <- c(8)
options$polynomial   <- 2 
options$regression   <- "lm"  # or "rlm"
options$tsdb.refresh <- 0

source("C:/R/Work/Energy/Models/Price/Spread.Analysis/main.analyze.one.spread.R")
main.analyze.one.spread(options,save)

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Spread/NYPP/IP2-MILL/"
options$shortNames   <- c("IP2","MILL") # short column names
options$region <- "NYPP"
options$hdata.startDate <- as.Date("2002-01-01")
options$hdata.endDate   <- as.Date("2005-01-31")
options$hdata.symbols   <- c("NYPP_DAM_GEN_MCC_23530", "NYPP_DAM_ZONE_MCC_61759")
#options$deal.start   <- as.Date("2005-02-01")
#options$deal.end     <- as.Date("2005-12-31") 
#options$omit.months  <- c("2002-05", "2003-04", "2002-04")
#options$use.data.from <- as.Date("2002-04-01")
options$focus.months <- c(1,6,7,8)
options$polynomial   <- 2 
options$regression   <- "lm"  # or "rlm"
options$tsdb.refresh <- 0

source("C:/R/Work/Energy/Models/Price/Spread.Analysis/main.analyze.one.spread.R")
main.analyze.one.spread(options,save)

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Spread/PJM/EHub-WHub/"
options$shortNames   <- c("WHub","EHub") # short column names
options$region <- "PJM"
options$hdata.startDate <- as.Date("2002-04-01")
options$hdata.endDate   <- as.Date("2005-01-31")
options$hdata.symbols   <- c("PJMDAM_WESTERN_HUB", "PJMDAM_EASTERN_HUB")
#options$deal.start   <- as.Date("2005-02-01")
#options$deal.end     <- as.Date("2005-12-31") 
#options$omit.months  <- c("2002-05", "2003-04", "2002-04")
#options$use.data.from <- as.Date("2002-01-01") 
options$polynomial   <- 2 
options$regression   <- "lm"  # or "lm"
options$focus.months <- c(1,4,7,8,10)
options$tsdb.refresh <- 1

source("C:/R/Work/Energy/Models/Price/Spread.Analysis/main.analyze.one.spread.R")
main.analyze.one.spread(options,save)

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Spread/PJM/WHub-BGE_Apr02/"
options$shortNames   <- c("WHub","BGE") # short column names
options$region <- "PJM"
options$hdata.startDate <- as.Date("2002-04-01")
options$hdata.endDate   <- as.Date("2005-01-31")
options$hdata.symbols   <- c("PJMDAM_WESTERN_HUB", "PJMDAM_BC_ZONE")
#options$deal.start   <- as.Date("2005-02-01")
#options$deal.end     <- as.Date("2005-12-31") 
#options$omit.months  <- c("2002-05", "2003-04", "2002-04")
#options$use.data.from <- as.Date("2002-04-01")
options$focus.months <- c(1,4,7,8,10)
options$polynomial   <- 2 
options$regression   <- "lm"  # or "lm"
options$tsdb.refresh <- 0

source("C:/R/Work/Energy/Models/Price/Spread.Analysis/main.analyze.one.spread.R")
main.analyze.one.spread(options,save)

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Spread/PJM/WHub-BGE_Apr02_LMP/"
options$shortNames   <- c("WHub","BGE") # short column names
options$region <- "PJM"
options$hdata.startDate <- as.Date("2002-04-01")
options$hdata.endDate   <- as.Date("2005-01-31")
options$hdata.symbols   <- c("PJMLMP_WESTERN_HUB", "PJMLMP_BC_ZONE")
#options$deal.start   <- as.Date("2005-02-01")
#options$deal.end     <- as.Date("2005-12-31") 
#options$omit.months  <- c("2002-05", "2003-04", "2002-04")
#options$use.data.from <- as.Date("2002-04-01")
options$focus.months <- c(1,4,7,8,10)
options$polynomial   <- 2 
options$regression   <- "lm"  # or "lm"
options$tsdb.refresh <- 1

source("C:/R/Work/Energy/Models/Price/Spread.Analysis/main.analyze.one.spread.R")
main.analyze.one.spread(options,save)

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/All/Risk/Analysis/Analysis.Spread/PJM/WHub-BGE_from2002/"
options$shortNames   <- c("WHub","BGE") # short column names
options$hdata.file   <- "hdata.csv"    # name of file with historical data
options$airport.name <- "BWI"
#options$deal.start   <- as.Date("2005-02-01")
#options$deal.end     <- as.Date("2005-12-31") 
#options$omit.months  <- c("2002-05", "2003-04", "2002-04")
options$use.data.from <- as.Date("2002-01-01") 
options$polynomial   <- 2 
options$regression   <- "rlm"  # or "lm"

source("C:/R/Work/Energy/Models/Price/Spread.Analysis/main.analyze.one.spread.R")
main.analyze.one.spread(options,save)

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/Risk/2005, Q1/Model Validation/Analysis/Analysis.Spread/Hub-ME/"
options$shortNames   <- c("ME","Hub") # short column names
options$hdata.file   <- "hdata.csv"    # name of file with historical data
options$airport.name <- "BOS"
options$deal.start   <- as.Date("2005-02-01")
options$deal.end     <- as.Date("2005-12-31") 
#options$omit.months  <- c("2002-05", "2003-04", "2002-04")
options$polynomial   <- 1 
options$regression   <- "rlm"  # or "lm"

source("C:/R/Work/Energy/Models/Price/Spread.Analysis/main.analyze.one.spread.R")
main.analyze.one.spread(options,save)


#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/Risk/2005, Q1/Structured Deals/Exelon-TCC/Analysis.Spread/"
options$shortNames   <- c("Sithe","Pleasant Valley") # short column names
options$hdata.file   <- "hdata.csv"    # name of file with historical data
options$airport.name <- "LGA"
options$deal.start   <- as.Date("2005-02-01")
options$deal.end     <- as.Date("2005-12-31") 
options$omit.months  <- c("2002-05", "2003-04", "2002-04")
options$polynomial   <- 1 
options$regression   <- "rlm"  # or "lm"

source("C:/R/Work/Energy/Models/Price/Spread.Analysis/main.analyze.one.spread.R")
main.analyze.one.spread(options,save)

#-------------------------------------------------------------------------
rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/Risk/2004, Q4/Deal Review/Entergy Pilgrim/Analysis.Spread/"
options$shortNames   <- c("NEPOOL Hub","Pilgrim") # short column names
options$hdata.file   <- "hdata.csv"    # name of file with historical data
options$airport.name <- "BOS" 

source("C:/R/Work/Energy/Models/Price/Spread.Analysis/main.analyze.one.spread.R")
main.analyze.one.spread(options,save)


rm(list=ls())
save=NULL; options=NULL
save$Analysis$all <- 1
save$dir <- "S:/Risk/2004, Q4/Structured Deals/First Energy/Analysis.Spread/"
options$shortNames   <- c("West Hub","FE") # short column names
options$hdata.file   <- "hdata.csv"    # name of file with historical data
options$airport.name <- "BWI" 

source("C:/R/Work/Energy/Models/Price/Spread.Analysis/main.analyze.one.spread.R")
main.analyze.one.spread(options,save)



