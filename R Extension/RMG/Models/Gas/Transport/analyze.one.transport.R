# Analyze one spread at a time.
# Where MM is a dataframe with columns: [year month day hour spread]
# Sample from the empirical pdf and calculate the pdf of the
#    simulated spread and it's quantiles by month.
#
# Written by Adrian Dragulescu on 29-Oct-2004


analyze.one.transport <- function(MM, save, options){

  options$between <- gsub("_"," ",options$between)

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

  save$filename <- paste(save$dir, "hMean_month" ,sep="")  
  options$table$tex.caption  <- paste("Historical mean spread by month for ",
             options$between, ".", sep="")
  OutT <- mean.by.month(MM, options, save)

  if (options$bootstrap==1){
    bootstrap.spread(MM, save, options)
  }

  
  
} 


#   #------------ Violin plot of spread by month -------
#   mmmyy <- format(as.Date(paste(MM$year,MM$month,MM$day, sep="-")), "%b%y")
#   u.mmmyy <- unique(mmmyy)
#   mmmyy <- factor(mmmyy, levels=u.mmmyy)
  
#   require(Simple)
#   par(xaxt="n")
# #  simple.violinplot(spread.log ~ mmmyy, col="wheat1", ylim=c(-20,20))
#   boxplot(MM$spread ~ mmmyy, col="wheat1", ylim=c(-0.75,0.75))

#   points(1:length(u.mmmyy), tapply(spread.log, mmmyy, mean), pch=19)
#   par(xaxt="s")
#   axis(1, at=1:length(u.mmmyy), labels=as.character(u.mmmyy), las=2, cex.axis=0.7)
