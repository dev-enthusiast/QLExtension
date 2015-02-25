# Calculates the quantiles of a variable by season.
# Where data is a dataframe with columns [year month day hour x]
#            colum hour is optional
#
# written by Adrian Dragulescu on 24-Nov-2004


quantiles.by.season <- function(data,options,save){

  if (nargs()==1){ options <- list()}
  if (length(options$probs)==0){
    options$probs <- c(0,0.05,seq(0.1,0.9,0.1),0.95,1)}
  source("C:/R/Work/MyLibrary/TimeFunctions/calculate.season.R")
  myMean <- function(x){mean(x, na.rm=T)}

  data <- calculate.season(data)
  OutT=NULL
  uSeasons <- as.character(unique(data$season))
  for (m in uSeasons){
    ind  <- which(data$season==m)
    OutT <- cbind(OutT,
           quantile(data$x[ind], probs=options$probs, na.rm=T))
  }
  OutT <- cbind(OutT, quantile(data$x, probs=options$probs, na.rm=T))
  colnames(OutT) <- c(uSeasons, "All History")
  OutT <- as.data.frame(OutT)
  OutT <- cbind(Quantile=rownames(OutT), OutT)
 
  OutT <- rbind(OutT, OutT[nrow(OutT),])
  rownames(OutT) <- 1:nrow(OutT)
  OutT[,1] <- as.character(OutT[,1])
  
  means <- tapply(data$x, data$season, myMean)
  OutT[nrow(OutT),1] <- "Mean"
  OutT[nrow(OutT),2:(length(uSeasons)+1)] <- means[uSeasons]
  OutT[nrow(OutT),length(uSeasons)+2] <-  mean(data$x, na.rm=T)
  OutT[,2:ncol(OutT)] <- round(OutT[,2:ncol(OutT)], digits=2)

  if (save$Analysis$all){
   write.table(OutT, save$Tables$filename, sep=",", quote=F, row.names=F)
  }
  return(OutT)
}
