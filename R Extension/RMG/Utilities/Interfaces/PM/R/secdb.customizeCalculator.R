## Why we're using this function?  I asked Wang the message below.
##
## Can you commit this script somewhere in SecDb?  Your user area is OK.
## I cannot commit any secdb scripts anymore.  If you can do it soon, it
## would be great.  Let’s put it in your user area first, and once it
## stabilizes we can move it to _lib RMG SecDB Bridge.
##
## I have to move some trades from CNE retail portfolio to PM manage the
## congestion, so I want to automate it as much as possible.  I ran into
## a weird issue in _lib commod calc utils, @calcutils::customize
## specific.  There is this test If( !IsError( Double( GsDtToDtValue(
## delPt )))), where delPt is the delivery point (which should be a
## string).  I cannot pass GsDt from R, so the code errors out here.
## Silly.  So I want to make a wrapper that casts the string delPt into
## GsDt in SecDb.  Something like this…
##
## see: example: excelsecdb elec load

secdb.customizeCalculator <- function(fullCalculatorName,
  quantity=NULL, fixPrice=NULL,
  recalculate=TRUE, comments=NULL)
{
  if (!is.null(quantity) & !is.null(fixPrice)){
    if (!(identical(colnames(quantity), colnames(fixPrice))))
      stop("Mismatch between quantity and fixPrice colnames!")
  }

  calculatorName <- paste(secdb.getValueType(fullCalculatorName, "Calc Prefix"),
                          secdb.getValueType(fullCalculatorName, "Description"))
  if (nchar(calculatorName)==0)
    stop("Check the fullCalculatorName.  It is probably wrong.")
  
  trader  <- secdb.getValueType(fullCalculatorName, "Trader")
  group   <- secdb.getValueType(fullCalculatorName, "Group")

  # always clear the custom details before writing
  lib  <- "_lib commod calc utils"
  func <- "CalcUtils::ClearCustomDetails"
  s <- secdb.getValueType(fullCalculatorName, "Calc Security")
  res  <- secdb.invoke(lib, func, s)  # works now, may work with fullCalculatorName too  
  rLog("Cleared custom details")
  
  # always change start/end date of a calculator to match the inputs 
  func <- "CalcUtils::CustomizeStartEnd"
  startDt <- format(quantity$month[1], "%d%b%y")
  endDt   <- format(seq(quantity$month[nrow(quantity)], by="1 month",
                        length.out=2)[2]-1, "%d%b%y")
  res <- secdb.invoke(lib, func, fullCalculatorName, startDt, endDt)
  if (res != "Successful update"){
    stop("Cannot set calculator Start/End date.")
  } else {
    rLog("Successfuly changed start/end date.")
  }
  
  calcSec <- secdb.getValueType(fullCalculatorName, "Calc Security")
  aux <- toupper(strsplit(calcSec, "_")[[1]][1])
  serviceType <- if (aux == "FUEL") {
    "Fuel"
  } else if (aux == "ELPHY") {
    "Energy"
  } else {
    stop("Cannot determine service type for this calculator!")
  }
  fsi     <- secdb.getValueType(calcSec, "Float Security Info")
  startdt <- as.Date(secdb.convertTime(sapply(secdb.getValueType(calcSec,
    "Date Specs"), "[[", "Start Date")))

  fsiDelivPt <- toupper(sapply(fsi, "[[", "Delivery Point"))
  fsiBucket  <- toupper(sapply(fsi, "[[", "Quantity Bucket Type"))
  noFixed <- length(secdb.getValueType(calcSec, "Fixed Security Info"))
  noFloat <- length(fsi)
  
  cat("Customizing: ", calculatorName, "\nTrader:      ", trader,
    "\nGroup:       ", group, "\n\n")

  if (noFixed != noFloat)
    stop(paste("Calculator has ", noFixed, " fix legs and ",
               noFloat, " floating legs!", sep=""))
  
  lib  <- "_LIB R Bridge"
  func <- "R Bridge::CustomizeCalc"
    
  # customize the quantity
  if (!is.null(quantity)){
    if (any(quantity$month != startdt))
      stop("The dates in quantity don't align with the dates in the calculator!")
    serviceType   <- serviceType            
    customize     <- "Quantity"          
    cnames  <- colnames(quantity)[-1]
    buckets <- gsub(".*_(.*)$", "\\1", cnames)
    deliveryPoints <- gsub("(.*)_(.*)$", "\\1", cnames)

    data  <- vector("list", length=nrow(quantity))
    for (r in 2:ncol(quantity))        
    {
      deliveryPoint <- deliveryPoints[r-1] 
      loadType      <- buckets[r-1]
      for (d in 1:length(data))
        data[[d]] <- list(quantity$month[d], quantity[d,r])
      cat("Inserting custom quantities for delivery point: ", deliveryPoint,
          " and bucket: ", loadType, "\n", sep="")

      res <- secdb.invoke(lib, func, group, trader, calculatorName,
        serviceType, deliveryPoint, loadType, customize, data, recalculate)
      if (res != "Succesful update")
        stop("Failed to update quantity for", colnames(quantity)[r], "\n")
    }
  }

  # customize the Fix Price
  if (!is.null(fixPrice)){
    if (any(fixPrice$month != startdt))
      stop("The dates in quantity don't align with the dates in the calculator!")
    serviceType <- "Price"
    customize   <- "Fixed Price"

    data  <- vector("list", length=nrow(fixPrice))
    for (r in 1:length(fsiBucket))  # loop over calculator rows        
    {
      thisCol <- paste(fsiDelivPt[r], fsiBucket[r], sep="_")
      indC <- which(colnames(fixPrice) == thisCol)
      if (length(indC) != 1)
        stop("Cannot find ", thisCol, " in colnames of fixPrice.\n", sep="")
      for (d in 1:length(data))
        data[[d]] <- list(fixPrice$month[d], fixPrice[d,indC])
      cat("Inserting custom fixPrice for delivery point:", fsiDelivPt[r],
          " and bucket: ", fsiBucket[r], "\n", sep="")

     loadType <- r
     #browser() 
     res <- secdb.invoke(lib, func, group, trader, calculatorName,
        serviceType, deliveryPoint, loadType, customize, data, recalculate)
      if (res != "Succesful update")
        stop("Failed to update Fix Price for", colnames(quantity)[r], "\n")
    }
  }

  if (!is.null(comments)) {
    lib <- "_lib elec load parse addin fns"
    func <- "LoadParse::CalcComment"
    secdb.invoke(lib, func, fullCalculatorName, comments)  
  }
   
}












## require(SecDb)
## Sys.setenv(tz="")
## cnames <- calculator.list("CPS Trading Group", "E47187")

## ##   lib  <- "_lib commod calc utils"
## ##   func <- "CalcUtils::Find"
## ##   secdb.invoke(lib, func, group, trader, cnames[1])



## group          <- "CPS Trading Group"
## trader         <- "E47187"
## calculatorName <- "ECFD Spreads Retail AAD"
## deliveryPoint  <- "CT DA"

## serviceType   <- "Price"                # or "Energy" for quantity
## loadType      <- 1                      # or "5X16", "2X16H", "7X8" for quantity
## customize     <- "Fixed Price"          # or "Quantity", or "Price"

## serviceType   <- "Energy"            
## loadType      <- "Peak"                 # or "2X16H", "7X8" for quantity
## customize     <- "Quantity"          

## recalculate   <- TRUE

## dates <- seq(as.Date("2009-06-01"), as.Date("2013-12-01"), by="month")
## data  <- vector("list", length=55)
## for (d in 1:length(data))
##   data[[d]] <- list(dates[d], d)








# fullCalcName = "Calc ECFD Spreads Retail AA   0"
# calcSecurity <- secdb.getValueType(fullCalcName, "Calc Security")
# secInfo <- secdb.getValueType(calcSecurity, "Security Info")
# cd <- secdb.getValueType(calcSecurity, "Custom Data")



## secdb.calculatorGetCustomDetails <- function(fullCalcName)
## {

##   lib  <- "_lib commod calc utils"
##   func <- "CalcUtils::CustomDetails"
  
##   calcSecurity <- secdb.getValueType(fullCalcName, "Calc Security")
##   res <- secdb.invoke(lib, func, calcSecurity)

##   return(res)
## }


## code <- paste('sec=GetSecurity("',calcSecurity,'", SDB_REFRESH_CACHE); setValue(Start Month(sec), "F10"); UpdateSecurity(sec);', sep="")
## secdb.evaluateSlang(code)
