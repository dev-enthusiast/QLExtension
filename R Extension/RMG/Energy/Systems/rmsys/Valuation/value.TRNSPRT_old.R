# Value transport deals using spread options, Kirk approximation.
# 
#
# NOT WORKING YET!!!

value.TRNSPRT <- function(vrp, dd, deals, options){

  source("H:/user/R/RMG/Finance/BlackScholes/spread.option.R")
  cat(paste("Working on inst.type=", options$instrument.type.id, sep=""))
  flush.console()
##   dd  <- subset(dd, INSTRUMENT.TYPE.ID==options$instrument.type.id)
##   deal.ids <- unique(dd$DEAL.ID)
##   vrp   <- subset(vrp, DEAL.ID %in% deal.ids)
##   deals <- subset(deals, DEAL.ID %in% deal.ids)

  query <- paste("select ",
    "dd.deal_id, dd.leg_id, dd.version, dd.BUY_SELL_FLAG, ",
    "dd.CALL_PUT_FLAG, dd.START_DATE, dd.END_DATE, dd.VOLUME, ", 
    "dd.FREQ, dd.INDEX1, dd.INDEX1_ADDER, dd.INDEX1_MPLIER, ", 
    "dd.INDEX2, dd.INDEX2_ADDER, dd.INDEX2_MPLIER, dd.CORR_INDEX, ", 
    "dd.VOL_INDEX1, dd.VOL_INDEX2, dd.STRIKE, dd.STRIKE_INDEX, ", 
    "dd.FUEL2 from deal_details dd where INSTRUMENT_TYPE_ID = 8 ",
    "and dd.END_DATE > to_date('",options$cob, "', 'yyyy-mm-dd')", sep="")
  options$con <- odbcConnect("RMSYSP", uid="RMSYS", pwd="RMSYS")
  dd <- sqlQuery(options$con, query)
  colnames(dd) <- gsub("_", ".", colnames(dd))
  dd$START.DATE <- as.Date(dd$START.DATE)
  dd$END.DATE <- as.Date(dd$END.DATE)
  
  #------------------ get only the deals that got valued --------------
  #dd <- subset(dd, DEAL.ID %in% unique(vrp$DEAL.ID))

  #-------------------------------- get the prices --------------------
  uPriceID <- sort(unique(c(dd$INDEX1, dd$INDEX2, dd$CORR.INDEX)))
  uPriceID <- paste("('",paste(uPriceID,sep="",collapse="', '"),"')",sep="")
  query <- paste("select index_num, from_date, to_date, ",
    "price from price_points where effective_date=",
    "to_date('", options$cob, "', 'yyyy-mm-dd') and index_num in ",
    uPriceID, sep="")
  PP <- sqlQuery(options$con, query)
  colnames(PP) <- c("INDEX1", "START.DATE", "END.DATE", "PRICE1")
  dd <- merge(dd, PP, all.x=T)
  colnames(PP) <- c("INDEX2", "START.DATE", "END.DATE", "PRICE2")
  dd <- merge(dd, PP, all.x=T)
  colnames(PP) <- c("VOL.INDEX1", "START.DATE", "END.DATE", "VOL1")
  dd <- merge(dd, PP, all.x=T)
  colnames(PP) <- c("VOL.INDEX2", "START.DATE", "END.DATE", "VOL2")
  dd <- merge(dd, PP, all.x=T)
  colnames(PP) <- c("CORR.INDEX", "START.DATE", "END.DATE", "CORR")
  dd <- merge(dd, PP[,-3], all.x=T)

  P1 <- dd$PRICE1*dd$INDEX1.MPLIER + dd$INDEX1.ADDER
  P2 <- dd$PRICE2*dd$INDEX2.MPLIER + dd$INDEX2.ADDER
  
  cols <- c("DEAL.ID", "LEG.ID", "VERSION", "BUY.SELL.FLAG", "CALL.PUT.FLAG",
     "START.DATE", "END.DATE", "VOLUME", "FUEL2", "CORR")
  MM <- dd[, cols]; MM$COB <- options$cob; MM <- cbind(MM, P1, P2)

  
  
  vrp <- subset(vrp, DEAL.ID %in% unique(dd$DEAL.ID))
  
  cols <- c("COB","VAL.ID","DEAL.ID","LEG.ID", "SIDE", "CONTRACT.DATE",
    "PRICE", "VAL.PRICE", "FXRATE", "IR", "NOTIONAL", "MTM", "EXP.DATE",
    "PAYMENT.DATE", "VOL", "STRIKE")
  vrp <- vrp[,colnames(vrp)%in%cols]
  vrp[1:3,]
  # get the deals that have not expired yet
  vrp <- subset(vrp, vrp$EXP.DATE >= vrp$COB)
  
  vrp$Tex <- as.numeric(vrp$EXP.DATE-vrp$COB)/365
  vrp$DF  <- exp(-vrp$IR*as.numeric(vrp$PAYMENT.DATE-vrp$COB)/365)
  # NOTE: the NOTIONAL is negative when you sold the option!
  
  #-----------------------------------Get the CALL.PUT flag-----
  aux <- as.data.frame(table(dd$CALL.PUT.FLAG, dd$DEAL.ID))
  aux <- aux[-which(aux$Freq==0),]  # the deal is either a call or a put
  aux$Freq <- NULL
  colnames(aux) <- c("CALL.PUT.FLAG","DEAL.ID")
  vrp <- merge(vrp, aux)
  cp.flag <- ifelse(vrp$CALL.PUT.FLAG=="C","CALL","PUT")
 
  base.deals <- data.frame(S=vrp$PRICE, sigma=vrp$VOL, Tex=vrp$Tex, r=0,
    K=vrp$STRIKE, type=cp.flag)
  vrp$RMG.unit <- vrp$DF*blackscholes(base.deals)$value
  vrp$RMG.MTM  <- vrp$NOTIONAL * vrp$RMG.unit
  #---- add the ETI
  vrp <- merge(deals[,c("DEAL.ID", "DEAL.REFERENCE.CD")], vrp)
  head(vrp)

  cat("   Done.\n"); flush.console()
  return(vrp)
}


##   base.deals <- cbind(base.deals, NOTIONAL=vrp$NOTIONAL, DF=vrp$DF,
##                       CONTRACT.DATE=vrp$CONTRACT.DATE)
##   colnames(base.deals)[which(colnames(base.deals)=="S")] <- "PRICE"
##   sdeals <- make.shifted.deals(base.deals, scenario)
##   colnames(sdeals)[which(colnames(sdeals)=="PRICE")] <- "S"  
##   value  <- blackscholes(sdeals)$value 
##   sdeals$MTM <- value*sdeals$NOTIONAL*sdeals$DF
  
##   res <- aggregate(sdeals$MTM, by=list(CONTRACT.DATE=sdeals$CONTRACT.DATE,
##                                         SHIFT=sdeals$SHIFT), sum)
##   colnames(res)[3] <- "MTM"
##   res <- cbind(INSTRUMENT.TYPE="FINOPT", res)
