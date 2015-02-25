#  
#
#
# Written by Adrian Dragulescu on 28-Feb-2005


main.analyze.transport <- function(options, save){

  require(xtable); require(MASS)
  source("H:/user/R/RMG/Utilities/tsdbRead.R")
  source("H:/user/R/RMG/TimeFunctions/intersect.ts.R")
  source("H:/user/R/RMG/TimeFunctions/select.onpeak.R")
  source("H:/user/R/RMG/TimeFunctions/mean.by.month.R")
  source("H:/user/R/RMG/Statistics/quantiles.by.season.R")
  source("H:/user/R/RMG/Statistics/quantiles.by.month.R")
  source("H:/user/R/RMG/Models/Gas/Transport/bootstrap.spread.R")

  if (length(options$shortNames)==0) {options$shortNames=NULL}
  if (length(options$omit.months)==0){options$omit.months=NULL}
  if (length(options$use.data.from)==0){
    options$use.data.from <- options$hdata.startDate}
  if (length(options$tsdb.refresh)==0){options$tsdb.refresh <- 1}
  if (length(options$bootstrap)==0){options$bootstrap <- 1}
  if (length(options$HS)==0){options$HS <- 0}
  if (!file.exists(save$dir)){dir.create(save$dir)}  
  setwd(save$dir)
  
  MM <- tsdbRead(options,save)  
  R   <- nrow(MM);  C <- ncol(MM)
  options$between   <- paste(colnames(MM)[5], " (", options$shortNames[2],
       ") - ",colnames(MM)[4], " (", options$shortNames[1], ")" , sep="") 
  options$main      <- paste(colnames(MM)[5],"-",colnames(MM)[4],sep="")
  options$dateRange <- paste(format(options$use.data.from, "%b%y"), "to",
                           format(options$hdata.endDate, "%b%y"))
  colnames(MM)[4:5] <- options$shortNames
  options$spread <- paste(colnames(MM)[6],"-",colnames(MM)[5])
  options$period <- paste(MM[1,"year"], "to", MM[R, "year"])
  MM  <- cbind(MM, spread=MM[,5]-MM[,4])
  MM  <- cbind(MM, spread.fc=MM[,5]-MM[,4]/(1-options$fuel.charge))
  MM[1:10,]
  
  if (options$use.data.from!=options$hdata.startDate){
    days <- as.Date(paste(MM$year, MM$month, MM$day, sep="-"))
    ind  <- which(days==options$use.data.from)
    MM   <- MM[-(1:(ind[1]-1)),]
  }

  source("H:/user/R/RMG/Models/Gas/Transport/analyze.one.transport.R")
  analyze.one.transport(MM, save, options)

#browser()
  cat("Start creation of report... ")
  flush.console()
  source("H:/user/R/RMG/Models/Gas/Transport/make.pdf.R")
  make.pdf(options)
  cat("Done!  \n Please open your pdf report.\n")
  flush.console()

  
  
  
}
