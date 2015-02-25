# A suite of functions for marking nodes, primarily used by the
# Condor/overnightNepoolNodesMark.R script.
#
# Functions:
#   - apply_month_constraints
#   - constructNodeMark
#   - .extend_history    
#   - .extendPeriodically
#   - format_pmView_for_marking : just a reshape
#   - .getHistLossfactors.NEPOOL : 
#   - .getHistMark  : get historical components for a ptid
#   - .getSecDbMark : get SecDb mark  for a delivery point
#   - .get_hist_fwd_stacked : stack hist and fwd marks for comparison,
#                             LMP only
#   - plotMarks
#   - .readAllMatWNOutput : read the strats WN regressions
#   - .readMatWNOutput
#   - .makeMatWNInput     : output in the stupid format for WeatherNorm
#   - .getMatWNmarkedNodes
#   - forecastLossFactors
#   - makePMview
#   - stack_hist_fwd  : stack historical and pmView

#########################################################################
# X is a data.frame similar to pmView one with columns:
#   c("month", "bucket", "value", ...)
# pass only pmView for now ...
#
apply_month_constraints <- function(X, constraintsMon=list(janfeb=c(1,2),
  julaug=c(7,8)))
{
  X <- X[, c("month", "bucket", "component", "value")]
  
  X$year <- as.numeric(format(X$month, "%Y"))
  X$mon  <- as.numeric(format(X$month, "%m"))

  for (i in seq_along(constraintsMon)){
    ind <- which(X$mon %in% constraintsMon[[i]])
    aux <- X[ind,]

    # average the months in the constraintsMon list
    mux <- cast(aux, year + bucket + component ~ ., mean)
    colnames(mux)[ncol(mux)] <- "value"
    mux <- expand.grid.df(mux, data.frame(mon=constraintsMon[[i]]))
    mux$month <- as.Date(paste(mux$year, mux$mon, "01", sep="-"))
    
    # delete this chunk, append it at the bottom
    X <- X[-ind,]        
    X <- rbind(X, mux)   
  }

  X <- X[order(X$month, X$bucket, X$component),]
  X$year <- NULL
  X$mon  <- NULL
  
  return(X)  
}


#########################################################################
# X contains the forward view, which is a data.frame
#   with columns c("month", "bucket", "component", "value")
#   where component has c("congestion", "loss%") calculated relative to Hub
#
# returns another data.frame with columns c("month", "bucket", "component",
#   "value") where components are c("loss%", "loss$", "congestion",
#   "hubMark","nodeMark")
#
# See functions calculate_node_mark in NEPOOL/Marks/lib.mark.nodes.SecDb 
# for a cleaner version
#
constructNodeMark <- function(X, asOfDate=Sys.Date(),
  startDate, endDate=NULL)
{
  if (is.null(endDate))
    endDate <- max(X$month)
  
  # check that you have the congestion and loss% in input data.frame
  if (!all(c("loss%", "congestion") %in% unique(X$component)))
    stop("Don't have loss% or congestion component in input X.")
  
  if (!all(c("5X16", "2X16H", "7X8") %in% unique(X$bucket)))
    stop("Missing one of the buckets 5X16, 2X16H, 7X8 in input X.")

  if ("statistic" %in% names(X)){  # you passed in WN spread!
    X <- subset(X, statistic=="Mean")  
  }

  # get the hours by month.  I'm not sure why I'm doing this here
  # I think it had to do with the fact that the cong view was
  # hourly?!  -- AAD 1/8/2013 
  HRS <- ntplot.bucketHours(bucket=c("5X16", "2X16H", "7X8"),
     startDate=min(X$month), endDate=endDate)
  names(HRS) <- c("bucket", "month", "hours")

  hubMark <- subset(X, component=="hubMark")  # assume the user is smart
  if (nrow(hubMark)==0){
    rLog("Get the hubMark from SecDb.")
    commodity <- "COMMOD PWR NEPOOL PHYSICAL"
    location  <- "HUB"
    bucket    <- c("5X16", "2X16H", "7X8")
    hubMark   <- secdb.getElecPrices(asOfDate, startDate, endDate, commodity,
      location, bucket)
  }
  # add the hours 
  hubMark <- merge(hubMark, HRS, by=c("month", "bucket"), all.x=TRUE) 
  hubMark <- hubMark[,c("month", "bucket", "value", "hours")]

  # check that X data.frame has the right columns & if it needs extensions
  if (any(!(c("month", "bucket", "component", "value") %in% names(X))))
    stop(paste("Incorrect or incomplete X input data.frame.\n",
      "You need month, bucket, component and value columns."))
  if (max(hubMark$month) > max(X$month))
    X <- PM:::.extendPeriodically(X, unique(hubMark$month))
  
  X <- merge(X, hubMark[, c("month", "bucket", "hours")])  # add hours
  
  # the Congestion component 
  cong <- subset(X, component=="congestion")
  MCC  <- tapply(cong$value*cong$hours, cong$month, sum)/
          tapply(cong$hours, cong$month, sum)

  # the Loss component
  lossP <- subset(X, component=="loss%")  
  loss  <- merge(lossP, hubMark[,c("month", "bucket", "value")],
                 by=c("month", "bucket"), all.x=TRUE)
  loss$value <- loss$value.x * loss$value.y      # in $ terms
  loss$component <- "loss$"
  MLC <- tapply(loss$value*loss$hours, loss$month, sum)/
         tapply(loss$hours, loss$month, sum)

  # the Energy component (use the hubMark)
  hub <- hubMark
  hub$component <- "hubMark"
  hub <- subset(hub, bucket %in% c("5X16", "2X16H", "7X8"))

  nodeMark <- rbind(
    lossP[,c("month", "bucket", "component", "value")],                    
     loss[,c("month", "bucket", "component", "value")],
     cong[,c("month", "bucket", "component", "value")],
      hub[,c("month", "bucket", "component", "value")])

  # calculate the node mark by summing over components: Hub + Cong + Losses
  res <- cast(nodeMark, month + bucket ~ ., sum,
    subset=component %in% c("hubMark", "congestion", "loss$"))
  names(res)[3] <- "value"
  res$component <- "nodeMark"

  nodeMark <- rbind(nodeMark, res)  # keep it for reference

  return(nodeMark)
}


#########################################################################
# Just reshape the pmView for inclusion in Erik Bucht's marking
# spreadsheet. 
# Make sure you already applied the montly constraints with apply_month 
# Eric's spreadsheet S:\Trading\Bucht\Nepool Marks Inputs.xls
#
.format_fwdmark_for_Bucht_xls <- function(pmMark)
{
  # aggregate to peak/offpeak first
  colnames(pmMark)[which(names(pmMark)=="month")] <- "contract.dt"
  X <- combineBuckets(pmMark, weight="hour", buckets=c("2X16H", "7X8"),
    name="OFFPEAK")

  res <- cast(X, contract.dt + bucket ~ component, I)
  res$spread <- res$nodeMark - res$hubMark
  res$year <- as.numeric(format(res$contract.dt, "%Y"))
  res$mon  <- as.numeric(format(res$contract.dt, "%m"))
  
  res <- cast(res, bucket + year ~ mon, I, value="spread", fill=NA)
  res[,3:ncol(res)] <- round(res[,3:ncol(res)], 2) 
  
  res <- split(res[,-1], res$bucket)
  
  return(res)  
}


#########################################################################
# Called from .getHistMark to extend a node with previous history
# @param X the original return by .getHistMark
# @param parentPtid
#
.extend_history <- function(X, parentPtid, buckets, component,
                            start.dt, end.dt)
{
  hst <- .getHistMark.NEPOOL(parentPtid, buckets=buckets,
    component=component, start.dt=start.dt, end.dt=end.dt)
  cutoffMth <- min(subset(X, component=="nodeMark")$month)
  X <- subset(X, month >= cutoffMth)
  hst <- subset(hst, month < cutoffMth)
  res <- rbind(hst, X)

  res
}

#########################################################################
# X needs to have a column called month which is an R Date.  Column
# "value" will be periodically extended from the last 12 monthly
# values available.  All the other columns will be left unchanged.
# toMonths are only the months that need extension ...  YOU CAN PASS
# SEVERAL LOCATIONS AT THE SAME TIME!!!
#
.extendPeriodically <- function(X, toMonths)
{
  monthsX  <- sort(unique(X$month))
  toMonths <- toMonths[which(toMonths > max(monthsX))]
  
  last12X <- subset(X, month %in% tail(monthsX,12))
  last12X$mon   <- format(last12X$month, "%m")
  last12X$month <- NULL

  # extended X
  extX <- merge(data.frame(month=toMonths, mon=format(toMonths, "%m")),
    last12X, by="mon", all.x=TRUE)
  extX$mon <- NULL
  extX <- extX[order(extX$month), ]

  # put them together
  extX <- rbind(X, extX)
#  extX <- extX[order(extX$month, extX$bucket, extX$component), ]
  
  return(extX)
}

#########################################################################
# See the corresponding regional functions
#
.getHistLossfactors.NEPOOL <- function(ptid, buckets=c("5X16",
  "2X16H", "7X8"))
{
  res <- NULL
  symbols <- c(paste("NEPOOL_SMD_DA_", ptid, "_MargLoss", sep=""), 
    "NEPOOL_SMD_DA_4000_MargLoss", "NEPOOL_SMD_DA_4000_EngyComp")

  endTime   <- Sys.time()
  startTime <- endTime - 4*366*24*3600 # four years or so...
  aux <- FTR:::FTR.load.tsdb.symbols(symbols, startTime, endTime)
  lf <- data.frame(time=index(aux), value=(aux[,1]-aux[,2])/aux[,3])
  
  hrs <- data.frame(secdb.getHoursInBucket("NEPOOL", "5X16",
    startTime, endTime), bucket="5X16")
  hrs <- rbind(hrs, data.frame(secdb.getHoursInBucket("NEPOOL", "2X16H",
    startTime, endTime), bucket="2X16H"))
  hrs <- rbind(hrs, data.frame(secdb.getHoursInBucket("NEPOOL", "7X8",
    startTime, endTime), bucket="7X8"))
    
  lf <- merge(lf, hrs)
  
  return(lf)
}

#########################################################################
# See the corresponding regional functions
#
historicalMark <- function(ptid, buckets=c("5X16", "2X16H", "7X8"),
   region="NEPOOL", MAP=NULL, component=c("loss%", "congestion",
   "hubMark", "nodeMark"), skipDates=NULL,
   start.dt=as.POSIXct("2003-01-01"), end.dt=Sys.time())
{
  .getHistMark(ptid, buckets=buckets,
      component=component, skipDates=skipDates, start.dt=start.dt,
      end.dt=end.dt)
}


#########################################################################
# See the corresponding regional functions
# @return a data.frame with columns c("month", "bucket", "component", "value")
# 
.getHistMark <- function(ptid, buckets=c("5X16", "2X16H", "7X8"),
   region="NEPOOL", MAP=NULL, component=c("loss%", "congestion",
   "hubMark", "nodeMark"), skipDates=NULL,
   start.dt=as.POSIXct("2003-01-01"),
   end.dt=Sys.time(),
   buddy=FTR:::.get_buddy_node())
{
  res <- NULL
  if (region == "NEPOOL") {
    res <- .getHistMark.NEPOOL(ptid, buckets=buckets,
      component=component, skipDates=skipDates, start.dt=start.dt,
      end.dt=end.dt)
    if (ptid %in% as.numeric(names(buddy))) {
      rLog("Extend history of", ptid, "with", buddy[as.character(ptid)])
      res <- .extend_history(res, buddy[as.character(ptid)], buckets=buckets,
        component=component, start.dt=start.dt, end.dt=end.dt)
    }
  }
  
  if (region == "NYPP"){
    refPtid <- 24008
    res <- .getHistMark.NYPP(ptid, buckets=buckets, MAP=MAP, refPtid,
      skipDates=skipDates)
  }
  
  return(res)  
}

#########################################################################
# Get historical settles for the marks
# Losses are in % relative to Hub Energy component
# Congestion is relative to Hub Congestion component
# 
#
.getHistMark.NEPOOL <- function(ptid, buckets=c("5X16", "2X16H", "7X8"),
   component=c("loss%", "congestion", "hubMark", "nodeMark"),
   skipDates=NULL, start.dt=as.POSIXct("2003-01-01"), end.dt=Sys.time())
{
  res <- NULL
  if (is.null(skipDates)) {             # everything is easy...
    
    if ("loss%" %in% component) {
      # get historical Losses
      symbol <- paste("NEPOOL_SMD_DA_", ptid, "_MargLoss/",
                      "NEPOOL_SMD_DA_4000_EngyComp", sep="")
      lossesNode <- ntplot.bucketizeByMonth(symbol, buckets)
      lossesNode$lossesNode <- lossesNode$value
  
      symbol <- "NEPOOL_SMD_DA_4000_MargLoss/NEPOOL_SMD_DA_4000_EngyComp"
      lossesHub <- ntplot.bucketizeByMonth(symbol, buckets)
      lossesHub$lossesHub <- lossesHub$value

      # merge them together in case you have shorter history!
      losses <- lossesNode[,c("bucket", "month", "lossesNode")]
      losses <- merge(losses, lossesHub[,c("bucket", "month", "lossesHub")])
      losses$value <- (losses$lossesNode - losses$lossesHub)
      losses$component <- "loss%"

      res <- rbind(res, losses[,c("month", "bucket", "component", "value")])
    }

    if ("loss$" %in% component) {
      # get historical Losses
      symbol <- paste("NEPOOL_SMD_DA_", ptid, "_MargLoss", sep="")
      lossesNode <- ntplot.bucketizeByMonth(symbol, buckets)
      lossesNode$lossesNode <- lossesNode$value
  
      symbol <- "NEPOOL_SMD_DA_4000_MargLoss"
      lossesHub <- ntplot.bucketizeByMonth(symbol, buckets)
      lossesHub$lossesHub <- lossesHub$value

      # merge them together in case you have shorter history!
      losses <- lossesNode[,c("bucket", "month", "lossesNode")]
      losses <- merge(losses, lossesHub[,c("bucket", "month", "lossesHub")])
      losses$value <- (losses$lossesNode - losses$lossesHub)
      losses$component <- "loss$"

      res <- rbind(res, losses[,c("month", "bucket", "component", "value")])
    }
       
    if ("congestion" %in% component){
      # get historical Congestion
      symbol <- paste("NEPOOL_SMD_DA_", ptid, "_CongComp", sep="")
      congNode <- ntplot.bucketizeByMonth(symbol, buckets)
      congNode$congNode <- congNode$value
  
      symbol <- "NEPOOL_SMD_DA_4000_CongComp"
      congHub <- ntplot.bucketizeByMonth(symbol, buckets)
      congHub$congHub <- congHub$value

      # merge them together in case you have shorter history!
      cong <- congNode[,c("bucket", "month", "congNode")]
      cong <- merge(cong, congHub[,c("bucket", "month", "congHub")])  
      cong$value <- cong$congNode - cong$congHub
      cong$component <- "congestion"
    
      res <- rbind(res, cong[,c("month", "bucket", "component", "value")])
    }

    if ("hubMark" %in% component){
      # get the LMP's
      symbol <- "NEPOOL_SMD_DA_4000_LMP"
      LMPHub <- ntplot.bucketizeByMonth(symbol, buckets)
      LMPHub$component <- "hubMark"
    
      res <- rbind(res, LMPHub[,c("month", "bucket", "component", "value")])
    }

    if ("nodeMark" %in% component){
      symbol <- paste("NEPOOL_SMD_DA_", ptid, "_LMP", sep="")
      LMPNode <- ntplot.bucketizeByMonth(symbol, buckets)
      LMPNode$component <- "nodeMark"
    
      res <- rbind(res, LMPNode[,c("month", "bucket", "component", "value")])
    }

    
  } else {
     # get them one by one ... slower
     symbol <- NULL
     if ("loss%" %in% component){
       symbol <- c(symbol,
         paste("NEPOOL_SMD_DA_", ptid, "_MargLoss", sep=""),
         "NEPOOL_SMD_DA_4000_MargLoss", "NEPOOL_SMD_DA_4000_EngyComp")
     }
     if ("congestion" %in% component){
       symbol <- c(symbol,
         paste("NEPOOL_SMD_DA_", ptid, "_CongComp", sep=""),
         "NEPOOL_SMD_DA_4000_CongComp")
     }
     if ("hubMark" %in% component) {
       symbol <- c(symbol, "NEPOOL_SMD_DA_4000_LMP")
     }
     if ("nodeMark" %in% component){
       symbol <- c(symbol,
         paste("NEPOOL_SMD_DA_", ptid, "_LMP", sep=""))
     }
 
     DD <- FTR:::FTR.load.tsdb.symbols(symbol, start.dt, end.dt)
     DD <- cbind(datetime=index(DD), data.frame(DD))
     # to be continued ...
     
     DD$day <- as.Date(format(DD$datetime-3600, "%Y-%m-%d")) # move HE just before midnight
     DD <- subset(DD, !(day %in% skipDates))
     DD$day <- NULL    # save some memory
     
     res <- vector("list", length=length(buckets))
     for (b in 1:length(buckets)){         # have to loop over buckets 
       BB <- secdb.getBucketMask("nepool", buckets[b], DD$datetime[1],
                                 DD$datetime[nrow(DD)])  
       names(BB) <- c("datetime", "inBucket")
       BB <- merge(BB, DD, by="datetime")
       BB <- BB[BB$inBucket,]
       if (nrow(BB)==0) next

       BB$month <- as.Date(format(BB$datetime-3600, "%Y-%m-01")) # move HE just before midnight

       cnames <- names(BB)
       if ("loss%" %in% component) {
         ind1 <- which(cnames %in% paste("NEPOOL_SMD_DA_", ptid, "_MargLoss", sep=""))
         ind2 <- which(cnames %in% "NEPOOL_SMD_DA_4000_MargLoss")
         ind3 <- which(cnames %in% "NEPOOL_SMD_DA_4000_EngyComp")
         BB$value <- (BB[,ind1]-BB[,ind2])/BB[,ind3]
         aux <- data.frame(cast(BB, month ~ ., mean, na.rm=FALSE))
         names(aux)[2] <- "value"
         aux$component <- "loss%"
         res[[b]] <- rbind(res[[b]], cbind(bucket=buckets[b], aux))
       }
       if ("loss$" %in% component) {
         ind1 <- which(cnames %in% paste("NEPOOL_SMD_DA_", ptid, "_MargLoss", sep=""))
         ind2 <- which(cnames %in% "NEPOOL_SMD_DA_4000_MargLoss")
         BB$value <- (BB[,ind1]-BB[,ind2])
         aux <- data.frame(cast(BB, month ~ ., mean, na.rm=FALSE))
         names(aux)[2] <- "value"
         aux$component <- "loss$"
         res[[b]] <- rbind(res[[b]], cbind(bucket=buckets[b], aux))
       }       
       if ("congestion" %in% component){
         ind1 <- which(cnames %in% paste("NEPOOL_SMD_DA_", ptid, "_CongComp", sep=""))
         ind2 <- which(cnames %in% "NEPOOL_SMD_DA_4000_CongComp")
         BB$value <- BB[,ind1]-BB[,ind2]
         aux <- data.frame(cast(BB, month ~ ., mean, na.rm=FALSE))
         names(aux)[2] <- "value"
         aux$component <- "congestion"
         res[[b]] <-  rbind(res[[b]], cbind(bucket=buckets[b], aux))
       }
       if ("hubMark" %in% component){
         ind1 <- which(cnames %in% "NEPOOL_SMD_DA_4000_LMP")
         BB$value <- BB[,ind1]
         aux <- data.frame(cast(BB, month ~ ., mean, na.rm=FALSE))
         names(aux)[2] <- "value"
         aux$component <- "hubMark"
         res[[b]] <-  rbind(res[[b]], cbind(bucket=buckets[b], aux))
       }
       if ("nodeMark" %in% component){
         ind1 <- which(cnames %in% paste("NEPOOL_SMD_DA_", ptid, "_LMP", sep=""))
         BB$value <- BB[,ind1]
         aux <- data.frame(cast(BB, month ~ ., mean, na.rm=FALSE))
         names(aux)[2] <- "value"
         aux$component <- "nodeMark"
         res[[b]] <-  rbind(res[[b]], cbind(bucket=buckets[b], aux))
       }
     }
     res <- do.call("rbind", res)
      

  }  # end of if/else is.null(skipDates)
  
  # if you are right at the beginning of a month, some of the
  # entries might be NaN, e.g. the 2x16H bucket.
  ind <- which(!is.finite(res$value))
  if (length(ind)>0)
    res <- res[!(res$month %in% unique(res$month[ind])),]
  
  
  return(res)
}


#########################################################################
# Get historical settles for the NYPP marks 
# Losses are in % relative to Hub Energy component
# Congestion is relative to Hub Congestion component
#
.getHistMark.NYPP <- function(ptid, buckets=c("5X16", "2X16H", "7X8"),
   MAP, refPtid=24008)
{
  if (is.null(MAP))
    stop("Please pass the NYPP MAP data.frame to PM:::.getHistMark.")

  cat("Using ", refPtid, " as reference ptid to ", ptid, ".\n", sep="")
  
  # get historical Losses
  symbol    <- MAP[which(MAP$ptid == ptid), "tsdb.symbol"]
  symbolRef <- MAP[which(MAP$ptid == refPtid), "tsdb.symbol"]
  if (length(symbol)==0)
    stop("The ptid", ptid, "does not have a tsdb.symbol in the NYPP$MAP!")
  sym <- gsub("_MCC_", "_MCL_", symbol)
  lossesNode <- ntplot.bucketizeByMonth(sym, buckets, region="NYPP")
  
  sym <- gsub("_MCC_", "_MCL_", symbolRef)
  lossesRef <- ntplot.bucketizeByMonth(sym, buckets)

  sym <- gsub("_MCC_", "_LBM_", symbolRef)
  energyRef <- ntplot.bucketizeByMonth(sym, buckets)

  losses <- lossesNode
  losses$value <- (lossesNode$value - lossesRef$value)/energyRef$value
  losses$component <- "loss%"
  
  # get historical Congestion
  congNode <- ntplot.bucketizeByMonth(symbol, buckets)
  congRef  <- ntplot.bucketizeByMonth(symbolRef, buckets)   

  cong <- congNode
  cong$value <- congNode$value - congRef$value
  cong$component <- "congestion"

  # get the LBMP's
  LMPRef <- energyRef
  LMPRef$component <- "hubMark"

  sym <- gsub("_MCC_", "_LBM_", symbol)
  LMPNode <- ntplot.bucketizeByMonth(sym, buckets)
  LMPNode$component <- "nodeMark"
  
  res <- rbind(losses[,c("month", "bucket", "component", "value")],
                 cong[,c("month", "bucket", "component", "value")],
               LMPRef[,c("month", "bucket", "component", "value")],
              LMPNode[,c("month", "bucket", "component", "value")])
  
  return(res)
}

#########################################################################
# Get the SecDb mark for this node and for the Hub. 
#
.getSecDbMark <- function(delivPt, asOfDate, startDate, endDate,
   buckets=c("5X16", "2X16H", "7X8"), region="NEPOOL", refDelivPt="Hub")
{
  commodity <- paste("COMMOD PWR ", region, " PHYSICAL", sep="")
  nodeMark  <- secdb.getElecPrices(asOfDate, startDate, endDate, commodity,
    delivPt, buckets)
  nodeMark$component <- "nodeMark"

  if ((toupper(region)=="NYPP") && (refDelivPt=="Hub")){
    stop("Change the refName!  It cannot be Hub for NYPP.")
  } else {
    cat("Using ", refDelivPt, " as reference for ", delivPt, ".\n", sep="")
  }
    
  hubMark <- secdb.getElecPrices(asOfDate, startDate, endDate, commodity,
    refDelivPt, buckets)
  hubMark$component <- "hubMark"

  res <- rbind(nodeMark[,c("month", "bucket", "component", "value")],
                hubMark[,c("month", "bucket", "component", "value")])
  res$source <- "SecDb"

  return(res)
}

#######################################################################
# return the LMP spread to Hub stacked for all the buckets you want
#
.get_hist_fwd_stacked <- function(ptid=4004, deliveryPoint="CT DA",
   endDate=as.Date("2015-12-01"), buckets=c("Peak", "Offpeak", "Flat"))
{
  
  marksForDate <- Sys.Date()
  asOfDate  <- as.Date(secdb.dateRuleApply(marksForDate, "-0b"))   
  startDate <- as.Date(secdb.dateRuleApply(marksForDate, "+1e+1d"))
  endDate   <- endDate   # mark up to here   
  toMonths  <- seq(startDate, endDate, by="1 month")

  histNodeMark <- PM:::.getHistMark(ptid, buckets=buckets)
  histSpread <- cast(histNodeMark, month + bucket ~ component, I, fill=NA,
    subset=component %in% c("hubMark", "nodeMark"))
  histSpread$spread <- histSpread$nodeMark - histSpread$hubMark
  histSpread$source <- "Historical"

  secDbNodeMark <- PM:::.getSecDbMark(deliveryPoint, asOfDate, startDate,
    endDate, buckets=buckets)
  fwdSpread <- cast(secDbNodeMark, month + bucket ~ component, I, fill=NA)
  fwdSpread$spread <- fwdSpread$nodeMark - fwdSpread$hubMark
  fwdSpread$source <- "Fwd"
  
  spreadLMP <- rbind(histSpread, fwdSpread)
  spreadLMP <- spreadLMP[order(spreadLMP$bucket, spreadLMP$month),]

  spreadLMP <- data.frame(spreadLMP)
  spreadLMP$bucket <- toupper(as.character(spreadLMP$bucket))
  
  return(spreadLMP)
}



#########################################################################
# Plot all the marks
#
plotMarks <- function(allMarks, nodeName,
  buckets=c("5X16", "2X16H", "7X8"), save=FALSE, hist=36,
  fwd=12, ...)
{
  if (!("package:lattice" %in% search()))
    require(lattice)

  if (missing(buckets))
    buckets <- unique(allMarks$bucket)

  firstMonth <- seq(as.Date(format(Sys.Date(), "%Y-%m-01")), by=paste("-",hist,
    " months", sep=""), length.out=2)[2]
  allMarks <- subset(allMarks, month >= firstMonth)

  if ("Historical" %in% unique(allMarks$source)) {
    maxMonth <- max(allMarks[which(allMarks$source=="Historical"), "month"])
  } else {
    maxMonth <- nextMonth()
  }
  maxMonth <- seq(maxMonth, by=paste(fwd, "months"), length.out=2)[2]

  allMarks$component <- as.factor(allMarks$component)
  allMarks$source <- as.factor(allMarks$source)
  
  # plot the congestion
  fileOut <- paste("Congestion_", nodeName, sep="") 
  if (!save) windows()
  else pdf(file=paste(fileOut,".pdf", sep=""), width=8.5, height=11)
#    win.metafile(file=paste(fileOut,".emf",sep=""), width=8.5, height=11)
  aux <- subset(allMarks, component=="congestion" &
                          bucket %in% buckets &
                          month <= maxMonth)
  aux$bucket <- factor(aux$bucket, levels=c("5X16", "2X16H", "7X8"))
  aux$source <- factor(as.character(aux$source))
    
  print(xyplot(value ~ month|bucket, data=aux, groups=source,
    panel=function(x,y,...){
      panel.abline(h=0, col="gray", lty=2)
      panel.xyplot(x,y,...)
    },           
    type="o", layout=c(1,length(buckets)), scales="free",
    ylab="Congestion, $/MWh",
    key=simpleKey(space="top", points=T, text=levels(aux$source),
               columns=length(levels(aux$source))), 
    ...))
  if (save){
    dev.off()
    cat("Saved", fileOut, "\n")
  }

  if ("Historical" %in% unique(allMarks$source)) {
    # plot the historical losses
    fileOut <- paste("LossesHist_", nodeName, ".pdf", sep="") 
    if (!save) windows(8.5, 4.5) else pdf(file=fileOut, width=8.5, height=4.5)
    aux <- subset(allMarks, component=="loss%" &
                  bucket %in% buckets &
                  month <= maxMonth &
                  source == "Historical")
    print(xyplot(100*value ~ month, data=aux, groups=bucket,
      panel=function(x,y,...){
        panel.abline(h=0, col="gray", lty=2)
        panel.xyplot(x,y,...)
      },           
      type="o",
      ylab="Loss factors, %",
      key=simpleKey(space="top", points=T, text=levels(aux$bucket),
                 columns=length(levels(aux$bucket))),  
      ...))
    if (save){
      dev.off()
      cat("Saved", fileOut, "\n")
    }
  }
  
  # plot the forward losses, only for 5x16 bucket
  fileOut <- paste("LossesFwd_", nodeName, ".pdf", sep="") 
  if (!save) windows(8.5, 4.5) else pdf(file=fileOut, width=8.5, height=4.5)
  aux <- subset(allMarks, component=="loss%" &
                bucket %in% "5X16" &
                month <= maxMonth)
 
  print(xyplot(100*value ~ month, data=aux, groups=source,
    panel=function(x,y,...){
      panel.abline(h=0, col="gray", lty=2)
      panel.xyplot(x,y,...)
    },           
    type="o",
    ylab="Loss factors 5x16, %",
    key=simpleKey(space="top", points=T, text=levels(aux$source),
               columns=length(levels(aux$source))),  
    ...))
  if (save){
    dev.off()
    cat("Saved", fileOut, "\n")
  }
  
  # plot of the LMP spread
  fileOut <- paste("LMP_spread_Hub_to_", nodeName, ".pdf", sep="") 
  if (!save) windows() else pdf(file=fileOut, width=8.5, height=11)
  aux <- subset(allMarks, component %in% c("nodeMark", "hubMark") &
                          bucket %in% buckets &
                          month <= maxMonth)
  spread <- cast(aux, month + bucket + source ~ component, I, fill=NA)
  spread$value <- spread$nodeMark - spread$hubMark
  spread$bucket <- factor(spread$bucket, levels=c("5X16", "2X16H", "7X8"))
    
  print(xyplot(value ~ month|bucket, data=spread, groups=source,
    panel=function(x,y,...){
      panel.abline(h=0, col="gray", lty=2)
      panel.xyplot(x,y,...)
    },           
    type="o", layout=c(1,length(buckets)), scales="free",
    ylab="Spread to Hub, $",
    key=simpleKey(space="top", points=T, text=levels(aux$source),
               columns=length(levels(aux$source))),   
    ...))
  if (save) {
    dev.off()
    cat("Saved", fileOut, "\n")
  }

  invisible(spread)
}


#########################################################################
# Get the output of the WeatherNormSperad.m as a molten data.frame. 
#
.readAllMatWNOutput <- function(fileMCC, fileMLC)
{
  MCC <- .readMatWNOutput(fileMCC)
  MCC$component <- "congestion"

  MLC <- .readMatWNOutput(fileMLC)
  MLC$component <- "loss%"

  rbind(MCC, MLC)
}

#########################################################################
# Read the output of the Matlab weathernormspreads.  This output is 5
# stacked tables (one table for each bucket). 
#
.readMatWNOutput <- function(filename, next13MonthsOnly=TRUE)
{
  if (!("package:xlsReadWrite" %in% search()))
    require(xlsReadWrite)

  # first sheet is the WN spread, as a table. 
  aux <- read.xls(filename, colNames=FALSE, sheet=1, type="character")
  dataWN   <- NULL
  noMonths <- nrow(aux)/5     # there are 5 buckets 
  for (b in 1:5)
    dataWN[[b]] <- aux[((b-1)*noMonths+1):(b*noMonths),]
  names(dataWN) <- sapply(dataWN, function(x)x[1,1])
  dataWN <- lapply(dataWN, function(x){
    matrix(as.numeric(x[2:nrow(x),2:ncol(x)]), ncol=ncol(x)-1,
      dimnames=list(x[2:nrow(x),1], x[1,2:ncol(x)]))
  })
  dataWN <- melt(dataWN)
  rownames(dataWN) <- NULL
  colnames(dataWN) <- c("month", "statistic", "value", "bucket")
  dataWN$month  <- as.Date(dataWN$month, origin="1899-12-30")
  dataWN$source <- "WeatherNorm"
  
  
  if (next13MonthsOnly){
    maxMonth <- seq(min(dataWN$month), by="13 months", length.out=2)[2]
    dataWN   <- subset(dataWN, month <= maxMonth)
   }
  
  dataWN$bucket <- toupper(dataWN$bucket) 
  
  dataWN
}

#########################################################################
# Make Matlab WN input xls
# fwdEndDate needs to be an EOM date.
.makeMatWNInput <- function(ptid, deliv.pt, histStartDate, histEndDate,
   fwdStartDate, fwdEndDate, filename, sheetname, ...)
{
  inWN <- data.frame(Region=c("'NEPOOL"), Market="'DA",
    Component=c("'MargLoss", "'CongComp"),
    HistSt  = format(histStartDate, "'%d%b%y"),
    HistEnd = format(histEndDate,   "'%d%b%y"),
    FwdSt   = format(fwdStartDate,  "'%d%b%y"),
    FwdEnd  = format(fwdEndDate,    "'%d%b%y"),                     
    Source="'4000", Sink=paste("'", ptid, sep=""),
    SecdbSource="Hub", SecDbSink=paste("'", deliv.pt, sep=""),
    Charts="'NO", Ratio=c("'YES", "'NO"), IgnoreFile="")

  colnames(inWN) <- c("'Region (NEPOOL/NYPP):", "'Market (DA/RT):",
    "'Component (L/C/M):", "'Hist St:", "'Hist End:",
    "'Fcst St:", "'Fcst End:", "'Source:", "'Sink:", "'SecdbSource:",
    "'SecdbSink:", "'Charts", "'Ratio", "'IgnoreFile")

  xlsWriteSheet(inWN, filename, sheetname, ...)
  rLog("Wrote the Matlab input file to:", filename)
}


#########################################################################
# Get the nodes marked overnight by WN
#
.getMatWNmarkedNodes <- function()
{
   cont <- secdb.getValueType("LIW: Nepool Spreads Data", "Contents")
   
   LD <- cont[["LocData"]]
   deliv.pts <- sort(unlist(sapply(LD, function(x){x[2]})))
  
   return(deliv.pts)
}

#########################################################################
# Forecast loss factors.  Use only for next 12 months as in 
# makePMview.  Where
#   hLF - historical loss factors.  You can get with 
#         subset(PM:::.getHistMark(ptid), component=="loss%")
#   fwdMonths - which months you want to forecast in the future
#
forecastLossFactors <- function(hLF, fwdMonths, method="AR")
{
  aux <- split(hLF, hLF$bucket)  # extend each bucket separately

  if (method == "AR"){
    # weigth of past 12 months 
    coeff <- c(3, 2, 1, 0.5, 0.5, 0.5, 1, 1, 1, 1, 2, 2)  
    coeff <- coeff/sum(coeff)
    fLF <- NULL                    # forecast
    for (b in 1:length(aux)){      # loop over the buckets
      fLF[[b]] <- data.frame(month=fwdMonths, bucket=names(aux)[b],
        component="loss%", value=PM:::.propagateFilter(aux[[b]]$value,
        coeff, periodsAhead=12)) 
    }
  }

  fLF <- do.call(rbind, fLF)
  rownames(fLF) <- NULL
  
  return(fLF)
}


#########################################################################
# Use the technology in the FTR package to come up with a forward view
# nodeType = c("LOAD", "GEN") 
# The PM view is for the next 12 months ONLY.  You need to extend it 
# yourself.
#
makePMview <- function(ptid, nodeType="LOAD", methodLoss="AR",
  noSims=10000, skipDates=NULL)
{
  # get historical settle prices for congestion relative to Hub
  endDate   <- as.Date(format(Sys.Date(), "%Y-%m-01"))-1  # end of current month
  startDate <- seq(endDate+1, by="-36 months", length.out=2)[2] # last 3 years
  symbol   <- paste("nepool_smd_da_", ptid,
    "_congcomp - nepool_smd_da_4000_congcomp", sep="")
  res <- ntplot.bucketizeByDay(symbol, c("2X16H", "7X8", "5X16"), startDate,
                               endDate)

  hSP <- data.frame(cast(res, day ~ bucket, I, fill=NA))
  hSP <- zoo(hSP[,-1], hSP[,1])
  hSP <- hSP[, c("X2X16H", "X7X8", "X5X16")]

  if (!is.null(skipDates))
    hSP <- hSP[!(index(hSP) %in% skipDates), ]

  # get the next 12 months
  months <- seq(as.Date(format(Sys.Date(), "%Y-%m-01")), by="1 month",
                length.out=13)[-1]
  # get the mean congestion by month, for the next 12 months
  res <- matrix(NA, ncol=3, nrow=12, dimnames=list(as.character(months),
                                       c("2X16H", "7X8", "5X16")))
  for (m in 1:12){
    auction <- FTR.AuctionTerm(months[m], months[m])
    options <- FTR.makeDefaultOptions(auction=auction, NEPOOL)
    sSP <- FTR.simulate.SP(hSP, options, noSims=noSims)
    # I'm interested in the price I will be selling.
    # If market is lower, I'll buy lower from market. 
    # if mean > median, put the mark at mean   - the case of CT
    # if mean < median, put the mark at median - the case of RI
    if (nodeType == "LOAD")
      res[m,] <- pmax(apply(sSP, 1, median), apply(sSP, 1, mean))
    if (nodeType == "GEN")
      res[m,] <- pmin(apply(sSP, 1, median), apply(sSP, 1, mean))
      
  }
  res <- melt(res)
  names(res)[1:2] <- c("month", "bucket")
  res$bucket <- as.character(res$bucket)
  res$month <- as.Date(as.character(res$month))
  res$component <- "congestion"

  # forecast losses for next 12 months
  hLF <- PM:::.getHistMark(ptid, component="loss%", skipDates=skipDates)
  fLF <- forecastLossFactors(hLF, months, method=methodLoss)
  
  # put the congestion and the loss% together
  pmView <- rbind(res, fLF)
  pmView$source <- "PM"
  
  return(pmView)
}


#########################################################################
# compare historicals with forward
# Where spread is as returned by plotMarks.
#
## stack_hist_fwd <- function(spread, buckets=c("5X16", "7X8", "2X16H"))
## {
##   hist <- cast(spread, month ~ bucket, I, fill=NA,
##                subset=source=="Historical")
##   fwd <- cast(spread, month ~ bucket, I, fill=NA,
##                subset=source=="pmView")

##   return(rbind(hist, fwd))
## }
                       





##   hux <- split(aux, aux$bucket)  # historical 
##   coeff <- c(3, 2, 1, 0.5, 0.5, 0.5, 1, 1, 1, 1, 2, 2)
## #  coeff <- c(1,1)
##   coeff <- coeff/sum(coeff)
##   fux <- NULL                    # forecast
##   for (b in 1:length(hux)){      # loop over the buckets
##     fux[[b]] <- data.frame(month=months, bucket=names(hux)[b], component="loss%",
##       value=PM:::.propagateFilter(hux[[b]]$value, coeff, periodsAhead=12)) 
##   }
##   aux <- do.call(rbind, fux)
##   rownames(aux) <- NULL



