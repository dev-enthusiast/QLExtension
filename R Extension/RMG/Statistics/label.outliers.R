# Label the outliers from a plot of an lm model
# Where data - is a data frame with columns x, yTh, yEx, label
#
#  Written by Adrian Dragulescu on 31-Jan-2005


label.outliers <- function(data, options){

  if (nargs()==1){options=list()}
  if (length(options$cex)==0){options$cex <- 0.6}
  if (length(options$offset)==0){options$offset <- 0.2}  
  if (length(options$outlier.mult)==0){options$outlier.mult <- 1.65}

  std <- sd(data$yEx-data$yTh)
  ind <- which(abs(data$yEx-data$yTh)>options$outlier.mult*std)
  if (length(ind)>0){
    text(data$x[ind], data$yEx[ind], data$label[ind], pos="4",
         offset=options$offset, cex=options$cex)}
}
