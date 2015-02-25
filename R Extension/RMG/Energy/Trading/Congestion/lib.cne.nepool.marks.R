# Pull from SecDb all the marks CNE needs, NEPOOL + ONTARIO only
#
# compare_files            # show day on day differences
# pull_ancillaries_nepool
# pull_arr_nepool
# pull_gas
# pull_prices_ontario
# pull_prices_nepool
# pull_recs_nepool
# pull_vols_nepool
# .replace_hardcoded
# read_hedge_costs
# scale_arr_prices
# writecsv_ancillaries_nepool
# writecsv_capacity_nepool
# writecsv_energy_nepool
# writecsv_energy_ontario
# writecsv_hedgecosts_ancillaries
# writecsv_hedgecosts_capacity
# writecsv_hedgecosts_nepool
# writecsv_hedgecosts_ontario
# writecsv_hedgecosts_recs
# writecsv_nesc_file
# writecsv_recs_nepool
# writexlsx_buckets_gas_close
#
# _plot_energy
#
#
# Written by Adrian Dragulescu on 17-Feb-2010

## TESLA FORMATS
## File Name: <Product>_<Region>_<yyyymmdd>_<hhmmss>.txt
## File Contents: <Region>, <Location>, <Product>, <PriceType>,
## <SubType>, <Price>, <Date>, <Technology>

## ISSUE WITH LOCATION: In your file, you have the 2nd field
## (Location) as “CT – CLASS1”, etc.  This is not a valid location in
## TESLA.  The valid locations in TESLA for NEPOOL are:
## CT
## ME
## MA
## MassHub
## NEMA
## NH
## RI
## ROP
## SEMA
## VT
## WCMA

## ISSUE WITH PRODUCT: You have chosen “RENEW” as your 3rd field
## (Product).  You will have to use “REC” if you are loading RECs.

## ISSUE WITH PRICETYPE: You have use the value of “FLAT” in the 4th
## field (PriceType).  In TESLA, the expected values are:
## Peak
## OffPeak
## RTC
## 2x16H
## 7x8
## In the case of your file, you should use “RTC”.  The Windows
## Service will not recognize “FLAT” and will throw an error.

## ISSUE WITH SUBTYPE: In your file, you omitted the 5th field
## (SubType).  (In the ENERGY .txt file, energy does not have a sub
## type, and therefore, an extra comma is used to signify a “blank”).
## Since there is only 2 types of RECs (Voluntary or Compliance),
## there are only 2 sub type values stored in TESLA for RECs:
## Voluntary
## Compliance
## In the case of your file, you will need to use “Compliance”.

## ISSUE WITH TECHNOLOGY: In your file, you omitted the 8th field
## (Technology).  This is the only field that is optional.  However,
## for RECs, you have a technology type, and therefore, you need to
## specify one.  This is the place where I have to update the database
## to add these “Technology” types.  I will add the following types:
## CLASS I
## CLASS II
## CLASS II WTE
## CLASS III
## CLASS IV
## EXISTING
## SOLAR
## NEW
## AE
## Therefore, an example row in your .txt file can be:
## NEPOOL, CT, REC,  RTC, Compliance, 25, 03/01/2010, CLASS I

## The second line of the CSV file must be one of the following:
## Curve Type: Offer
## Curve Type: Basis
## Curve Type: HedgeCost
## Curve Type: Bid
## Curve Type: Mid



###########################################################################
###########################################################################
# Hardcoded values in format:
#   market.location.serviceType.bucket.startMth.endMth
#
ADDER_OVER_PJM_PEAK_VOLS <<- 0.10
RATIO_OFFPEAK_PEAK_VOL   <<- 0.70

HCV <<- NULL                               # Hard Coded Values  
#HCV[["NEPOOL.RENEW.MA SOL.FLAT"]] <- 600   # SecDB has it at $50!



###########################################################################
# compare day on day price changes to pinpoint changes, can be several
# files in the same format
#
compare_files <- function(files)
{
  RR <- NULL
  for (f in 1:length(files)){
    file <- files[f]
    rLog("Working on ", basename(file))
    aux <- read.csv(file, skip=1, header=FALSE)
    colnames(aux) <- c("market", "location", "serviceType", "serviceTypeDesc",
      "bucket", "subtype", "value", "mmddyyyy", "technology")
    asOfDate <- as.Date(substr(file, nchar(file)-18, nchar(file)-11), "%Y%m%d")
    colnames(aux)[7] <- as.character(asOfDate)
    aux$mmddyyyy <- as.Date(aux$mmddyyyy, format="%m/%d/%Y")
    if (f==1){
      RR <- aux
    } else {
      RR <- merge(RR, aux, all=TRUE)
    }
  }
  cols <- 9:ncol(RR)
  ind <- apply(RR[,cols], 1, function(x){sd(x) != 0})  
  res <- RR[which(ind),]
  rownames(res) <- NULL
  
  #res <- res[order(res$location, res$subtype, res$technology, res$mm),]
  #print(res, row.names=FALSE)
  
  return(res)
}


###########################################################################
# Pull NEPOOL ancillaries prices   
#
pull_ancillaries_nepool <- function(asOfDate, startDate, endDate, HC, IDM)
{
  rLog("Pulling ancillaries for NEPOOL ...")
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  useFutDates <- FALSE
  location    <- c("ROP")
  bucket      <- c("FLAT")
  serviceType <- c("FWDRES", "REG", "TR SCH2", "TR SCH3", "LOSSRMB")
  cD1 <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)

  # get OpRes, ROP
  location    <- c("DA", "RT")
  serviceType <- c("OPRES")
  cD2 <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)  

  # get FWDRES for CT, NEMA
  location    <- c("CT", "NEMA")
  bucket      <- c("FLAT")
  serviceType <- c("FWDRES")
  cD3 <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)

  # get RT RMR
  location    <- c("NEMA RT", "SEMA RT", "WCMA RT", "MAINE RT", "CT RT",
                   "RI RT", "NH RT")
  bucket      <- c("FLAT")
  serviceType <- c("RMR")
  cD4 <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  
  # get DA RMR -- not yet 3/19/2014 AAD
  ## location    <- c("NEMA DA", "SEMA DA", "WCMA DA", "MAINE DA", "CT DA",
  ##                  "RI DA", "NH DA")
  ## bucket      <- c("FLAT")
  ## serviceType <- c("RMR")
  ## cD7 <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
  ##   commodity, location, bucket, serviceType, useFutDates)
  
  # get RT OpRes, CT and NEMA
  location    <- c("CTRT", "NEMART")
  serviceType <- c("OPRES")
  cD5 <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)


  # get DRUplift
  code <- paste('link("_lib matlab functions"); @MATLAB::GetFutCurve(date("',
    format(asOfDate, "%d%b%y"), '"), date("', format(startDate, "%d%b%y"),
    '"), date("', format(endDate, "%d%b%y"),
    '"), "Commod PWR NEPOOL Physical", "", "7x24", "DRUplift");')
  cD6 <- as.data.frame(secdb.evaluateSlang(code))
  cD6 <- data.frame(asOfDate=asOfDate, location="ROP", bucket="FLAT",
    serviceType="DRUPLIFT", month=cD6$date, value=cD6$value)

  AA <- rbind(cD1, cD2, cD3, cD4, cD5, cD6)
  
  #=================================================================
  # get WinterCap  (for now the SecDb curve is 0.)
  location    <- "ROP"
  bucket      <- c("FLAT")
  serviceType <- c("WINTRCAP")
  cD7 <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates=FALSE)

  AA <- rbind(cD1, cD2, cD3, cD4, cD5, cD6, cD7)    
  
  rLog("Set FwdRes for ME, SEMA, WMA, RI, NH to ROP FwdRes.")
  zones <- c("ME", "SEMA", "WCMA", "RI", "NH")
  rop <- subset(AA, location=="ROP" & serviceType=="FWDRES")
  for (z in seq_along(zones)){
    rop$location <- zones[z]
    AA <- rbind(AA, rop)
  }

  
  rLog("Set RT RMR for ROP = 0.")
  rop <- subset(AA, location=="CT RT" & serviceType=="RMR")
  rop$location <- "ROP"
  rop$value <- 0
  AA <- rbind(AA, rop)      # OTHER ZONES?!
  ind <- which(AA$location=="CT RT" & AA$serviceType=="RMR")
  AA$location[ind] <- "CT"
  ind <- which(AA$location=="MAINE RT" & AA$serviceType=="RMR")
  AA$location[ind] <- "ME"
  ind <- which(AA$location=="NEMA RT" & AA$serviceType=="RMR")
  AA$location[ind] <- "NEMA"
  ind <- which(AA$location=="SEMA RT" & AA$serviceType=="RMR")
  AA$location[ind] <- "SEMA"
  ind <- which(AA$location=="WCMA RT" & AA$serviceType=="RMR")
  AA$location[ind] <- "WCMA"
  ind <- which(AA$location=="RI RT" & AA$serviceType=="RMR")
  AA$location[ind] <- "RI"
  ind <- which(AA$location=="NH RT" & AA$serviceType=="RMR")
  AA$location[ind] <- "NH"
  

  
  rLog("Set RT OpRes for ME, SEMA, WCMA to ROP RT OpRes.")
  ind <- which(AA$location=="RT" & AA$serviceType=="OPRES")     
  AA$location[ind] <- "ROP"; AA$serviceType[ind] <- "OPRES_RT"  
  ind <- which(AA$location=="CTRT" & AA$serviceType=="OPRES")
  AA$location[ind] <- "CT"; AA$serviceType[ind] <- "OPRES_RT"
  ind <- which(AA$location=="NEMART" & AA$serviceType=="OPRES")
  AA$location[ind] <- "NEMA"; AA$serviceType[ind] <- "OPRES_RT"
  zones <- c("ME", "SEMA", "WCMA", "RI", "NH")
  rop <- subset(AA, location=="ROP" & serviceType=="OPRES_RT")
  for (z in seq_along(zones)){
    rop$location <- zones[z]
    AA <- rbind(AA, rop)
  }

  
  rLog("Set DA OpRes for CT, ME, NEMA, SEMA, WCMA, RI, NH to ROP DA OpRes.")
  ind <- which(AA$location=="DA" & AA$serviceType=="OPRES")     
  AA$location[ind] <- "ROP"; AA$serviceType[ind] <- "OPRES_DA"  
  zones <- c("CT", "ME", "NEMA", "SEMA", "WCMA", "RI", "NH")
  rop <- subset(AA, location=="ROP" & serviceType=="OPRES_DA")
  for (z in seq_along(zones)){
    rop$location <- zones[z]
    AA <- rbind(AA, rop)
  }
  

  rLog("Set Reg for CT, ME, NEMA, SEMA, WCMA, RI, NH to ROP Reg.")
  zones <- c("CT", "ME", "NEMA", "SEMA", "WCMA", "RI", "NH")
  rop <- subset(AA, location=="ROP" & serviceType=="REG")
  for (z in seq_along(zones)){
    rop$location <- zones[z]
    AA <- rbind(AA, rop)
  }

  
  rLog("Set TrSch2 for CT, ME, NEMA, SEMA, WCMA, RI, NH to ROP TrSch2.")
  zones <- c("CT", "ME", "NEMA", "SEMA", "WCMA", "RI", "NH")
  rop <- subset(AA, location=="ROP" & serviceType=="TR SCH2")
  for (z in seq_along(zones)){
    rop$location <- zones[z]
    AA <- rbind(AA, rop)
  }

  
  rLog("Set TrSch3 for CT, ME, NEMA, SEMA, WCMA to ROP TrSch3.")
  zones <- c("CT", "ME", "NEMA", "SEMA", "WCMA")
  rop <- subset(AA, location=="ROP" & serviceType=="TR SCH3")
  for (z in seq_along(zones)){
    rop$location <- zones[z]
    AA <- rbind(AA, rop)
  }


  rLog("Set LossRmb for CT, ME, NEMA, SEMA, WCMA, RI, NH to ROP LossRmb.")
  zones <- c("CT", "ME", "NEMA", "SEMA", "WCMA", "RI", "NH")
  rop <- subset(AA, location=="ROP" & serviceType=="LOSSRMB")
  for (z in seq_along(zones)){
    rop$location <- zones[z]
    AA <- rbind(AA, rop)
  }

  rLog("Set WintrCap for CT, ME, NEMA, SEMA, WCMA, RI, NH to ROP WintrCap.")
  zones <- c("CT", "ME", "NEMA", "SEMA", "WCMA", "RI", "NH")
  rop <- subset(AA, location=="ROP" & serviceType=="WINTRCAP")
  for (z in seq_along(zones)){
    rop$location <- zones[z]
    AA <- rbind(AA, rop)
  }
  
  rLog("Set GISAdmin for all zones to $0.02/MWh.")
  rop <- subset(AA, location=="ROP" & serviceType=="LOSSRMB")
  rop$serviceType <- "GISADMIN"
  rop$value <- 0.02
  AA <- rbind(AA, rop)
  zones <- c("CT", "ME", "NEMA", "SEMA", "WCMA", "RI", "NH")
  for (z in seq_along(zones)){
    rop$location <- zones[z]
    AA <- rbind(AA, rop)
  }
  
  AA$bucket <- "RTC"
  AA$market <- "NEPOOL"
  AA <- subset(AA, is.finite(value))
  AA$subtype <- AA$serviceType
  AA$serviceType <- "ANCILLARY"
  
  AA$secdbValue <- AA$value

  # add hedge costs, intraday move
  hc <- subset(HC, market=="NEPOOL")
  hc$subtype <- hc$serviceType
  hc$serviceType <- NULL
  
  AA <- merge(AA, hc,  all.x=TRUE)
  AA <- merge(AA, IDM, all.x=TRUE)
  AA$hedgeCost[is.na(AA$hedgeCost)] <- 0  

  AA$value <- AA$secdbValue + AA$hedgeCost + AA$intraDayMove
  AA <- subset(AA, value != Inf)

  return(AA) 
}


  
###########################################################################
# Pull NEPOOL ARR prices 
# No "VT" 
pull_arr_nepool <- function(asOfDate, startDate, endDate)
{
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  location    <- c("MAINE", "NH", "CT", "RI", "SEMA", "WMA", "NEMA")
  bucket      <- c("FLAT")
  serviceType <- "ARR"
  useFutDates <- FALSE
  
  curveData <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  curveData$market <- "NEPOOL"
  curveData$secdbValue <- curveData$value
  curveData$hedgeCost  <- 0
  
  curveData$location <- gsub("^MAINE", "ME", curveData$location)
  curveData$location <- gsub("^WMA", "WCMA", curveData$location)
  curveData$bucket   <- gsub("^FLAT", "RTC", curveData$bucket)
  
  curveData$subtype  <- "ARR"
  curveData$serviceType <- "ANCILLARY"
  rLog("Done!")
  
  return(curveData)
}


###########################################################################
# Pull NEPOOL capacity, only ROP and CT
# 
pull_capacity_nepool <- function(asOfDate, startDate, endDate, HC, IDM)
{
  rLog("Pull capacity NEPOOL ...")
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  useFutDates <- FALSE
  location    <- c("ROP", "CT", "MAINE", "NEMA", "SEMA", "RI")
  bucket      <- c("FLAT")
  serviceType <- c("UCAP")
  CC1 <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)

  rLog("Set capacity prices for WCMA, NH to ROP")
  rop <- subset(CC1, location == "ROP")
  rop$location <- "NH"
  CC1 <- rbind(CC1, rop)
  rop$location <- "WCMA"
  CC1 <- rbind(CC1, rop)
  
  location    <- c("ROP", "MAINE", "CT", "NEMA")
  serviceType <- c("PCAP", "RTEG")
  CC2 <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)

  CC <- rbind(CC1, CC2)
  
  CC$location <- gsub("^MAINE", "ME", CC$location)
  CC$location <- gsub("^WMA", "WCMA", CC$location)
  
  CC$market <- "NEPOOL"
  CC$bucket <- "RTC"
  CC <- subset(CC, is.finite(value))
  CC$subtype <- CC$serviceType
  CC$serviceType <- "CAPACITY"
  CC$secdbValue <- CC$value

  # add hedge costs, intraday move
  # the understanding with CNE is that they will map the UCAP hedge costs
  # to all other capacity products
  hc <- subset(HC, market=="NEPOOL" & serviceType=="CAPACITY")
  CC <- merge(CC, hc,  all.x=TRUE)
  CC <- merge(CC, IDM, all.x=TRUE)
  CC$hedgeCost[is.na(CC$hedgeCost)] <- 0  

  CC$value <- CC$secdbValue + CC$hedgeCost + CC$intraDayMove
  CC <- subset(CC, value != Inf)
  rLog("Done!")
  
  return(CC)   
}

###########################################################################
# Pull Nymex close.  Not sure why I get a crash with getElecPrices!
#
pull_gas <- function(asOfDate, startDate, endDate)
{
  rLog("Pull HH closing ... ")
  aod <- format(asOfDate, "%d%b%y")
  sd <- format(nextMonth(asOfDate), "%d%b%y")
  ed <- format(endDate, "%d%b%y")
  
  secdb.evaluateSlang('Link("_lib matlab functions");')
  slang = paste('@MATLAB::GetFutCurve( Date("', aod, '"), Date("', sd,
    '"),  Date("', ed,'"), "Commod ng exchange", "", "7x24", "energy");',
    sep="")
  curveData <- secdb.evaluateSlang(slang)
  curveData <- data.frame(curveData)
  names(curveData)[1] <- "month"
  curveData <- subset(curveData, is.finite(value))  # remove expires
  curveData$commodity   <- "COMMOD NG EXCHANGE"
  curveData$location    <- "EXCHANGE"
  curveData$bucket      <- "FLAT"
  curveData$serviceType <- "ENERGY"
  curveData$market      <- "NG"
  curveData$asOfDate    <- asOfDate
  
  curveData$secdbValue <- curveData$value
  curveData$hedgeCost  <- 0
  curveData$value <- curveData$secdbValue + curveData$hedgeCost
  rLog("Done!")
  
  return(curveData)  
}
  
  
###########################################################################
# Pull AGT CG for 
#
pull_gas_agt <- function(asOfDate, startDate, endDate)
{
  rLog("Pull AGT close ... ")

  names <- c("COMMOD NG ALGCG FERC", "COMMOD NG ALGCG GAS-DLY MEAN",
             "COMMOD NG ALGCG PHYSICAL")
  months <- seq(startDate, endDate, by="month")
  asOfDate0 <- as.Date(secdb.dateRuleApply(asOfDate, "-1b"))
  agt <- tsdb.futStrip(names, asOfDate0, asOfDate, months)

  rLog("Done!")
  
  return(agt)  
}


###########################################################################
# Pull NEPOOL zonal energy prices 
#
pull_prices_nepool <- function(asOfDate, startDate, endDate, HC=NULL, IDM=NULL)
{
  rLog("Pull energy prices NEPOOL ...")
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  location    <- c("HUB", "MAINE", "NH", "VT", "CT", "RI", "SEMA",
                   "WMA", "NEMA")
  bucket      <- c("FLAT", "PEAK", "OFFPEAK", "2X16H", "7X8")
  serviceType <- "ENERGY"
  useFutDates <- FALSE
  
  curveData <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  curveData$market <- "NEPOOL"
  curveData$secdbValue <- curveData$value
  
  ind <- is.infinite(curveData$secdbValue)
  if (length(ind)>0)
    curveData[ind,"secdbValue"] <- 0
  
  curveData$bucket   <- gsub("^FLAT", "RTC", curveData$bucket)
  curveData$location <- gsub("^HUB",   "MASSHUB", curveData$location)
  curveData$location <- gsub("^MAINE", "ME", curveData$location)
  curveData$location <- gsub("^WMA",   "WCMA", curveData$location)

  curveData$value <- curveData$secdbValue
  if (!is.null(HC)){
    # add hedge costs, intraday move
    hc <- subset(HC, market=="NEPOOL" & serviceType=="ENERGY")
    curveData <- merge(curveData, hc,  all.x=TRUE)
    curveData$value <- curveData$value + curveData$hedgeCost
  }
  if (!is.null(IDM)){
    curveData <- merge(curveData, IDM, all.x=TRUE)
    curveData$value <- curveData$value + curveData$intraDayMove
  }

  curveData <- curveData[order(curveData$location, curveData$bucket), ]
  rLog("Done!")
  
  return(curveData)
}


###########################################################################
# Pull NEPOOL zonal energy prices for Sellers Choice
#
pull_prices_nepool_sc <- function(asOfDate, startDate, endDate, HC=NULL, IDM=NULL)
{
  rLog("Pull energy prices NEPOOL SC ...")
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  location    <- c("SC")
  bucket      <- c("PEAK", "OFFPEAK")
  serviceType <- "ENERGY"
  useFutDates <- FALSE
  
  curveData <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  curveData$market <- "NEPOOL"
  curveData$secdbValue <- curveData$value

  curveData
}


###########################################################################
# Pull ONTARIO zonal energy prices 
#
pull_prices_ontario <- function(asOfDate, startDate, endDate, HC, IDM)
{
  rLog("Pull energy prices ONTARIO ...")
  commodity   <- "COMMOD PWR IEMO PHYSICAL"
  location    <- c("ONT")
  bucket      <- c("FLAT", "PEAK", "OFFPEAK", "2X16H", "7X8")
  serviceType <- "ENERGY"
  useFutDates <- FALSE
  
  curveData <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  curveData$market <- "IEMO"
  curveData$secdbValue <- curveData$value
  
  ind <- is.infinite(curveData$secdbValue)
  if (length(ind)>0)
    curveData[ind,"secdbValue"] <- 0
  
  curveData$bucket   <- gsub("^FLAT", "RTC", curveData$bucket)
  
  # add hedge costs, intraday move
  hc <- subset(HC, market=="IEMO")
  curveData <- merge(curveData, hc,  all.x=TRUE)
  curveData <- merge(curveData, IDM, all.x=TRUE)
  
  curveData$value <- curveData$secdbValue + curveData$hedgeCost +
    curveData$intraDayMove
  curveData <- subset(curveData, value != Inf)
  rLog("Done!")
  
  return(curveData)
}



###########################################################################
# Pull RECs for NEPOOL
#
pull_recs_nepool <- function(asOfDate, startDate, endDate, HC=NULL, IDM=NULL)
{
  rLog("Pull RECs for NEPOOL")
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  location    <- c(
    "CT CLASS I", "CT CLASS II", "CT CLASS III",
    "MASS", "MA CLASS II", "MA WTE", "MA AE", "MA SOLAR", "MA SOLAR 2", 
    "RI NEW", "RI EXISTING",
    "ME CLASS I", "ME CLASS II",
    "NH CLASS I", "NH CLASS II", "NH CLASS III", "NH CLASS IV")
  bucket      <- c("FLAT")
  serviceType <- "RENEW"
  useFutDates <- FALSE
  
  curveData <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  curveData$market <- "NEPOOL"

  #curveData <- .replace_hardcoded(HCV["NEPOOL.RENEW.MA SOL.FLAT"], curveData)
  
  map <- data.frame(location=location, teslaLocation=c(
    "CT", "CT", "CT",
    "MA", "MA", "MA", "MA", "MA", "MA", 
    "RI", "RI",
    "ME", "ME",
    "NH", "NH", "NH", "NH"),
                    technology=c(
    "CLASS I", "CLASS II", "CLASS III",                               # CT
    "CLASS I", "CLASS II", "CLASS II WTE", "AE", "SOLAR", "SOLAR 2",  # MA
    "NEW", "EXISTING",                                                # RI
    "CLASS I", "CLASS II",                                            # ME 
    "CLASS I", "CLASS II", "CLASS III", "CLASS IV"))                  # NH
  
  curveData <- merge(curveData, map, all.x=TRUE)
  if (any(is.na(curveData$teslaLocation))){
    sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
      "adrian.dragulescu@constellation.com", "Wrong REC names for CNE marks!",
      "See Congestion/lib.cne.nepool.marks.R")
  }
  curveData$location   <- curveData$teslaLocation
  curveData$teslaLocation <- NULL
  curveData$secdbValue <- curveData$value
  curveData$bucket <- "RTC"
  
  if (!is.null(HC)){
    # add hedge costs, intraday move
    hc <- subset(HC, market=="NEPOOL" & serviceType=="RENEW")
    aux <- strsplit(hc$location, " - ")
    #hc$loc_tech  <- hc$location  # the original one
    hc$location  <- gsub(" +$", "", sapply(aux, "[", 1))
    hc$technology <- gsub(" +$", "", sapply(aux, "[", 2))
    
    curveData <- merge(curveData, hc,  all.x=TRUE)
    curveData$hedgeCost[is.na(curveData$hedgeCost)] <- 0
    curveData$value <- curveData$value + curveData$hedgeCost  
  }
  if (!is.null(IDM)){
   curveData <- merge(curveData, IDM, all.x=TRUE)
   curveData$value <- curveData$value + curveData$intraDayMove
  }
  
  curveData <- subset(curveData, value != Inf)

  curveData$serviceType <- gsub("^RENEW", "REC", curveData$serviceType)
  curveData$bucket  <- gsub("^FLAT", "RTC", curveData$bucket)
  curveData$subtype <- "COMPLIANCE"
  rLog("Done!")
  
  return(curveData)
}


###########################################################################
# Pull vols for NEPOOL
#
pull_vols_nepool <- function(asOfDate, startDate, endDate)
{
  rLog("Pull vols for NEPOOL ...")
  lib    <- "_lib matlab vol fns"
  #commod <- "commod pwx 5x16 physical"   NEPOOL is not maintained
  commod <- "commod pwj 5x16 west physical" 
  VV <- secdb.invoke(lib, 
    "GetDailyVol", commod, startDate, endDate, asOfDate)
  VV <- data.frame(VV)
  names(VV)[1] <- c("month")
  VV$month <- as.Date(format(VV$month, "%Y-%m-01"))
  VV$value <- VV$value + ADDER_OVER_PJM_PEAK_VOLS
  VV$bucket <- "PEAK"

  VV.off <- VV
  VV.off$bucket <- "OFFPEAK"
  VV.off$value <- VV.off$value*RATIO_OFFPEAK_PEAK_VOL
  
  VV <- rbind(VV, VV.off)  # stack'em
  
  VV$market   <- "NEPOOL"
  VV$location <- "MASSHUB"
  VV$serviceType  <- "VOL"
  VV$subtype <- "DAILY"
  rLog("Done!")
  
  return(VV)
}



###########################################################################
# replace the value in X with the correct value.
# value is a list.  Name of the list needs to be split
#
.replace_hardcoded <- function(value, X)
{
  aux <- strsplit(names(value), "\\.")[[1]]
  if (length(aux)==4){
    aux <- structure(aux, names=c("market", "serviceType", "location",
                          "bucket"))
  } else {
    stop("Wrong hardcoded string!")
  }
  aux <- data.frame(as.list(aux), hardCodedValue=value)

  X <- merge(X, aux, all.x=TRUE)
  ind <- which(!is.na(X$hardCodedValue))
  if (length(ind)>0)
    X$value[ind] <- X$hardCodedValue[ind]
  
  return(X)
}

###########################################################################
# Read hedge costs for NEPOOL and ONTARIO from the master spreadsheet
#
read_hedge_costs <- function(startDate, endDate, 
  fname="S:/All/Structured Risk/CNE/PMEast_Retail_Template.xlsx")
{
  rLog("Read hedge costs ...")
  require(xlsx)
  fname <- "S:/All/Structured Risk/CNE/PMEast_Retail_Template.xlsx"
  wb <- loadWorkbook(fname)
  sheets <- getSheets(wb)
  sheet  <- sheets[['Hedging Cost Inputs']]  

  startRow <- 14
  endRow   <- NULL  
  zones <- c("CT", "HUB", "MAINE", "NEMA", "NH", "RI",
    "SEMA", "VT", "WMA")

  months <- readColumns(sheet, 1, 1, startRow, endRow, header=FALSE, colClasses="Date")[,1]

  cnames <- as.vector(outer(zones, c("PEAK", "OFFPEAK"), paste, sep="_"))

  rLog("Pull NEPOOL energy hedge costs ..")
  aux <- readColumns(sheet, 2, 19, startRow, endRow, header=FALSE) # energy hedge costs
  rownames(aux) <- as.character(months)
  colnames(aux) <- cnames
  
  aux <- melt(as.matrix(aux))
  names(aux)[1] <- "month"
  aux[,1] <- as.Date(aux[,1])
  aux <- cbind(aux, do.call(rbind, strsplit(as.character(aux[,2]), "_")))
  names(aux)[(ncol(aux)-1):ncol(aux)] <- c("location", "bucket")
  aux$market <- "NEPOOL"
  aux$serviceType <- "ENERGY"
  aux <- aux[, c("market", "location", "bucket", "serviceType",
                 "month", "value")]
  bux <- subset(aux, bucket=="OFFPEAK")
  bux$bucket <- "7X8"
  aux <- rbind(aux, bux)
  bux$bucket <- "2X16H"
  aux <- rbind(aux, bux)
  
  HC <- aux
  rLog("OK")

  
  rLog("Pull NEPOOL Capacity hedge costs ... ")
  aux <- readColumns(sheet, 21, 29, startRow, endRow, header=FALSE) # capacity hedge costs
  
  rownames(aux) <- as.character(months)
  colnames(aux) <- zones
  aux <- melt(as.matrix(aux))
  names(aux)[1:2] <- c("month", "location")
  aux[,1] <- as.Date(aux[,1])
  aux$market <- "NEPOOL"
  aux$serviceType <- "CAPACITY"      
  aux$bucket <- "FLAT"
  aux <- aux[, c("market", "location", "bucket", "serviceType",
                 "month", "value")]
  #------------------------------------- add ROP hedge costs same as HUB
  rop <- subset(aux, location=="HUB")
  rop$location <- "ROP"
  aux <- rbind(aux, rop)  # add the rop
  HC  <- rbind(HC, aux)
  rLog("OK")

  
  rLog("Pull NEPOOL ForwardReserve hedge costs ... ")
  aux <- readColumns(sheet, 31, 39, startRow, endRow, header=FALSE) # fwdres hedge costs
  rownames(aux) <- as.character(months)
  colnames(aux) <- zones
  aux <- melt(as.matrix(aux))
  names(aux)[1:2] <- c("month", "location")
  aux[,1] <- as.Date(aux[,1])
  aux$market <- "NEPOOL"
  aux$serviceType <- "FWDRES"      
  aux$bucket <- "FLAT"
  aux <- aux[, c("market", "location", "bucket", "serviceType",
                 "month", "value")]
  #------------------------------------- add ROP hedge costs same as RI
  rop <- subset(aux, location=="RI")
  rop$location <- "ROP"
  aux <- rbind(aux, rop)  # add the rop
  HC <- rbind(HC, aux)
  rLog("OK")

  
  rLog("Pull NEPOOL RMR/LSCP hedge costs ... ")
  aux <- readColumns(sheet, 41, 49, startRow, endRow, header=FALSE) # rmr hedge costs
  rownames(aux) <- as.character(months)
  colnames(aux) <- zones
  aux <- melt(as.matrix(aux))
  names(aux)[1:2] <- c("month", "location")
  aux[,1] <- as.Date(aux[,1])
  aux$market <- "NEPOOL"
  aux$serviceType <- "RMR"     
  aux$bucket <- "FLAT"
  aux <- aux[, c("market", "location", "bucket", "serviceType",
                 "month", "value")]
  HC <- rbind(HC, aux)
  rLog("OK")

  
  rLog("Pull NEPOOL RECS hedge costs ... ")
  aux <- readColumns(sheet, 51, 66, startRow, endRow, header=FALSE)   # recs hedge costs ...
  rownames(aux) <- as.character(months)
  colnames(aux) <- toupper(as.character(readColumns(sheet, 51, 66, 9, 9, header=FALSE)))
  aux <- melt(as.matrix(aux))
  names(aux)[1:2] <- c("month", "location")
  aux[,1] <- as.Date(aux[,1], origin="1970-01-01")
  aux$market <- "NEPOOL"
  aux$serviceType <- "RENEW"    
  aux$bucket <- "FLAT"
  aux <- aux[, c("market", "location", "bucket", "serviceType",
                 "month", "value")]
  HC <- rbind(HC, aux)
  rLog("OK")

  
  rLog("Pull NEPOOL LossReimbursement hedge costs ... ")
  aux <- readColumns(sheet, 68, 76, startRow, endRow, header=FALSE) 
  rownames(aux) <- as.character(months)
  colnames(aux) <- zones
  aux <- melt(as.matrix(aux))
  names(aux)[1:2] <- c("month", "location")
  aux[,1] <- as.Date(aux[,1])
  aux$market <- "NEPOOL"
  aux$serviceType <- "LOSSRMB"     
  aux$bucket <- "FLAT"
  aux <- aux[, c("market", "location", "bucket", "serviceType",
                 "month", "value")]
  HC <- rbind(HC, aux)
  rLog("OK")


  rLog("Pull NEPOOL RT Operating Reserves hedge costs ... ")
  aux <- readColumns(sheet, 78, 80, startRow, endRow, header=FALSE)   
  rownames(aux) <- as.character(months)
  colnames(aux) <- c("ROP", "NEMA", "CT")
  aux <- as.data.frame(aux)
  aux$ME   <- aux$ROP
  aux$NH   <- aux$ROP
  aux$RI   <- aux$ROP
  aux$SEMA <- aux$ROP
  aux$WCMA <- aux$ROP
  aux$VT   <- aux$ROP
  aux <- melt(as.matrix(aux))
  names(aux)[1:2] <- c("month", "location")
  aux[,1] <- as.Date(aux[,1])
  aux$market <- "NEPOOL"
  aux$serviceType <- "OPRES_RT"     
  aux$bucket <- "FLAT"
  aux <- aux[, c("market", "location", "bucket", "serviceType",
                 "month", "value")]
  HC <- rbind(HC, aux)
  rLog("OK")
  
  
  rLog("Pull ONTARIO energy hedge costs ... ")
  aux <- readColumns(sheet, 84, 84, startRow, endRow, header=FALSE) 
  rownames(aux) <- as.character(months)
  colnames(aux) <- "ONT"
  aux <- melt(as.matrix(aux))
  names(aux)[1:2] <- c("month", "location")
  aux[,1] <- as.Date(aux[,1])
  aux$market <- "IEMO"
  aux$serviceType <- "ENERGY"    
  aux <- expand.grid.df(aux, data.frame(bucket=c("PEAK", "OFFPEAK", "7X8",
                                          "2X16H")))
  aux <- aux[, c("market", "location", "bucket", "serviceType",
                 "month", "value")]
  
  HC <- rbind(HC, aux)
  rLog("OK")

  names(HC)[which(names(HC)=="value")] <- "hedgeCost"
  HC <- subset(HC, month >= startDate & month <= endDate)
  HC$bucket   <- gsub("^FLAT", "RTC", HC$bucket)
  HC$location <- gsub("^HUB",   "MASSHUB", HC$location)
  HC$location <- gsub("^MAINE", "ME", HC$location)
  HC$location <- gsub("^WMA",   "WCMA", HC$location)

  suppressWarnings(HC$hedgeCost <- as.numeric(HC$hedgeCost))
  HC$hedgeCost[is.na(HC$hedgeCost)] <- 0   # set to zero if does not exist
  rLog("Done!")
  
  return(HC)
}


###########################################################################
# set intraday move
# This function is a stub for now...
#
read_intraday_move <- function(startDate, endDate)
{
  IDM <- data.frame(month=seq(startDate, endDate, by="1 month"),
                    intraDayMove=0)
  
  return(IDM)
}


###########################################################################
# ARR prices get scaled by a ratio because SMIS uses historical peak load 
# vs. coincident peak. 
## ratio <- read.csv(paste(DIR, "ratio_2010-02-26.csv", sep=""))
## asOfDate <- Sys.Date()
## maxDate  <- as.Date("2018-12-01")
## arr <- pull_arr_nepool(asOfDate, maxDate)
## arr <- scale_arr_prices(arr, ratio)
## res <- cast(arr, month ~ location, I, fill=NA)
#
scale_arr_prices <- function(arr, ratio)
{
  # NOT USED ANYMORE -- WILL USE A SIMPLE 0.90 ratio!
  arr$mon <- as.numeric(format(arr$month, "%m"))
  arr <- merge(arr, ratio, by="mon")
  arr$value <- arr$value * arr$ratio
  arr <- arr[, -which(names(arr) %in% c("mon", "ratio"))]
  arr <- arr[order(arr$location, arr$month, arr$bucket), ]

  ## # for 48 months out, cut down arr prices by 0.5!
  ## # outMonth <- seq(as.Date(format(arr$asOfDate[1], "%Y-%m-01")),
  ## #  by="48 months", length.out=2)[2]
  ## outMonth <- as.Date("2014-01-01")
  ## ind <- which(arr$month > outMonth)
  ## arr$value[ind] <- 0.5*arr$value[ind]
  
  ## outMonth <- as.Date("2016-01-01")
  ## ind <- which(arr$month > outMonth)
  ## arr$value[ind] <- 0.5*arr$value[ind]
  
  return(arr)
}


###########################################################################
#
writecsv_ancillaries_nepool <- function(AA, ARR)
{
  out <- AA[,c("market", "location", "serviceType", "bucket",
    "subtype", "value", "month")]
  
  out <- rbind(out, ARR[,c("market", "location", "serviceType", "bucket",
    "subtype", "value", "month")])

  out$mmddyyyy <-  format(out$month, "%m/%d/%Y")
  out$serviceTypeDesc <- "OFFER"
  out$technology <- ""
  out <- out[,c("market", "location", "serviceType", "serviceTypeDesc",
    "bucket", "subtype", "value", "mmddyyyy", "technology")]
  out$value <- sprintf("%f", out$value)
  
  timestamp <- format(Sys.time(), "%Y%m%d_%H%M%S")
  fname <- paste(DIROUT, "ANCILLARY_NEPOOL_", timestamp, ".txt", sep="")
  out <- c("User Name: PROCMON",
     "Curve Type: Offer",      
     capture.output(write.table(out, file="",
     quote=FALSE, col.names=FALSE, row.names=FALSE, sep=",")))
  writeLines(out, con=fname)
  rLog("Wrote", fname)
}

###########################################################################
#
writecsv_basis_nepool <- function(NN)
{
  Sys.sleep(3)
  out <- subset(NN, bucket %in% c("PEAK", "2X16H", "7X8"))
  names(out)[which(names(out)=="secdbValue")] <- "value"
  out$mmddyyyy <- format(out$month, "%m/%d/%Y")
  out <- cast(out, market + serviceType + bucket +
              mmddyyyy ~ location, I, fill=0)
  ih <- which(names(out)=="MASSHUB")
  hub <- out[,"MASSHUB"]; out <- out[, -ih]
  out[,5:12] <- out[,5:12] - hub
  out <- melt(data.frame(out), id=1:4)
  names(out)[5] <- "location"
  out$serviceType <- "ENERGY"      
  out$subtype <- ""
  out$technology <- ""
  out$serviceTypeDesc <- "BASIS"
  
  out <- out[,c("market", "location", "serviceType", "serviceTypeDesc",
    "bucket", "subtype", "value", "mmddyyyy", "technology")]
  out$value <- sprintf("%f", out$value)
  
  timestamp <- format(Sys.time(), "%Y%m%d_%H%M%S")
  fname <- paste(DIROUT, "ENERGY_NEPOOL_", timestamp, ".txt", sep="")
  out <- c("User Name: PROCMON",
     "Curve Type: Basis",                 
     capture.output(write.table(out, file="",
     quote=FALSE, col.names=FALSE, row.names=FALSE, sep=",")))
  writeLines(out, con=fname)
  rLog("Wrote", fname)
}


###########################################################################
#
writecsv_capacity_nepool <- function(CC)
{
  out <- CC
  out$mmddyyyy <-  format(out$month, "%m/%d/%Y")
  out$technology <- ""
  out$serviceTypeDesc <- "OFFER"  
  out <- out[,c("market", "location", "serviceType", "serviceTypeDesc",
    "bucket", "subtype", "value", "mmddyyyy", "technology")]
  out$value <- sprintf("%f", out$value)

  timestamp <- format(Sys.time(), "%Y%m%d_%H%M%S")
  fname <- paste(DIROUT, "CAPACITY_NEPOOL_", timestamp, ".txt", sep="")
  out <- c("User Name: PROCMON",
     "Curve Type: Offer",                 
     capture.output(write.table(out, file="",
     quote=FALSE, col.names=FALSE, row.names=FALSE, sep=",")))
  writeLines(out, con=fname)
  rLog("Wrote", fname)
}



###########################################################################
#
writecsv_energy_nepool <- function(NN)
{
  out <- subset(NN, bucket %in% c("PEAK", "2X16H", "7X8"))
  out$mmddyyyy <- format(out$month, "%m/%d/%Y")
  out$value <- sprintf("%f", out$value)
  out$subtype <- ""
  out$technology <- ""
  out$serviceTypeDesc <- "OFFER"
  out <- out[,c("market", "location", "serviceType", "serviceTypeDesc",
    "bucket", "subtype", "value", "mmddyyyy", "technology")]

  timestamp <- format(Sys.time(), "%Y%m%d_%H%M%S")
  fname <- paste(DIROUT, "ENERGY_NEPOOL_", timestamp, ".txt", sep="")
  out <- c("User Name: PROCMON",
     "Curve Type: Offer",                 
     capture.output(write.table(out, file="",
     quote=FALSE, col.names=FALSE, row.names=FALSE, sep=",")))
  writeLines(out, con=fname)
  rLog("Wrote", fname)  
}


###########################################################################
#
writecsv_energy_ontario <- function(OO)
{
  out <- subset(OO, bucket %in% c("PEAK", "OFFPEAK"))
  out$mmddyyyy <- format(out$month, "%m/%d/%Y")
  out$value <- sprintf("%f", out$value)
  out$subtype <- ""
  out$technology <- ""
  out$serviceTypeDesc <- "OFFER"
  out <- out[,c("market", "location", "serviceType", "serviceTypeDesc",
    "bucket", "subtype", "value", "mmddyyyy", "technology")]

  timestamp <- format(Sys.time(), "%Y%m%d_%H%M%S")
  fname <- paste(DIROUT, "ENERGY_IEMO_", timestamp, ".txt", sep="")
  out <- c("User Name: PROCMON",
     "Curve Type: Offer",      
     capture.output(write.table(out, file="",
     quote=FALSE, col.names=FALSE, row.names=FALSE, sep=",")))
  writeLines(out, con=fname)
  rLog("Wrote", fname)  
}


###########################################################################
#
writecsv_hedgecosts_ancillaries <- function(AA, ARR)
{
  Sys.sleep(3)
  out <- AA
  out$mmddyyyy <- format(out$month, "%m/%d/%Y")
  out$technology <- ""
  out$serviceTypeDesc <- "HEDGECOST"
  out <- out[,c("market", "location", "serviceType", "serviceTypeDesc",
    "bucket", "subtype", "hedgeCost", "mmddyyyy", "technology")]
  names(out)[which(names(out)=="hedgeCost")] <- "value"
  out$value <- sprintf("%f", out$value)

  timestamp <- format(Sys.time(), "%Y%m%d_%H%M%S")
  fname <- paste(DIROUT, "ANCILLARY_NEPOOL_", timestamp, ".txt", sep="")
  out <- c("User Name: PROCMON",
     "Curve Type: HedgeCost",      
     capture.output(write.table(out, file="",
     quote=FALSE, col.names=FALSE, row.names=FALSE, sep=",")))
  writeLines(out, con=fname)
  rLog("Wrote", fname)  
  
}


###########################################################################
#
writecsv_hedgecosts_capacity <- function(CC)
{
  Sys.sleep(3)
  out <- CC
  out$mmddyyyy <- format(out$month, "%m/%d/%Y")
  out$technology <- ""
  out$serviceTypeDesc <- "HEDGECOST"
  out <- out[,c("market", "location", "serviceType", "serviceTypeDesc",
    "bucket", "subtype", "hedgeCost", "mmddyyyy", "technology")]
  names(out)[which(names(out)=="hedgeCost")] <- "value"
  out$value <- sprintf("%f", out$value)

  timestamp <- format(Sys.time(), "%Y%m%d_%H%M%S")
  fname <- paste(DIROUT, "CAPACITY_NEPOOL_", timestamp, ".txt", sep="")
  out <- c("User Name: PROCMON",
     "Curve Type: HedgeCost",      
     capture.output(write.table(out, file="",
     quote=FALSE, col.names=FALSE, row.names=FALSE, sep=",")))
  writeLines(out, con=fname)
  rLog("Wrote", fname)  
  
}


###########################################################################
#
writecsv_hedgecosts_nepool <- function(NN)
{
  Sys.sleep(3)
  out <- subset(NN, bucket %in% c("PEAK", "2X16H", "7X8"))
  out$mmddyyyy <- format(out$month, "%m/%d/%Y")
  out$subtype <- ""
  out$technology <- ""
  out$serviceTypeDesc <- "HEDGECOST"
  out <- out[,c("market", "location", "serviceType", "serviceTypeDesc",
    "bucket", "subtype", "hedgeCost", "mmddyyyy", "technology")]
  names(out)[which(names(out)=="hedgeCost")] <- "value"
  out$value <- sprintf("%f", out$value)

  timestamp <- format(Sys.time(), "%Y%m%d_%H%M%S")
  fname <- paste(DIROUT, "ENERGY_NEPOOL_", timestamp, ".txt", sep="")
  out <- c("User Name: PROCMON",
     "Curve Type: HedgeCost",      
     capture.output(write.table(out, file="",
     quote=FALSE, col.names=FALSE, row.names=FALSE, sep=",")))
  writeLines(out, con=fname)
  rLog("Wrote", fname)  
  
}


###########################################################################
#
writecsv_hedgecosts_ontario <- function(OO)
{
  Sys.sleep(3)
  out <- subset(OO, bucket %in% c("PEAK", "OFFPEAK"))
  out$mmddyyyy <- format(out$month, "%m/%d/%Y")
  out$subtype <- ""
  out$technology <- ""
  out$serviceTypeDesc <- "HEDGECOST"
  out <- out[,c("market", "location", "serviceType", "serviceTypeDesc",
    "bucket", "subtype", "hedgeCost", "mmddyyyy", "technology")]
  names(out)[which(names(out)=="hedgeCost")] <- "value"
  out$value <- sprintf("%f", out$value)

  timestamp <- format(Sys.time(), "%Y%m%d_%H%M%S")
  fname <- paste(DIROUT, "ENERGY_IEMO_", timestamp, ".txt", sep="")
  out <- c("User Name: PROCMON",
     "Curve Type: HedgeCost",      
     capture.output(write.table(out, file="",
     quote=FALSE, col.names=FALSE, row.names=FALSE, sep=",")))
  writeLines(out, con=fname)
  rLog("Wrote", fname)  
  
}


###########################################################################
#
writecsv_hedgecosts_recs <- function(RR)
{
  Sys.sleep(3)
  out <- RR
  out$mmddyyyy <- format(out$month, "%m/%d/%Y")
  out$serviceTypeDesc <- "HEDGECOST"
  out <- out[,c("market", "location", "serviceType", "serviceTypeDesc",
    "bucket", "subtype", "hedgeCost", "mmddyyyy", "technology")]
  names(out)[which(names(out)=="hedgeCost")] <- "value"
  out$value <- sprintf("%f", out$value)

  timestamp <- format(Sys.time(), "%Y%m%d_%H%M%S")
  fname <- paste(DIROUT, "REC_NEPOOL_", timestamp, ".txt", sep="")
  out <- c("User Name: PROCMON",
     "Curve Type: HedgeCost",                 
     capture.output(write.table(out, file="",
     quote=FALSE, col.names=FALSE, row.names=FALSE, sep=",")))
  writeLines(out, con=fname)
  rLog("Wrote", fname)  
  
}



###########################################################################
# for uploading to TESLA
#
writecsv_recs_nepool <- function(RR)
{
  out <- RR
  out$mmddyyyy <- format(out$month, "%m/%d/%Y")
  out$serviceTypeDesc <- "OFFER"
  out <- out[,c("market", "location", "serviceType", "serviceTypeDesc",
    "bucket", "subtype", "value", "mmddyyyy", "technology")]
  out$value <- sprintf("%f", out$value)

  timestamp <- format(Sys.time(), "%Y%m%d_%H%M%S")
  fname <- paste(DIROUT, "REC_NEPOOL_", timestamp, ".txt", sep="")
  out <- c("User Name: PROCMON",
     "Curve Type: Offer",      
     capture.output(write.table(out, file="",
     quote=FALSE, col.names=FALSE, row.names=FALSE, sep=",")))
  writeLines(out, con=fname)
  rLog("Wrote", fname)  
}


###########################################################################
# for uploading to TESLA
#
writecsv_vols_nepool <- function(VV)
{
  out <- VV
  out$mmddyyyy <- format(out$month, "%m/%d/%Y")
  out$subtype <- "DAILY"
  out$technology <- ""
  out$serviceTypeDesc <- "OFFER"
  out <- out[,c("market", "location", "serviceType", "serviceTypeDesc",
    "bucket", "subtype", "value", "mmddyyyy", "technology")]
  out$value <- sprintf("%f", out$value)
 
  timestamp <- format(Sys.time(), "%Y%m%d_%H%M%S")
  fname <- paste(DIROUT, "VOL_NEPOOL_", timestamp, ".txt", sep="")
  out <- c("User Name: PROCMON",
     "Curve Type: Offer",      
     capture.output(write.table(out, file="",
     quote=FALSE, col.names=FALSE, row.names=FALSE, sep=",")))
  writeLines(out, con=fname)
  rLog("Wrote", fname)  
}


###########################################################################
# fname <- paste(DIROUT, "audit_2011-01-18.csv", sep="")
#
writecsv_audit_file <- function(fname, NN, CC, AA, ARR, RR)
{
   NN$subtype    <- ""
   NN$technology <- ""
   CC$technology <- ""
   AA$technology <- ""
   ARR$technology <- ""
   ARR$intraDayMove <- 0

   res <- rbind(NN, AA, ARR, CC, RR)
   write.csv(res, file=fname, row.names=FALSE)
}


###########################################################################
# Visualize the energy
#
.plot_curves <- function( NN, ARR )
{
  require(lattice)

  # energy 
  xyplot(value ~ month|bucket, data=NN, groups=NN$location,
         type="l", subset=bucket %in% c("PEAK", "2X16H", "7X8"),
         layout=c(1,3),
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=4))

  # ARRs
  xyplot(value ~ month|bucket, data=ARR, groups=ARR$location,
         type="l", 
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=4))

  # FWDRES all values are very close together
  xyplot(value ~ month|bucket, data=AA, groups=location,
         type="l",
         subset=subtype == "FWDRES", 
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=5))
  
  # FWDRES all values are very close together
  xyplot(value ~ month|bucket, data=AA, groups=location,
         type="l",
         subset=subtype == "DRUPLIFT", 
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=4))
 

  
  
}
 










