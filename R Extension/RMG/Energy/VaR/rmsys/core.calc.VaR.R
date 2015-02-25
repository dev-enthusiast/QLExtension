# Calculate VaR for different factors given PnL and the factors
# Where:
#   mPnL - a melted data.frame of all class.Factors and PnL
#   class.Factors - a vector with the factors to calculate
#   margins - boolean. For 2 class factors it calculates margins.
#
# Written by Adrian Dragulescu on 23-Nov-2006

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

core.calc.VaR <- function(PnL, class.Factors, margins=FALSE){
  nf <- ncol(class.Factors)
  aux <- aggregate(PnL, as.list(class.Factors), sum, na.rm=T)
  OutT <- data.frame(as.matrix(aux[,1:nf]),
                value = apply(aux[,-(1:nf)], 1, function(x){4*sd(x)}))
  colnames(OutT)[1:nf] <- colnames(class.Factors)
  return(OutT)
}

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
