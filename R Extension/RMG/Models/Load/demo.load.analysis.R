# Main file to analyze a Loadwiz account load, and produce a pdf report
#
#
# Written by Adrian Dragulescu on 14-Mar-2005

#---------------------------------------------------------------------
rm(list = ls(all=TRUE))   # clear the memory
save=NULL; options=NULL

save$dir$main <- "S:/Risk/2005,Q3/Structured_Deals/West_Penn_RFP/Analysis.Load/"
options$account.name <- "APWP_1_SEC"     # LoadWiz account name
options$airport.name <- "PIT"

source("H:/user/R/RMG/Models/Load/load.model.main.R")
load.model.main(save, options)

#---------------------------------------------------------------------
rm(list = ls(all=TRUE))   # clear the memory
save=NULL; options=NULL

save$dir$main <- "S:/Risk/2005,Q3/Structured_Deals/West_Penn_RFP/Analysis.Load/"
options$account.name <- "APWP_2_SEC"     # LoadWiz account name
options$airport.name <- "PIT"

source("H:/user/R/RMG/Models/Load/load.model.main.R")
load.model.main(save, options)

#---------------------------------------------------------------------
rm(list = ls(all=TRUE))   # clear the memory
save=NULL; options=NULL

save$dir$main <- "S:/Risk/2005,Q2/Structured_Deals/NStar_DS/BECO/Analysis.Load/"
options$account.name <- "BecoR1"     # LoadWiz account name
options$airport.name <- "BOS"

source("H:/user/R/RMG/Models/Load/load.model.main.R")
load.model.main(save, options)

#---------------------------------------------------------------------
rm(list = ls(all=TRUE))   # clear the memory
save=NULL; options=NULL

save$dir$main <- "S:/Risk/2005,Q1/Structured_Deals/MECO/Analysis.Load/"
options$loadName     <- "MECOG1"
options$account.name <- "MECOG1"     # LoadWiz account name
options$airport.name <- "BOS"
options$forecast$startDate <- strptime("2005-05-01", format="%Y-%m-%d")
options$forecast$endDate   <- strptime("2006-04-30", format="%Y-%m-%d")

source("C:/R/Work/Energy/Models/Load/load.model.main.R")
load.model.main(save, options)

