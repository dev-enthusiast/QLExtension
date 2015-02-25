##################################################################
# Make the marks relative to zone by substracting the zonal marks
# X must have c(ptid, month, bucket, value)
#
spreadToZone <- function(X, marksForDate=Sys.Date(),
  maxMonth=as.Date("2017-12-01"), region="NEPOOL")
{

  rLog("/n#########################################################")
  rLog("This function has been superseeded by spreadToParent.  Stop using it.")
  rLog("/n#########################################################")
  
  if (region != "NEPOOL")
    stop("Other regions not supported yet.")
 
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")
  
  # mark the DA only 
  aux <- subset(MAP, ptid %in% unique(X$ptid))[,c("ptid",
    "deliv.pt", "zone.id")]
  aux$deliv.pt <- paste(aux$deliv.pt, "DA")  
  Xz <- merge(X, aux, by="ptid", all.x=TRUE)
  names(Xz)[which(names(Xz)=="value")] <- "node.value"

  uZones   <- as.numeric(unique(Xz$zone.id))
  MAPzones <- subset(MAP, ptid %in% uZones)[,c("ptid", "deliv.pt")]
  
  location <- MAPzones$deliv.pt
  bucket   <- c("5X16", "2X16H", "7X8")

  asOfDate  <- as.Date(secdb.dateRuleApply(marksForDate, "-1b"))
  startDate <- as.Date(secdb.dateRuleApply(marksForDate, "+1e+1d"))
  endDate   <- maxMonth   # mark up to here   
  res <- secdb.getElecPrices(asOfDate, startDate, endDate,
    "COMMOD PWR NEPOOL PHYSICAL", location, bucket)
  res <- merge(res, MAPzones, all.x=TRUE, by.x="location",
               by.y="deliv.pt")
  names(res)[which(names(res)=="value")] <- "zone.value"
  names(res)[which(names(res)=="ptid")]  <- "zone.id"
  
  Xz <- merge(Xz, res[,c("bucket", "month", "zone.id", "zone.value")],
    by=c("bucket", "month", "zone.id"))
  Xz$value <- Xz$node.value - Xz$zone.value

  return(Xz)
}
