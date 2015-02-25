# Calculates the quantiles of a variable by month.
# Where data is a dataframe with columns [year month day hour x]
#            colum hour is optional
#       options$labels$mmmyy = 1 switches between different label formats
#
#
# written by Adrian Dragulescu on 24-Nov-2004


quantiles.by.month <- function(data,options,save){

  if (nargs()==1){ options <- list()}
  if (length(options$probs)==0){
    options$probs <- c(0,0.05,seq(0.1,0.9,0.1),0.95,1)}  
  if (length(options$labels$mmmyy)==0){options$labels$mmmyy <- 0}

  myMean <- function(x){mean(x, na.rm=T)}
  OutT=NULL
  if (options$labels$mmmyy == 1){   # for incomplete years
    days  <- as.Date(paste(data$year,data$month,data$day,sep="-"))
    fdays <- format(days, "%b%y"); uMonths <- unique(fdays)
    for (m in uMonths){
      ind <- which(fdays==m)
      OutT <- cbind(OutT,
           quantile(data$x[ind], probs=options$probs, na.rm=T))      
    }
    OutT   <- cbind(OutT, quantile(data$x, probs=options$probs, na.rm=T))
    means  <- tapply(data$x, fdays, myMean)
    colnms <- uMonths
  } else {                            # when you don't care about years
    uMonths <- sort(as.numeric(unique(data$month)))
    for (m in uMonths){
      ind  <- which(data$month==m)
      OutT <- cbind(OutT,
           quantile(data$x[ind], probs=options$probs, na.rm=T))
    }
    OutT   <- cbind(OutT, quantile(data$x, probs=options$probs, na.rm=T))
    means  <- tapply(data$x, data$month, myMean)
    colnms <- month.abb[uMonths]
  }
    
  if (length(uMonths)==12){
    colnames(OutT) <- c(colnms, "Year")
  } else {
    colnames(OutT) <- c(colnms, "All")
  }
  OutT <- as.data.frame(OutT)
  OutT <- cbind(Quantile=rownames(OutT), OutT)
 
  OutT <- rbind(OutT, OutT[nrow(OutT),])
  rownames(OutT) <- 1:nrow(OutT)
  OutT[,1] <- as.character(OutT[,1])
  
  OutT[nrow(OutT),1] <- "Mean"
  OutT[nrow(OutT),2:(length(uMonths)+1)] <- means[uMonths]
  OutT[nrow(OutT),length(uMonths)+2] <-  mean(data$x, na.rm=T)
  OutT[,2:ncol(OutT)] <- round(OutT[,2:ncol(OutT)], digits=2)

  if (save$Analysis$all){
   write.table(OutT, save$Tables$filename, sep=",", quote=F, row.names=F)
  }
  return(OutT)
}
