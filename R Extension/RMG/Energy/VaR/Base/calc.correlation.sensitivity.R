# Calculate sensitivity to correlations. 
# Experimental. 
#
# Written by Adrian Dragulescu on 30-Apr-2007

perturb.VaR <- function(h, cormat, std, VaR, labels){
  new.VaR <- array(NA, c(nrow(cormat),nrow(cormat)))
  for (i in 1:(nrow(cormat)-1)){
    for (j in (i+1):nrow(cormat)){
      new.cor <- cormat
      new.cor[i,j] <- new.cor[j,i] <- max(min(cormat[i,j] + h, 1), -1)
      new.VaR[i,j] <- 4*sqrt(t(std) %*% new.cor %*% std)
    }
  }
  change.VaR <- new.VaR - VaR$Total
  change.VaR <- data.frame(label=as.character(labels),
    perturbation=as.character(h), change=as.numeric(change.VaR))
  return(na.omit(change.VaR))
}

calc.correlation.sensitivity <- function(IVaR, options){
  cat("Calculating sensitivities to correlations ... ")
  if (length(IVaR)==0){                 # if looping over the days ... 
    return()   # not implemented yet
  }
  load(paste(options$save$reports.dir.run, "all.PnL.RData", sep=""))
  load(paste(options$save$reports.dir.run, "all.VaR.RData", sep=""))
  
  CF     <- strsplit(names(all.PnL), "\\|")       # class factors
  n      <- which(CF %in% c("market"))            # do it just for "market" now!!
  cf     <- CF[[n]]
  no.cf  <- length(CF[[n]])                       # number of class factors
  aux    <- all.PnL[[n]]
  aux    <- t(aux[, -which(names(aux) %in% cf)])
  cormat <- cor(aux)
  std    <- sd(aux)
  labels <- outer(all.PnL[[n]][,cf], all.PnL[[n]][,cf], FUN="paste", sep=" vs. ")
  if (length(labels)==1){
    cat("Cannot calculate correlation, only one market. \n")
    return()
  }
  h <- 0.01                                       # the perturbation in correlations
  cormat[is.na(cormat)] <- 0                      # just in case ...
  change.VaR <- perturb.VaR(h, cormat, std, VaR, labels)
  
  change.VaR <- change.VaR[order(change.VaR$change, decreasing=TRUE),]
  change.VaR <- change.VaR[1:min(nrow(change.VaR), 20), ]  # show top 20 only.
  caption <- paste("VaR sensitivity with respect to correlations.  ",
    "To aggregate the effect of the correlations by market, we calculate the ",
    "correlation between the daily PnL time-series by market.", sep="")
  file <-  paste(options$save$reports.dir.run, "corr_sensitivity_by_",
    paste(cf, sep="", collapse="_"), sep="")
  add.to.row <- list(); add.to.row$pos <- list(0); add.to.row$command <- c("\\endhead \n")
  show.totals <- FALSE
  write.2D.table(change.VaR, cf, VaR, caption, file, show.totals, add.to.row)
  cat("Done!\n")
}
