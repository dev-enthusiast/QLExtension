# Given a daily or hourly time series, calculate the monthly mean. 
# Where data is a data frame with column names: year, month, day, hour, x 
#       res  is a data frame with column names: year, month, mean.x
#
#       save$filename="hub.prices" 
#       options$one.column=1 to return data as a data.frame in a column
#
# Last modified by Adrian Dragulescu on 7-Dec-2004

mean.by.month <- function(data, options, save){

#browser()
  if (length(save$Analysis$all)==0){save$Analysis$all <- 1}
  if (length(options$one.column)==0){options$one.column <- 0}
  if (length(options$table$tex.caption)==0){options$table$tex.caption <- ""}
  if (length(options$table$size)==0){options$table$size <- "small"}
  if (length(options$table$tex.digits)==0){options$table$tex.digits <- 2}
  
  uYears  <- unique(data$year)
  uMonths <- sort(unique(data$month))
  myMean <- function(x){mean(x, na.rm=TRUE)}

  OutT  <- tapply(data$x, list(data$month, data$year), myMean)
  OutTT <- cbind(OutT,  All.Years=tapply(data$x, data$month, myMean))
  rownames(OutTT) <- month.abb[as.numeric(rownames(OutT))]
  OutTT <- rbind(OutTT, All.Months=c(tapply(data$x, data$year, myMean),
                          myMean(data$x)))

  if (save$Analysis$all){
   aux <- cbind(Month=rownames(OutTT), OutTT)
   aux <- rbind(colnames(aux), aux)
   filename <- paste(save$filename, ".csv", sep="")
   write.table(aux, filename, sep=",", quote=F, row.names=F, col.names=F)
   require(xtable)
   filename <- paste(save$filename, ".tex", sep="")
   print.xtable(xtable(OutTT, caption=options$table$tex.caption,
      display=c("s", rep("f",ncol(OutTT))),
      digits=c(0,rep(options$table$tex.digits, ncol(OutTT)))),
      file=filename, size=options$table$size)
  }
  if (options$one.column==1){
    OutT <- as.data.frame(OutT); OutT <- stack(OutT)
    OutT <- data.frame(year=as.numeric(as.character(OutT$ind)),
                  month=rep(uMonths, length(uYears)),
                  Mean=OutT$values)
  }
  return(OutT)
} 
