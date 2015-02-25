# A shell function for spread.option.R for use with xls  
# Payoff max(F1-HR*F2-K,0)
# source("H:/user/R/RMG/Finance/BlackScholes/xls.spread.option.R")
# res <- xls.spread.option(F1, F2, V1, V2, Rho, K, Tx, DF, HR, what)
#
# Written by Adrian Dragulescu on 4-Nov-2004

xls.spread.option <- function(F1, F2, V1, V2, Rho, K, Tx, DF,
       HR, what, method){

  source("H:/user/R/RMG/Finance/BlackScholes/spread.option.R")
  if (nargs()==9){what <- "value"}
  if (nargs()==10){method <- "kirk"}
  options=res=NULL
  options$method <- method
  data <- data.frame(F1=as.numeric(F1), F2=as.numeric(F2),
       V1=as.numeric(V1), V2=as.numeric(V2), Rho=as.numeric(Rho),
       K =as.numeric(K),  Tx=as.numeric(Tx), DF=as.numeric(DF),
       HR=as.numeric(HR))
  if (tolower(what)=="value"){
    res$value <- spread.option(data, options)$value
  }
  if (tolower(what)=="delta1"){       #----------------------- DELTAS
    options$deltas <- 1
    res$value <- spread.option(data, options)$delta1
  }
  if (tolower(what)=="delta2"){
    options$deltas <- 1
    res$value <- spread.option(data, options)$delta2
  }
  if (tolower(what)=="vega1"){       #----------------------- VEGAS
    options$vegas <- 1
    res$value <- spread.option(data, options)$vega1
  }
  if (tolower(what)=="vega2"){
    options$vegas <- 1
    res$value <- spread.option(data, options)$vega2
  }  
  if (tolower(what)=="deltarho"){    #----------------------- RHO
    options$delta.rho <- 1
    res$value <- spread.option(data, options)$delta.Rho
  }
 return(res)
}

  


