# Check the spread regression in Slang.
#
# Written by Adrian Dragulescu on 13-Oct-2004
# Modifiedy by Wang Yu to
#   1. Comparasion of Regression Results
#   2. Comparasion of calculated Curves with System Marks

check.Regression <- function (MM,FutPrices,Market_Loc,options,save)
{
  eps = 0.1
  maxprice = 200
  # _UT elec nepool spread regress
  # _UT elec nypp spread regress
  # remove prices lower than 0.1 or higher than 200
                                        
  uBuckets <- as.character(unique(MM$Bucket))
  std  <- matrix(NA, ncol=length(uBuckets),
        nrow=12, dimnames=list(month.abb, uBuckets))
  AllOutM <- list()
  
  for (bucket in uBuckets)
  {
    indB  <- which(MM$Bucket==bucket)
    cutMM <- MM[indB,]
    OutM  <- matrix(NA, ncol=5, nrow=12)
    for (m in 1:12)
    {
      ind <- which(as.numeric(cutMM[,"month"])==m 
                   & cutMM[,3] < maxprice & cutMM[,4] < maxprice 
                   & cutMM[,3] > eps      & cutMM[,4] > eps )

      x <- cutMM[ind,3]
      y <- cutMM[ind,4]
      reg  <- lm(y~x)
      sreg <- summary(reg)
      if (gsub(" ","",bucket) == FutPrice$Bucket[1])
      {       
        IndBase <- which(FutPrice$Month == m)
        Samples <- FutPrice$BasePrice[IndBase]      
          
        CI <- predict(reg, data.frame(x=Samples), interval = "confidence",level = 0.95)
        FutPrice$CILower[IndBase] <- CI[,"lwr"]
        FutPrice$CIUpper[IndBase] <- CI[,"upr"]
        
        FutPrice$RMGFit[IndBase] <- CI[,"fit"]
       
        PI <- predict(reg, data.frame(x=Samples), interval = "prediction",level = 0.95)
        if (bucket == "7x8")
           {Intervals = (PI[,"upr"] - PI[,"lwr"])/sqrt(8) }
        else
          {Intervals = (PI[,"upr"] - PI[,"lwr"])/sqrt(16) }   
          
        FutPrice$PILower[IndBase] <- FutPrice$RMGFit[IndBase] - Intervals
        FutPrice$PIUpper[IndBase] <- FutPrice$RMGFit[IndBase] + Intervals
      }
                      
      std[m,bucket]  <- sd(reg$residuals)                                           
      OutM[m,c(1,3)] <- as.numeric(sreg$coefficients[,1])                                                       
      OutM[m,c(2,4)] <- as.numeric(sreg$coefficients[,2])
      OutM[m,5] <- sreg$r.squared
       
      PredictInterval <- predict(reg, interval = "prediction",level = 0.95) 
      PredictInterval <- cbind(PredictInterval,Hist = y)
        
     fileName <- paste(save$dir$plots,Market_Loc,month.abb[m],gsub(" ","",bucket),".pdf", sep="")    
    browser()
     if (save$Analysis$all){pdf(fileName, width=4.0, heigh=4.0)}
     # Square plot 
     par(pty="s") 
     matplot(x, PredictInterval[,-1], col=c("red","red","blue"),
         main=paste(month.abb[m], ", Bucket ", bucket, sep=""),
         cex.main=1, xlab=names(MM)[3], ylab=names(MM)[4],           
         type=c("l","l","p"), pch=".",xlim=range(c(x,PredictInterval)), ylim=range(c(x,PredictInterval)))
     # Add a straight line
     abline(reg, col="black") 
     if (save$Analysis$all){dev.off()}
        
    }
  
  colnames(OutM) <- c("Intercept","StdDev.Intercept","Slope",
                      "StdDev.Slope","R.Squared")     
  AllOutM[[bucket]] <- OutM
  
  }

  res<- list(Parms=AllOutM, Results=FutPrice)
   
}