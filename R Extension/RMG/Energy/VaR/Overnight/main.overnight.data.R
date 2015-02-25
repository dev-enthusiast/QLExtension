# Main program that shows the flow of the overnight data jobs. 
#
#
# Last modified by Adrian Dragulescu on 28-Mar-2007

rm(list=ls())
options <- NULL
options$asOfDate   <- as.Date("2007-05-18")
#options$asOfDate   <- Sys.Date()
#options$environment <- list(data="prod", code="dev", reports="dev")
options$source.dir  <- "H:/user/R/RMG/Energy/VaR/"
source(paste(options$source.dir,"Overnight/set.parms.overnight.R",sep=""))
options <- set.parms.overnight(options)
#--------------------------------------------------------------------
## source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
## options <- overnightUtils.loadOvernightParameters("2007-05-15")

get.spot.fx.rates(options)

get.curve.info(options)

get.one.day.prices(options$asOfDate, options)
chP <- get.corrected.prices(options)
correct.hPrices.file(chP, options)
make.mkt.price.files(options)
fill.mkt.price.files(options)

get.one.day.vols(options$asOfDate, options)
## cVV <- get.corrected.vols(options)
## correct.hVols.file(cVV, options)
make.mkt.vol.files(options)
fill.mkt.vol.files(options)



source(paste(options$source.dir,"Overnight/fill.one.ts.curve.R",sep=""))
source(paste(options$source.dir,"Overnight/fill.mkt.files.R",sep=""))
fill.mkt.price.files(options)

fill.mkt.vol.files(options)


source(paste(options$source.dir,"Overnight/overnightPositionJob.R",sep=""))


make.range.mkt.files("prices", as.Date("2007-03-30"),
  as.Date("2007-04-26"), options)
## make.range.mkt.files("vols", as.Date("2007-03-24"),
##   as.Date("2007-04-25"), options)


get.range.days.prices(as.Date("2006-11-10"), as.Date("2007-04-26"), options)
get.range.days.vols(as.Date("2006-11-10"), as.Date("2007-04-26"), options)


## make.range.mkt.files("prices", as.Date("2007-03-23"),
##   as.Date("2007-04-26"), options)


