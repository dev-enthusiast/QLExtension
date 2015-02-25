# Split prices for different buckets using a linear relationship between
# the kids.  So, given FLAT price, calculate a PEAK and OFFPEAK price. 
# It assumes that the PEAK and OFFPEAK price are proportional
#

splitBucket <- function(X, toSplit=c("PEAK", "OFFPEAK"),
  fromBucket="FLAT", region="NEPOOL", verbose=FALSE)
{
  if (any(!c("month", toSplit, fromBucket) %in% names(X)))
    stop("Input X data.frame is missing one of required columns!")

  reg <- lm(X[,toSplit[1]]~X[,toSplit[2]])

  if (verbose){
    print(summary(reg))
    plot(X[,toSplit[2]], X[,toSplit[1]], col="blue", xlab=toSplit[1],
         ylab=toSplit[2])
    abline(reg, col="gray")
  }

  beta <- coef(reg)
  ind  <- which(apply(is.na(X[,toSplit]), 1, all)) # find the index of months
  fX   <- X[ind,]           # future X, what you'll work with
  fmonths <- fX[, "month"]  # future months
  
  # get the number of hours for the future months
  hrs <- ntplot.bucketHours(toSplit, min(fmonths), max(fmonths),
    region=region, period="Month")
  names(hrs)[2] <- "month"
  hrs <- cast(hrs, month ~ bucket, I, fill=NA)
  # make sure you get only the hours for the months you want to split
  hrs <- merge(hrs, data.frame(month=fX[,"month"]), by="month", all.y=TRUE)
  alpha <- hrs[,toSplit[1]]/(hrs[,toSplit[1]]+hrs[,toSplit[2]])
  
  fX[, toSplit[2]] <- (fX[,fromBucket]/alpha - beta[1])/(beta[2] + 1/alpha - 1)
  fX[, toSplit[1]] <- (1 - 1/alpha)*fX[, toSplit[2]] + fX[,fromBucket]/alpha

  if (verbose){
    points(fX[,toSplit[2]], fX[,toSplit[1]], col="red")
    legend("topleft", legend=c("original", "split"), text.col=c("blue", "red"), bty="n")
  }
  X[ind,] <- fX

  return(X)
}





  
