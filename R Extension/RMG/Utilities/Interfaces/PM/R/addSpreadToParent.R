##################################################################
# Add a spread to a parent mark. 
# parentMark is a data.frame with columns:
#   c("delivPt", "bucket", "month", "value")
#
addSpreadToParent <- function(sprdToParent, parentMark,
  asOfDate=Sys.Date(), matrix=TRUE)
{
  if (any(!c("delivPt", "parentDelivPt", "bucket", "month",
    "value") %in% names(sprdToParent)))
    stop("Missing one of delivPt, parentDelivPt, bucket, month,
      value columns from sprdToParent data.frame.")

  if (missing(parentMark)){
    locations <- unique(sprdToParent$parentDelivPt)
    startDate <- as.Date(secdb.dateRuleApply(asOfDate, "+1e+1d"))
    maxMonth  <- max(sprdToParent$month)
    parentMark  <- secdb.getElecPrices(asOfDate, startDate, maxMonth,
      commodity="COMMOD PWR NEPOOL PHYSICAL", location=locations, 
      bucket=c("2X16H", "5X16", "7X8"))
    names(parentMark)[c(2,5,6)] <- c("parentDelivPt", "month", "parentMark")
  } else {
    # check that you've got the right columns ...
    if (any(!c("parentDelivPt", "bucket", "month", "parentMark")
      %in% names(parentMark)))
      stop("Missing one of parentDelivPt, bucket, month, parentMark 
        columns from parentMark data.frame.")
  }
  
  res <- merge(sprdToParent[, c("delivPt", "parentDelivPt", "bucket",
    "month", "value")], parentMark[, c("parentDelivPt", "bucket", "month",
    "parentMark")])

  res$addedValue <- res$value
  res$value <- res$parentMark + res$value

  ind <- which(is.na(res$value))
  if (length(ind)>0){
    rLog("The rows below have NA price for parent or child.  Please check.")
    print(res[ind, ])
    stop("Stop and fix it.")
  }

  if (matrix){
    res <- cast(res, month ~ bucket + delivPt, I, fill=NA)
    ind <- c(1, order(sub(".*_", "", names(res)[-1]))+1)
    res <- res[,ind]
    rnames <- format(res[,1], "%m/%d/%Y")
    res <- res[,-1]
    rownames(res) <- rnames
    cnames <- names(res)
    res <- as.matrix(data.frame(res))
    colnames(res) <- cnames
  }
  
  return(res)
}
  


