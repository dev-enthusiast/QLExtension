# Analyze one spread at a time.
# Where MM is a dataframe with columns: [year month day hour spread]
# Sample from the empirical pdf and calculate the pdf of the
#    simulated spread and it's quantiles by month.
#
# Written by Adrian Dragulescu on 29-Oct-2004


analyze.one.spread <- function(MM, save, options){

  myMean <- function(x)(mean(x, na.rm=TRUE))
  options$between <- gsub("_"," ",options$between)
  cat("   Start basic tables spread ... "); flush.console()    
  
  #-----------------------------Historical quantile tables--------
  MM$x <- MM$spread
  fName <- paste("hist.quantiles.month.",options$main,".csv", sep="")
  save$Tables$filename <- paste(save$dir,fName,sep="")
  OutT <- quantiles.by.month(MM,options,save)
  OutT[,1] <- gsub("%","",OutT[,1])
  caption  <- paste("Historical quantiles by month for the spread ", options$between, " based on historical data from ", options$dateRange, ".", sep="")
  fileName <- paste(save$dir,"hQuantiles_month.tex", sep="")
  print(xtable(OutT, caption=caption), file=fileName, size="scriptsize")
  
  fName <- paste("hist.quantiles.season.",options$main,".csv", sep="")
  save$Tables$filename <- paste(save$dir,fName,sep="")
  OutT <- quantiles.by.season(MM,options,save)
  OutT[,1] <- gsub("%","",OutT[,1])
  caption  <- paste("Historical quantiles by season for the spread ", options$between, " based on historical data from ", options$dateRange, ".", sep="")
  OutT <- as.data.frame(t(OutT))
  colnames(OutT) <- as.character(as.matrix(OutT[1,]))
  OutT <- cbind(rownames(OutT), OutT)
  rownames(OutT) <- 1:nrow(OutT);  OutT <- OutT[-1,]
  colnames(OutT)[1] <- "Quantiles"
  fileName <- paste(save$dir,"hQuantiles_season.tex", sep="")
  print(xtable(OutT, caption=caption, align=rep("r", ncol(OutT)+1)),
        file=fileName, size="footnotesize")

  #--------------TABLE: Mean spread for Peak hours by month----
  MM.peak  <- select.onpeak(MM); ind.peak <- MM.peak$ind.PEAK
  save$filename <- paste(save$dir,"hMean_month_peak", sep="")
  options$table$tex.caption  <- paste("Historical onpeak mean spread by month for ",
                                      options$between, ".", sep="")
  OutT <- mean.by.month(cbind(MM.peak, x=MM.peak$spread), options, save)

  
  MM.offpeak <- MM[-ind.peak,]
  save$filename <- paste(save$dir,"hMean_month_offpeak", sep="")
  options$table$tex.caption  <- paste("Historical offpeak mean spread by month for ",
                                      options$between, ".", sep="")
  OutT <- mean.by.month(cbind(MM.offpeak, x=MM.offpeak$spread), options, save)


  options$table$tex.caption  <- paste("Historical 7$\times$24 mean spread by month for ",
                                      options$between, ".", sep="")
  save$filename <- paste(save$dir, "hMean_month",sep="")
  OutT <- mean.by.month(MM, options, save)
  OutT <- rbind(OutT, All.months=apply(OutT,2,myMean))
  cat("   Done.\n"); flush.console() 

  #------------- Weather normalized spread -----------
  if (length(options$airport.name)!=0){
    weather.normalize.spread(MM, OutT, options, save)
  }
  daily.spread.regression(MM, options, save)

  if ((options$region=="PJM")&(options$other.variables==1)){
    daily.spread.regression.other(MM, options, save)    
  }
  
  if (options$bootstrap==1){
   cat("   Start simulating spread ... "); flush.console()    
   bootstrap.spread(MM, save, options)
   cat("   Done.\n"); flush.console()
  }
  
} 


#   #------------ Violin plot of spread by month -------
#   mmmyy <- format(as.Date(paste(MM$year,MM$month,MM$day, sep="-")), "%b%y")
#   u.mmmyy <- unique(mmmyy)
#   mmmyy <- factor(mmmyy, levels=u.mmmyy)
#   spread.log=NULL
#   ind <- which(MM$spread>0);  spread.log[ind] <- log(MM$spread[ind])
#   ind <- which(MM$spread<0);  spread.log[ind] <- -log(-MM$spread[ind])
#   ind <- which(MM$spread==0); spread.log[ind] <- MM$spread[ind]
  
#   require(Simple)
#   par(xaxt="n")
#   simple.violinplot(spread.log ~ mmmyy, col="wheat1", ylim=c(-20,20))
#   points(1:length(u.mmmyy), tapply(spread.log, mmmyy, mean), pch=19)
#   par(xaxt="s")
#   axis(1, at=1:length(u.mmmyy), labels=as.character(u.mmmyy), las=2, cex.axis=0.7)
