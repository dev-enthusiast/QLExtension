# Main function to create the tex and csv tables for the VaR report for a
# given run.   
# 
# Written by Adrian Dragulescu on 24-Nov-2006

make.VaR.summary <- function(PnL, class.Factors, file){
  summary.Fun <- function(x){c(4*sd(x), mean(x)-quantile(x, probs=0.05), min(x))}
  OutT <- array(0, dim=c(3,3), dimnames=list(c("4std", "95\\%", "MaxLoss"),
                                 c("Delta", "Vega", "Total")))
  ind  <- which(class.Factors$type == "Delta")
  if (length(ind)>0){OutT[,1] <- apply(as.matrix(colSums(PnL[ind,])),2, summary.Fun)}
  ind  <- which(class.Factors$type == "Vega")
  if (length(ind)>0){OutT[,2] <- apply(as.matrix(colSums(PnL[ind,])),2, summary.Fun)}
  OutT[,3] <- apply(as.matrix(colSums(PnL)),2, summary.Fun) 
  OutT     <- apply(OutT,2,dollar,0,F)# prettify only the numeric variables
  print(xtable(OutT, caption="VaR summary."), file=paste(file, ".tex", sep=""),
    size="small", floating=FALSE, include.rownames=TRUE)
  write.csv(OutT, file=paste(file, ".csv", sep=""), row.names=F)
  return(OutT)
}

write.2D.table <- function(OutT,var.names,VaR,caption,file,add.totals=T,add.to.row=NULL){
  if (!is.data.frame(OutT)){OutT <- data.frame(OutT)}
  colnames(OutT) <- gsub("X","",colnames(OutT)) # if cast was used
  if (add.totals){
    OutT <- cbind(OutT, Total=VaR[[var.names[1]]]$Total$value)
    OutT <- rbind(OutT, c(NA, VaR[[var.names[2]]]$Total$value,VaR$Total))
    OutT[nrow(OutT),1] <- "Total"
  }
  ind <- which(sapply(OutT, is.numeric))
  OutT[,ind] <- apply(as.matrix(OutT[,ind]),2,dollar,0,F)# prettify only the numeric variables
  OutT[,ind] <- apply(as.matrix(OutT[,ind]), 2, function(x){gsub("NA", "  ",x)})
  size <- "small"
  if (ncol(OutT) >  15){size <- "scriptsize"} 
  if (ncol(OutT) >= 18){size <- "tiny"}
  print(xtable(OutT, caption=caption), file=paste(file, ".tex", sep=""), size=size,
    tabular.environment="longtable", floating=FALSE, include.rownames=FALSE,
    add.to.row=add.to.row)
  write.csv(OutT, file=paste(file, ".csv", sep=""), row.names=F)
}

make.VaR.tables <- function(PnL, run, options){

  all.PnL <- PnL[[1]]   # flatten out the PnL packet list ...
  p <- 2
  while (p <= length(PnL)){
    for (c in names(all.PnL)){
       ind <- grep("-", names(PnL[[p]][[c]]))
       all.PnL[[c]] <- cbind(all.PnL[[c]], PnL[[p]][[c]][,ind])}
    p <- p+1
  }
  save(all.PnL, file=paste(options$save$reports.dir.run, "all.PnL.RData", sep=""))
  
  CF <- strsplit(names(all.PnL), "\\|")                   # class factors
  ind.1C <- which(sapply(CF, function(x)(length(x)==1)))  # one classification

  #----------------------------------------- can do summary VaR only! -----
  file <- paste(options$save$reports.dir.run, "var_summary", sep="")
  aux  <- data.frame(type=all.PnL[["type"]][,1])
  summary.VaR <- make.VaR.summary(all.PnL[["type"]][,-1], aux, file)
  if (length(options$summary.only)>0 && options$summary.only){return(summary.VaR)}
  
  VaR <- NULL
  VaR$Total <- 4*sd(colSums(all.PnL[[1]][,-1]))  # first class is single
  #------------------------------- Calc VaR for one classification ----------------
  for (n in (1:length(all.PnL))[ind.1C]){
    pnl <- all.PnL[[n]]
    res <- core.calc.VaR(pnl[,-1], data.frame(pnl[,1]))
    names(res)[1] <- names(all.PnL)[n]
    VaR[[names(all.PnL)[n]]] <- list(Total=res)
  }
  #------------------------------- Calc VaR for 2 classification or more ----------
  for (n in (1:length(all.PnL))[-ind.1C]){
    pnl   <- all.PnL[[n]]
    cf    <- CF[[n]] 
    no.cf <- length(CF[[n]])                       # number of class factors > 1
    res <- core.calc.VaR(pnl[,-(1:no.cf)], pnl[,1:no.cf])
    txt <- paste("c(VaR[[cf[1]]],list(", paste(cf[-1], sep="", collapse="=list("),
      "=list(Total=res)", paste(rep(")", no.cf), collapse=""), sep="")       
    VaR[[cf[1]]] <- eval(parse(text=txt))          # build the nested list 
  }
  save(VaR, file=paste(options$save$reports.dir.run, "all.VaR.RData", sep=""))
  
  #------------- make VaR tables (for 2 classifications or more) ----------------
  for (n in (1:length(all.PnL))[-ind.1C]){
    cf   <- CF[[n]]
    no.cf <- length(CF[[n]])                       # number of class factors
    txt  <- paste("VaR$", paste(cf, sep="", collapse="$"), "$Total", sep="") 
    res  <- eval(parse(text=txt))        
    txt  <- paste("cast(res, ", paste(cf[-no.cf], sep=" ", collapse=" + "),
      " ~ ", cf[no.cf], ")", sep="")     # cast formula 
    OutT <- eval(parse(text=txt))
    OutT[,1] <- as.character(OutT[,1])
    caption <- paste("VaR by ", paste(cf, sep="", collapse=" and "), ".", sep="")
    file <-  paste(options$save$reports.dir.run, "var_by_", paste(cf, sep="", 
       collapse="_"), sep="")
    add.to.row <- list(); add.to.row$pos <- list(0); add.to.row$command <- c("\\endhead \n")
    if (no.cf==2){show.totals <- TRUE} else {show.totals <- FALSE}
    write.2D.table(OutT, cf, VaR, caption, file, show.totals, add.to.row)
  }
}
