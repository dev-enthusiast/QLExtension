# Price american options of futures using the CoxRossRubinstein binomial model.
# Do one at a time for now.
#
# Written by Adrian Dragulescu on 1-Sep-2006

price.american <- function(F, sigma, Tex, K, r, type, options){

  if (nargs()==6){options <- NULL}
  if (length(options$no.steps)==0){options$no.steps <- 300}
  if (length(options$underlier)==0){options$underlier <- "futures"}
  if (!is.element(type, c("CALL","PUT"))){
    cat("Unknown option type!"); return(NULL)}
  if (Tex == 0){
    ifelse (type=="CALL", V <- pmax(F - K, 0), V <- pmax(K-F, 0))
    return(V)
  }
  dt  <- Tex/options$no.steps
  DF  <- exp(-r*dt)
  u   <- exp(sigma*sqrt(dt))
  d   <- 1/u
  dd  <- d*d
  pUP <- switch(options$underlier, futures  = (1-d)/(u-d),
                                   equity   = (1/DF-d)/(u-d))
  pDW <- 1-pUP
  top <- F*u^options$no.steps
  FF  <- top*dd^(0:(options$no.steps))  # futures values at expiration
  # terminal value :
  ifelse (type=="CALL", V <- pmax(FF - K, 0), V <- pmax(K-FF, 0))
  
  for (s in seq(options$no.steps, 1, by=-1)){   # loop backwards
    FF <- u*FF[-1]                              # prices at this step
    EV <- DF*(pUP*V[-length(V)] + pDW*V[-1]) # expected value
    ifelse (type=="CALL", V <- pmax(FF - K, EV), V <- pmax(K-FF, EV))
  }
  return(V)
}
