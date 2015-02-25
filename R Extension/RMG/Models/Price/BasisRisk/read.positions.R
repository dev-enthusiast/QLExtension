# Read the house positions from CPSPROD database or from a file 
#
#
#  Written by Adrian Dragulescu on 24-Mar-2005


read.positions <- function(save, options){

  if (length(options$quantity.file)==0){
      source(paste(save$source.dir, "read.positions.from.db.R", sep=""))     
      Q.tot <- read.positions.from.db(save, options)
      filename <- paste(save$dir.tables, "quantity.total.csv", sep="")
      aux <- cbind(rownames(Q.tot), Q.tot); colnames(aux)[1] <- "Month"
      write.table(aux, file=filename, sep=",", row.names=F)
  } else {
      Q.tot <- csv.read(options$quantity.file)
  }

  Q.tot$accounting <- factor(Q.tot$accounting, levels=c(levels(Q.tot$accounting), "all"))
  Q.tot$zone <- factor(Q.tot$zone, levels=options$short.names)
  Q.flat <- tapply(Q.tot$delta, list(Q.tot$month, Q.tot$zone,
                          Q.tot$accounting), sum)
  Q.flat[,,"all"] <- apply(Q.flat, c(1,2), sum, na.rm=T)
  rownames(Q.flat) <- format(as.Date(rownames(Q.flat)), "%Y-%m")
  Q.tot$month   <- format(as.Date(Q.tot$month), "%Y-%m")
  
  #----------------------------------------------------------------
  for (accounting in dimnames(Q.flat)[[3]]){
    filename <- paste(save$dir.tables, "quantities_",accounting,".tex", sep="")
    print.xtable(xtable(Q.flat[,,accounting], caption=paste("Positions in ",
       options$deltas.unit," by month and zone for ", accounting, "accounting.",
       sep=""), digits=rep(0, ncol(Q.flat)+1)), file=filename, size="small")
  }
  
  #--------------------------Plot the quantities--------------------
  filename  <- paste(save$dir.plots, "quantity.pdf", sep="")
  pdf(filename, width=6, height=6)
  bkg.col <- trellis.par.get("background")
  bkg.col$col <- "white"
  trellis.par.set("background", bkg.col)
  sym.col <- trellis.par.get("superpose.symbol")
  sym.col$col  <- c("blue", "red", "black", "green")
  sym.col$pch <- c(1,2,3,4)
  trellis.par.set("superpose.symbol", sym.col)
  print(xyplot(delta/options$scale.factor ~ zone|month, data=Q.tot,
         groups=accounting,       
##          panel=function(x,y){
##            panel.xyplot(x,y)
##            panel.abline(h=0, col="gray")
##          },
         key=list(points=Rows(sym.col, 1:length(levels(Q.tot$accounting))),
           text=list(levels(Q.tot$accounting)),
           columns = length(levels(Q.tot$accounting))),      
         scales=list(x=list(rot=90)),
         ylab="Delta, MMWh",
         main=paste(options$run.name, ", as of ", options$asOfDate))
  )
  dev.off()
  
  return(Q.flat)
}

