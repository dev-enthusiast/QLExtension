# Set general options for the run
#
#
# Written by Adrian Dragulescu on 23-Mar-2005

set.parms.model <- function(save, options){
  
require(Simple); require(xtable); require(chron); require(lattice)
require(RODBC); require(R.matlab)
if (length(options$asOfDate)==0){options$asOfDate <- Sys.Date()-1}
if (length(options$do$hist.analysis)==0){options$do$hist.analysis <- 1}
if (length(options$do$simulate.spreads)==0){options$do$simulate.spreads <- 1}
if (length(save$Analysis$all)==0){save$Analysis$all <- 1}
if (length(save$Analysis$plots)==0){save$Analysis$plots <- 1}
if (length(options$noSims)==0){options$noSims <- 10} # number of simulations
if (length(options$scale.factor)==0){options$scale.factor <- 1000000} 

save$source.dir <- "H:/user/R/RMG/Models/Price/BasisRisk/"
save$dir.hdata <- "S:/All/Risk/Data/Energy/"
save$dir.main.root  <- paste("S:/All/Risk/Analysis/Zonal.Risk/", options$run.name,
                        "/", sep="")
save$dir.main   <- paste(save$dir.main.root, options$asOfDate, "/", sep="")
save$dir.plots  <- paste(save$dir.main, "Plots/", sep="")
save$dir.tables <- paste(save$dir.main, "Rtables/", sep="")
save$dir.model  <- paste(save$dir.main.root, "Hist.Analysis/", sep="")
if (file.access(save$dir.main)!=0)  {dir.create(save$dir.main)}
if (file.access(save$dir.plots)!=0) {dir.create(save$dir.plots)}
if (file.access(save$dir.tables)!=0){dir.create(save$dir.tables)}
if (file.access(save$dir.model)!=0) {dir.create(save$dir.model)}

#--------------------------------------------------------------------------
source(paste(save$source.dir, "Custom.Runs/", options$run.name, ".R", sep=""))
options  <- get(options$run.name)(save, options)

#--------------------------------------------------------------------------
source(paste(save$source.dir, "read.zonal.LMP.R", sep=""))
source(paste(save$source.dir, "simulate.spreads.R", sep=""))
source(paste(save$source.dir, "read.positions.R", sep=""))
source(paste(save$source.dir, "calculate.zonal.risk.R", sep=""))
source(paste(save$source.dir, "simulate.spread.bySeason.R", sep=""))
source(paste(save$source.dir, "Historical.Analysis/main.hist.analysis.R", sep=""))

source("H:/user/R/RMG/Statistics/sample.from.Ndim.R")
source("H:/user/R/RMG/Statistics/quantiles.by.factor.R")
source("H:/user/R/RMG/TimeFunctions/bucket.ts.R")
source("H:/user/R/RMG/TimeFunctions/calculate.season.R")
source("H:/user/R/RMG/TimeFunctions/ymd.R")
source("H:/user/R/RMG/TimeFunctions/select.onpeak.R")
source("H:/user/R/RMG/TimeFunctions/mean.by.month.R")
source("H:/user/R/RMG/Utilities/tsdbRead.R") 
source("H:/user/R/RMG/Utilities/read.prices.from.CPSTEST.R")
source("H:/user/R/RMG/Utilities/read.deltas.from.CPSPROD.R")
source("H:/user/R/RMG/Utilities/lib.matlab.to.SecDB.R")

#--------------------------------------------------------------------------
fdata=NULL
fdays  <- seq(options$fdata$startDate, options$fdata$endDate, by="day")
fdata  <- data.frame(ymd(fdays), date=fdays)
options$season <- "season"
season <- calculate.season(fdata[,c("year","month","day")], options)
fdata  <- cbind(fdata, season=season$season)
fdata  <- cbind(fdata, yyyymm=format(fdata$date, "%Y-%m"))

options$fdata$noDays <- length(fdays)  # number of days to simulate
options$fdata$months <- format(seq(options$fdata$startDate,
                  options$fdata$endDate, by="month"), format="%Y-%m")
options$fdata$range <- paste(format(options$fdata$startDate, "%b%y"), "to",
                             format(options$fdata$endDate, "%b%y"))

return(list(fdata, save, options))
}

#options$omit.months  <- c("2002-05", "2003-04", "2002-04")
#options$use.data.from <- as.Date("2002-04-01")


