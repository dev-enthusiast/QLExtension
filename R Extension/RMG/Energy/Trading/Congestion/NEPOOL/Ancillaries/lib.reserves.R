# TMSR  - 10min spinning reserves
# TMNSR - 10min non-spinning reserves
# TMOR  - 30min operating reserves
#
# Price of TMSR > Price of TMNSR > Price of TMOR
#
# under Nepool/Nepool Data/Prices and Designations (all the way to the bottom)
#
# hourly reserve clearing price RMCP
#  - 7000 is the ROP Requirement, 7001 = SWCT, 7002 = CT, 7003 = NEMA
#  
#
# NEPOOL_RT_7000_TMNSR_PRICE
# NEPOOL_RT5M_7000_TMNSR_PRICE
# NEPOOL_RT5M_7000_TMOR_PRICE
# NEPOOL_RT5M_7000_TMSR_PRICE
#
# What SecDb calls
#  "OpRes DA" is just DA Economic NCPC
#  "OpRes RT" is RT Economic NCPC + RT Reserve costs
#   for NEMA, CT we have "OpRes NEMART", "OpRes CTRT"
#
#
# Functions:
#   .monthly_actuals_rtreserves
#   .pull_hist_rtreserves
#   .compare_ccg_vs_pool_deviation
#



#######################################################################
# HH is the result of .pull_hist_rtreserves
# the output, MM is ready to paste into Sheet2 of spreadsheet
# S:\All\Structured Risk\NEPOOL\Ancillary Services\Monthly Ancillary Updates\Actuals\Reserves in ASM Phase II.xls
#
.monthly_actuals_rtreserves <- function( month=currentMonth() ) 
{
  startDt <- as.POSIXct(paste(format(month), "01:00:00"))
  endDt   <- as.POSIXct(paste(format(nextMonth(month)), "00:00:00"))

  # get the historical hourly data
  HH <- .pull_hist_rtreserves(startDt, endDt)
  # window(HH[,grepl("^ct\\.", colnames(HH))], start=as.POSIXct("2013-06-20 01:00:00"), end=as.POSIXct("2013-06-21 00:00:00"))
  
  # aggregate it by month for sheet Pivot2
  MM <- aggregate(HH[,c("rop.tmnsr.fwdres.cost", "rop.tmor.fwdres.cost",
    "rop.rtlo", "ct.tmnsr.fwdres.cost",
    "ct.tmor.fwdres.cost", "ct.rtlo", "nema.tmnsr.fwdres.cost",
    "nema.tmor.fwdres.cost", "nema.rtlo",  "rop.tmnsr.rtreserve.cost",
    "rop.tmor.rtreserve.cost", "rop.tmsr.rtreserve.cost",
    "ct.tmnsr.rtreserve.cost", "ct.tmor.rtreserve.cost",
    "ct.tmsr.rtreserve.cost", "nema.tmnsr.rtreserve.cost",
    "nema.tmor.rtreserve.cost", "nema.tmsr.rtreserve.cost",
    "rop.rtlo.5x16", "ct.rtlo.5x16", "nema.rtlo.5x16",
    "rop.rtlo", "ct.rtlo", "nema.rtlo"
    )], as.Date(format(index(HH)-1, "%Y-%m-01")), sum, na.rm=TRUE)

  
  # calculate the fwdres rates (in $/MWh)
  MM$rop.tmnsr.fwdres.rate <- MM$rop.tmnsr.fwdres.cost/MM$rop.rtlo.5x16
  MM$rop.tmor.fwdres.rate  <- MM$rop.tmor.fwdres.cost/MM$rop.rtlo.5x16

  MM$ct.tmnsr.fwdres.rate <- MM$ct.tmnsr.fwdres.cost/MM$ct.rtlo.5x16
  MM$ct.tmor.fwdres.rate  <- MM$ct.tmor.fwdres.cost/MM$ct.rtlo.5x16
  
  MM$nema.tmnsr.fwdres.rate <- MM$nema.tmnsr.fwdres.cost/MM$nema.rtlo.5x16
  MM$nema.tmor.fwdres.rate  <- MM$nema.tmor.fwdres.cost/MM$nema.rtlo.5x16
  
  MM$rop.fwdres.rate  <-  MM$rop.tmnsr.fwdres.rate  + MM$rop.tmor.fwdres.rate
  MM$ct.fwdres.rate   <-  MM$ct.tmnsr.fwdres.rate   + MM$ct.tmor.fwdres.rate
  MM$nema.fwdres.rate <-  MM$nema.tmnsr.fwdres.rate + MM$nema.tmor.fwdres.rate

  # calculate the rtreserve rates (in $/MWh)
  MM$rop.tmnsr.rtreserve.rate <- MM$rop.tmnsr.rtreserve.cost/MM$rop.rtlo
  MM$rop.tmor.rtreserve.rate  <- MM$rop.tmor.rtreserve.cost/MM$rop.rtlo
  MM$rop.tmsr.rtreserve.rate  <- MM$rop.tmsr.rtreserve.cost/MM$rop.rtlo

  MM$ct.tmnsr.rtreserve.rate <- MM$ct.tmnsr.rtreserve.cost/MM$ct.rtlo
  MM$ct.tmor.rtreserve.rate  <- MM$ct.tmor.rtreserve.cost/MM$ct.rtlo
  MM$ct.tmsr.rtreserve.rate  <- MM$ct.tmsr.rtreserve.cost/MM$ct.rtlo

  MM$nema.tmnsr.rtreserve.rate <- MM$nema.tmnsr.rtreserve.cost/MM$nema.rtlo
  MM$nema.tmor.rtreserve.rate  <- MM$nema.tmor.rtreserve.cost/MM$nema.rtlo
  MM$nema.tmsr.rtreserve.rate  <- MM$nema.tmsr.rtreserve.cost/MM$nema.rtlo
  
  MM$rop.rtreserve.rate  <-  MM$rop.tmnsr.rtreserve.rate  +
    MM$rop.tmor.rtreserve.rate + MM$rop.tmsr.rtreserve.rate
  MM$ct.rtreserve.rate   <-  MM$ct.tmnsr.rtreserve.rate   +
    MM$ct.tmor.rtreserve.rate + MM$ct.tmsr.rtreserve.rate
  MM$nema.rtreserve.rate <-  MM$nema.tmnsr.rtreserve.rate +
    MM$nema.tmor.rtreserve.rate + MM$nema.tmsr.rtreserve.rate
  
  MM
}




#######################################################################
# get the historical data fro the spreadsheet:
# S:\All\Structured Risk\NEPOOL\Ancillary Services\Monthly Ancillary Updates\Actuals\Reserves in ASM Phase II.xls
# In tsdb, from the "Nepool SR RSVCHG Reports"
#
#
.pull_hist_rtreserves <- function( start.dt=as.POSIXct("2008-01-01 01:00:00"),
                             end.dt=Sys.time())
{
  symbols <- c(
    "NERpt_SR_RSVCH_4001P_FRSysChgRt",  "me.tmnsr.fwdres.rate",
    "NERpt_SR_RSVCH_4001P_LZFRIncChR",  "me.tmor.fwdres.rate",
    "NERpt_SR_RSVCH_4001N_LZRTRChgRt",  "me.tmnsr.rtreserve.rate",
    "NERpt_SR_RSVCH_4001O_LZRTRChgRt",  "me.tmor.rtreserve.rate",
    "NERpt_SR_RSVCH_4001S_LZRTRChgRt",  "me.tmsr.rtreserve.rate",
    "NEPOOL_SMD_RT_4001_LD",            "me.rtlo",
               
    "NERpt_SR_RSVCH_4002P_FRSysChgRt",  "nh.tmnsr.fwdres.rate",
    "NERpt_SR_RSVCH_4002P_LZFRIncChR",  "nh.tmor.fwdres.rate",
    "NERpt_SR_RSVCH_4002N_LZRTRChgRt",  "nh.tmnsr.rtreserve.rate",
    "NERpt_SR_RSVCH_4002O_LZRTRChgRt",  "nh.tmor.rtreserve.rate",
    "NERpt_SR_RSVCH_4002S_LZRTRChgRt",  "nh.tmsr.rtreserve.rate",
    "NEPOOL_SMD_RT_4002_LD",            "nh.rtlo",
               
    "NERpt_SR_RSVCH_4003P_FRSysChgRt",  "vt.tmnsr.fwdres.rate",
    "NERpt_SR_RSVCH_4003P_LZFRIncChR",  "vt.tmor.fwdres.rate",
    "NERpt_SR_RSVCH_4003N_LZRTRChgRt",  "vt.tmnsr.rtreserve.rate",
    "NERpt_SR_RSVCH_4003O_LZRTRChgRt",  "vt.tmor.rtreserve.rate",
    "NERpt_SR_RSVCH_4003S_LZRTRChgRt",  "vt.tmsr.rtreserve.rate",
    "NEPOOL_SMD_RT_4003_LD",            "vt.rtlo",
               
    "NERpt_SR_RSVCH_4004P_FRSysChgRt",  "ct.tmnsr.fwdres.rate",  # fwd res system charge rate, Tab 5
    "NERpt_SR_RSVCH_4004P_LZFRIncChR",  "ct.tmor.fwdres.rate",   # fwd res incremental charge rate, Tab 5
    "NERpt_SR_RSVCH_4004N_LZRTRChgRt",  "ct.tmnsr.rtreserve.rate",
    "NERpt_SR_RSVCH_4004O_LZRTRChgRt",  "ct.tmor.rtreserve.rate",
    "NERpt_SR_RSVCH_4004S_LZRTRChgRt",  "ct.tmsr.rtreserve.rate",
    "NEPOOL_SMD_RT_4004_LD",            "ct.rtlo",
               
    "NERpt_SR_RSVCH_4005P_FRSysChgRt",  "ri.tmnsr.fwdres.rate",
    "NERpt_SR_RSVCH_4005P_LZFRIncChR",  "ri.tmor.fwdres.rate",
    "NERpt_SR_RSVCH_4005N_LZRTRChgRt",  "ri.tmnsr.rtreserve.rate",
    "NERpt_SR_RSVCH_4005O_LZRTRChgRt",  "ri.tmor.rtreserve.rate",
    "NERpt_SR_RSVCH_4005S_LZRTRChgRt",  "ri.tmsr.rtreserve.rate",
    "NEPOOL_SMD_RT_4005_LD",            "ri.rtlo",
               
    "NERpt_SR_RSVCH_4006P_FRSysChgRt",  "sema.tmnsr.fwdres.rate",
    "NERpt_SR_RSVCH_4006P_LZFRIncChR",  "sema.tmor.fwdres.rate",
    "NERpt_SR_RSVCH_4006N_LZRTRChgRt",  "sema.tmnsr.rtreserve.rate",
    "NERpt_SR_RSVCH_4006O_LZRTRChgRt",  "sema.tmor.rtreserve.rate",
    "NERpt_SR_RSVCH_4006S_LZRTRChgRt",  "sema.tmsr.rtreserve.rate",
    "NEPOOL_SMD_RT_4006_LD",            "sema.rtlo",
               
    "NERpt_SR_RSVCH_4007P_FRSysChgRt",  "wma.tmnsr.fwdres.rate",
    "NERpt_SR_RSVCH_4007P_LZFRIncChR",  "wma.tmor.fwdres.rate",
    "NERpt_SR_RSVCH_4007N_LZRTRChgRt",  "wma.tmnsr.rtreserve.rate",
    "NERpt_SR_RSVCH_4007O_LZRTRChgRt",  "wma.tmor.rtreserve.rate",
    "NERpt_SR_RSVCH_4007S_LZRTRChgRt",  "wma.tmsr.rtreserve.rate",
    "NEPOOL_SMD_RT_4007_LD",            "wma.rtlo",
               
    "NERpt_SR_RSVCH_4008P_FRSysChgRt",  "nema.tmnsr.fwdres.rate",
    "NERpt_SR_RSVCH_4008P_LZFRIncChR",  "nema.tmor.fwdres.rate",
    "NERpt_SR_RSVCH_4008N_LZRTRChgRt",  "nema.tmnsr.rtreserve.rate",
    "NERpt_SR_RSVCH_4008O_LZRTRChgRt",  "nema.tmor.rtreserve.rate",
    "NERpt_SR_RSVCH_4008S_LZRTRChgRt",  "nema.tmsr.rtreserve.rate",
    "NEPOOL_SMD_RT_4008_LD",            "nema.rtlo"
  )


  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))

  HH <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], start.dt, end.dt)
  colnames(HH) <- symbols[,"shortNames"]
  head(HH)

  HH[which(is.na(HH))] <- 0

  # calculate other variables used (I'm not calling them ccg. as
  # in the spreadsheet! because they are pool costs)
  HH$rop.tmnsr.fwdres.cost <- (
    HH$me.tmnsr.fwdres.rate   * HH$me.rtlo +
    HH$nh.tmnsr.fwdres.rate   * HH$nh.rtlo + 
    HH$vt.tmnsr.fwdres.rate   * HH$vt.rtlo + 
    HH$ri.tmnsr.fwdres.rate   * HH$ri.rtlo + 
    HH$sema.tmnsr.fwdres.rate * HH$sema.rtlo + 
    HH$wma.tmnsr.fwdres.rate  * HH$wma.rtlo )
  HH$rop.tmor.fwdres.cost <- (
    HH$me.tmor.fwdres.rate   * HH$me.rtlo +
    HH$nh.tmor.fwdres.rate   * HH$nh.rtlo + 
    HH$vt.tmor.fwdres.rate   * HH$vt.rtlo + 
    HH$ri.tmor.fwdres.rate   * HH$ri.rtlo + 
    HH$sema.tmor.fwdres.rate * HH$sema.rtlo + 
    HH$wma.tmor.fwdres.rate  * HH$wma.rtlo )
  HH$rop.rtlo <- ( HH$me.rtlo + HH$nh.rtlo +
    HH$vt.rtlo + HH$ri.rtlo + HH$sema.rtlo + HH$wma.rtlo )
  
  HH$ct.tmnsr.fwdres.cost <- HH$ct.tmnsr.fwdres.rate   * HH$ct.rtlo
  HH$ct.tmor.fwdres.cost  <- HH$ct.tmor.fwdres.rate    * HH$ct.rtlo 
  HH$ct.rtlo2 <- HH$ct.rtlo
  
  HH$nema.tmnsr.fwdres.cost <- HH$nema.tmnsr.fwdres.rate   * HH$nema.rtlo
  HH$nema.tmor.fwdres.cost  <- HH$nema.tmor.fwdres.rate    * HH$nema.rtlo 
  HH$nema.rtlo2 <- HH$nema.rtlo

  HH$rop.tmnsr.rtreserve.cost <- (
    HH$me.tmnsr.rtreserve.rate   * HH$me.rtlo +
    HH$nh.tmnsr.rtreserve.rate   * HH$nh.rtlo + 
    HH$vt.tmnsr.rtreserve.rate   * HH$vt.rtlo + 
    HH$ri.tmnsr.rtreserve.rate   * HH$ri.rtlo + 
    HH$sema.tmnsr.rtreserve.rate * HH$sema.rtlo + 
    HH$wma.tmnsr.rtreserve.rate  * HH$wma.rtlo )
  HH$rop.tmor.rtreserve.cost <- (
    HH$me.tmor.rtreserve.rate   * HH$me.rtlo +
    HH$nh.tmor.rtreserve.rate   * HH$nh.rtlo + 
    HH$vt.tmor.rtreserve.rate   * HH$vt.rtlo + 
    HH$ri.tmor.rtreserve.rate   * HH$ri.rtlo + 
    HH$sema.tmor.rtreserve.rate * HH$sema.rtlo + 
    HH$wma.tmor.rtreserve.rate  * HH$wma.rtlo )
  HH$rop.tmsr.rtreserve.cost <- (
    HH$me.tmsr.rtreserve.rate   * HH$me.rtlo +
    HH$nh.tmsr.rtreserve.rate   * HH$nh.rtlo + 
    HH$vt.tmsr.rtreserve.rate   * HH$vt.rtlo + 
    HH$ri.tmsr.rtreserve.rate   * HH$ri.rtlo + 
    HH$sema.tmsr.rtreserve.rate * HH$sema.rtlo + 
    HH$wma.tmsr.rtreserve.rate  * HH$wma.rtlo )
  
  HH$ct.tmnsr.rtreserve.cost <- HH$ct.tmnsr.rtreserve.rate   * HH$ct.rtlo 
  HH$ct.tmor.rtreserve.cost  <-  HH$ct.tmor.rtreserve.rate   * HH$ct.rtlo 
  HH$ct.tmsr.rtreserve.cost  <- HH$ct.tmsr.rtreserve.rate    * HH$ct.rtlo 

  HH$nema.tmnsr.rtreserve.cost <- HH$nema.tmnsr.rtreserve.rate  * HH$nema.rtlo 
  HH$nema.tmor.rtreserve.cost  <- HH$nema.tmor.rtreserve.rate   * HH$nema.rtlo 
  HH$nema.tmsr.rtreserve.cost  <- HH$nema.tmsr.rtreserve.rate   * HH$nema.rtlo 

  indPeak <- secdb.getBucketMask("NEPOOL", "5x16", index(HH)[1],
                                 index(HH)[nrow(HH)])
  HH$ct.rtlo.5x16   <- HH$ct.rtlo   * indPeak[, "5x16"]
  HH$nema.rtlo.5x16 <- HH$nema.rtlo * indPeak[, "5x16"]
  HH$rop.rtlo.5x16  <- HH$rop.rtlo  * indPeak[, "5x16"]

 
  HH
}


#######################################################################
# retrurn the monthly average to paste on Sheet2
#
.update_xlsx_reserves <- function( months )
{
  RR <- do.call(rbind,
   lapply(months, .monthly_actuals_rtreserves))

  rLog("\n\n============================================================")
  aux <- RR
  colnames(aux) <- 1:ncol(aux)
  print(aux[,1:24])
  rLog("\nPaste this on Monthly Ancillary Updates/Actuals/Reserves in ASM Phase II.xls")
  rLog("in sheet Pivot2.  After that, extend the Summary sheet.")
  
  
}




#######################################################################
#######################################################################
#
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.reserves.R")
  #source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.R")


  
  asOfDate  <- as.Date(secdb.dateRuleApply(Sys.Date(), "-0b"))
  startTime <- as.POSIXct("2013-03-01 01:00:00")   # for hist data
  endTime   <- Sys.time()                          # for hist data 
  HH <- .pull_hist_rtreserves(startTime, endTime)





    
  MM <- .monthly_actuals_rtreserves( as.Date("2013-04-01") )


  ###############################################################
  # How many times has NEMA TMOR bind in summer 2014
  #
  rt <- tsdb.readCurve("NERpt_SR_RSVCH_4008O_LZRTRChgRt",
                       as.POSIXct("2014-06-01 01:00:00"),
                       as.POSIXct("2014-10-01 00:00:00"))
  mask <- secdb.getBucketMask( "nepool", "5x16",
                              as.POSIXct("2014-06-01 01:00:00"),
                              as.POSIXct("2014-10-01 00:00:00"))

  rt2 <- merge(rt, mask)
  rt2 <- rt2[which(rt2[,3]),]
  
  rt2$day <- format(rt2$time-1, "%Y-%m-%d")
  res <- cast(rt2, day ~ ., mean, value="value")
  length(which(res$`(all)` != 0))
  
  

  
}
