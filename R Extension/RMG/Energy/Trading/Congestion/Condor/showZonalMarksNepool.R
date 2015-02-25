# Show zonal spreads for the day before
#
# .addHours
# .expandTerm
# .getZonalMarks
# .getZonalMarks.NEPOOL
# .getZonalMarks.NYPP
#
# Written by AAD on 25-Feb-2009


#################################################################
# Get the hours for the buckets you need
#
.addHours <-  function(curveData)
{
  bucket <- c("OFFPEAK", "PEAK", "FLAT")
  
  res <- PM:::ntplot.bucketHours(bucket, min(curveData$month),
    max(curveData$month), region="NEPOOL", period="Month")
  colnames(res) <- c("bucket", "month", "hrs")

  res <- merge(curveData, res, by=c("month", "bucket"), all.x=TRUE)
  
  res
}


#################################################################
#
.expandTerm <- function(term, asOfDate=Sys.Date())
{
  TT <- data.frame(
    startDate = as.Date(sapply(term, function(x) {
      y <- termStartDate(x)
      if (y < asOfDate) y <- nextMonth(asOfDate)
      y  
    }), origin="1970-01-01"),
    endDate   = as.Date(sapply(term, termEndDate), origin="1970-01-01"))
                    
  TT <- subset(TT, endDate >= asOfDate & endDate > startDate)

  TT <- lapply(split(TT[,c("startDate", "endDate")], rownames(TT)),
    function(x) {
      print(x)
      seq(x$startDate, x$endDate, by="1 month")
    })
  TT <- melt(TT)
  names(TT)    <- c("month", "period")
  rownames(TT) <- NULL

  
  return(TT)
}



#################################################################
# Put together the NEPOOL and NYISO marks
#
.getZonalMarks <-  function(asOfDate, term)
{
  mNEPOOL <- .getZonalMarks.NEPOOL(asOfDate, term)
  mNYPP   <- .getZonalMarks.NYPP(asOfDate, term)

  res <- merge(mNEPOOL, mNYPP, by=c("bucket", "period"), all=TRUE)

  res$bucket <- gsub("OFFPEAK", "OFFPK", res$bucket)
  res[,3:ncol(res)] <- round(res[,3:ncol(res)], 2)
  res <- split(res[,-1], res$bucket)
  return(res)
}

#################################################################
#
.getZonalMarks.NEPOOL <- function(asOfDate, term)
{
  TT <- .expandTerm(term)

  startDate  <- min(TT$month)
  endDate    <- max(TT$month)
  delivPt    <- c("CT", "SEMA", "RI", "Maine", "VT", "WMA", "NH", "Nema", "Hub")
  bucket     <- c("PEAK", "OFFPEAK", "FLAT")
  commodity  <- "COMMOD PWR NEPOOL PHYSICAL"

  curveData <- secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, delivPt, bucket )
  
  names(curveData)[which(names(curveData)=="date")]     <- "month"
  names(curveData)[which(names(curveData)=="location")] <- "delivPt"
  curveData <- curveData[,c("delivPt", "month", "bucket", "value")]

  curveData <- subset(curveData, value != Inf) # remove expired months
  
  curveData  <- merge(curveData, TT) #, all.y=TRUE)

  curveData  <- .addHours(curveData)  # add the hours
  
  aux <- ddply(curveData, c("period", "bucket", "delivPt"), function(x){
    sum(x$value * x$hrs)/sum(x$hrs)
  })
  aux <- cast(aux, period + bucket ~ delivPt, I, fill=NA, value="V1")
  
  # substract the Hub price
  indHub <- which(names(aux)=="Hub")
  Hub <- aux[,indHub]
  aux <- aux[,-indHub]
  aux[,3:ncol(aux)] <- aux[,3:ncol(aux)]-Hub
  res <- data.frame(aux[,1:2], Hub=Hub, aux[3:ncol(aux)])
    
  return(res)
}

#################################################################
#
.getZonalMarks.NYPP <- function(asOfDate, term)
{
  TT <- .expandTerm(term)
  
  startDate  <- min(TT$month)
  endDate    <- max(TT$month)
  delivPt    <- c("West", "Cen", "Cap", "HudV", "NYC", "LI")
  zoneLetter <- structure(c("A", "C", "F", "G", "J", "K"), names=delivPt)
  bucket     <- c("PEAK", "OFFPEAK", "FLAT")
  commodity  <- "COMMOD PWR NYPP PHYSICAL"

  curveData <- secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, delivPt, bucket )
  
  names(curveData)[which(names(curveData)=="date")]     <- "month"
  names(curveData)[which(names(curveData)=="location")] <- "delivPt"
  curveData <- curveData[,c("delivPt", "month", "bucket", "value")]

  curveData  <- merge(curveData, TT) #, all.y=TRUE)
  curveData$zone <- zoneLetter[curveData$delivPt]
  
  curveData <- subset(curveData, value != Inf) # remove expired months
  curveData  <- .addHours(curveData)  # add the hours
  
  aux <- ddply(curveData, c("period", "bucket", "delivPt"), function(x){
    sum(x$value * x$hrs)/sum(x$hrs)
  })
  znMark <- cast(aux, period + bucket ~ delivPt, I, fill=NA, value="V1")

  colNames <- c("C/A", "G/A", "J/G", "G/F", "K/J")
  res <- cbind(znMark[,1:2],
    'C/A'=znMark[,"Cap"]-znMark[,"West"], 
    'G/A'=znMark[,"HudV"]-znMark[,"West"],
    'J/G'=znMark[,"NYC"]-znMark[,"HudV"],
    'G/F'=znMark[,"HudV"]-znMark[,"Cap"],
    'K/J'=znMark[,"LI"]-znMark[,"NYC"])
  
  return(res)
}


#################################################################
#################################################################
options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE)
require(methods)   # not sure why this does not load!
require(CEGbase)
require(CEGterm)
require(SecDb)
require(reshape)
require(PM)


# Initialize SecDb for use on Procmon
DB = "!Bal ProdSource;!BAL DEV!home!E47187;!BAL Home"
Sys.setenv(SECDB_SOURCE_DATABASE=DB)
Sys.setenv(SECDB_DATABASE="Prod")

rLog(paste("Start proces at ", Sys.time()))
returnCode <- 0    # succeed = 0

cArgs <- commandArgs()
if (length(cArgs)>6){
  asOfDate <- as.Date(cArgs[7])  # pass asOfDate ... 
} else {
  asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-0b"))
}

rLog(paste("Running report for", as.character(asOfDate)))
rLog("\n\n")

promptMonth <- termFromDatePair(seq(as.Date(format(Sys.Date(),
  "%Y-%m-01")), by="1 month",length.out=2)[2])
term <- c(promptMonth, "FG 15", "NQ 15", 
  "CAL 13", "CAL 14", "CAL 15", "CAL 16", "CAL 17",
  "CAL 18", "CAL 19") # "SPR 09", "NQ 09", "BAL 10", "Q4 10",
rLog("Get zonal marks for:", as.character(asOfDate))
try(MM <- .getZonalMarks(asOfDate, term))
if (class(MM)=="try-error")
  returnCode <- 1                 # fail

out  <- c("", 
  "PEAK:", capture.output(print(MM$PEAK, row.names=FALSE)), "",
  "FLAT:", capture.output(print(MM$FLAT, row.names=FALSE)), "",
  "OFFPEAK:", capture.output(print(MM$OFFPK, row.names=FALSE))
)

# show the changes in marks from yesterday!
yday <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
rLog("Get zonal marks for previous day:", as.character(yday))
YY <- try(.getZonalMarks(yday, term))
if (class(YY) != "try-error") {
  YY  <- melt(YY); colnames(YY)[3]  <- "P1"     # yesterday
  MMM <- melt(MM); colnames(MMM)[3] <- "P2"     # today
  dP <- merge(YY, MMM, all=TRUE)

  dP$value <- dP$P2 - dP$P1
  dP$P1 <- dP$P2 <- NULL
  dP <- cast(dP, L1 + period ~ variable, I, fill=0)
  dP <- split(dP[,-1], dP$L1)

  out  <- c(out, "\n\nChanges in marks:\n", 
    "PEAK:", capture.output(print(dP$PEAK, row.names=FALSE)), "",
    "FLAT:", capture.output(print(dP$FLAT, row.names=FALSE)), "",
    "OFFPEAK:", capture.output(print(dP$OFFPK, row.names=FALSE))
  ) 
}


rLog("Email results")
sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
#  "adrian.dragulescu@constellation.com, kevin.telford@constellation.com",
  "adrian.dragulescu@constellation.com",
  paste("Zonal spreads and changes", as.character(asOfDate)), out)

rLog(paste("Done at ", Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}





## # show the differences between PM view and SecDb marks
## rLog("Get PM view")
## try(PM <- .getChangesToPMview(MM, term))
## if (class(PM)!="try-error"){
##   out <- c(out, "\n\nPM view over SecDb marks (+ numbers are NOT good for selling load):\n", 
##     "PEAK:", capture.output(print(PM$PEAK, row.names=FALSE)), "",
##     "FLAT:", capture.output(print(PM$FLAT, row.names=FALSE)), "",
##     "OFFPEAK:", capture.output(print(PM$OFFPK, row.names=FALSE))
##   ) 
## }


#################################################################
# Show the difference between the marks and the PM view
#
## .getChangesToPMview <- function(MM, term)
## {
##   dir    <- "S:/All/Structured Risk/NEPOOL/Marks/AAD/"
##   zones  <- c("MAINE", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA")
##   pmView <- NULL
##   # read the files produced with Congestion/Condor/dailyPMzonalViewNepool.R
##   for (z in 1:length(zones)){
##     aux <- try(read.csv(paste(dir, "allMarks_", zones[z], ".csv", sep="")))
##     if (class(aux)=="try-error") next
##     aux$zone <- zones[z]
##     pmView[[z]] <- subset(aux, component %in% c("nodeMark", "hubMark") &
##                         source=="pmView")
##   }
##   pmView <- do.call(rbind, pmView)

##   pmView <- cast(pmView, zone + month + bucket ~ component, I, fill=NA)
##   pmView$value <- pmView$nodeMark - pmView$hubMark
##   pmView$hubMark <- pmView$nodeMark <- NULL

##   TT <- .expandTerm(term)
  
##   toMonths <- seq(min(TT$month), max(TT$month), by="1 month")
  
##   pmView <- data.frame(pmView)
##   pmView$month <- as.Date(pmView$month)
##   pmView <- PM:::.extendPeriodically(pmView, toMonths)  # extend
##   names(pmView)[2] <- "contract.dt"

##   HRS <- ntplot.bucketHours(c("2X16H", "7X8", "5X16"), region="NEPOOL",
##     startDate=min(pmView$contract.dt), endDate=max(pmView$contract.dt))
##   names(HRS)[2:3] <- c("contract.dt", "hours")
##   pmView <- merge(pmView, HRS, by=c("contract.dt", "bucket"))

##   # add offpeak
##   offpeak <- combineBuckets(pmView, weight="hour", buckets=c("2X16H", "7X8"),
##     name="OFFPEAK") 
##   pmView <- rbind(pmView, subset(offpeak, bucket=="OFFPEAK"))

##   # add flat
##   flat <- combineBuckets(pmView, weight="hour", buckets=c("2X16H", "7X8",
##     "5X16"), name="FLAT")
##   pmView <- rbind(pmView, subset(flat, bucket=="FLAT"))
##   pmView <- subset(pmView, bucket %in% c("5X16", "OFFPEAK", "FLAT"))
##   names(pmView)[which(names(pmView)=="contract.dt")] <- "month"

  
##   pmView <- merge(pmView, TT, all.y=TRUE)
##   pmView <- na.omit(pmView)
##   PM <- cast(pmView, bucket + period ~ zone, mean, na.rm=TRUE, fill=NA)
##   PM[,3:ncol(PM)] <- round(PM[,3:ncol(PM)], 2)
##   PM <- data.frame(PM)
##   PM$bucket <- gsub("OFFPEAK", "OFFPK", PM$bucket)
##   PM$bucket <- gsub("5X16", "PEAK", PM$bucket)
##   PM <- melt(PM, id=1:2)
##   names(PM)[4] <- "markPM"

##   MM <- melt(MM); colnames(MM)[3] <- "P1"     # marks
##   names(MM) <- c("period", "variable", "markSecDb", "bucket")
##   MM$variable <- toupper(MM$variable)

##   dP <- merge(PM, MM, all=TRUE, by=c("period", "bucket", "variable"))
##   dP$value <- dP$markPM - dP$markSecDb
##   dP$markPM <- dP$markSecDb <- NULL

##   dP <- cast(dP, bucket + period ~ variable, I, fill=0)
##   dP <- split(dP[,-1], dP$bucket)

##   return(dP)
  
## }
