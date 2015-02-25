# Do a reconciliation between ISO reports and CNE cap tags. 
# Spit a report out.
#
# .get.monthly.ucap.obligation
# .get.daily.zonal.ucap.peak.obligation
# .write.xlsx(filename, month, ucap.iso.zone, cap.secdb)
# run_report
#
# History:
#  - 2010-12-15: there was a problem with ME data we spotted.  Abizar fixed it
#    going forward, so with an asOfDate of 2010-12-31, things look good.
#


##############################################################################
.format.FCMCLOSTLDTL <- function(ucap.iso){
  colnames(ucap.iso) <- tolower(colnames(ucap.iso))
  ucap.iso[,c(3,7)] <- sapply(ucap.iso[,c(3,7)], as.numeric)
  ucap.iso$date <- as.Date(ucap.iso$trading.date, "%m/%d/%Y")
  ucap.iso$daily.ucap <- ucap.iso$customer.share.peak.contributions
  
  ucap.iso
}

.format.MTHSTLICAP <- function(ucap.iso){
  colnames(ucap.iso) <- tolower(colnames(ucap.iso))
  ucap.iso[,c(3,5)] <- sapply(ucap.iso[,c(3,5)], as.numeric)
  ucap.iso$date <- as.Date(ucap.iso$date, "%m/%d/%Y")
  
  ucap.iso$daily.ucap <- ucap.iso$daily.ucap.peak.contribution.value
  ucap.iso
}


##############################################################################
# Aggregate by zone the ucap peak contribution of each asset.
# just a merge
.get.daily.zonal.ucap.peak.obligation <- function(ucap.iso, assetId)
{
  ucap.iso <- merge(
    ucap.iso[,c("date", "asset.id", "daily.ucap")],
    assetId, by="asset.id", all.x=TRUE)
  if (any(is.na(ucap.iso$zone))){
    rLog("You have unmapped Asset Ids!",
      unique(ucap.iso$asset.id[is.na(ucap.iso$zone)]))
    stop("Fix it!")
  }
  ucap.iso <- ucap.iso[,c("asset.id", "zone", "date", "daily.ucap")]
  names(ucap.iso)[4] <- "ucap.iso"
  
  ucap.iso.zone <- cast(ucap.iso, zone + date ~ ., sum, fill=NA, value="ucap.iso")
  names(ucap.iso.zone)[3] <- "ucap.iso" 
##   ucap.iso.zone <- cast(ucap.iso.zone, zone ~ ., mean, fill=NA, value="ucap.iso")
##   names(ucap.iso.zone)[2] <- "ucap.iso" 
  
  return(ucap.iso.zone)  
}


##############################################################################
# write results to xlsx
#
.write_results <- function(res, filename)
{
  res$diff <- res$ucap.secdb - res$ucap.iso
  
  wb <- createWorkbook()
  sheet <- createSheet(wb, "Sheet1")
  setZoom(sheet, 80, 100)
  rows  <- createRow(sheet, rowIndex=1:40)
  cells <- createCell(rows, colIndex=1:30)
  dateFormat <- CellStyle(wb) + DataFormat("m/d/yyyy")
  cs.fill <- CellStyle(wb) + Fill(backgroundColor="skyblue",
    foregroundColor="skyblue", pattern="SOLID_FOREGROUND")
  
  # write the ISO numbers
  aux <- cast(res, date ~ zone, I, fill=NA, value="ucap.iso")  
  rOffset <- 3
  mapply(setCellValue, cells[rOffset,1:ncol(aux)], colnames(aux))  # colnames
  for (ic in 1:ncol(aux))
    mapply(setCellValue, cells[(1:nrow(aux))+rOffset,ic], aux[,ic])
  lapply(cells[(1:nrow(aux))+rOffset,1], setCellStyle, dateFormat)           
  addMergedRegion(sheet, 1, 1, 1, ncol(aux))
  setCellValue(cells[[1,1]], "Daily UCAP Peak Contribution Value (ISO)")
  setCellStyle(cells[[1,1]], cs.fill)
  autoSizeColumn(sheet, 1)
  
  # write the SecDb numbers
  aux <- cast(res, date ~ zone, I, fill=NA, value="ucap.secdb")  
  rOffset <- 3; cOffset <- 9
  mapply(setCellValue, cells[rOffset,(1:ncol(aux))+cOffset], colnames(aux))  # colnames
  for (ic in 1:ncol(aux))
    mapply(setCellValue, cells[(1:nrow(aux))+rOffset,ic+cOffset], aux[,ic])
  lapply(cells[(1:nrow(aux))+rOffset,1+cOffset], setCellStyle, dateFormat)           
  addMergedRegion(sheet, 1, 1, 10, 9+ncol(aux)) 
  setCellValue(cells[[1,10]], "Cap tags from CER data warehouse")
  setCellStyle(cells[[1,10]], cs.fill)
  autoSizeColumn(sheet, 10)
  
  # write the diff numbers
  aux <- cast(res, date ~ zone, I, fill=NA, value="diff")  
  rOffset <- 3; cOffset <- 18
  mapply(setCellValue, cells[rOffset,(1:ncol(aux))+cOffset], colnames(aux))  # colnames
  for (ic in 1:ncol(aux))
    mapply(setCellValue, cells[(1:nrow(aux))+rOffset,ic+cOffset], aux[,ic])
  lapply(cells[(1:nrow(aux))+rOffset,1+cOffset], setCellStyle, dateFormat)           
  addMergedRegion(sheet, 1, 1, 19, 18+ncol(aux)) 
  setCellValue(cells[[1,19]], "Diff: SecDb - ISO")
  setCellStyle(cells[[1,19]], cs.fill)
  autoSizeColumn(sheet, 19)
  
  saveWorkbook(wb, file=filename)
  rLog("Wrote file: ", filename)
}


##############################################################################
# Do the report -- what's called by procmon
#
run_report <- function(month)
{
  DIR_OUT <- "S:/All/Structured Risk/NEPOOL/ConstellationNewEnergy/Capacity/ISO.Reconciliation/"

  ZZ <<- data.frame(ptid=c(4000:4008), zone=c("HUB", "ME", "NH", "VT",
    "CT", "RI", "SEMA", "WCMA", "NEMA"), locationName=c(".H.INTERNAL_HUB",
    ".Z.MAINE", ".Z.NEWHAMPSHIRE", ".Z.VERMONT", ".Z.CONNECTICUT",
    ".Z.RHODEISLAND", ".Z.SEMASS", ".Z.WCMASS", ".Z.NEMASSBOST"),
    location=c("HUB", "MAINE", "NH", "VT", "CT", "RI", "SEMA", "WMA",
      "NEMA"))

  # you should get this from a recent load settlement report, it is dynamic
  rLog("Read the map of asset.id to zone. From Eric Riopko")
  file.map <- paste("S:/All/Structured Risk/NEPOOL/ConstellationNewEnergy",
     "/CNE NEPOOL Asset ID's.xlsx", sep="")
  assetIdMap <- read.xlsx(file.map, 1)


  rLog("Get the booked capacity from CERDW as booked from SecDb")
  asOfDate <- as.Date(secdb.dateRuleApply(month, "-1b"))
  cap.secdb <- get_booked_capacity_cerinp(month, asOfDate)
  cap.secdb$locationName <- paste(".Z.", cap.secdb$zone, sep="")
  cap.secdb$zone <- NULL
  cap.secdb <- merge(cap.secdb, ZZ[,c("zone", "locationName")],
    all.x=TRUE, by="locationName")
  cap.secdb$ucap.secdb <- -cap.secdb$capacity   # make it the same sign as iso


  rLog("Get the capacity from the ISO")
  if (month >= as.Date("2010-06-01")){      
    filename <- sort(.get_report_csvfilenames(org="CNE", month=month,
      reportName="SD_FCMCLOSTLDTL"), decreasing=TRUE)[1]
    ucap.iso <- get_FCMCLOSTLDTL(filename, keep=6)
    ucap.iso <- .format.FCMCLOSTLDTL(ucap.iso)
  } else {       # pre-FCM
    filename <- sort(.get_report_csvfilenames(org="CNE", month=month,
      reportName="SD_MTHSTLICAP_"), decreasing=TRUE)[1]
    ucap.iso <- get_MTHSTLICAP(filename, keep=4)
    ucap.iso <- .format.MTHSTLICAP(ucap.iso)
  }
  #lapply(cap.iso, head)


  rLog("Aggregate the daily peak ucap contribution by zone from ISO")
  ucap.iso.zone <- .get.daily.zonal.ucap.peak.obligation(ucap.iso, assetIdMap)


  # compare SecDb and ISO report
  res <- merge(ucap.iso.zone[,c("zone", "date", "ucap.iso")],
     cap.secdb[,c("zone", "date", "ucap.secdb")], by=c("zone", "date"), all=TRUE)
  head(res)


  filename <- paste(DIR_OUT, month, "_ucap_recon.xlsx", sep="")
  .write_results(res, filename)

  return(0)
}

##############################################################################
##############################################################################
# for testing
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(SecDb)
  require(xlsx)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.capacity.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/CNE/Capacity/reconcile_cne_cap_tags.R")

  # two months back from today 
  month <- as.Date("2012-01-01")

  run_report(month)
  
}


#=============================================================================
# NOTES:
# - NH fix for underestimating the PSNH captags by 15% was implemented in
# 2011-10-15 or so
#




##############################################################################
##############################################################################
#ddply(cap.secdb, .(zone), function(x){summary(x$capacity)})
#cap.secdb.avg <- ddply(cap.secdb, .(zone), function(x){mean(x$capacity)})
#names(cap.secdb.avg)[2] <- "ucap.secdb"


#ucap.iso.zone <- .get.monthly.ucap.obligation(cap.iso, assetId)


## diff.zone <-  ddply(res, .(zone), function(x){
## #  browser()
##   aux <- sum(x$secdb.capacity) - sum(x$daily.ucap.peak.iso)
##   r1 <- mean(x$daily.ucap.peak.iso)
##   r2 <- mean(x$secdb.capacity - x$daily.ucap.peak.iso)
##   r3 <- aux/sum(x$daily.ucap.peak.iso)
##   return(c(r1, r2, r3))
## })
## names(diff.zone) <- c("zone", "avg.iso.ucap(MW)", "avg.secdb-iso(MW)",
##                       "secdb-iso(%)")
## diff.zone <- cbind(month=month, diff.zone)
## print(diff.zone, row.names=FALSE)



## require(lattice)
## aux <- melt(res, id=1:2)
## xyplot(value ~ date|zone, data=aux, groups=variable,
##   scales="free")



## # if you get positions from CNE Load Portfolio, you have only
## # CT, ME, NEMA, ROP
## ucap <- .get_cne_positions(as.Date("2010-04-30"), what="UCAP")
## ucap <- ucap[which(ucap$month==month),]





################################################################

## ##############################################################################
## # not used now ... 
## .get.monthly.ucap.obligation <- function(cap.iso, assetId)
## {
##   ucap.iso <- cap.iso[[3]]  #  Monthly.UCAP.Obligation.per.Load.Asset.per.Participant
##   colnames(ucap.iso) <- tolower(colnames(ucap.iso))
##   ucap.iso[,c(2,4:8)] <- sapply(ucap.iso[,c(2,4:8)], as.numeric)
##   ucap.iso <- merge(ucap.iso, assetId, by="asset.id", all.x=TRUE)
##   if (any(is.na(ucap.iso$zone))){
##     rLog("You have unmapped Asset Ids!",
##       unique(ucap.iso$asset.id[is.na(ucap.iso$zone)]))
##     stop("Fix it!")
##   }
##   ucap.iso <- ucap.iso[,c("asset.id", "zone",
##     "monthly.ucap.obligation.per.load.asset.per.participant")]
##   names(ucap.iso)[3] <- "monthly.ucap.iso" 
##   ucap.iso.zone <- cast(ucap.iso, zone ~ ., sum, fill=0, value="monthly.ucap.iso")
##   names(ucap.iso.zone)[2] <- "ucap.iso" 

##   return(ucap.iso.zone)
## }

