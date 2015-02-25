# A wrapper for main.run.VaR.R used for testing purposes. 
# 
#
#

rm(list=ls())
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
options <- overnightUtils.loadOvernightParameters("2007-05-18")

source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
run.all <-  get.portfolio.book(options)
sort(unique(run.all$RUN_NAME))
run <- NULL

run$run.name <- "NYTRADE"
#run$run.name <- "GASOIL Portfolio"
#run$run.name <- "GANDHI TOTAL EAST PORTFOLIO"
#run$run.name <- "PJM ACCRUAL PORTFOLIO"
#run$run.name <- "RMG DISCLOSED TRADING"
#run$run.name <- "JADDISON TRADING PORTFOLIO"
#run$run.name <- "JKIRKPATRICK TRADING PORTFOLIO"
#run$run.name <- "RMSYS EAST PORTFOLIO"
run$specification <- subset(run.all, RUN_NAME == toupper(run$run.name))

source(paste(options$source.dir,"Base/set.parms.run.R",sep=""))
source(paste(options$source.dir,"Base/main.run.VaR.R",sep=""))
main.run.VaR(run, options)




#=================================================================
options <- NULL
options$asOfDate   <- as.Date("2007-05-18")
#options$asOfDate   <- Sys.Date()
options$environment <- list(data="prod", code="dev", reports="dev")
options$run$is.overnight <- FALSE
options$source.dir  <- "H:/user/R/RMG/Energy/VaR/"
source(paste(options$source.dir,"Overnight/set.parms.overnight.R",sep=""))
options <- set.parms.overnight(options)



##################################################################
asOfDate <- "2007-04-05"
run.name <- "RMG NORLING PORTFOLIO"
source("H:/user/R/RMG/Energy/VaR/Overnight/run.one.VaR.R")
run.one.VaR(asOfDate, run.name)

asOfDate <- "2007-04-05"
run.name <- "RMG UK PWR GAS CARBON WTI PORTFOLIO"
source("H:/user/R/RMG/Energy/VaR/Overnight/run.one.VaR.R")
run.one.VaR(asOfDate, run.name)



## options <- run <- NULL
## options$asOfDate   <- as.Date("2007-04-05")
## #options$asOfDate   <- Sys.Date()
## options$keep.log   <- TRUE
## options$source.dir <- "H:/user/R/RMG/Energy/VaR/"
## source(paste(options$source.dir,"Overnight/set.parms.overnight.R",sep=""))
## options <- set.parms.overnight(options)
## sink()



## source(paste(options$source.dir,"Base/main.run.VaR.R",sep=""))
## main.run.VaR(run, options)


