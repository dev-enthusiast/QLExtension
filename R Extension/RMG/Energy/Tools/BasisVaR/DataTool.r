#
#  source("H:/user/R/RMG/Energy/Tools/BasisVaR/DataTool.r")
#

require(SecDb)
get.historic.hourly.data <- function(symbols, dateSt, dateEnd, mergeType=FALSE){
# Download prices, uses NT PLot symbols.
#
# intraday  : for intraday data. for daily data set to FALSE.
# union     : take the union of two data sets, will eliminate NA's. Takes intersection otherwise.


  for (i in 1:length(symbols)) {
    new.data = tsdb.readCurve(symbols[i], dateSt, dateEnd)
    colnames(new.data)[2] = symbols[i]
    if (i==1) {
      data = new.data
      
    } else {
      data = merge(data, new.data, by=c("time"), all=mergeType)
    }

  }
  return(data)
  
}

get.historic.daily.data <- function(symbols, dateSt, dateEnd, mergeType=FALSE){
# Download prices, uses NT PLot symbols.
#
# intraday  : for intraday data. for daily data set to FALSE.
# union     : take the union of two data sets, will eliminate NA's. Takes intersection otherwise.
  require(SecDb)

  i=1
  for (i in 1:length(symbols)) {
    new.data = tsdb.readCurve(symbols[i], dateSt, dateEnd)
    colnames(new.data)[2] = symbols[i]
    if (i==1) {
      data = new.data

    } else {
      data = merge(data, new.data, by=c("date"), all=mergeType)
    }
  }
  return(data)

}

merge.historic.hourly.daily.data <- function(hourly.series,daily.series){
# Download prices, uses NT PLot symbols.
#
# intraday  : for intraday data. for daily data set to FALSE.
# union     : take the union of two data sets, will eliminate NA's. Takes intersection otherwise.
  hourly.series$date = as.Date(format(hourly.series$time,"%Y-%m-%d"))

  hourly.series = merge(hourly.series,daily.series,by="date",all=TRUE)
  
  return(hourly.series[,2:ncol(hourly.series)])
  
}

get.timedate.info = function(hourly.series) {

  new.data = data.frame(hourly.series$time)
  colnames(new.data)[1] ="time"
  
  new.data$date = as.Date(format(hourly.series$time,"%Y-%m-%d"))
  new.data$yyyymm = format(hourly.series$time,"%Y%m")
  new.data$Month= format(hourly.series$time,"%m")
  new.data$yday = format(hourly.series$time,"%j")
  new.data$hour = format(hourly.series$time,"%H")
  
  return(new.data)
}

create.factor.matrix = function(label.list,no_rows, factor_column, row_names=NULL) {

  reduced.label.list = sort(unique(label.list))
  no_factors = length(reduced.label.list)
  
  
  fact_matrix = (matrix(gl(no_factors,no_rows,labels=reduced.label.list),nrow=no_rows,ncol=no_factors))
  colnames(fact_matrix) = reduced.label.list


  rep_fact = matrix(factor_column,nrow=no_rows,ncol=no_factors)
  fact_regr_matrix = (rep_fact ==fact_matrix) *1
  colnames(fact_regr_matrix) = reduced.label.list
  rownames(fact_regr_matrix) = row_names


  factor.matrix = list()
  factor.matrix$fact_matrix = fact_matrix
  factor.matrix$no_factors = no_factors
  factor.matrix$fact_regr_matrix = fact_regr_matrix
  factor.matrix$labelList = reduced.label.list
  return(factor.matrix)

}


create.uniform.matrix = function(N,k) {
######## creates an Nxk matrix of uniformly distributed random numbers.
######## makes sure that the sample is antithetic across the N

  NSimsUp = N%/%2
  NSimsBase = N-2*NSimsUp
  
  Z= array(runif(NSimsUp*k),dim=c(NSimsUp,k))
  Z= rbind(Z,1-Z)
  if (NSimsBase>0)  {
    Z = rbind(Z,array(0.5,dim=c(NSimsBase,k)))
  }
  return(Z)

}

ntplot.roll = function(symbol) {
  x = secdb.invoke("_LIB NTPLOT Functions", "NTPLOT::roll", 
  symbol, "then-3")


}