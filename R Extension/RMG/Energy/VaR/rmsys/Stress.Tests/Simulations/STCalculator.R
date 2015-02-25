################################################################################
# External Libraries
#
require(reshape); require(rggobi)

source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Energy/VaR/Base/aggregate.run.positions.R")
source("H:/user/R/RMG/Utilities/Database/R/extend.price.sims.R")
source("H:/user/R/RMG/Utilities/Database/R/read.forward.price.sims.R")

## if (!exists("sPP") || !is.environment(sPP)){  # allow caching, hopefully no name conflict!
##   sPP <<- .getFileHashEnv(ST.options$sim.filehash.dir)
## }
################################################################################
# File Namespace
#
STCalculator = new.env(hash=TRUE)
sPP.env <- .getFileHashEnv(ST.options$sim.filehash.dir)  # local to this env


################################################################################
# Constants

# The number of curve simulation rows that can be rbound before being aggregated
# together.  This is used to ensure we don't fill up the hosts memory on large
# portfolios
STCalculator$.MAX_SUBSET_SIZE = 2500


################################################################################
# This function takes the position data and calculates the potential exposure.
# It then summarizes the data via indirect and direct modes and outputs
# the results.
#
STCalculator$calc.PnL <- function( QQ, sPP, ST.options) 
{
  uCurves <- unique(QQ$curve.name)
  if (!is.null(ST.options$PnL$by.curve)){ # only the curves you want
    QQ <- subset(QQ, curve.name %in% ST.options$PnL$by.curve)
  }
  if (!is.null(ST.options$PnL$by.month)){ # only the months you want
    QQ <- subset(QQ, curve.name %in% ST.options$PnL$by.month)
  }
  
  uCurves <- unique(QQ$curve.name)
  PnL <- array(NA, dim=c(length(uCurves), dim(sPP)[3]),
               dimnames=list(uCurves, 1:dim(sPP)[3])) 
  for (curve in uCurves){   # loop in case data sets are big...
    qq  <- subset(QQ, curve.name==curve)
    spp <- sPP[curve, , ]      

    # NEED TO EXTEND THE CURVES!!!!  TRUNCATE POSITIONS BELOW!
    qq  <- merge(qq, data.frame(contract.dt=as.Date(dimnames(spp)[[1]]),
                                nrow.spp=1:nrow(spp)))
    
    spp <- spp[qq$nrow.spp, ] 
    spp <- spp - qq$F0   # substract the current market
    PnL[curve,] <-  qq$position %*% spp     
  }
  
  if (ST.options$PnL$aggregate){
    PnL <- colSums(PnL)  # if everything is fine, you shouldn't have NA's
  }
  return(PnL)
}

################################################################################
#
STCalculator$.loadPositions <- function( ST.options ) 
{
  run.all <-  get.portfolio.book(ST.options)
  ST.options$run$specification <- subset(run.all,
    RUN_NAME == toupper(ST.options$run$run.name))
  if (nrow(ST.options$run$specification)==0){
    rLog("Could not find the run.name in the portfolio hierarchy.")
  }
  
  # from VaR/Base/aggregate.run.positions.R
  QQ <- .getDeltas(ST.options$run, ST.options)  
  QQ$vol.type <- NULL
  
  # add current day prices
  filename <- paste(ST.options$save$datastore.dir, "Prices/hPrices.",
    ST.options$asOfDate, ".RData", sep="")
  load(filename)
  QQ <- merge(QQ, hP, all.x=T)
  names(QQ)[4] <- "F0"           # asOfDate forward prices  
  
  return(QQ)
}


################################################################################
#
STCalculator$.getSimPrices <- function( pricing.dt, QQ,
                                       sPP.env, ST.options ) 
{
  uCurves <- unique(QQ$curve.name)
  contract.dts <- sort(unique(QQ$contract.dt))
  
  source("H:/user/R/RMG/Utilities/Database/R/read.forward.price.sims.R")
  res <- read.forward.price.sims(uCurves, NULL, ST.options$pricing.dt,
      path.to.filehash=ST.options$sim.filehash.dir, env=sPP.env)

  res <- res[,1,,]  # get only this pricing date

  return(res)
}

################################################################################
# Prepare the prices for ggobi use. 
#
STCalculator$.prepareGGOBI <- function( sPP, ST.options )
{
  sPP <- sPP[ST.options$keep$curves, , ]
  
  contract.dts <- as.Date(dimnames(sPP)[[2]])
  no.months <- length(contract.dts)
  no.sims   <- dim(sPP)[3]
  no.curves <- dim(sPP)[1]
  months <- as.numeric(format(contract.dts, "%m"))

  # add the first 3 months ...
  aux <- t(matrix(sPP[,1:3,], nrow=(dim(sPP)[1]*3), ncol=dim(sPP)[3]))
  colnames(aux) <- as.vector(outer(dimnames(sPP)[[1]], contract.dts[1:3], "paste"))
  sPP.agg <- aux
  
  # aggregate the first 4 quarters ...
  quarter.start <- months[1] %/% 3 + 1
  year <- format(contract.dts[1], "%Y")
  aux <- array(NA, dim=c(no.sims, (length(quarter.start:4))*no.curves))
  for (i in quarter.start:4){
    ind.qtr <- (i-1)*3 + 1:3
    ind.col  <- ((i-1)*no.curves+1):(i*no.curves)
    aux[,ind.col] <- t(apply(sPP[,ind.qtr,], c(1,3), mean))
  }
  quarters <- paste(year, quarter.start:4, sep="QTR")
  colnames(aux) <- as.vector(outer(dimnames(sPP)[[1]], quarters, "paste"))
  sPP.agg <- cbind(sPP.agg, aux)
  
  # aggregate the first 3 years of prices ...
  ind.Jan <- which(months==1)              # find the January
  if (ind.Jan[length(ind.Jan)]+11 > no.months){
    ind.Jan <- ind.Jan[-length(ind.Jan)]}

  aux <- array(NA, dim=c(no.sims, length(ind.Jan)*no.curves))
  for (i in 1:length(ind.Jan)){
    ind.year <- ind.Jan[i]:(ind.Jan[i]+11)
    ind.col  <- ((i-1)*no.curves+1):(i*no.curves)
    aux[,ind.col] <- t(apply(sPP[,ind.year,], c(1,3), mean))
  }
  years <- format(contract.dts[ind.Jan], "%Y")
  colnames(aux) <- as.vector(outer(dimnames(sPP)[[1]], years, "paste"))
  sPP.agg <- cbind(sPP.agg, aux)
   
  return(sPP.agg)
  
}

################################################################################
# Investigate bad events
#
STCalculator$.investigate.losses <- function( sPP, PnL, ST.options)
{
  rnames <- as.vector(outer(dimnames(sPP)[[1]], dimnames(sPP)[[2]], "paste"))
  sPP <- matrix(sPP, nrow=dim(sPP)[1]*dim(sPP)[2], ncol=dim(sPP)[3])
  rownames(sPP) <- rnames

  sPP <- t(sPP)

  

  
  ind.bad <- which(PnL < -20)



  
  options <- NULL
  options$no.cuts <- 10
  myQuantiles <- function(x){quantile(x, probs=options$prob.cuts, na.rm=T)}
  myFindInt <- function(x){findInterval(x, myQuantiles(x),rightmost.closed=T)} 
  options$prob.cuts <- c(0, seq(1/options$no.cuts/2, 1-1/options$no.cuts/2,
                                by=1/options$no.cuts), 1)
  bin.X <- apply(sPP[ind.bad,], 2, myFindInt)

  summary.bin.X <- apply(bin.X, 1, summary)
  ind <- which(summary.bin.X["Median", ] < 3)
  
  
  
}
