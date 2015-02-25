################################################################################
# External Libraries
#
require(reshape); require(SecDb); require(UsingR)

source("H:/user/R/RMG/Utilities/RLogger.R")
#source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")

################################################################################
# File Namespace
#
SP.Analysis <- new.env(hash=TRUE)    # spread environment

################################################################################
# Constants
SP.Analysis$.SAVE_DIR_ROOT <- "S:/All/Risk/Analysis/Analysis.Spread/"


################################################################################
# 
SP.Analysis$main <- function(SP.options){

  rLog("Read historical spot prices ... ")
  hPP <- SP.Analysis$.read.spot.prices( SP.options ) 
   
  rLog("Read historical forward prices ...")
  hFWD <- SP.Analysis$.read.forward.prices( SP.options )

  SP.Analysis$make.plots( hPP, hFWD, SP.options )

}


################################################################################
#
SP.Analysis$make.plots <- function( hPP, hFWD, SP.options ) 
{
  MM <- hPP[, c("day", "value")]
  MM$month <- substr(as.character(MM$day), 6, 7)

  MM <- split(MM$value, MM$month)

  boot.fun <- function(x, sims=1000){
    res <- sample(x, size=20*sims, replace=TRUE)
    res <- matrix(res, ncol=sims)
    return(colMeans(res))
  }  
  MM <- lapply(MM, boot.fun)   # bootstrap the daily spread 
  
  MM <- melt(MM)
  windows()
  violinplot(MM$value ~ MM$L1, col="wheat")
  mtext("Month", side=1, line=3)
  mtext("Bootstrapped monthly spread pdf", side=2, line=3)
  mtext(paste(SP.options$curves[[1]]$shortName, " - ", 
    SP.options$curves[[2]]$shortName, " (", SP.options$curves[[1]]$bucket,
    ")", sep=""), side=3, font=2)
  
  colors <- c("blue", "red", "darkgreen")
  uYears <- unique(format(hFWD$contract.dt, "%Y"))
  uMonths <- unique(MM$L1) 
  for (y in seq_along(uYears)){
    ind  <- grep(uYears[y], hFWD$contract.dt)
    aux  <- hFWD[ind,]
    aux  <- subset(aux, substr(aux$contract.dt, 6, 7) %in% uMonths)
    lines(aux$value, lwd=2, col=colors[y])
    points(aux$value, col=colors[y], pch=19)
  }
  avg <- tapply(MM$value, list(MM$L1), mean)
  points(avg, col="black", pch=1)
  
  legend("topright", legend=c(uYears, "hist mean"), col=c(colors, "black"),
    pch=c(rep(20, length(uYears)), 1), text.col=c(colors, "black"), bty="n")
  
}


################################################################################
#
SP.Analysis$.read.spot.prices <- function( SP.options ) 
{

   start.dt <- as.POSIXct(paste(SP.options$start.dt, "00:00:00"))
   end.dt   <- as.POSIXct(paste(SP.options$end.dt, "23:00:00"))
   
   rLog("Pulling ", SP.options$curves[[1]]$tsdb)
   hp.1 <- tsdb.readCurve(SP.options$curves[[1]]$tsdb, start.dt, end.dt)
   if (names(hp.1)[1]=="time"){
     hours <- secdb.getHoursInBucket( SP.options$curves[[1]]$region,
       SP.options$curves[[1]]$bucket, start.dt, end.dt )
     hp.1 <- merge(hp.1, hours)
     hp.1$day <- as.Date(format(hp.1$time, "%Y-%m-%d"))
     hp.1 <- aggregate(hp.1[,2], list(hp.1$day), mean)
   } else {
     days <- unique(as.Date(format(hours$time, "%Y-%m-%d")))
     hp.1 <- subset(hp.1, date %in% days)     
   }
   names(hp.1) <- c("day", SP.options$curves[[1]]$shortName)     

   rLog("Pulling ", SP.options$curves[[2]]$tsdb)
   hp.2 <- tsdb.readCurve(SP.options$curves[[2]]$tsdb, start.dt, end.dt)
   hours <- secdb.getHoursInBucket( SP.options$curves[[2]]$region,
       SP.options$curves[[2]]$bucket, start.dt, end.dt )
   if (names(hp.2)[1]=="time"){
     hp.2 <- merge(hp.1, hours)
     hp.2$day <- format(hp.2$time, "%Y-%m-%d")
     hp.2 <- aggregate(hp.2[,2], list(hp.2$day), mean)
   } else {
     days <- unique(as.Date(format(hours$time, "%Y-%m-%d")))
     hp.2 <- subset(hp.2, date %in% days)     
   }
   names(hp.2) <- c("day", SP.options$curves[[2]]$shortName)     

   hPP <- merge(hp.1, hp.2)
   hPP$value <- hPP[,2] - hPP[,3]

   return(hPP)
}
################################################################################
#
SP.Analysis$.read.forward.prices <- function( SP.options ) 
{

  contract.start <- as.Date(secdb.dateRuleApply(SP.options$asOfDate, "+1e"))+1
  contract.end <- as.Date(secdb.dateRuleApply(SP.options$asOfDate, "+3y"))
  contracts <- seq(contract.start, contract.end, by="month")
  asOfDate <- SP.options$asOfDate
  commods  <- c(SP.options$curves[[1]]$commod.curve,
                SP.options$curves[[2]]$commod.curve)
  
  hFWD <- NULL
  while (is.null(hFWD)){
    rLog(paste("Getting the forward curve for ", asOfDate))
    hFWD <- tsdb.futStrip( commods, asOfDate, asOfDate, 
      contracts)
    asOfDate <- as.Date(secdb.dateRuleApply(asOfDate, "-1b"))
  }  

  hFWD <- cast(hFWD, contract.dt ~ curve.name, I)
  hFWD$value <- hFWD[, SP.options$curves[[1]]$commod.curve] -
                hFWD[, SP.options$curves[[2]]$commod.curve]
  return(hFWD)
}

###########################################################################
###########################################################################
###########################################################################

##   MM <- lapply(MM,  function(x){
##     qq <- quantile(x, probs=c(0.05, 0.95))   # remove some outliers
##     x <- x[-which(x<qq[1] | x>qq[2])]
##     })
  

##   aux <- hPP[, c("day", "value")]
##   aux$yyyy.mm.dd <- format(hPP$day, "%Y-%m-%d")
##   aux$year    <- format(hPP$day, "%Y")
##   aux$month   <- format(hPP$day, "%m")
  
##   TT <- cast(aux, month ~ year, mean)

##   MM <- cast(aux, yyyy.mm.dd ~ ., mean)
##   names(MM)[2] <- "value"


## set.parms.model <- function(save, options){
##   require(xtable); require(MASS)
##   if (length(options$shortNames)==0) {options$shortNames=NULL}
##   if (length(options$omit.months)==0){options$omit.months=NULL}
##   if (length(options$use.data.from)==0){
##     options$use.data.from <- options$hdata.startDate}
##   if (length(options$regression)==0) {options$regression <- "lm"}
##   if (length(options$polynomial)==0) {options$polynomial <- 1}
##   if (length(options$bootstrap)==0) {options$bootstrap <- 1}
##   if (options$region=="PJM")   {options$airport.name <- "BWI"}
##   if (options$region=="NEPOOL"){options$airport.name <- "BOS"}
##   if (options$region=="NYPP")  {options$airport.name <- "LGA"}
##   if (length(options$tsdb.refresh)==0){options$tsdb.refresh <- 1}
##   if (length(options$other.variables)==0){options$other.variables <- 0}
  
##   if (length(options$HS)==0){options$HS <- 1}
##   if (!file.exists(save$dir)){dir.create(save$dir)}  
##   setwd(save$dir)
##   options$source.dir <- "H:/user/R/RMG/Models/Price/Spread.Analysis/"
##   options$cvs.root   <- "H:/user/R/RMG/"
  
##   source(paste(options$source.dir, "analyze.one.spread.R", sep=""))
##   source(paste(options$source.dir, "weather.normalize.spread.R", sep=""))
##   source(paste(options$source.dir, "daily.spread.regression.R", sep=""))
##   source(paste(options$source.dir, "daily.spread.regression.other.R", sep=""))
##   source(paste(options$source.dir, "bootstrap.spread.R", sep=""))

##   source(paste(options$cvs.root, "TimeFunctions/intersect.ts.R", sep=""))
##   source(paste(options$cvs.root, "TimeFunctions/select.onpeak.R", sep=""))
##   source(paste(options$cvs.root, "TimeFunctions/mean.by.month.R", sep=""))
##   source(paste(options$cvs.root, "Statistics/quantiles.by.season.R", sep=""))
##   source(paste(options$cvs.root, "Statistics/quantiles.by.month.R", sep=""))
##   source(paste(options$cvs.root, "Utilities/tsdbRead.R", sep=""))
##   source(paste(options$cvs.root, "Statistics/label.outliers.R")
##   source(paste(options$cvs.root, "Utilities/Database/LIM/read.LIM.R")
##   dyn.load(paste(options$cvs.root, "Utilities/Database/LIM/Source/RMGxmimR.dll")       

         

##   return(list(save, options))
## }
