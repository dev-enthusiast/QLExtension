# Calculate the BS greeks.  Same inputs as the blackscholes.R function. 
# "DSTRIKE"  calculates the first derivative with respect to strike.
# "DSTRIKE2" calculates the second derivative with respect to strike.
#
# options$calculate <- c("DELTA", "VEGA")
# type <- "CALL" #or "PUT"
# Return a data frame with the columns that you want. 
#
# Written by Adrian Dragulescu on 8-May-2006

greeksEU <- function(S, sigma, Tex, K, r, type, options)
{
  if (nargs()==6){options <- list()}
  if (length(options$calculate)==0){options$calculate <- c("DELTA", "GAMMA",
     "THETA", "VEGA", "RHO")}

  OutT <- data.frame(S, sigma, Tex, K, r, type)
  d1   <- (log(S/K) + (r+0.5*sigma^2)*Tex)/(sigma*sqrt(Tex))
  d2   <- (log(S/K) + (r-0.5*sigma^2)*Tex)/(sigma*sqrt(Tex))
  flag <- rep(1, nrow(OutT))
  flag[type=="PUT"] <- -1
  Nd1  <- pnorm(flag*d1, mean=0, sd=1)   
  Nd2  <- pnorm(flag*d2, mean=0, sd=1)
  dNd1 <- exp(-d1^2/2)/sqrt(2*pi) 
  
  if ("DELTA" %in% options$calculate){OutT$DELTA <- flag*Nd1}
  if ("GAMMA" %in% options$calculate){OutT$GAMMA <- dNd1/(sigma*S*sqrt(Tex))}
  if ("THETA" %in% options$calculate){
    OutT$THETA <- -sigma*S*dNd1/(2*sqrt(Tex))-flag*r*K*exp(-r*Tex)*Nd2
  }
  if ("VEGA" %in% options$calculate){OutT$VEGA <- S*sqrt(Tex)*dNd1} 
  if ("RHO"  %in% options$calculate){OutT$RHO  <- K*Tex*exp(-r*Tex)*Nd2}
  if ("DSTRIKE" %in% options$calculate){OutT$DSTRIKE <- -flag*exp(-r*Tex)*Nd2}
  if ("DSTRIKE2" %in% options$calculate){
    dNd2 <- exp(-d2^2/2)/sqrt(2*pi)
    OutT$DSTRIKE2 <- exp(-r*Tex)*dNd2/(sigma*K*sqrt(Tex))
  }  
  return(OutT)
}
