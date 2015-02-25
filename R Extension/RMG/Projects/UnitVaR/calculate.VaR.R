#
#
#
# Calcuate VaR for Last 70 Business days
# Input must have history longer than 70 business days

calculate.VaR <- function(FutCurve)
{

  VaR <- matrix(NA, nrow=length(FutCurve$contract.dts), ncol=length(as.POSIXct(FutCurve$AsOfDate)),
         dimnames=list(names(FutCurve$contract.dts), as.character(FutCurve$AsOfDate)))
  for (d in 1:length(as.POSIXct(FutCurve$AsOfDate)))
  {
    ind <- max(which(FutCurve$pricing.dts<=FutCurve$AsOfDate[d]))
    for (c in 1:length(FutCurve$contract.dts))
    {
      aux <- data.frame(prices=FutCurve$FutCurves[(ind-70):ind, FutCurve$contract.dts[[c]]])
      pc  <- diff(apply(aux, 1, mean))
      VaR[c, d] <- 4*sd(pc)
    }
  }
  return(VaR)  
}
