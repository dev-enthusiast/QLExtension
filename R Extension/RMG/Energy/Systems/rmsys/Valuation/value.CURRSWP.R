# Value currency swaps.  Not working yet. 
#
#
#

value.CURRSWP <- function(vrp, dd, deals, options){

  cat(paste("Working on inst.type=", options$instrument.type.id, sep=""))
  flush.console()
  dd  <- subset(dd, INSTRUMENT.TYPE.ID==options$instrument.type.id)
  deal.ids <- unique(dd$DEAL.ID)
  vrp <- subset(vrp, DEAL.ID %in% deal.ids)
  vrp <- merge(dd[,c("DEAL.ID", "LEG.ID")], vrp)
  deals <- subset(deals, DEAL.ID %in% deal.ids)
  
  cols <- c("COB","VAL.ID","DEAL.ID","LEG.ID", "SIDE", "CONTRACT.DATE",
    "PRICE", "VAL.PRICE", "FXRATE", "IR", "NOTIONAL", "MTM", "PAYMENT.DATE")
  vrp <- vrp[,colnames(vrp)%in%cols]
  vrp[1:3,]

  vrp$Tex <- as.numeric(vrp$PAYMENT.DATE-vrp$COB)/365
  vrp$DF  <- exp(-vrp$IR*vrp$Tex)
  vrp <- subset(vrp, Tex>=0)          # get only deals in the future

  vrp$RMG.unit <- vrp$VAL.PRICE*vrp$DF
  vrp$RMG.MTM  <- vrp$NOTIONAL*vrp$RMG.unit
  ind <- which(vrp$SIDE == 2)
  if (length(ind)>0){vrp$RMG.MTM[ind] <- -vrp$RMG.MTM[ind]}
  
  #---- add the ETI
  vrp <- merge(deals[,c("DEAL.ID", "DEAL.REFERENCE.CD")], vrp, all.y=T)
  head(vrp)
  cat("   Done.\n"); flush.console()
  return(vrp)
}
  
##   #-----------------------------------Get the BUY.SELL flag-----
##   aux <- as.data.frame(table(dd$BUY.SELL.FLAG, dd$DEAL.ID))
##   aux <- aux[-which(aux$Freq==0),]  # the deal is either a buy or a sell
##   colnames(aux)[1:2] <- c("BUY.SELL.FLAG","DEAL.ID")
##   aux <- merge(vrp[,c("DEAL.ID","SIDE")], aux)   # this is slow...
##   aux$sign <- ifelse(aux$BUY.SELL.FLAG=="B",1,-1)
##   ind <- which(aux$SIDE==2)
##   if (length(ind)>0){aux$sign[ind] <- -aux$sign[ind]}
##   vrp$sign <- aux$sign

##   vrp$RMG.MTM <- vrp$sign*vrp$VAL.PRICE*vrp$NOTIONAL*vrp$DF

##   #-----------------------get index.mplier and index.adder in vrp----
##   aux <- data.frame(DEAL.ID=dd$DEAL.ID, LEG.ID=dd$LEG.ID)
##   dd.ex <- cbind(aux, SIDE=1, INDEX.MPLIER=dd$INDEX1.MPLIER,
##                  INDEX.ADDER=dd$INDEX1.ADDER)
##   dd.ex <- rbind(dd.ex, cbind(aux, SIDE=2, INDEX.MPLIER=dd$INDEX2.MPLIER,
##                               INDEX.ADDER=dd$INDEX1.ADDER))
##   vrp <- merge(vrp, dd.ex)                    # this is slow... 

##   vrp$quantity <- vrp$NOTIONAL * vrp$sign 
##   cols <- c("DEAL.ID","LEG.ID", "SIDE","CONTRACT.DATE", "DF", 
##             "quantity", "PRICE", "VAL.PRICE", "INDEX.MPLIER", "INDEX.ADDER")
##   base.deals <- vrp[,which(colnames(vrp) %in% cols)]

##   return(list(res,vrp))
## }

##   no.packets <- nrow(base.deals)/options$packet.size + 1
##   res <- NULL
##   for (p in 1:no.packets){
##     ind <- (options$packet.size*(p-1)+1) : min(options$packet.size*p, nrow(base.deals))
##     sdeals <- make.shifted.deals(base.deals[ind,], scenario)
##     sdeals$VAL.PRICE <- sdeals$PRICE*sdeals$INDEX.MPLIER + sdeals$INDEX.ADDER
##     sdeals$MTM <- sdeals$quantity*sdeals$VAL.PRICE*sdeals$DF
##     aux <- aggregate(sdeals$MTM, by=list(
##                CONTRACT.DATE = as.character(sdeals$CONTRACT.DATE),
##                SHIFT         = sdeals$SHIFT), sum)
##     res <- rbind(res, aux)
##   }
  
##   res <- aggregate(res$MTM, by=list(CONTRACT.DATE = res$CONTRACT.DATE,
##                SHIFT = res$SHIFT), sum)
##   colnames(res)[3] <- "MTM"
##   res <- cbind(INSTRUMENT.TYPE="PHYFWD", res)
