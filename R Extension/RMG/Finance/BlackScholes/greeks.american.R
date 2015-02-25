# Price american options on futures using the Cox-Ross-Rubinstein binomial
# model. Do one at a time for now, using finite differences.  It is a slow
# function. 
#
# F     <- 10
# sigma <- 0.80
# K     <- 10
# Tex   <- 0.1
# type  <- "PUT"
# source("H:/user/R/RMG/Finance/BlackScholes/price.american.R")
# source("H:/user/R/RMG/Finance/BlackScholes/greeks.american.R")
# greeks.american(F, sigma, Tex, K, 0.05, type)
#
# Written by Adrian Dragulescu on 8-Feb-2006

finite.diff1 <- function(dataRR, dataLL, h){
  valueRR <- price.american(dataRR$F, dataRR$sigma, dataRR$Tex, dataRR$K,
                            dataRR$r, dataRR$type)
  valueLL <- price.american(dataLL$F, dataLL$sigma, dataLL$Tex, dataLL$K,
                            dataLL$r, dataLL$type)
  return((valueRR-valueLL)/(2*h))
}
finite.diff2 <- function(dataRR, data, dataLL, h){
  valueRR <- price.american(dataRR$F, dataRR$sigma, dataRR$Tex, dataRR$K,
                            dataRR$r, dataRR$type)
  valueLL <- price.american(dataLL$F, dataLL$sigma, dataLL$Tex, dataLL$K,
                            dataLL$r, dataLL$type)
  value   <- price.american(data$F, data$sigma, data$Tex, data$K,
                            data$r, data$type)  
  return((valueRR - 2*value + valueLL)/(h^2))
}

greeks.american <- function(F, sigma, Tex, K, r, type, options){
  if (nargs()==6){options <- NULL}
  if (length(options$calculate)==0){options$calculate <- c("DELTA", "GAMMA",
     "VEGA")}
  if (length(options$pert)==0){options$pert <- 0.001}

  data <- data.frame(F, sigma, Tex, K, r, type)
  res <- NULL
  if ("DELTA" %in% options$calculate){
    dataRR <- dataLL <- data
    h <- data$F*options$pert
    dataRR$F  <- data$F + h
    dataLL$F  <- data$F - h
    res$delta <- finite.diff1(dataRR, dataLL, h)
  }
  if ("VEGA" %in% options$calculate){
    dataRR <- dataLL <- data
    h <- data$sigma*options$pert
    dataRR$sigma <- data$sigma + h
    dataLL$sigma <- data$sigma - h
    res$vega <- finite.diff1(dataRR, dataLL, h)
  }
   if ("GAMMA" %in% options$calculate){
    dataRR  <- dataLL <- data
    h <- data$F*options$pert
    dataRR$F <- data$F + h
    dataLL$F <- data$F - h
    res$gamma <- finite.diff2(dataRR, data, dataLL, h)
  }
  return(res)
}
