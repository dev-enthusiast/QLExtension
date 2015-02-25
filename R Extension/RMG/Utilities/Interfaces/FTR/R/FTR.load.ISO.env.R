#
#

FTR.load.ISO.env <- function(region="NEPOOL", datasets=c("MAP", "HRS",
  "hP", "SP"))
{
  cat(paste("Loading ISO.env", region, "\n"))
  ISO.env <- NULL
  if (toupper(region)=="NEPOOL"){
    .FTR.load.ISO.env.NEPOOL()
    if ("MAP" %in% datasets) NEPOOL$MAP <- NEPOOL$.loadMAP()
    if ("HRS" %in% datasets) NEPOOL$HRS <- FTR.bucketHours("NEPOOL",
                               end.dt=as.POSIXlt("2017-12-31 23:00:00"))
    if ("hP" %in% datasets) NEPOOL$hP <- NEPOOL$.loadHistoricalPrice()
    if ("SP" %in% datasets) NEPOOL$SP <- NEPOOL$.loadSettlePrice()
    if ("CP" %in% datasets) NEPOOL$CP <- NEPOOL$.loadClearingPrice()
    if ("MAP.BC" %in% datasets) NEPOOL$MAP.BC <- NEPOOL$.make.BC.MAP()
    if ("AWD" %in% datasets) NEPOOL$AWD <- NEPOOL$.loadAwards()
    if ("LT_FTR_BIDS" %in% datasets) NEPOOL$LT_FTR_BIDS <- NEPOOL$.loadLT_FTR_BIDS()
  }
  
  if (toupper(region)=="NYPP"){
    .FTR.load.ISO.env.NYPP()
  }

  if (toupper(region)=="PJM"){
    .FTR.load.ISO.env.PJM()
  }
  
  if (toupper(region)=="MISO"){
    .FTR.load.ISO.env.MISO()
  }

  
}

.FTR.load.ISO.env.NEPOOL <- function(){
  source("S:/All/Risk/Software/R/prod/Energy/Trading/Congestion/lib.NEPOOL.iso.R")
 }

.FTR.load.ISO.env.NYPP <- function(){
  source("S:/All/Risk/Software/R/prod/Energy/Trading/Congestion/lib.NYPP.iso.R")   
}

.FTR.load.ISO.env.PJM <- function(){
  source("S:/All/Risk/Software/R/prod/Energy/Trading/Congestion/lib.PJM.iso.R")
  PJM$MAP <- PJM$.loadMAP()
}

.FTR.load.ISO.env.MISO <- function(){
  source("S:/All/Risk/Software/R/prod/Energy/Trading/Congestion/lib.MISO.iso.R")
  MISO$MAP <- MISO$.loadMAP()
}
