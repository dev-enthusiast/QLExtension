# Get the most recent congestion prices, now for NEPOOL only
#
#
# Written by AAD on 15-Apr-2008

##   source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.load.tsdb.symbols.R")
##   nodes <- c(379, 380)
##   tsdb.symbols <- paste("NEPOOL_SMD_DA_", nodes, "_CongComp", sep="")
##   source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.load.tsdb.symbols.R")
##   aux <- FTR.load.tsdb.symbols(tsdb.symbols[c(1,12,13)], start.dt, Sys.time())

#  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.iso.R")

#########################################################################
# There's a version for data.frames of this function in package plyr but
# got weird in plyr_0.1.9.  
# Stack two zoo objects, allowing for non matching columns.  The index
# must be non-overlapping. 
#
rbind.zoo.fill <- function(x, y)
{
  if (length(intersect(index(x), index(y)))>0)
    stop("Zoo objects have overlapping indices!")
  
  X <- x; Y <- y
  colnames.xy <- sort(unique(c(colnames(x), colnames(y))))

  y_not_in_x <- colnames.xy[!(colnames.xy %in% colnames(x))]
  if (length(y_not_in_x)>0){
    X <- cbind(X, zoo(matrix(NA, ncol=length(y_not_in_x),
      nrow=length(index(X)), dimnames=list(NULL, y_not_in_x)), index(X)))
  }

  x_not_in_y <- colnames.xy[!(colnames.xy %in% colnames(y))]
  if (length(x_not_in_y)>0){
    Y <- cbind(Y, zoo(matrix(NA, ncol=length(x_not_in_y ),
      nrow=length(index(Y)), dimnames=list(NULL, x_not_in_y)), index(Y)))
  }
  
  XY <- rbind(X, Y)

  # sort the columns
  aux <- do.call(rbind, strsplit(colnames(XY), "\\."))
  aux <- data.frame(ord=1:ncol(XY), bucket=aux[,1], ptid=as.numeric(aux[,2]))
  aux <- aux[order(-xtfrm(aux$bucket), aux$ptid), ]
  XY  <- XY[, aux$ord]
  
  return(XY)    
}



#########################################################################
# Add NEPOOL prices ... 
#
daily.price.append.NEPOOL <- function(start.dt=NULL)
{
  rLog(paste(Sys.time(), "Start NEPOOL congestion update\n"))
  FTR.load.ISO.env("NEPOOL")


  if (is.null(start.dt)){
    # get the prices currently in the environment
    hP <- NEPOOL$hP
    start.dt <- index(hP)[length(index(hP))]  # add one day
    start.dt <- as.POSIXct(paste(start.dt, "00:00:00")) 
  } else {
    # start from scratch! 
    hP <- NULL
  }
    
  nodes <- sort(unique(NEPOOL$MAP$ptid))
  # get the new congestion prices
  hP.new <- NEPOOL$.makePriceSummaries(nodes, start.dt=start.dt, save=F)

  # check if there is index overlap, and remove the old prices
  if (index(hP.new)[1] <= index(hP)[length(index(hP))])
    hP <- window(hP, end = (index(hP.new)[1]-1)) 

  # stack them
  hP <- rbind.zoo.fill(hP, hP.new)

  rLog("Save the results back...")
  filename <- paste(NEPOOL$options$awards.dir, "RData/hPrices.RData", sep="")
  save(hP, file=filename)
  rLog("Done.")
  
}

#########################################################################
# Add NYPP prices ... 
# if you pass in a start.dt, it will overwrite the existing one ... 
daily.price.append.NYPP <- function(start.dt=NULL)
{
  rLog(paste(Sys.time(), "Start NYPP congestion update\n"))
  FTR.load.ISO.env("NYPP")


  if (is.null(start.dt)){
    # get the prices currently in the environment
    hP <- NYPP$hP
    start.dt <- index(hP)[length(index(hP))]  # add one day
    start.dt <- as.POSIXct(paste(start.dt, "00:00:00")) 
  } else {
    # start from scratch
    hP <- NULL
  }
    
  #source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NYPP.iso.R")
  filein <- "att_e_poi_pow_init_reconfig_tcc_auctions_AUT2008_07-21-2008_CLEAN.pdf"
  nodes <- NYPP$.read.auction.ptids(filein)$ptid
  nodes <- c(nodes, 61844, 61845, 61846, 61847) # add HQ, NPX, OH, PJM!
  
  # get the new congestion prices
  hP.new <- NYPP$.makePriceSummaries(nodes, start.dt=start.dt)

  # check if there is index overlap, and remove the old prices
  if (index(hP.new)[1] <= index(hP)[length(index(hP))])
    hP <- window(hP, end = (index(hP.new)[1]-1)) 

  # stack them
  hP <- rbind.zoo.fill(hP, hP.new)

  rLog("Save the results back...")
  filename <- paste(NYPP$options$awards.dir, "RData/hPrices.RData", sep="")
  save(hP, file=filename)
  rLog("Done.")
  
}

#########################################################################
#########################################################################
#########################################################################
require(FTR); require(CEGbase)
Sys.setenv(tz="")

daily.price.append.NEPOOL()
daily.price.append.NYPP()
# daily.price.append.NYPP(as.Date("2003-11-01")) # to REDO the file
rLog("Done.")



