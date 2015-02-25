#
#  This function returns a data frame contains Location Name and PnodeID 
#
#  source("H:/user/R/RMG/Utilities/SecDb/R/get.locations.from.SecDB.r")
#  res <- get.locations.from.SecDB()
#  
#  Written by Michael Zhang on July 19, 2007

get.locations.from.SecDB <- function() {
                          
  library(SecDb)
  secdb.setDatabase("!Bal Prod 1;ReadOnly{!Bal Prod 2}")
  
  valueType1 = secdb.getValueType("PJM Report Manager", "Location Info Query")
  
  location.map <- function(x) {
    res <- array(unlist(x)[c("Name Upload", "PnodeID")])
    return(res)
  }

  locations <- unlist(lapply(valueType1, location.map))
  locations <- matrix(locations, ncol=2, byrow=T)
  res <- data.frame(locations)
  colnames(res) <- c("Node", "PNODEID")
 
  return(res)
}