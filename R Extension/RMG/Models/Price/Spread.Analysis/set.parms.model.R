# Moved a lot of initialization on this file
#
#
# Written by Adrian Dragulescu on 31-May-2005

set.parms.model <- function(save, options){

  require(xtable); require(MASS)
  if (length(options$shortNames)==0) {options$shortNames=NULL}
  if (length(options$omit.months)==0){options$omit.months=NULL}
  if (length(options$use.data.from)==0){
    options$use.data.from <- options$hdata.startDate}
  if (length(options$regression)==0) {options$regression <- "lm"}
  if (length(options$polynomial)==0) {options$polynomial <- 1}
  if (length(options$bootstrap)==0) {options$bootstrap <- 1}
  if (options$region=="PJM")   {options$airport.name <- "BWI"}
  if (options$region=="NEPOOL"){options$airport.name <- "BOS"}
  if (options$region=="NYPP")  {options$airport.name <- "LGA"}
  if (length(options$tsdb.refresh)==0){options$tsdb.refresh <- 1}
  if (length(options$other.variables)==0){options$other.variables <- 0}
  
  if (length(options$HS)==0){options$HS <- 1}
  if (!file.exists(save$dir)){dir.create(save$dir)}  
  setwd(save$dir)
  options$source.dir <- "H:/user/R/RMG/Models/Price/Spread.Analysis/"
  options$cvs.root   <- "H:/user/R/RMG/"
  
  source(paste(options$source.dir, "analyze.one.spread.R", sep=""))
  source(paste(options$source.dir, "weather.normalize.spread.R", sep=""))
  source(paste(options$source.dir, "daily.spread.regression.R", sep=""))
  source(paste(options$source.dir, "daily.spread.regression.other.R", sep=""))
  source(paste(options$source.dir, "bootstrap.spread.R", sep=""))

  source(paste(options$cvs.root, "TimeFunctions/intersect.ts.R", sep=""))
  source(paste(options$cvs.root, "TimeFunctions/select.onpeak.R", sep=""))
  source(paste(options$cvs.root, "TimeFunctions/mean.by.month.R", sep=""))
  source(paste(options$cvs.root, "Statistics/quantiles.by.season.R", sep=""))
  source(paste(options$cvs.root, "Statistics/quantiles.by.month.R", sep=""))
  source(paste(options$cvs.root, "Utilities/tsdbRead.R", sep=""))
  source(paste(options$cvs.root, "Statistics/label.outliers.R")
         

  return(list(save, options))
}
