# Calculate VaR for different factors given PnL and the factors
# Where:
#   PnL - a data.frame with detailed PnL
#   class.Factors - a dataframe with the factors to aggregate by
#   doVaR - boolean.  If doVaR=FALSE, returns the aggregated PnL
#
# Last modified by Adrian Dragulescu on 3-Apr-2007

core.calc.VaR <- function(PnL, class.Factors, doVaR=TRUE){
  nf <- ncol(class.Factors)
  aux <- aggregate(PnL, as.list(class.Factors), sum, na.rm=T)
  if (doVaR){
    OutT <- data.frame(as.matrix(aux[,1:nf]),
       value = apply(aux[,-(1:nf)], 1, function(x){4*sd(x)}))
  } else {OutT <- aux}
  colnames(OutT)[1:nf] <- colnames(class.Factors)
  return(OutT)
}

# core.calc.VaR <- function(mPnL, factors, margins=T){
#   formula <- paste(paste(factors, collapse=" + "), "+ variable ~ .")
#   C1 <- cast(mPnL, eval(formula), sum)  # calculate PnL by these factors
#   if (length(factors)!=2){
#     formula <- paste(paste(factors, collapse=" + "), " ~ .")
#     C2 <- cast(C1, eval(formula), function(x){4*sd(x)})
#   } else {
#     formula <- paste(factors[1], "~", factors[2])
#     C2 <- cast(C1, eval(formula), function(x){4*sd(x)}, margins=margins)
#   } 
#   return(C2)
# }

#   ind <- which(margins)
#   if (length(ind)>0){   
#     for (i in ind){
#       cF <- data.frame(class.Factors[,i])
#       colnames(cF) <- colnames(class.Factors)[i]
#       mOutT <- core.calc.VaR(PnL, cF, margins=F)
#       mOutT <- data.frame(mOutT, matrix("Total",ncol=(ncol(class.Factors)-1),
#                           dimnames=list(NULL, colnames(class.Factors)[-i])))
#       OutT  <- rbind(OutT, mOutT)
#     }
#  }

#   Use melt/cast if aggregate fails...
#   MM <- melt(cbind(class.Factors, PnL))
#   fmula <- paste(paste(colnames(class.Factors),collapse=" + "), "~ variable")
#   CC <- cast(MM, eval(fmula), sum)
