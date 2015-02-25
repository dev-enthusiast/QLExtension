# Calculate Marginal and Component VaR.  
# Requires the PnL to be claculated for all days.  If options$parallel$is.necessary
# (you looped over the days) it will trigger another loop.  Sorry, no other way.  
#
# Written by Adrian Dragulescu on 30-Apr-2007

calc.CVaR <- function(PnL, IVaR, run, class.Factors, options){

  cat("Calculating CVaR ... ")
  if (length(IVaR)==0){                 # if looping over the days ... 
    return()   # not implemented yet
  }
  load(paste(options$save$reports.dir.run, "all.PnL.RData", sep=""))
  load(paste(options$save$reports.dir.run, "all.VaR.RData", sep=""))
  dV <- colSums(all.PnL[[1]][,-1])    # first class is single

  indDates <- grep("-", names(IVaR$Changes))
  mVaR <- function(x){cov(x, dV)}
  marginal.VaR  <- 4*apply(IVaR$Changes[,indDates], 1, mVaR)/sd(dV)
  component.VaR <- marginal.VaR*IVaR$Changes$position

  CF <- strsplit(names(all.PnL), "\\|")                   # class factors
  ind.2C <- which(sapply(CF, function(x)(length(x)==2)))  # one classification

  #------------- make CVaR tables (for 2 classifications or more) ------------
  for (n in (1:length(all.PnL))[ind.2C]){
    cf    <- CF[[n]]
    no.cf <- length(CF[[n]])                       # number of class factors
    res   <- cbind(class.Factors[, CF[[n]]], value=component.VaR)
##     txt   <- paste("cast(res, ", paste(cf[-no.cf], sep=" ", collapse=" + "),
##       " ~ ", cf[no.cf], ", sum)", sep="")     # cast formula 
##     OutT <- eval(parse(text=txt))    
##     OutT[,1] <- as.character(OutT[,1])

    OutT <- data.frame(tapply(res$value, list(res[,1], res[,2]), sum))
    OutT <- eval(parse(text=paste("cbind(", cf[1], "=rownames(OutT), OutT)", sep="")))
    
    caption <- paste("Component VaR by ", paste(cf, sep="", collapse=" and "), ".", sep="")
    file <-  paste(options$save$reports.dir.run, "cvar_by_", paste(cf, sep="", 
       collapse="_"), sep="")
    add.to.row <- list(); add.to.row$pos <- list(0); add.to.row$command <- c("\\endhead \n")
    show.totals <- FALSE
    write.2D.table(OutT, cf, VaR, caption, file, show.totals, add.to.row)
  }  
  cat("Done!\n")
  return(component.VaR)
}
