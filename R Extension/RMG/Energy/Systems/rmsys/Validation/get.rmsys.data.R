# Pull rmsys data for later use.  You can pass a metacalc.id if you know it. 
#
#
# Written by Adrian Dragulescu on 8-Sep-2006


get.rmsys.data <- function(options){
  
  source("H:/user/R/RMG/Utilities/Database/rmsys/get.official.run.R")
  options$con <- odbcConnect("rmsysp", uid="rmsys_read", pwd="rmsys_read")

  if (length(options$metacalc.id)==0){
    loptions <- options; loptions$COB <- options$asOfDate
    aux <- get.official.run(loptions)
    options$metacalc.id <- aux$METACALC_ID
    options$cob <- as.character(aux$COB)
    cat(paste("Using metacalc.id = ", options$metacalc.id, " for ",
    "close of business day ", options$cob, ".\n", sep="")); flush.console()
#  options$metacalc.id <- 957
  }
    
  query <- paste("SELECT vrp.COB, vrp.VAL_ID, vrp.DEAL_ID, vrp.LEG_ID, ",
     "vrp.SIDE, vrp.CONTRACT_DATE, vrp.PRICE, vrp.VAL_PRICE, vrp.FXRATE, ",
     "vrp.IR, vrp.NOTIONAL, vrp.MTM, vrp.EXP_DATE, vrp.PAYMENT_DATE, ",
     "vrp.VOL, vrp.STRIKE, vrp.CORR, vrp.delta, vrp.vega ",
     "FROM val_results_position vrp, calc_kickoff ck, deals d ",
     "where vrp.val_id in (select calc_id from calc_kickoff where " ,
     "metacalc_id = ", options$metacalc.id, ") ",             
     "and vrp.val_id = ck.calc_id and d.deal_id = vrp.deal_id", sep="")
     #and vrp.PORTFOLIO_ID in ", options$portfolio.id)
  vrp <- sqlQuery(options$con, query)
  colnames(vrp) <- gsub("_", ".", colnames(vrp))
  vrp$COB <- as.Date(vrp$COB); vrp$EXP.DATE <- as.Date(vrp$EXP.DATE)
  vrp$CONTRACT.DATE <- as.Date(vrp$CONTRACT.DATE)
  vrp$PAYMENT.DATE <- as.Date(vrp$PAYMENT.DATE) 
  save(vrp, file="C:/temp/vrp.RData")

  query <- paste("select dd.DEAL_ID, dd.LEG_ID, dd.VERSION, ", 
    "dd.INSTRUMENT_TYPE_ID, dd.BUY_SELL_FLAG, dd.CALL_PUT_FLAG, ",
    "dd.START_DATE, dd.END_DATE, dd.INDEX1_MPLIER, dd.INDEX1_ADDER, ",
    "dd.INDEX2_MPLIER, dd.INDEX2_ADDER from deal_details dd", sep="") 
  dd <- sqlQuery(options$con, query)
  colnames(dd) <- gsub("_", ".", colnames(dd))
  save(dd, file="C:/temp/dd.RData")

  query <- paste("select d.DEAL_ID, d.VERSION, d.STATUS, d.DEAL_REFERENCE_CD ",
    "from deals d", sep="")
  deals <- sqlQuery(options$con, query)
  colnames(deals) <- gsub("_", ".", colnames(deals))
  save(deals, file="C:/temp/deals.RData")
  
  odbcCloseAll()
  return(list(vrp, dd, deals, options))
}
