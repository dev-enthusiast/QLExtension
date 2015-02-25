# CCG ARRs are booked into NSARR1
# CNE ARRs are booked into CNPISOAN, usually with a comment: ARR
#
# See PM:::libMarkARRs.R for all the details
#
#

##########################################################################
#
.decay_fwdARR <- function(fwdARR, ratio=0.90, baseYear=2012)
{
  fwd0 <- subset(fwdARR, year <  baseYear)   # <-- usually the bal-year
  fwd1 <- subset(fwdARR, year == baseYear)   # <-- you need one full year here!
  if (nrow(fwd1) != 48)
    stop("Are you sure you have a full year?")
  
  fwd2 <- fwd1     # +1 year from the base year
  fwd2$arr   <- fwd1$arr*ratio   
  fwd2$year  <- as.character(as.numeric(fwd1$year) + 1)
  fwd2$month <- as.Date(paste(fwd2$year[1], format(fwd1$month, "-%m-%d"), sep=""))
  
  fwd3 <- fwd1
  fwd3$arr   <- fwd2$arr*ratio
  fwd3$year  <- as.character(as.numeric(fwd1$year) + 2)
  fwd3$month <- as.Date(paste(fwd3$year[1], format(fwd1$month, "-%m-%d"), sep=""))
  
  fwd4 <- fwd1
  fwd4$arr   <- fwd3$arr*ratio
  fwd4$year  <- as.character(as.numeric(fwd1$year) + 3)
  fwd4$month <- as.Date(paste(fwd4$year[1], format(fwd1$month, "-%m-%d"), sep=""))
  
  fwd5 <- fwd1
  fwd5$arr   <- fwd4$arr*ratio
  fwd5$year  <- as.character(as.numeric(fwd1$year) + 4)
  fwd5$month <- as.Date(paste(fwd5$year[1], format(fwd1$month, "-%m-%d"), sep=""))

  fwd6 <- fwd1
  fwd6$arr   <- fwd5$arr*ratio
  fwd6$year  <- as.character(as.numeric(fwd1$year) + 5)
  fwd6$month <- as.Date(paste(fwd6$year[1], format(fwd1$month, "-%m-%d"), sep=""))
  
  rbind(fwd0, fwd1, fwd2, fwd3, fwd4, fwd5, fwd6)
}


##########################################################################
##########################################################################
#
require(CEGbase)
require(SecDb)
require(zoo)
require(reshape)
require(FTR)
require(RODBC)
#require(PM)         # PM/R/libMarkARRs.R   
require(lattice)
source("H:/user/R/RMG/Utilities/Interfaces/PM/R/libMarkARRs.R")


asOfDate  <- Sys.Date()
startDate <- as.Date("2004-01-01")
endDate   <- as.Date("2017-12-01")
#delivPt <- c("MAINE", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA")
#zones   <- 4001:4008       # should match delivPt!
ZZ <<- data.frame(
  ptid = 4001:4008,
  delivPt = c("MAINE", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA"),
  zone = c("ME", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA"))

######################################################################
# get historical ARR data
HH <- get.hist.ARR(zones=as.character(ZZ$ptid), startDate=startDate)

# sum the total dollar share contribution for all the zones
ind <- grep("ZLSD", colnames(HH))
HH$arr_total_dollars <- apply(HH[, ind], 1, sum) 

plot(HH$arr_total_dollars/1000000, col="blue", type="o",
     ylab="Total ARR dollar allocation, M$")
# declining in time ...

# historical ARR payments by year
hARRByYear <- round(aggregate(HH$arr_total_dollars/1000000,
  format(index(HH), "%Y"), sum), 2)
hARRByYear

## # historicals for a given zone ... (current year is truncated)
## ind <- grep("4005_ZLSD", colnames(HH))
## total_zone <- apply(HH[, ind], 1, sum) 
## round(tapply(total_zone, format(as.Date(names(total_zone)), "%Y"), sum))


######################################################################
# get SecDb ARR marks
fwdARR.SecDb <- get.SecDb.ARR(delivPt=ZZ$delivPt, endDate=endDate,
  asOfDate=asOfDate, startDate=asOfDate)
fwdARR.SecDb$location <- gsub("^MAINE", "ME", fwdARR.SecDb$location)


######################################################################
# Get the implied forward congestion marks and interpolate them
# You can use other values if you belive in them
#
CC <- PM:::secdb.getImpliedCongestionMarks(asOfDate=Sys.Date(),
  endDate=endDate)
CC <- subset(CC, delivPt != "HUB")
CC$delivPt <- as.character(CC$delivPt)
CC <- merge(CC, ZZ)
CC$bucket <- gsub("^PEAK", "ONPEAK", CC$bucket)
CC$delivPt <- gsub("^MAINE", "ME", CC$delivPt)


## ## thisDelivPt <- "SEMA"
## xyplot(basisMark ~ month | bucket * delivPt, data=CC,
##   subset=delivPt %in% c("NEMA", "NH"), scales="free")



######################################################################
# Try to see if there is a relationship between CP and dollar amounts
# by zone.  Plot them.  There is a nice relationship for most regions.
# You can create a filter by month, name, class.type, etc.
# FILTER is a data frame with things that you want to keep!

focusZone <- "NH" #"CT" #"ME"  #"WMA" #"NH"  #"NEMA" #"SEMA" #"CT" #ME #RI
CC     <- .adjustCongestionMarks(CC, focusZone=focusZone)  # from libMarkARRs.R
custom <- .customize.zone(zone=focusZone)
MM     <- .study.ARR.regression(HH, focusZone=focusZone, FILTER=custom$FILTER)

######################################################################
# 
source("H:/user/R/RMG/Utilities/Interfaces/PM/R/libMarkARRs.R")
rLog("Make PM view for ARR's")
fwdARR <- .make.PMview.ARR(MM, CC, HH, focusZone=focusZone)   # PM:::



######################################################################
# some regions are not using the regression but a fixed number
# WMA, NEMA.  Suggested by Mike Adams. 
#
specialZones <- c("WMA", "NEMA")              # take hist avg
if (focusZone %in% specialZones){
  aux <- cast(MM, class.type + zone ~., mean,
    subset=zone %in% specialZones & start.dt >= as.Date("2008-01-01"))
  names(aux) <- c("bucket", "delivPt", "flatARR")

  fwdARR <- merge(fwdARR, aux, all.x=TRUE)
  ind <- which(!is.na(fwdARR$flatARR))
  if (length(ind)>0){
    fwdARR$arr[ind] <- fwdARR$flatARR[ind]
    fwdARR$arr.se[ind] <- NA
  }    
  fwdARR$flatARR <- NULL
}

# CT gets special treatment.  Still to high!  Don't have monthly values
# higher than annual ones for the future as we are bearish anyway.
#
if (focusZone == "CT"){
  aux <- cast(fwdARR, month + bucket + delivPt ~ auction, I, value="arr")
  ind <- which(aux$monthly > aux$annual)
  aux$monthly[ind] <- aux$annual[ind]
  aux <- melt(aux, id=1:3)
  fwdARR <- merge(fwdARR, aux)
  fwdARR$arr <- fwdARR$value
  fwdARR$value <- NULL
  fwdARR$arr <- 0.7*fwdARR$arr
}

# ME gets special treatment.  Still to high!  Mark it at 0.65*arr
if (focusZone == "ME"){
  ind <- which(fwdARR$delivPt == "ME")
  fwdARR$arr[ind] <- 0.65*fwdARR$arr[ind]
}

# NEMA gets special treatment.  Still to high!  Mark it at 0.75*arr
if (focusZone == "NEMA"){
  ind <- which(fwdARR$delivPt == "NEMA" & fwdARR$month < as.Date("2012-01-01"))
  fwdARR$arr[ind] <- 0.35*fwdARR$arr[ind]
  ind <- which(fwdARR$delivPt == "NEMA" & fwdARR$month >= as.Date("2012-01-01"))
  fwdARR$arr[ind] <- 0.35*fwdARR$arr[ind]   # keep them the same
}

# NH gets special treatment.  Still to high!  Mark it at 0.75*arr
if (focusZone == "NH"){
  ## ind <- which(fwdARR$delivPt == "NH")
  ## fwdARR$arr[ind] <- 0.75*fwdARR$arr[ind]
  ind <- which(fwdARR$delivPt == "NH" & fwdARR$bucket=="OFFPEAK" & fwdARR$auction=="monthly")
  fwdARR$arr[ind] <- 3500
  ind <- which(fwdARR$delivPt == "NH" & fwdARR$bucket=="OFFPEAK" & fwdARR$auction=="annual")
  fwdARR$arr[ind] <- 3300
  ind <- which(fwdARR$delivPt == "NH" & fwdARR$bucket=="ONPEAK" & fwdARR$auction=="monthly")
  fwdARR$arr[ind] <- 12000
  ind <- which(fwdARR$delivPt == "NH" & fwdARR$bucket=="ONPEAK" & fwdARR$auction=="annual")
  fwdARR$arr[ind] <- 20000  
}

# SEMA -- Overwrite.  Mark it at last year's annual 
if (focusZone == "SEMA"){
  ind <- which(fwdARR$delivPt == "SEMA" & fwdARR$bucket=="OFFPEAK" & fwdARR$auction=="monthly")
  fwdARR$arr[ind] <- 3500
  ind <- which(fwdARR$delivPt == "SEMA" & fwdARR$bucket=="OFFPEAK" & fwdARR$auction=="annual")
  fwdARR$arr[ind] <- 6500
  ind <- which(fwdARR$delivPt == "SEMA" & fwdARR$bucket=="ONPEAK" & fwdARR$auction=="monthly")
  fwdARR$arr[ind] <- 8000
  ind <- which(fwdARR$delivPt == "SEMA" & fwdARR$bucket=="ONPEAK" & fwdARR$auction=="annual")
  fwdARR$arr[ind] <- 36000
}

# WMA gets special treatment.  Mark it at 0.4 where you have it.
if (focusZone == "WMA"){
  ind <- which(fwdARR$delivPt == "WMA")
  fwdARR$arr[ind] <- 0.4*fwdARR$arr[ind]
}


fwdARR <- .decay_fwdARR(fwdARR, ratio=0.90, baseYear=2012)


# sanity check, aggregate the numbers by year
(fARRByYear  <- cast(fwdARR, year ~ ., sum, fill=NA, value="arr"))

# look at the standard error of the estimate
cast(fwdARR, year ~ ., function(x){sqrt(sum(x^2))}, fill=NA, value="arr.se")

# allocation by year and zone
fARRByYearZone <- cast(fwdARR, year + delivPt ~ ., sum, fill=NA, value="arr")



#.plot.histfwd.ARR.dollars(HH, fwdARR, delivPt=focusZone)



######################################################################
# make the ARR marks for PM view
# sum the monthly and annual arr dollars by month, delivPt
fwdARR.pm <- aggregate(fwdARR[,"arr"], fwdARR[,c("month", "delivPt")], sum)
colnames(fwdARR.pm)[3] <- "arr"
aux <- aggregate(fwdARR[,"arr.se"], fwdARR[,c("month", "delivPt")],
  function(x){sqrt(sum(x^2))})
colnames(aux)[3] <- "arr.se"
fwdARR.pm <- merge(fwdARR.pm, aux)

LL <- PM:::.read.peak.load.forecast()  # add the peak load
LL$delivPt <- gsub("^MAINE", "ME", LL$delivPt)
fwdARR.pm <- merge(fwdARR.pm, LL, all.x=TRUE)
fwdARR.pm$arr.mark    <- fwdARR.pm$arr/fwdARR.pm$PeakLoad
fwdARR.pm$arr.mark.se <- fwdARR.pm$arr.se/fwdARR.pm$PeakLoad


#source("H:/user/R/RMG/Utilities/Interfaces/PM/R/libMarkARRs.R")
# look at the forecast, the history and the SecDb mark
.plot.histfwd.ARR.marks(HH, fwdARR.pm, fwdARR.SecDb, delivPt=focusZone)


# see the ARRs in $/MWh
PM:::.calc.fwdARR.dollarMWh(fwdARR.pm, delivPt=focusZone)




######################################################################
# save to file the PM view!
#  
mark <- cast(fwdARR.pm, month ~ delivPt, I, value="arr.mark", subset=delivPt==focusZone)
fname <- paste("S:/All/Structured Risk/NEPOOL/Ancillary Services/",
  "ARR Valuation/Sent/", asOfDate, "_arr_marks_", focusZone, ".csv", sep="")
write.csv(mark, file=fname, row.names=FALSE)
rLog("Wrote", fname)

windows()
plot(mark$month, mark[,2], type="o", col="blue")




######################################################################
# Compare realized ARR dollars vs. marks dollars
#
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.loads.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.load.R")
histARR.dollars <- realized_ARR_pool(what="dollars")
## print(rbind(histARR.dollars[,c("month", focusZone)],   # put hist and fwd together
##   cast(fwdARR.pm, month ~ delivPt, sum, value="arr")), row.names=FALSE)

LL <- PM:::.read.peak.load.forecast()  # add the peak load
LL$delivPt <- gsub("^MAINE", "ME", LL$delivPt)
aux <- LL; aux$month <- as.Date(aux$month)
aux$location <- aux$delivPt; aux$delivPt <- NULL
fwdARR.SecDb.dollars <- merge(fwdARR.SecDb, aux, by=c("month", "location"))
fwdARR.SecDb.dollars$arrDollars <- fwdARR.SecDb.dollars$value * fwdARR.SecDb.dollars$PeakLoad
fwdARR.SecDb.dollars$value <- NULL
fwdARR.SecDb.dollars <- cast(fwdARR.SecDb.dollars, month ~ location, I, value="arrDollars")

print(rbind(histARR.dollars,   # put hist and marks together
            fwdARR.SecDb.dollars), row.names=FALSE)
# paste to xlsx





######################################################################
# chop 2016+ until we get better clarity
#
focusZone <- "WMA"
fwdARR.SecDb <- get.SecDb.ARR(delivPt=focusZone, endDate=endDate,
  asOfDate=asOfDate, startDate=asOfDate)
mark <- cast(fwdARR.SecDb, month ~ location, I, value="value", subset=location==focusZone)
ind  <- which(mark$month >= as.Date("2016-01-01"))
mark[ind,2] <- 0.2*mark[ind,2]

fname <- paste("S:/All/Structured Risk/NEPOOL/Ancillary Services/",
  "ARR Valuation/Sent/", asOfDate, "_arr_marks_", focusZone, ".csv", sep="")
write.csv(mark, file=fname, row.names=FALSE)
rLog("Wrote", fname)




#########################################################################
HH[,grep("4002", colnames(HH))]

