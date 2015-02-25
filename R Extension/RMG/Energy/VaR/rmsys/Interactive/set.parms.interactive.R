# Define general settings for an interactive VaR run.  Assumes that  
# prepare.overnight.VaR has run for the asOfDate.  
#
# Written by Adrian Dragulescu on 27-Nov-2006

set.parms.interactive <- function(options){

  require(RODBC); require(reshape); require(xtable)

  options$calc$no.hdays      <- 70
  options$run$is.overnight   <- FALSE
  
  options$source.dir         <- "H:/user/R/RMG/Energy/VaR/rmsys/"
  options$save$reports.dir   <- "S:/All/Risk/Data/VaR/Reports/"
  options$save$datastore.dir <- "S:/All/Risk/Data/VaR/"
  options$save$do.IVaR.file  <- 1 

  runs <- sort(list.files(path=options$save$datastore.dir, pattern="options"))
  ind  <- grep(options$asOfDate, sort(runs))
  if (length(ind)==0 | options$run$useLivePositions){  # get live positions
    filename <- paste(options$save$datastore.dir, runs[length(runs)], sep="")
  } else {                                             # get the asOfDate 
    filename <- paste(options$save$datastore.dir, runs[ind], sep="")
  }
  load(filename)         # load the nightly options
  options$connections <- options.overnight$connections
  options$save <- options.overnight$save
  options$calc <- options.overnight$calc
  options$connections$CPSPROD <- odbcConnect("CPSPROD",
     uid="stratdatread", pwd="stratdatread")
  options$connections$VCTRLP <- odbcConnect("VCTRLP",
     uid="vcentral_read", pwd="vcentral_read")
  options$connections$RMSYS <- odbcConnect("RMSYSP",
     uid="rmsys_read", pwd="rmsys_read")
  
##   source(paste(options$source.dir,"Intraday/get.intraday.positions.R",sep="")) 
##   source(paste(options$source.dir,"Overnight/get.VCentral.positions.R",sep=""))
  source(paste(options$source.dir,"Overnight/aggregate.all.market.states.R",sep=""))
  source(paste(options$source.dir,"Overnight/get.historical.prices.R",sep=""))
  source(paste(options$source.dir,"Overnight/get.corrected.marks.R",sep=""))
  
  source(paste(options$source.dir,"main.run.VaR.R",sep=""))
  source(paste(options$source.dir,"make.class.Factors.R",sep=""))
  source(paste(options$source.dir,"utilities.VaR.R",sep=""))
  source(paste(options$source.dir,"core.calc.VaR.R",sep=""))
  source(paste(options$source.dir,"make.VaR.report.R",sep=""))
  source(paste(options$source.dir,"make.pdf.R",sep=""))

  source("H:/user/R/RMG/Utilities/write.xls.R")

  
  return(options)
}
