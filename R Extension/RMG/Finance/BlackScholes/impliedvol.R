# Calculates the implied volatility for EU call and put options
# 
# din (=data in) is a data frame
#    din$type  ="CALL" or "PUT", character
#    din$price - price of option, numeric
#    din$S     - current value of underlier
#    din$Tex    - expiration time in years, numeric
#    din$K     - strike price, numeric
#    din$r     - intrest rate, numeric
#
# Last modified by Adrian Dragulescu on 25-Nov-2005


source("H:/user/R/RMG/Finance/BlackScholes/blackscholes.R")

impliedvol <- function(din,options){
  if (nargs()==1){ options <- list()}
  objectiveFun <- function(sigma){
    priceBS <- blackscholes(S,sigma,Tex,K,r,type)$value
    return(priceBS-priceMKT) # calculate the deviation
  }
  
  nOptions <- nrow(din)
  IVol <- value.intrinsic <- array(NA, dim=c(nOptions))

  #-------------------------- check that value is above intrinsic value---
  aux <- din; aux$Tex <- 0; aux$sigma <- 0
  value.intrinsic <- blackscholes(aux)$value
  for (p in 1:nOptions){
    type <<- din$type[p];    priceMKT <<- din$price[p]
    S    <<- din$S[p];            Tex <<- din$Tex[p]
    K    <<- din$K[p];              r <<- din$r[p]
    if (priceMKT <= value.intrinsic[p]){next}
    IVol[p] <- uniroot(objectiveFun, interval=c(0,8))$root
  }
  return(cbind(din, IVol=IVol))  
}


