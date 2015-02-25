# Deal with MX issues. 
#
# .compare_load_demandbids 
# .get_day1_PnL
# .get_mx_demandbids    - demand bids as received from MX
# .get_mx_loadassetIDs
# .get_mx_rtload        - actual NEPOOL load from RTLOAD report
# .get_mx_load_forecast - short term   
# .get_mx_trades

## Two products MX sells.  Fixed and Variable.  The Fixed has a contract
## term, the Variable is just a month ahead fixed price product.
## Currently we have the Variable forecasted into perpetuity.  They use
## that information and various haircuts in advance of fixing the price
## to understand their budgets etc.
#
# FP is fixed price and VP is a month ahead product in which the price
# is fixed before the beginning of the month.
#
# short term load forecasts   <----- 
## NEPOOL_ld_fcst_CLP_MX_FP
## NEPOOL_ld_fcst_CLP_MX_VP
## NEPOOL_ld_fcst_UI_MX_FP
## NEPOOL_ld_fcst_UI_MX_VP
## NEPOOL_ld_fcst_MECO_SEMA_MX_FP
## NEPOOL_ld_fcst_MECO_SEMA_MX_VP
## NEPOOL_ld_fcst_MECO_NEMA_MX_FP
## NEPOOL_ld_fcst_MECO_NEMA_MX_VP
## NEPOOL_ld_fcst_MECO_WMA_MX_FP
## NEPOOL_ld_fcst_MECO_WMA_MX_VP
## NEPOOL_ld_fcst_WMECO_WMA_MX_FP
## NEPOOL_ld_fcst_WMECO_WMA_MX_VP

# long term load forecasts <----
## ISONE_LT_CLP_CONNEC_MX_F
## ISONE_LT_CLP_CONNEC_MX_V
## ISONE_LT_MECO_NEMASS_MX_F
## ISONE_LT_MECO_NEMASS_MX_V
## ISONE_LT_MECO_SEMASS_MX_F
## ISONE_LT_MECO_SEMASS_MX_V
## ISONE_LT_MECO_WCMASS_MX_F
## ISONE_LT_MECO_WCMASS_MX_V
## ISONE_LT_UI_Connec_MX_F
## ISONE_LT_UI_Connec_MX_V



#########################################################################
# get the load and the demandbids and prepare them for analysis
#
.compare_load_demandbids <- function(startDt, endDt)
{
  LD <- .get_mx_rtload(startDt, endDt, byZone=TRUE)
  LD <- cbind(datetime=as.POSIXct(rownames(LD)), data.frame(LD))
  rownames(LD) <- NULL
  colnames(LD) <- gsub("^X", "", colnames(LD))
  LD <- melt(LD, id=1)
  LD$type <- "load"
  LD$mw   <- -LD$value
  LD$zone <- LD$variable
  
  
  DB <- .get_mx_demandbids(startDt, endDt)  # by zone, long format
  DB <- DB[,c("datetime", "zone", "mw")]
  DB$type  <- "demandbids"

  data <- rbind(DB[,c("datetime", "zone", "mw", "type")],
                LD[,c("datetime", "zone", "mw", "type")])

  HE <- as.numeric(format(data$datetime, "%H"))
  data$bucket <- ifelse(HE %in% 8:23, "7X16", "7X8")

  # NOT FINISHED!
  
  data
}

#########################################################################
# 
#
.get_day1_PnL <- function( days )
{
  
  trades <- lapply(as.list(days), function(day) {
  
  })
  
}



#########################################################################
# the short term forecast (1 day ahead) that we get from MX every day
#
get_mx_load_forecast <- function(startDt, endDt)
{
  symbs <- c("mx_nepool_ld_fcst_ct",
             "mx_nepool_ld_fcst_me",
             "mx_nepool_ld_fcst_nema",
             "mx_nepool_ld_fcst_nh",
             "mx_nepool_ld_fcst_ri",
             "mx_nepool_ld_fcst_sema",
             "mx_nepool_ld_fcst_vt",
             "mx_nepool_ld_fcst_wma")

  res <- FTR:::FTR.load.tsdb.symbols(symbs, startDt, endDt)
  colnames(res) <- gsub(".*_(.*)$", "\\1", colnames(res))

  out <- matrix(0, nrow=nrow(res), ncol=8, dimnames=list(format(index(res)),
    c("ct", "me", "nema", "nh", "ri", "sema", "vt", "wma")))
  out[, colnames(res)] <- res

  out
}

#########################################################################
# MX load assets when transfered to us. 
#
.get_mx_loadassetIDs <- function()
{
  aux <- read.csv(cc <- textConnection("
    10828,MECO_SEMA_DTEET_LOAD,4006
    10829,MECO_WCMA_DTEET_LOAD,4007
    10830,MECO_NEMA_DTEET_LOAD,4008
    12380,MXENERGY_NANT_LOAD,4006
    13543,MXENERGY_UI_LOAD,4004
    14215,MXENERGY_WMECO_LOAD,4007
    14216,MXENERGY_CPL_LOAD,4004"
    ), header=FALSE, col.names=c("assetID", "assetName", "zone"))
  close(cc)
  
  aux
}


#########################################################################
# Actual NEPOOL load from tsdb
#
.get_mx_rtload <- function(startDt, endDt, byZone=FALSE)
{
  ids <- .get_mx_loadassetIDs()

  symbs <- paste("nerpt_sd_load_", ids$assetID, "_prodobl", sep="")

  LD <- FTR:::FTR.load.tsdb.symbols(symbs, startDt, endDt)

  if (byZone){
    names(LD) <- gsub("nerpt_sd_load_(.*)_prodobl", "\\1", names(LD))

    iz <- split(ids, ids$zone)
    res <- lapply(iz, function(x){
      aux <- LD[,as.character(x$assetID),drop=FALSE]
      apply(aux, 1, sum)      
    })
    LD <- do.call(cbind, res)
  }
  
  LD
}
  

#########################################################################
# 
#
.get_mx_trades <- function(asOfDate, from="R_ARCHIVE")
{
  if (from == "R_ARCHIVE") {
    QQ <- .read_R_position_archive(asOfDate)
    QQ <- subset(QQ, portfolio %in% c("NEPOOL ACCRUAL PORTFOLIO",
                                      "NEPOOL MTM HEDGES OF ACCRUAL"))
    QQ <- subset(QQ, trading.book %in% "MXNEACC")
  }

  QQ
}



#########################################################################
#########################################################################
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)
  
  #source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.mx.R")

  startDt <- as.POSIXct("2011-08-01 01:00:00")
  endDt   <- Sys.time()

  #LD <- .get_mx_rtload(startDt, endDt, byZone=TRUE)
  
  #DB <- .get_mx_demandbids(startDt, endDt)  # by zone, long format

  data <- .compare_load_demandbids(startDt, endDt)

  # check how we've done historically on 
  days <- seq(as.Date("2012-04-01"), as.Date("2012-12-05"), by="1 day")
  bal <- PM:::secdb.runBalReport( days, "MXNEACC")

  require(lattice)
  xyplot(New.Trades/1000 ~ Period, data=bal, type=c("g", "h"),
     xlab="Period",
     ylab="Day 1 PnL, $000")
  
  
  plot(bal$Period, bal$New.Trades/1000, type="h", col="blue",
       xlab="Period",
       ylab="Day 1 PnL, $000")
  
  
  xyplot(mw ~ datetime|zone, data=data, groups=type,
         scales="free", type="l")
  
  
  
  
  
  
}




## #########################################################################
## # Actual NEPOOL load from SD_RTLOAD report
## #
## .get_mx_rtload_OLD<- function(startDt, endDt)
## {
##   ids <- .get_mx_loadassetIDs()

##   months <- as.Date(unique(format(seq(startDt, endDt, by="1 day"),
##                                   "%Y-%m-01")))
##   fnames <- NULL
##   for (m in 1:length(months)){
##     fnames <- c(fnames, .get_report_csvfilenames(org="CCG",
##       month=as.Date(months[m]), reportName="SD_RTLOAD"))
##   }

##   res <- lapply(as.list(fnames), function(x){
##     #browser()
##     asOfDate <- substr(strsplit(basename(x), "_")[[1]][4], 1, 8)
##     aux <- .process_report(x)[[1]]
##     aux <- subset(aux, Asset.ID %in% ids$assetID)
##     cbind(Report.Date=asOfDate, aux)
##   })
##   res <- do.call(rbind, res)
##   rownames(res) <- NULL

## }
  
