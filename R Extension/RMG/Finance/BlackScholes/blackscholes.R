# Calculate the BlackScholes value of a CALL/PUT Option
#
# C(S,t) = S*N(d1) - K*e^{-r*(T-t)}N(d2)
# P(S,t) = K*e^{-r*(T-t)}N(-d2) - S*N(-d1)
#    where: - S is the current asset price
#	    - sigma is the asset volatility per year
#           - r is the interest rate per year
#           - Tex is the time to expiry in years
#           - K is the excercise price of the option (strike price)
#           - type = "CALL" or "PUT", as a factor
#          blacksholes(S,sigma,Tex,K,r)
# Example: blacksholes(100,0.1,1,120,0.08)
#
# written by Adrian Dragulescu on 02/10/2001
# modified by A.D. on 07/26/2002
# ported to R from Matlab on 1-May-2003
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

blackscholes <- function(S,sigma,Tex,K,r,type){
  if (nargs()==1){     # you can pass a data.frame as input
    sigma <- S$sigma; Tex <- S$Tex; K <- S$K; r <- S$r
    type  <- S$type;  S <- S$S
  }
  if (!all(levels(type)%in%c("CALL", "PUT"))){
    stop("Unknown option type.  Not CALL or PUT.")
  }
  if (any(Tex<0)){stop("Some options have already expired.  Remove them.")}
  if (any(sigma<0)){stop("Some volatilities are < 0. Remove them.")}
  res <- list()
  ifelse(Tex==0 | sigma==0, {d1 <- Inf; d2 <- Inf}, 
         {d1 <- (log(S/K) + (r+0.5*sigma^2)*Tex)/(sigma*sqrt(Tex)); 
          d2 <- d1 - sigma*sqrt(Tex)}
  )  
  Nd1  <- pnorm(d1, mean=0, sd=1); cNd1 <- 1-Nd1;
  Nd2  <- pnorm(d2, mean=0, sd=1); cNd2 <- 1-Nd2; 

  res$value <- array(NA, dim=length(type))
  ind <- which(type=="CALL")
  if (length(ind)>0){                           # this are the CALLS
    ifelse(Tex[ind]==0, 
      res$value[ind] <- pmax(S[ind]-K[ind],0),  # intrinsic value only
      res$value[ind] <- S[ind]*Nd1[ind]-K[ind]*exp(-r[ind]*Tex[ind])*Nd2[ind]
    )
  }
  if (length(ind)>0){ind <- (1:length(S))[-ind]} else {ind <- (1:length(S))}
  if (length(ind)>0){                          # this are the PUTS
    ifelse(Tex[ind]==0,
      res$value[ind] <- pmax(K[ind]-S[ind],0), # intrinsic value only
      res$value[ind] <- K[ind]*exp(-r[ind]*Tex[ind])*cNd2[ind]-S[ind]*cNd1[ind]
    )
  }
  return(res)
}

