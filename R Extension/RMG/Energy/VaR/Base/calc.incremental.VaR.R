# Calculate Incremental VaR.  
# Requires the PnL to be claculated for all days.  If options$parallel$is.necessary
# (you looped over the days) it will trigger another loop.  Sorry, no other way.  
#
# Written by Adrian Dragulescu on 16-May-2007

main.incremental.VaR <- function(cf, IVaR, class.Factors){  # for two class factors only
  vars <- data.frame(unique(class.Factors[, cf]), incremental.VaR=NA)
  rownames(IVaR$Changes) = NULL
  rownames(class.Factors) = NULL
  rownames(vars) = NULL
  for (r in 1:nrow(vars)){
    ind <- as.numeric(rownames(eval(parse(text=paste("subset(class.Factors[,cf], ", cf[1], 
      "==as.character(vars[r,cf[1]]) & ", cf[2], "==as.character(vars[r, cf[2]]))", sep="")))))

    ivar <- IVaR$Changes[-ind, ]
    ind.Dates <- grep("-", names(ivar))
    vars[r, "incremental.VaR"] <- 4*sd(as.numeric(t(ivar$position) %*% as.matrix(ivar[, ind.Dates])))
  }
  return(vars)
}

calc.incremental.VaR <- function(PnL, IVaR, run, class.Factors, options){

  cat("Calculating Incremental VaR ... ")
  if (length(IVaR)==0){                 # if looping over the days ... 
    return()   # not implemented yet
  }
  load(paste(options$save$reports.dir.run, "all.PnL.RData", sep=""))
  load(paste(options$save$reports.dir.run, "all.VaR.RData", sep=""))

  CF <- strsplit(names(all.PnL), "\\|")                   # class factors
  ind.2C <- which(sapply(CF, function(x)(length(x)==2)))  # two classification

  #----- make incremental VaR tables (for 2 classifications only) ------------
  for (n in (1:length(all.PnL))[ind.2C]){
    cf  <- CF[[n]]
    no.cf <- length(CF[[n]])
    res <- main.incremental.VaR(cf, IVaR, class.Factors)
    res$value <- res$incremental.VaR - VaR$Total
    res$incremental.VaR <- NULL
##     txt   <- paste("cast(res, ", paste(cf[-no.cf], sep=" ", collapse=" + "),
##       " ~ ", cf[no.cf], ", sum)", sep="")     # cast formula
##     txt <- paste("tapply(res$value, )")
##     OutT <- eval(parse(text=txt))
##     OutT[,1] <- as.character(OutT[,1])    # stay away from reshape....
    
    OutT <- data.frame(tapply(res$value, list(res[,1], res[,2]), sum))
    OutT <- eval(parse(text=paste("cbind(", cf[1], "=rownames(OutT), OutT)", sep="")))
    
    caption <- paste("Incremental VaR by ", paste(cf, sep="", collapse=" and "), ".",
      "  If you remove the position defined by the rows and the columns of the table, ",
      "total VaR of the portfolio will change by the amount in the table field.", sep="")
    file <-  paste(options$save$reports.dir.run, "ivar_by_", paste(cf, sep="", 
       collapse="_"), sep="")
    add.to.row <- list(); add.to.row$pos <- list(0); add.to.row$command <- c("\\endhead \n")
    show.totals <- FALSE
    write.2D.table(OutT, cf, VaR, caption, file, show.totals, add.to.row)
  }  
  cat("Done!\n")
}
