# Given a daily or hourly time series, calculate the monthly statistics
#   specified by the function options$fun.    
# Where data is a data frame with column names: year, month, day, hour, x 
#       res  is a data frame with column names: year, month, total.x
# Last modified by Adrian Dragulescu on 20-Feb-2004

summarize.monthly <- function(data, options){

  if (nargs()==1){options=NULL}
  if (length(options$one.column)==0){options$one.column <- 0}
  if (length(options$fun)==0){options$fun <- "sum"}
  
  uYears <- unique(data$year)
  uMonths <- sort(unique(data$month))
   
  res=NULL               # the output results
  for (yr in uYears){
    aux <- data[which(data$year==yr),]
    sumMth <- tapply(aux$x, factor(aux$month), options$fun)
    sumYr <- data.frame(year=rep(yr, attributes(sumMth)$dim),
                  month=as.numeric(attributes(sumMth)$dimnames[[1]]),
                  x=as.numeric(sumMth))
    res <- rbind(res, sumYr) 
  }
  rownames(res) <- (1:dim(res)[1])

  if (options$one.column==0){
     myMean <- function(x){mean(x, na.rm=T)}
     OutT=matrix(NA, nrow=length(uMonths)+1, ncol=length(uYears)+1,
       dimnames=list(c(month.abb[uMonths], "All Months"),
                     c(uYears, "All Years")))
     for (y in 1:length(uYears)){
        ind <- which(res$year==uYears[y])
        OutT[res$month[ind],y] <- res$x[ind]
     }
     R <- nrow(OutT); C <- ncol(OutT)
     OutT[1:(R-1),C] <- apply(OutT[1:(R-1),1:(C-1)], 1, myMean)
     OutT[R,1:(C-1)] <- apply(OutT[1:(R-1),1:(C-1)], 2, myMean)     
     OutT[R,C] <- apply(as.matrix(data$x), 2, options$fun)
     res <- as.data.frame(OutT)
  }
  
  return(res)
} # end of function
