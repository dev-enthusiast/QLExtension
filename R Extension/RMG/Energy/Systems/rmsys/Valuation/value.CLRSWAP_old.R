#
#
#
#

value.CLRSWAP <- function(vrp, dd, deals){

  dd  <- subset(dd, INSTRUMENT.TYPE.ID==22)
  deal.ids <- unique(dd$DEAL.ID)
  vrp   <- subset(vrp, DEAL.ID %in% deal.ids)
  deals <- subset(deals, DEAL.ID %in% deal.ids)
  
  cols <- c("COB","VAL.ID","DEAL.ID","LEG.ID", "SIDE", "CONTRACT.DATE",
    "PRICE", "VAL.PRICE", "FXRATE", "IR", "NOTIONAL", "MTM", "EXP.DATE")
  vrp <- vrp[,colnames(vrp)%in%cols]
  vrp[1:3,]

  vrp$Tex <- as.numeric(vrp$EXP.DATE-vrp$COB)/365
  vrp$DF  <- exp(-vrp$IR*vrp$Tex)

  #-----------------------------------Get the BUY.SELL flag-----
  aux <- as.data.frame(table(dd$BUY.SELL.FLAG, dd$DEAL.ID))
  aux <- aux[-which(aux$Freq==0),]  # the deal is either a buy or a sell
  colnames(aux)[1:2] <- c("BUY.SELL.FLAG","DEAL.ID")
  aux <- merge(vrp[,c("DEAL.ID","SIDE")], aux)
  aux$sign <- ifelse(aux$BUY.SELL.FLAG=="B",1,-1)
  ind <- which(aux$SIDE==2)
  if (length(ind)>0){aux$sign[ind] <- -aux$sign[ind]}
  vrp$sign <- aux$sign

  vrp$RMG.MTM <- vrp$sign*vrp$VAL.PRICE*vrp$NOTIONAL*vrp$DF

  #-----------------------get index.mplier and index.adder in vrp----
  aux <- data.frame(DEAL.ID=dd$DEAL.ID, LEG.ID=dd$LEG.ID)
  dd.ex <- cbind(aux, SIDE=1, INDEX.MPLIER=dd$INDEX1.MPLIER,
                 INDEX.ADDER=dd$INDEX1.ADDER)
  dd.ex <- rbind(dd.ex, cbind(aux, SIDE=2, INDEX.MPLIER=dd$INDEX2.MPLIER,
                              INDEX.ADDER=dd$INDEX1.ADDER))
  vrp <- merge(vrp, dd.ex)
  
  cols <- c("DEAL.ID","LEG.ID", "SIDE","CONTRACT.DATE", "DF", "sign",
            "NOTIONAL", "PRICE", "VAL.PRICE", "INDEX.MPLIER", "INDEX.ADDER" )
  base.deals <- vrp[,which(colnames(vrp) %in% cols)]
  sdeals <- make.shifted.deals(base.deals, scenario)
  sdeals$VAL.PRICE <- sdeals$PRICE*sdeals$INDEX.MPLIER + sdeals$INDEX.ADDER
  sdeals$MTM <- sdeals$sign*sdeals$VAL.PRICE*sdeals$NOTIONAL*sdeals$DF
  
  res <- aggregate(sdeals$MTM, by=list(CONTRACT.DATE=sdeals$CONTRACT.DATE,
                                        SHIFT=sdeals$SHIFT), sum)
  colnames(res)[3] <- "MTM"
  res <- cbind(INSTRUMENT.TYPE="CLRSWAP", res)
  return(list(res,vrp))
}

