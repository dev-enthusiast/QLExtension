# Read futures curve data from MDS and format it the way you need it.
# Where
#   source("H:/user/R/RMG/Utilities/futCurve.R")
#   options$curve.id     <- c(1, 9090, 13035) # NG, PWJ WEST 5x16, WTI
#   options$pricing.dt   <- seq(as.Date("2005-09-01"), by="day", length.out=5)
#   options$contract.dt  <- seq(as.Date("2006-01-01"), by="month", length.out=24)
#   options$format.2D    <- 1   # format output as a matrix?
#   options$remove.balmo <- 1   # remove balance of the month prices
#   res <- futCurve(options)
#
# Returns a 3D array, [pricing.dt, contract.dt, curve.id]
#
# Written by Adrian Dragulescu on 16-Jun-2005

futCurve <- function(options){

  if (length(options$pricing.dt)==0){options$pricing.dt <- Sys.Date()}
  if (length(options$format.2D)==0){options$format.2D <- 0}
  if (length(options$remove.balmo)==0){options$remove.balmo <- 0}
    
  require(RODBC)
  #----------------------------------------------Build the Query---------
  ids <- paste(options$curve.id, collapse=", ")
  minPD <- format(min(options$pricing.dt), "%m/%d/%Y")
  maxPD <- format(max(options$pricing.dt), "%m/%d/%Y")
  between.pd <- paste(" (effective_dt BETWEEN TO_DATE('", minPD,
      "', 'MM/DD/YYYY') AND TO_DATE('", maxPD, "', 'MM/DD/YYYY')) ", sep="")
  between.cd <- ""
  if (length(options$contract.dt)!=0){
     minCD <- format(min(options$contract.dt), "%m/%d/%Y")
     maxCD <- format(max(options$contract.dt), "%m/%d/%Y")    
     between.cd <- paste(" AND (start_dt BETWEEN TO_DATE('", minCD,
      "', 'MM/DD/YYYY') AND TO_DATE('", maxCD, "', 'MM/DD/YYYY')) ", sep="")
  }
  query <- paste("SELECT curve_id, effective_dt, start_dt as contract, curve_value as price FROM mds.Curve_Data_T WHERE curve_id in (", ids, ") AND ", between.pd, between.cd, "ORDER BY curve_id, start_dt, effective_dt")

  #----------------------------------------------Read the data-------------
  cat("Reading data from MDS ... "); flush.console();
  con <- odbcConnect("MDS", uid="MDS_R", pwd="MDS_R")
  res <- sqlQuery(con, query)
  odbcClose(con)
  colnames(res) <- c("curve.id", "pricing.dt", "contract.dt", "price")
  res[,"contract.dt"] <- as.Date(res[,"contract.dt"])
  res[,"pricing.dt"]  <- as.Date(res[,"pricing.dt"])
  res[1:10,]
  cat("Done.\n"); flush.console();

  if (options$remove.balmo){             # remove bal-month data
    ind <- which(format(res$pricing.dt, "%b%y")==format(res$contract.dt, "%b%y"))
    res <- res[-ind,] 
  }
  res[,"pricing.dt"]  <- factor(res[,"pricing.dt"], levels=options$pricing.dt)
  res[,"contract.dt"] <- factor(res[,"contract.dt"], levels=options$contract.dt)  
  
  MM <- array(NA, dim=c(length(options$pricing.dt), length(options$contract.dt),
     length(options$curve.id)), dimnames=list(as.character(options$pricing.dt),
     as.character(options$contract.dt), options$curve.id)) 
  for (c in 1:length(options$curve.id)){   # arrange MM in matrix form
    aux <- subset(res, curve.id==options$curve.id[c])
    MM[,,c] <- tapply(aux$price, list(aux$pricing.dt, aux$contract.dt), sum)
  }
  if (length(options$curve.id)==1){  # for one commodity make it 2D
    MM <- MM[,,1]                    # and remove the weekends & holidays 
    MM <- MM[-which(apply(is.na(MM),1,all)),]
  }
  return(MM)
}

#   uDates     <- sort(unique(res[,"pricing.dt"]))
#   uContracts <- format(sort(unique(res[,"contract.dt"])), "%b%y")
#   res[,"pricing.dt"]  <- factor(res[,"pricing.dt"], levels=uDates)  
#   res[,"contract.dt"] <- format(res[,"contract.dt"], "%b%y")
#   res[,"contract.dt"] <- factor(res[,"contract.dt"], levels=uContracts)  

# MM[[1]][1:20,1:10]
#

#  query <- "SELECT curve_id, effective_dt, start_dt as contract_dt, end_dt as to_dt, curve_value FROM mds.Curve_Data_T WHERE effective_dt <= '6-jun-2005' AND curve_id in (1) order by curve_id, start_dt, effective_dt"

#   res$risk.factor <- paste(res$curve.id, res$contract.dt, sep=".")
#   MM <- tapply(res$price, list(res$pricing.dt, res$risk.factor), sum)
