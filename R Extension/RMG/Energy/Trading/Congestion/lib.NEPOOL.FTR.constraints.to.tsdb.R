# Push the NYISO constraints to tsdb on the PM side
#
# ..binding_constraints_as.zoo
# .make_valid_symbol
# .update_tsdb_symbols
#


#################################################################
# Get the NEPOOL binding constraints and format them as a zoo object
# @param interval an list of Interval objects
# @param 
##
..binding_constraints_as.zoo <- function( intervals )
{
  if (class(intervals) == "Interval")
    intervals <- list(intervals)
  
  res <- lapply(intervals, function(interval) {
    startDt <- interval$startDt
    endDt   <- interval$endDt
  
    BC <- get_binding_constraints(region="NEPOOL", startDt=startDt, endDt=endDt,
      type=c("DA"))
    subset(BC, time >= startDt & time <= endDt)
   })

  BC <- do.call(rbind, res)

  bc <- sort(unique(BC$constraint.name))

  # create a hash value from the constraint name to use for the constraint
  hash <- data.frame(constraint.name = bc,
    symbol = sapply(bc, .make_valid_symbol),
    value  = sapply(bc, function(x) {
      aux <- murmur3.32(.make_valid_symbol(x))/2147483647
      if (abs(aux)<0.1)  # small values don't look good on the plot
        aux <- 10*aux

      aux
    }))
  rownames(hash) <- NULL
  
  BC <- merge(BC, hash, by="constraint.name")

  x <- cast(BC, time ~ symbol, max, fill=0, value="value")  # why max? probably because of DST

  time <- toHourly( intervals[[1]] )
  for (i in seq_along(intervals)[-1]) 
    time <- c(time, toHourly(intervals[[i]]))
 
  ind <- data.frame(time=time)
  x <- merge(x, ind, all=TRUE)
  x[is.na(x)] <- 0
  x <- zoo(x[,-1], x$time)

  list(x, hash)
}




#################################################################
# tsdb does not approve of having "+", "-", "*", "/", "|", " " in symbol name
# 1) Replace "_" with ""
# 2) Replace "-" with "_"
# 3) Replace " +" with ""
#
#
.make_valid_symbol <- function(constraintName)
{
  x <- gsub("_", "", constraintName)
  x <- gsub("-", "_", x)
  x <- gsub("\\+", "", x)
  paste("DABC_", gsub(" ", "", x), sep="")
}


################################################################################
# Write the data to tsdb
# @param X a data.frame with columns ["dataframe", name]
#
.update_tsdb_symbols <- function(X, description)
{
  if (is.null(X) || nrow(X)==0)
    return()
  
  isNewSymbol <- FALSE
  symb <- colnames(X)[2]
  if ( !tsdbExists(symb) ) {
    rLog("creating symbol", symb)
    path <- paste("T:/Data/pm/", symb, ".bin", sep="")
    description <- paste("NEPOOL DA binding constraint", constraint.name)
    dataSource  <- "http://www.iso-ne.com/isoexpress/web/reports/grid/-/tree/constraint-da"
    realTimeFlag <- TRUE
    tsdbCreate(symb, path, description, dataSource, realTimeFlag)
    isNewSymbol <- TRUE
  }
  
  rLog("updating symbol", symb)
  tsdbUpdate(symb, data.frame(time=X$datetime, value=X[,2]))

  isNewSymbol
}


#################################################################
#################################################################
# 
#
.test <- function()
{
  require(CEGbase)
  require(SecDb)
  require(Tsdb)
  require(CRR)
  require(hashFunction)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.Interval.R")
  #source("H:/user/R/RMG/Energy/Trading/Congestion/lib.TCC.analysis.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.constraints.to.tsdb.R")
  
  load_ISO_environment(region="NEPOOL")

  intervals <- list(Interval("2014-01-01 01:00:00", "2014-12-01 00:00:00"))
  aux <- ..binding_constraints_as.zoo( intervals )  
  bc <- aux[[1]]
  hash <- aux[[2]]

  newSymbols <- NULL
  for (i in seq_len(ncol(bc))) {
    X <- data.frame(datetime=index(bc), bc[,i])
    colnames(X)[2] <- colnames(bc)[i]
    constraint.name <- subset(hash, symbol==colnames(bc)[i])$constraint.name
    description <- paste("NEPOOL DA binding constraint", constraint.name)
    isNewSymbol <- .update_tsdb_symbols( X, description )
    if (isNewSymbol)
      newSymbols <- c(newSymbols, colnames(bc)[i])
  }

  
  cat(paste(newSymbols, sep="", collapse=";\n"))  # paste into the ntplot

  
  
  

}
