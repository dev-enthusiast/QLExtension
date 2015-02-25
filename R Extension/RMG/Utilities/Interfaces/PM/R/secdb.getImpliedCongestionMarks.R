# Given the SecDb mark, substract historical losses to get the  
# implied congestion mark.
#
# Written by Adrian Dragulescu on 22-Oct-2010

###########################################################################
# Average loss component for the last 12 months for ONE ptid.
# NEPOOL only!
#
.get.historical.lossFactors <- function(ptid, last=12, asOfDate=Sys.Date(),
   buckets=c("PEAK", "OFFPEAK"))
{
  # get historical Losses
  symbol <- paste("NEPOOL_SMD_DA_", ptid, "_MargLoss", sep="")
  lossesNode <- ntplot.bucketizeByMonth(symbol, buckets)
  lossesNode$lossesNode <- lossesNode$value
  
  symbol <- "NEPOOL_SMD_DA_4000_MargLoss"
  lossesHub <- ntplot.bucketizeByMonth(symbol, buckets)
  lossesHub$lossesHub <- lossesHub$value

  symbol <- "NEPOOL_SMD_DA_4000_EngyComp"
  energyHub <- ntplot.bucketizeByMonth(symbol, buckets)
  energyHub$energyHub <- energyHub$value

  # merge them together in case you have shorter history!
  losses <- lossesNode[,c("bucket", "month", "lossesNode")]
  losses <- merge(losses, lossesHub[,c("bucket", "month", "lossesHub")])
  losses <- merge(losses, energyHub[,c("bucket", "month", "energyHub")])
  losses$value <- (losses$lossesNode - losses$lossesHub)/losses$energyHub
  losses$component <- "loss%"

  # pick the last 12 months and avg loss factors ...
  maxMon  <- min(max(losses$month), asOfDate)
  hMonths <- seq(maxMon, by="-1 month", length.out=last)
  losses  <- subset(losses, month %in% hMonths)

  avgLosses <- cast(losses, bucket ~ ., mean, na.rm=TRUE )
  colnames(avgLosses)[2] <- "lossFactor"
  avgLosses$ptid <- ptid

  return(data.frame(avgLosses))  
}

secdb.getImpliedCongestionMarks <- function(asOfDate=Sys.Date(),
  startDate=Sys.Date(), endDate=as.Date("2017-12-01"), region="NEPOOL",
  delivPt=c("CT", "SEMA", "RI", "Maine", "VT", "WMA", "NH", "Nema", "Hub"),
  bucket=c("PEAK", "OFFPEAK"), ptid=structure(4000:4008, names=c("HUB",
  "MAINE", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA")))
{
  if (region != "NEPOOL")
    stop('Only region "NEPOOL" is currently implemented.')
  delivPt <- toupper(delivPt)

  if (any(!(names(ptid) %in% delivPt)))
    stop("names(ptid) should be the delivPt!")

  if (length(delivPt)==1)
    stop("Add the HUB to the delivPt and ptid!")  

  commodity <- paste("COMMOD PWR", region, "PHYSICAL")
  
  curveData <- secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, delivPt, bucket )
  names(curveData)[which(names(curveData)=="date")]     <- "month"
  names(curveData)[which(names(curveData)=="location")] <- "delivPt"
  curveData <- curveData[,c("delivPt", "month", "bucket", "value")]

  curveData <- subset(curveData, value != Inf) # remove expired months

  aux <- cast(curveData, month + bucket ~ delivPt, mean, fill=NA)
#browser()
  # get the full basis by substracting the Hub price
  indHub <- which(names(aux)=="HUB")
  Hub <- aux[,indHub]
  aux <- aux[,-indHub]
  aux[,3:ncol(aux)] <- aux[,3:ncol(aux)]-Hub
  res <- cbind(aux[,1:2], HUB=Hub, aux[3:ncol(aux)])
  BB  <- melt(res, id=1:2)
  colnames(BB)[3:4] <- c("delivPt", "basisMark")
  
  # get the recent loss factors by zone 
  lossFactor <- NULL
  for (p in ptid){
    rLog("Getting historical loss factors for ptid", p)
    lossFactor[[as.character(p)]] <- .get.historical.lossFactors(p,
       last=12, asOfDate=asOfDate, buckets=bucket)
  }
  lossFactor <- do.call(rbind, lossFactor)
  rownames(lossFactor) <- NULL

  # add delivPt
  lossFactor <- merge(lossFactor, data.frame(ptid=ptid, delivPt=names(ptid)))

  # get the loss marks!
  LL <- merge(res[,c("month", "bucket", "HUB")],
              lossFactor[,c("bucket", "lossFactor", "delivPt")], all=TRUE)
  LL$lossMark <- LL$lossFactor * LL$HUB
  
  LL <- merge(BB, LL[,c("month", "bucket", "delivPt", "lossMark")])
  LL$congMark <- LL$basisMark - LL$lossMark
  LL$congMark[LL$delivPt=="HUB"] <- 0
  
  return(LL)
}
