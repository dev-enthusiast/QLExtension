##################################################################
# Make the marks relative to zone by substracting the zonal marks
# X must have c(delivPt, month, bucket, value),
# where value is the nodeMark. 
#
spreadToParent <- function(X, asOfDate=Sys.Date(),
  maxMonth=as.Date("2017-12-01"), region="NEPOOL", MAP_DP=NULL)
{
  if (region != "NEPOOL")
    stop("Other regions not supported yet.")

  if (is.null(MAP_DP))
    load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/MAP_DP.RData")
  
  cnames <- c("delivPt", "month", "bucket", "value")
  if (any(!(cnames %in% names(X))))
    stop("Missing bucket, month, delivPt or value in the colums of X.")

  if (class(X$bucket)!="character")
    X$bucket <- as.character(X$bucket)
  if (class(X$delivPt)!="character")
    X$delivPt <- as.character(X$delivPt)
    
  if (!("parentDelivPt" %in% names(X))){
    rDebug("I'll get the parentDelivPt from SecDb.")
    parents <- subset(MAP_DP, delivPt %in% unique(X$delivPt))
    X <- X[,cnames]
    X <- merge(X, parents[,c("delivPt", "parentDelivPt")], all.x=TRUE)
  } else {
    rDebug("Using the parentDeliveryPt that you specified in X input.")
    X$parentDelivPt <- as.character(X$parentDelivPt)  # problems later if factor!
  }
  cnames  <- c(cnames, "parentDelivPt")
  X <- X[,cnames]
  
  # get the parent marks
  location     <- unique(X$parentDelivPt)
  startDate    <- as.Date(secdb.dateRuleApply(asOfDate, "+1e+1d"))
  endDate      <- maxMonth   # mark up to here
  parentBucket <- unique(X$bucket) 
  res <- secdb.getElecPrices(asOfDate, startDate, endDate,
    "COMMOD PWR NEPOOL PHYSICAL", location, parentBucket)
  res <- res[,c(2,3,5,6)]
  names(res) <- c("parentDelivPt", "bucket", "month", "parentMark")

  # put together the child marks with the parent marks
  res <- merge(X, res, all.x=TRUE)
  res$childMark <- res$value
  
  # Sometimes, for long tems, the parent is Inf.  Keep the same spread 
  # between parent and Hub to calculate the node/parent spread. 
  if (any(res$parentMark ==Inf)){
    ind <- res$parentMark ==Inf
    fromMth <- min(res$month[ind])
    if (as.numeric(fromMth - asOfDate) > 10*365){
      rLog("--- Missing parent marks from", as.character(fromMth))
      rLog("Extend with last 12mth of parent/hub spread.")
      spreadOverHub <- res[,c("month", "bucket", "delivPt", "childMark")]
      names(spreadOverHub)[3:4] <- c("delivPt", "value")
      spreadOverHub$parentDelivPt <- "HUB"
      spreadOverHub <- spreadToParent(spreadOverHub, maxMonth=max(res$month))
      names(spreadOverHub)[which(names(spreadOverHub)=="parentMark")] <- "HUB"
      res <- merge(res, spreadOverHub[,c("month", "bucket", "HUB")])
      res$parentOverHub <- res$parentMark - res$HUB
    
      MM <- res[!ind, c("month", "bucket", "parentDelivPt", "delivPt", "parentOverHub")]
      names(MM)[ncol(MM)] <- "value"
      MMext <- PM:::.extendPeriodically(MM, sort(unique(res$month)))
      names(MMext)[ncol(MMext)] <- "parentOverHubExt"
      res <- merge(res, MMext, all.x=TRUE)
      res$parentOverHub <- res$parentOverHubExt
      res$parentOverHubExt <- NULL
      res$parentMark <- res$HUB + res$parentOverHub
      res$HUB <- res$parentOverHub <- NULL
    }
  }

  res$value <- res$childMark - res$parentMark
  
  return(res)
}
