# 
# 
# get_cleared_demandbids
#

###########################################################################
# Compare ccg short term load forecast with settlements 
# 
compare_demandbids_with_settlement <- function(day)
{
  

}




###########################################################################
# Get the demand bids that have cleared in DA
# from sd_DALOCSUM report folder in tsdb
# Note that the NERpt_SD_DAClrd_4001_ClearedMW has both the demand bids + virtuals at that location!
# The NERpt_DALoS_4001_ClrdBids has only the demand bids, which is correct!
#
get_cleared_demandbids <- function(startDt, endDt=Sys.now(), company=c("exgen"))
{
  symbs <- if (company == "exgen") {    
    c("NERpt_DALoS_4001_ClrdBids", "ME",   "exgen",  "iso",
      "NERpt_DALoS_4002_ClrdBids", "NH",   "exgen",  "iso", 
      "NERpt_DALoS_4003_ClrdBids", "VT",   "exgen",  "iso",
      "NERpt_DALoS_4004_ClrdBids", "CT",   "exgen",  "iso",
      "NERpt_DALoS_4005_ClrdBids", "RI",   "exgen",  "iso",
      "NERpt_DALoS_4006_ClrdBids", "SEMA", "exgen",  "iso",
      "NERpt_DALoS_4007_ClrdBids", "WMA",  "exgen",  "iso",
      "NERpt_DALoS_4008_ClrdBids", "NEMA", "exgen",  "iso")
    
    } else if (company == "cne") {
    c("NCRpt_DALoS_4001_ClrdBids", "ME",   "cne",    "iso",
      "NCRpt_DALoS_4002_ClrdBids", "NH",   "cne",    "iso", 
      "NCRpt_DALoS_4003_ClrdBids", "VT",   "cne",    "iso",
      "NCRpt_DALoS_4004_ClrdBids", "CT",   "cne",    "iso",
      "NCRpt_DALoS_4005_ClrdBids", "RI",   "cne",    "iso",
      "NCRpt_DALoS_4006_ClrdBids", "SEMA", "cne",    "iso",
      "NCRpt_DALoS_4007_ClrdBids", "WMA",  "cne",    "iso",
      "NCRpt_DALoS_4008_ClrdBids", "NEMA", "cne",    "iso")
  } else { stop("wrong company!") }
  
  symbs <- matrix(symbs, byrow=TRUE, ncol=4)

  res <- FTR:::FTR.load.tsdb.symbols(as.character(symbs[,1]), startDt, endDt)
  colnames(res) <- gsub(".*_DALoS_(.*)_ClrdBids", "\\1", colnames(res))
  head(res)  

  
  res            
}




###########################################################################
###########################################################################
#
.test <- function()
{
  require(CEGbase)
  require(reshape)
  require(zoo)
  require(SecDb)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.demandbids.R")

  startDt <- as.POSIXct(paste(format(Sys.Date()-5), "01:00:00"))
  endDt   <- as.POSIXct(paste(format(Sys.Date()), "00:00:00"))  # 2

  res <- get_cleared_demandbids(startDt, endDt)
  

  
  

  

}
