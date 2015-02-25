# A function to classify a set of curvenames. 
# The proper classification should come from somewhere else, maybe a FOIT SOX 
# db table.  This is just a quick hand.
#
# The location column is not meant to be the same as the delivPt used in the
# elec locations.
#

classifyCurveName <- function(curvenames, by=c("bucket", "peak/offpeak",
  "location", "market", "service"))
{
  curvenames <- toupper(curvenames)  # just to be sure
  MM <- data.frame(pos=1:length(curvenames), curve=curvenames)
  
  uCurves    <- unique(curvenames)
  uMM <- data.frame(curve=uCurves)

  sCurves <- strsplit(uCurves, " ")

  if ("peak/offpeak" %in% by){
    uMM$pkoffpk <- NA
    uMM$pkoffpk[grep(" 2X16.* ", uMM$curve)]   <- "OFFPEAK"
    uMM$pkoffpk[grep(" 7X8 ", uMM$curve)]      <- "OFFPEAK"
    uMM$pkoffpk[grep(" 5X16 ", uMM$curve)]     <- "PEAK"
    uMM$pkoffpk[grep(" 7X24 ", uMM$curve)]     <- "FLAT"
    uMM$pkoffpk[grep(" PWX UCAP ", uMM$curve)] <- "FLAT"
    
    ind <- regexpr("^COMMOD PW", uMM$curve) != 1
    uMM$pkoffpk[ind] <- "FLAT"
  }

  
  if ("bucket" %in% by){
    uMM$bucket <- NA
    bucket <- sapply(sCurves, "[[", 3)
    
    ind <- grep("[[:digit:]]X.*", bucket)
    uMM$bucket[ind] <- bucket[ind]

    ind <- regexpr("^COMMOD PW", uMM$curve) != 1
    uMM$bucket[ind] <- "7X24"

    ind <- regexpr("^COMMOD PWX UCAP", uMM$curve) == 1
    uMM$bucket[ind] <- "7X24"
    
    ind <- regexpr("^COMMOD PWX 2X16 PHYSICAL", uMM$curve) == 1
    uMM$bucket[ind] <- "2X16H"
  }

  
  if ("market" %in% by) {
    uMM$market <- sapply(sCurves, "[[", 2)
  }

  
  if ("location" %in% by){
    location <- sapply(sCurves, function(x){x[min(4,length(x))]})
    ind <- (regexpr("^COMMOD PWX", uMM$curve)==1) & (location == "PHYSICAL")
    location[ind] <- "HUB DA"  # was MAHUB, better to use HUB for SecDb tieouts

    ind <- (regexpr("^COMMOD (NG|CO2)", uMM$curve)==1)
    location[ind] <- sapply(sCurves[ind], function(x){x[3]})

    ind <- (regexpr("^COMMOD HO", uMM$curve)==1)  # this may be a good default!
    location[ind] <- sapply(sCurves[ind], function(x){paste(x[3:length(x)], collapse=" ")})


    location[grep(" BOS PHYSICAL", uMM$curve)] <- "BOSTON"
    
    
    uMM$location <- unlist(location)
  }

  if ("service" %in% by) {
    uMM$service <- "ENERGY"
    
    uMM$service[(regexpr(" UCAP ", uMM$curve)>0)] <- "UCAP"
    uMM$service[(regexpr(" ISOFEE ", uMM$curve)>0)] <- "ISOFEE"
    uMM$service[(regexpr(" VLRCST ", uMM$curve)>0)] <- "VLRCST"
  }
  

  # put them together
  MM <- merge(MM, uMM, by="curve", all.x=TRUE, sort=FALSE)
  MM <- MM[order(MM$pos),]  # reorder them
  MM$pos <- NULL

  return(MM)
}


.test.classifyCurveName <- function()
{
  curvenames <- c("COMMOD NG KLEEN PHYSICAL", "COMMOD NG ALGCG FERC",
                  "COMMOD NG ALGCG GAS-DLY MEAN", "COMMOD HO NYH CFOB PLTM",
                  "COMMOD PWX 5X16 BOS PHYSICAL")
  classifyCurveName(unique(curvenames))


  

}
