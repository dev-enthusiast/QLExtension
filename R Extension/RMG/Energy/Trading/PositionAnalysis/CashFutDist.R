CashFutDist <- function(CashDiff, FutDiff, Value0, options, Type) {
  browser()
  ArrayDiffComp <- array(0, dim = (options$nDays + 1))
  ReturnComp    <- array(0, dim = options$nSim)
  Return       <- array(0, dim = options$nSim)
  ArrayDiffComp <- array(0, dim = (options$nDays + 1))
  ReturnComp    <- array(0, dim = options$nSim)
  ReturnFutComp    <- array(0, dim = options$nSim)
  lengthCashDiff   <- length(CashDiff)
  lengthFutDiff    <- length(FutDiff)

  for (i in 1:options$nSim) {
    indCash <- sample(lengthCashDiff, (options$nDays + 1))
    for (j in 1:(options$nDays+1))
      ArrayDiffComp[j] <- sum(CashDiff[indCash[1:j]])
    ReturnComp[i] <- sum(exp(ArrayDiffComp)-1)/options$nDays
    Return[i] <- exp(CashDiff[indCash[1]])-1
    ReturnFutComp[i] <- (1+ReturnComp[i])*(exp(sum(FutDiff[sample(lengthFutDiff, options$nDaysFut)])))-1
  }
  if(options$flagValue == TRUE) {
    Return <- Value0*Return
    ReturnComp <- Value0*ReturnComp
    ReturnFutComp <- Value0*ReturnFutComp
    xlabel <- "Delta(Value) ($M)"
    nFactor <- 1
    tp <- "($M)"
  } else {
    Return <- sign(Value0)*Return
    ReturnComp <- sign(Value0)*ReturnComp
    ReturnFutComp <- sign(Value0)*ReturnFutComp
    xlabel <- "Delta(Value) (%)"
    nFactor <- 100
    tp <- "(%)"
  }
  printDist(ReturnComp, xlabel, nFactor, paste(Type, "CashCompAv_", sep = ""), tp, options,
            paste(Type, " Cash Compound Value as of ", sep = ""))
  printDist(Return, xlabel, nFactor, paste(Type, "CashAv_", sep = ""), tp, options,
            paste(Type, " Cash Value as of ", sep = ""))
  printDist(ReturnFutComp, xlabel, nFactor, paste(Type, "FutCashCompAv_", sep = ""), tp, options,
            paste(Type, " Fut Cash Compound Value as of ", sep = ""))
}
printDist <- function(Return, xlabel, nFactor, fnameDiff, tp, options, Type) {
  fName <- paste(options$dir, "/", fnameDiff, format(options$MOI,"%b_%Y"), ".pdf", sep = "")
  pdf(fName, width=11.0, heigh=8.0)
  qntl <- quantile(Return, probs = c(0.01*options$percentile, 0.5, 1-0.01*options$percentile))
  plot(nFactor*sort(Return),(c(1:length(Return))-0.5)/options$nSim, type = "l", lwd = 2, xlab = xlabel, ylab = "CDF",
       main = paste(Type,  format(options$asOfDate,"%b/%d/%Y"), ", Positions for ", format(options$MOI,"%b/%Y"),
       sep = ""))
  points(nFactor*qntl, c(0.01*options$percentile, 0.5, 1-0.01*options$percentile), lwd = 4, col = "red")
  grid()
  text(nFactor*min(Return),1, paste("Q(", as.character(100-options$percentile) ,
       "%)=", format(nFactor*qntl[[3]], digits = 3), tp, sep = ""), adj = c(0,1), cex = 1.5)
  text(nFactor*min(Return),0.9, paste("Q(", as.character(50) ,
       "%)=", format(nFactor*qntl[[2]], digits = 3), tp, sep = ""), adj = c(0,1), cex = 1.5)
  text(nFactor*min(Return),0.8, paste("Q(", as.character(options$percentile) ,
       "%)=", format(nFactor*qntl[[1]], digits = 3), tp, sep = ""), adj = c(0,1), cex = 1.5)
  grid()
  dev.off()
}