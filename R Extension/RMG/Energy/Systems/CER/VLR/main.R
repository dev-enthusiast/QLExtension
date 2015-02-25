
rm(list=ls())
require(reshape); require(SecDb); require(MASS)

options <- NULL
# how many days you should have before extending
## options$min.no.overlap.days <- 365
## options$aggregate$start.dt  <- as.Date("2006-10-01")
## options$aggregate$end.dt    <- as.Date("2007-09-30")
## options$aggregate$days <- seq(options$aggregate$start.dt,
##   options$aggregate$end.dt, by="day" )

## options$output.base.dir <- "S:/All/Risk/Projects/CER.Integration/VLR/"
## options$alarm.no.hdays <- 10 # if you have less than 10 points for
##                              # the LT regression, report.

  require(RODBC)
  dir <- "//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/"
  con.str <- paste("FileDSN=",dir,"CERINT.dsn",
      ";UID=cer_pmo_ro;PWD=cerpmoro;", sep="")
  options$con <- odbcDriverConnect(con.str)  


busunit.id <- 6           #  New England = 6, Texas = 7 

source("H:/user/R/RMG/Energy/Systems/CER/VLR/utils.R")
RR <- get.all.edc.and.rateclasses(busunit.id, options)
#RR <- get.all.sic.and.profiles(busunit.id, options)

## uWeather <- unique(RR$weather)
## for (weather in uWeather){
##   loadData <- get.all.load.weather(busunit.id, weather, options)
##   filename <- paste("R:/VLR Data/load.7.", weather, ".RData", sep="")
##   save(loadData, file=filename)
## }


# make the condor queue
for (r in 1:nrow(RR)){
 cat("BU_ID='7'\n")
 cat(paste("WEATHER_ZN='", RR$weather[r], "'\n", sep=""))
 cat(paste("SIC_CODE='", RR$sic.code[r], "'\nQueue\n\n", sep=""))
}

for (r in 1:nrow(RR)){
  file <- paste("R:/VLR Data/load.7.", RR$weather[r], ".", RR$sic.code[r],
                ".RData", sep="")
  aux <- file.info(file)$size
  if (!is.na(aux) & aux < 100){
    file.remove(file)
  } 
  if (file.exists(file)){
    cat("INPUT_PATH='//nas-msw-07/rmgapp/VLR Data/load.7.",
        RR$weather[r], ".", RR$sic.code[r], ".RData'\n", sep="")
    cat("Queue\n\n")
  }
}

for (r in 1:nrow(RR)){
  file <- paste("R:/VLR Data/load.", busunit.id, ".", RR$edc.name[r],
    ".", RR$edc.ratecls[r], ".RData", sep="")
  aux <- file.info(file)$size
  if (!is.na(aux) & aux < 100){
    file.remove(file)
  } 
  if (file.exists(file)){
    cat("INPUT_PATH='//nas-msw-07/rmgapp/VLR Data/load.", busunit.id,
      ".", RR$edc.name[r], ".", RR$edc.ratecls[r], ".RData'\n", sep="")
    cat("Queue\n\n")
  }
}


#=========================================================================
#  Do the load aggregation 
#=========================================================================
res <- NULL
for (r in 1:nrow(RR)){
  file <- paste("R:/VLR Data/load.7.", RR$weather[r], ".", RR$sic.code[r],
                ".RData", sep="")
  if (!file.exists(file)){next}
  rLog("Working on ", file)
  load(file)
  loadData$month <- format(loadData$time, "%Y-%m")
  if ((length(unique(loadData$data.source)))>1){
    rLog("Several data sources...")
  }
  aux <- tapply(loadData$kw, list(loadData$month), sum, na.rm=T) 
  aux <- data.frame(weather=RR$weather[r], sic=RR$sic.code[r], month=names(aux),
                    kwh=aux)
  res <- rbind(res, aux)
}
write.csv(res, file="S:/All/Risk/Projects/CER.Integration/VLR/Results/Simple4/agg.Texas.csv",
          row.names=FALSE)





##########################################################################
#                     OLD STUFF  
##########################################################################
##########################################################################

# Start loop over edc.names and rateclasses
edc.name  <- as.character(RR$edc.name[1])
rateclass <- as.character(RR$edc.ratecls[1])
rLog("Working on region =", busunit.id)
rLog("Working on edc name =", edc.name)
rLog("Working on rateclass =", rateclass)
source("H:/user/R/RMG/Energy/Systems/CER/VLR/utils.R")
HH <- get.all.load(busunit.id, edc.name, rateclass, options)


DD <- unique(HH[,c("ne.acctno", "read.dt")])
DD$value <- 1
DD <- cast(DD, read.dt ~ ne.acctno)
DD$total <- apply(DD[,-1], 1, sum, na.rm=T)

(overlap.days <- table(DD$total))
rLog(paste("Max number of accounts overlapping is", max(overlap.days),
           ".", sep=""))
rLog(paste("The number of unique accounts is", length(uAccounts),
           ".", sep=""))

ind <- which(overlap.days > options$min.no.overlap.days)
if (length(ind) == 0){
  rLog("ERROR: Don't have overlapping ", options$min.no.overlap.days,
       "days of data!")
  # DO SOMETHING !!!
} else {
  base.ntuple <- max(ind)
  rLog("The maximum ntuple is", base.ntuple)
}

#matplot(DD[,-1], type="l")

wth.station <- "BOS"


