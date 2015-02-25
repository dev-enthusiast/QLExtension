# Given a set of positions, return the payoff by region for a given month.
# Where qdata is a position data.frame
#       pdata is a market data.frame with prices and vols
#       options$focus.month <- as.Date("2006-03-01")
#
# Written by Adrian Dragulescu on 11-Sep-2005


payoff.Ex <- function(S,K,S0,type){ # Payoff at expiration
  res <- array(NA, dim=length(S))
  ind <- which(type=="CALL")
  if (length(ind)>0){res[ind] <- pmax(S[ind]-K[ind],0)}
  ind <- which(type=="PUT")
  if (length(ind)>0){res[ind] <- pmax(K[ind]-S[ind],0)}
  ind <- which(type=="FORWARD")                   
  if (length(ind)>0){res[ind] <- S[ind]-S0[ind]}
  return(res)
}
payoff.BS <- function(S,K,S0,Tex,type,options){ # Payoff at expiration
  res <- array(NA, dim=length(S))
  ind <- which(type=="CALL")
  if (length(ind)>0){
    res[ind] <- blackscholes(S[ind],0.50,Tex[ind],K[ind],0)$call}
  ind <- which(type=="PUT")
  if (length(ind)>0){
    res[ind] <- blackscholes(S[ind],0.50,Tex[ind],K[ind],0)$put}
  ind <- which(type=="FORWARD")                   
  if (length(ind)>0){res[ind] <- S[ind]-S0[ind]} 
  return(res)
}
#--------------------------------------------------------------
payoff.by.month <- function(qdata, pdata, options){

  if (length(options$no.points)==0){options$no.points <- 101}
  qdata <- subset(qdata, MONTHLY_DATE == options$focus.month)
  fwd   <- pdata[as.character(options$focus.month),]
  if (length(fwd)==1){names(fwd) <- options$region.name}
  Tex <- as.numeric(options$focus.month-options$asOfDate)/365.25

  res <- matrix(NA, nrow=options$no.points, ncol=length(options$region.name),
         dimnames=list(1:options$no.points, options$region.name))
  res.Ex <- res
  for (region in options$region.name){
    S <- seq(as.numeric(fwd[region])-2.5, as.numeric(fwd[region])+2.5,
             by=options$increment)  
    data <- subset(qdata, REGION_NAME == region)
    no.instruments <- dim(data)[1]
    if (no.instruments>0){   # if there are missing regions in qdata
      aux <- data.frame(S    = rep(S, each=no.instruments), 
                        K    = rep(data[,"STRIKE"], options$no.points),
                        S0   = as.numeric(fwd[region]),
                        Tex  = Tex,
                        type = rep(data[,"OPTION_TYPE"], options$no.points))
      uPay <- payoff.Ex(aux$S, aux$K, aux$S0, aux$type) # calculate unit payoff
      uPay <- matrix(uPay, ncol=options$no.points, nrow=no.instruments)
      pPay.Ex <- as.numeric(data[,"NOTIONAL"] %*% uPay) # portfolio payoff

      uPay <- payoff.BS(aux$S, aux$K, aux$S0, aux$Tex, aux$type) 
      uPay <- matrix(uPay, ncol=options$no.points, nrow=no.instruments)
      pPay <- as.numeric(data[,"NOTIONAL"] %*% uPay) # portfolio payoff 

      value.now <- approx(S, pPay.Ex, fwd[region])$y
      pPay.Ex   <- (pPay.Ex - value.now)*10000  # from Lots to MMBTU's
      pPay      <- (pPay - value.now)*10000
    } else { pPay <- NA; pPay.Ex <- NA}
    res[,region]    <- pPay
    res.Ex[,region] <- pPay.Ex
  }
  return(list(res,res.Ex))
}
