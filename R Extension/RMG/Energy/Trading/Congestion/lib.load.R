# 
# get_coincident_peak -- given two time series, extract the coinciden peak
# get_fwd_ccg_loads_tsdb -- 
# get_hist_zonal_load -- read historical zonal NEPOOL load from tsdb
# 
# tsdb symbols for tlp loads from Ling
## NEPOOL_ME_tsdb
## NEPOOL_NH_tsdb
## NEPOOL_CT_tsdb
## NEPOOL_VT_tsdb
## NEPOOL_RI_tsdb
## NEPOOL_NEMA_tsdb
## NEPOOL_SEMA_tsdb
## NEPOOL_WMA_tsdb


## ###########################################################################
## # The RACM forecast for the NEPOOL zonal loads.  -- ONLY FOR CAL 11 --
## # Got symbols by Long Han, done on 2/26/2010
## # 
## get_fwd_ccg_loads_tsdb <- function(startTime, endTime, zone=c("CT", "ME",
##   "NEMA", "SEMA", "RI", "NH", "WMA"))
## {
##   DIR <- "H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/CNE/"
##   symbs <- read.csv(paste(DIR, "cne_tsdb_symbols.csv", sep=""))
##   symbs$zone <- NULL
##   symbs <- subset(symbs, participant=="pool")
  
##   symbs$longzone <- gsub("NEPOOL_FCST_(.*)_DEMAND", "\\1", symbs$symbol)

##   ZZ <- data.frame(longzone=c("CT", "ME", "NEMA",
##     "SEMA", "RI","NH", "WCMA"), zone=c("CT", "ME",
##     "NEMA", "SEMA", "RI", "NH", "WMA"))
  
##   symbs <- merge(symbs, ZZ, all.x=TRUE)
##   symbs <- symbs[which(symbs$zone %in% zone), ]
##   symbs <- split(symbs$symbol, symbs$zone)

##   LL <- vector("list", length=length(symbs))
##   for (r in 1:length(LL)){
##     rLog("Working on zone", names(symbs)[r]) 
##     LL[[r]] <- FTR:::FTR.load.tsdb.symbols(symbs[[r]], startTime, endTime)
##   }
##   LL <- do.call(cbind, LL)
##   names(LL) <- names(symbs)

##   LL
## }





#######################################################################
# Get coincident peak in x when X peaks.
# x, X are zoo objects. x,X can have multiple columns.  Then
# compare only on matching column names. 
# 
#
get_coincident_peak <- function(x, X, byMonth=TRUE)
{
  if (byMonth)
  {
    yyyymm.X <- format(index(X), "%Y-%m")
    XX <- cbind(datetime=index(X), yyyymm=yyyymm.X, as.data.frame(X))
    XX <- melt(XX, id=1:2)

    aux <- split(XX, list(XX$yyyymm, XX$variable))
    peakLoad <- lapply(aux, function(x){
      x[which.max(x$value),]})
    peakLoad <- do.call("rbind", peakLoad)
    rownames(peakLoad) <- NULL
    names(peakLoad)[ncol(peakLoad)] <- "peak.X"
    peakLoad$variable <- as.character(peakLoad$variable)

    # extract the coincident peak, and calculate the peak.x
    xx <- cbind(datetime=index(x), as.data.frame(x))
    xx <- melt(xx, id=1)
    names(xx)[ncol(xx)] <- "cpeak.x"
    xx$variable <- as.character(xx$variable)
    xx$yyyymm <- format(xx$datetime, "%Y-%m")
    maxx <- cast(xx, variable + yyyymm ~., max, fill=NA, value="cpeak.x")
    names(maxx)[ncol(maxx)] <- "peak.x"
    
    res <- merge(peakLoad, xx, all.x=TRUE)
    res <- merge(res, maxx, all.x=TRUE)
  } else {
    stop("Only byMonth=TRUE is implemented!")
  }

  return(res)
}


#######################################################################
# Get coincident peak in x when X peaks.
# x, X zoo objects. x can have multiple columns.
# 
#
get_hist_zonal_load <- function(startTime, endTime, zone=c("CT", "ME",
  "NEMA", "SEMA", "RI", "NH", "WMA"), market=c("DA", "RT"))
{
  ZZ <- data.frame(ptid=4001:4008, zone=c("ME", "NH", "VT", "CT", 
    "RI", "SEMA", "WMA", "NEMA"))

  ZZ <- ZZ[which(ZZ$zone %in% zone),]
  symbols <- paste("nepool_zonal_", ZZ$ptid, "_load_", market, sep="")
  LL.z <- FTR:::FTR.load.tsdb.symbols(symbols, startTime, endTime)
  colnames(LL.z) <- ZZ$zone
  
  LL.z
}


