# Trunk of the process
#
#
# last modified by Adrian Dragulescu on 13-Feb-2004


load.model.main <- function(save, options){

setwd(save$dir$main)

source("H:/user/R/RMG/Models/Load/set.parms.load.model.R")
aux <- set.parms.load.model(save, options)
save <- aux[[1]]; options <- aux[[2]]
load(options$file.hist.temp)   # load Temperature calibration

cat("\n Loading historical data... "); flush.console()
#----------------------------------------Load Historical data 
hdata <- read.table(paste("Data/hdata_",options$account.name,".csv",
         sep=""), header = T, sep =",", row.names = NULL)

YMD     <- paste(hdata$year,hdata$month,hdata$day, sep="/")  
hdaysCH <- posix2chron(strptime(YMD, format="%Y/%m/%d")) # chron 
hdata   <- cbind(hdata, hdays=as.character(hdaysCH))
options$historical$startDate <- chron2posix(hdaysCH[1])
options$historical$endDate   <- chron2posix(hdaysCH[length(hdaysCH)])
rm(hdaysCH)
if (options$scale.factor!=1){hdata$load <- options$scale.factor*hdata$load}
hdata[1:10,]
cat("Done.\n")

#-----------------------------------------------Load TLP data
cat("\n Loading TLP forecasted load... "); flush.console()
fdata.TLP <- read.table(paste("Data/fdata_",options$account.name,".csv",
             sep=""), header = T, sep =",", row.names = NULL)
if (options$scale.factor!=1){
  fdata.TLP$load <- options$scale.factor*fdata.TLP$load}
fdata.TLP[1:10,]
aux <- paste(fdata.TLP$year[1], fdata.TLP$month[1], fdata.TLP$day[1], sep="-")
options$forecast$startDate <- strptime(aux, format="%Y-%m-%d")
N   <- nrow(fdata.TLP) 
aux <- paste(fdata.TLP$year[N], fdata.TLP$month[N], fdata.TLP$day[N], sep="-")
options$forecast$endDate   <- strptime(aux, format="%Y-%m-%d")

cat("Done.\n")

#--------------------------------------------Deconstruct Load
cat("\n Regressing load to temperature... "); flush.console()
res <- deconstruct.load(hdata, save, options)
hdata  <- res[[1]]; load.month <- res[[2]]
reg.LT <- res[[3]]; reg.DT     <- res[[4]]
cat("Done.\n"); flush.console()

#---------------------------Construct Weather Normalized Load 
cat("\n Forecast RMG load... "); flush.console()
fdata.RMG <- construct.load(load.month, reg.LT, reg.DT, save, options)
fdata.RMG[1:10,]
cat("Done.\n")

#------------------------Construct Historial Load Simulations 
cat("\n Simulate load using historical temperatures... "); flush.console()
hs.load <- hist.simulate.load(temp.hist.scenarios, reg.LT, reg.DT, options)
cat("Done.\n")

#------------------------------Plot Historical & Forecasted Load 
cat("\n Plot and export summary tables... "); flush.console()
plot.hist.load(hdata,save,options)
cat("Done hist. "); flush.console()
plot.forecasted.load(hdata, fdata.RMG, fdata.TLP, hs.load, reg.LT, save, options)
cat("Done.\n")

#-------------------------Plot VLR distribution
filename <- paste("Data/vlr_",options$account.name,".csv", sep="")
if (file.access(filename, mode=0)==0){
  cat("\n Read VLR data and save the analysis... "); flush.console()
  vlr <- read.csv(filename, header = T, sep =",")
  colnames(vlr) <- gsub("X", "", colnames(vlr))
  vlr[1:10, 1:5]
  save$Tables$filename     <- "Rtables/vlr_quantiles.csv"
  options$labels.vlr$mmmyy <- 1
  source(paste(options$path.L, "plot.vlr.R", sep=""))
  plot.vlr(vlr, save, options)
  cat("Done.\n"); flush.console()
  options$vlr <- 1
} else {
  cat("\n No VLR... "); flush.console()
  options$vlr <- 0
}

#------------------------------------Make pdf report----
make.pdf(save, options)

}

##############################################################
##############################################################




