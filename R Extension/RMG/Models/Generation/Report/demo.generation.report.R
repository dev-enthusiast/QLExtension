#
#
# Written by Adrian Dragulescu on 15-Apr-2005

rm(list=ls())
options=save=NULL
save$dir.main.root <- "S:/Risk/2006,Q2/Structured.Deals/Power/Elwood/"
options$unit.name  <- "Elwood Toll"
options$run        <- 10122
options$asOfDate   <- as.Date("2006-05-15") 
# options$table$tex.digits <- 2

source("H:/user/R/RMG/Models/Generation/Report/main.model.generation.R")
main.model.generation(save, options)


#=============================================================================
rm(list=ls())
options=save=NULL
save$dir.main.root <- "S:/Risk/2005,Q4/Structured_Deals/Cordova/"
options$unit.name  <- "Cordova Toll"
options$run        <- 9745
options$asOfDate   <- "TLP Run 7251" 

source("H:/user/R/RMG/Models/Generation/Report/main.model.generation.R")
main.model.generation(save, options)

#=============================================================================
rm(list=ls())
options=save=NULL
save$dir.main.root <- "S:/Risk/2005,Q2/M_and_A/Saltend/"
options$unit.name  <- "Saltend"
options$run        <- 4819
options$asOfDate   <- "TLP Run 4819 (base case), as of 4/21/2005" 

source("C:/R/Work/Energy/Models/Valuation/Generation/main.model.generation.R")
main.model.generation(save, options)

