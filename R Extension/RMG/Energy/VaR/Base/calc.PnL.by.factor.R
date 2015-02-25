# Calculate PnL by the classification factors you are interested in.
# Called by main.run.VaR.R from inside the loop over the packets. 
# PnL is a list of lenght equal to the number of classifications.  Each
# element of the list gets appended the pnl from the packet. 
#
# Last modified by Adrian Dragulescu on 3-Apr-2007

calc.PnL.by.factor <- function(IVaR, run, class.Factors, options){

  indDates <- grep("-", names(IVaR$Changes))
  all.PnL  <- IVaR$Changes$position * IVaR$Changes[,indDates] # lowest detail

  CF <- strsplit(run$classification, "\\|")   # class factors
  ind.1C <- which(sapply(CF, function(x)(length(x)==1)))  # one classification
  uCF <- unique(unlist(CF))
  ind <- uCF %in% names(class.Factors)
  if (any(!ind)){
    cat("Could not find", uCF[ind], "in class.Factors.R\n")
  }
 
  pnl <- NULL
  #--------------- PnL by one classification factor -----------------------
  for (i in 1:ncol(class.Factors)){
    cFactor <- data.frame(class.Factors[,i])
    names(cFactor) <- names(class.Factors)[i]
    pnl[[colnames(cFactor)]] <- core.calc.VaR(all.PnL, cFactor, FALSE)
  }
  #--------------- PnL by two or more classification factors --------------
  for (i in (1:length(run$classification))[-ind.1C]){
    cf  <- CF[[i]]
    res <- core.calc.VaR(all.PnL, class.Factors[,cf], FALSE)
    label <- paste(cf, sep="", collapse="|")
    pnl[[label]] <- res
  }  
  return(pnl)
}

