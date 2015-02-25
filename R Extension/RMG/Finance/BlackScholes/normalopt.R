#****************************************************************
##	Adlib Name:	 	normalopt
##	Usage:			normalopt(cp,f,k,v,term,rd)
##	Parameters: 	cp   - call or put
##				        f    - forward price
##	              k    - strike price
##                v    - annualized s.d. of spot price
##	              term - time to expiration
##        	      rd   - domestic simple interest rates
##	Summary:	 Option on a normal random variable
##	References:		European Options
##	Description:	Premium for option a normal random variable
##					      Interest rates are simple rates, actual/365 basis.
##                Time to expiration is in number of years.
##                Standard dev and interest rates are
##					             entered as absolute numbers, not percentages,
##	                     i.e. 0.12, not 12.
##                good for vector operations
##	Example:	normalopt("Q_OPT_PUT",1.45,1.40,1.2,1.0,0.09) = 0.415058416
##  Example2:   callPut = c("Q_OPT_PUT", "Q_OPT_PUT","Q_OPT_CALL")
##              forward = c(1.45, 2.0, 3.0)
##              strike  = c(1.40, 2.0, 2.7)
##              vol     = c(1.2, 0.2,0)
##              exptime = c(1.0, 2.0, 4.0)
##              ccRd = 0.09
##            normalopt(callPut, forward, strike, vol, exptime, ccRd) =  0.41505841 0.09425015 0.20930290
#***************************************************************/

normalopt <- function(callPut, forward, strike, vol, exptime, ccRd)
{  
  if (any( vol < 0.0 ) | any(is.na(vol))) stop("normalopt: negative vol or NA entered");
  if (any( exptime < 0.0 ) | any(is.na(exptime)))
    stop("normalopt:negative time to expriration or NA entered");

  signs = as.integer(callPut == "Q_OPT_CALL") - as.integer(callPut == "Q_OPT_PUT")
  intrinsic = signs * (forward-strike)
  discount = exp(-ccRd * exptime)
  volSqrtT = vol * sqrt(exptime)
  index = (volSqrtT == 0)

  #handle cases in which time or vol == 0
  arg = ifelse(volSqrtT == 0, intrinsic*(10^8),  intrinsic/volSqrtT)      
  densityfunc = exp(-0.5*arg*arg)/sqrt(2*pi)
  norm = pnorm(arg,0,1)
  price = discount * (intrinsic*norm + volSqrtT*densityfunc)
  return(price)
}
