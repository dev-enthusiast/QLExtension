# Remove balance of the month data from forward power data.
# Where data is a data.frame with rows = pricing dates  in ISO format yyyy-mm-dd
#                            and  cols = contract dates in "%b%y" format
#    or data is a data.frame with cols = pricing.dt (ISO), contract.dt (ISO), price
#
# Returns data in the same format 
# 
# Written by Adrian Dragulescu on 22-Jun-2005

remove.balmo.from.fwd <- function(data){

  if ("price" %in% colnames(data)){  # data has one column of prices
    
    ind  <- which(format(data$contract.dt, "%b%y")==format(data$pricing.dt, "%b%y"))
    data <- data[-ind,]              #remove balmo prices
    
  } else {                           # data has a matrix of prices    
    sData <- cbind(stack(data), pricing.dt=rep(rownames(data), ncol(data)))
    ind   <- which(sData$ind==format(as.Date(sData$pricing.dt), "%b%y"))
    sData <- sData[-ind,]            #remove balmo prices

    contracts <- sort(as.Date(paste(1,unique(sData$ind), sep=""), "%d%b%y"))
    contract.dt <- factor(sData$ind, levels=format(contracts, "%b%y"))

    data <- tapply(sData$values, list(sData$pricing.dt, contract.dt), mean)
    ind  <- which(apply(is.na(data), 2, all))  # remove columns with all NA's
    data <- data[, -ind]
  }

  return(data)
}

