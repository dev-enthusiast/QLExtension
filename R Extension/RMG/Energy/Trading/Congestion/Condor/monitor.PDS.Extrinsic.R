## script used by procmon job RMG/R/Reports/Energy/Trading/Congestion/monitor.PDS.Extrinsic
## script queries simcube for NEPOOL gen extrinsic and also fetches the ATM HR Call option value, puts them in an email
## fetchExtrinscForComponent - aggregate exrtinsic value by year
## fetchYrHR - get heat rate by year
## save.monthly.Data - save monthly extrimsic data
## report.PDS.Extrinsic - create email report
## report.KLEEN.Oil.PDS - create report about Kleen oil burn

## Aggregate yearly  extrinsic value from SimCube query
fetchExtrinscForComponent <- function(xNow, xPrev, component, aggr="sum"){

    colnames <- c("month", "bucket", "flatintr", "shapeintr", "full")

    ## filter component from Simcube data
    nowComp <- dcast(xNow,month+bucket~reportgroup, sum, value.var = c(component), na.rm = TRUE)
    prevComp <- dcast(xPrev,month+bucket~reportgroup, sum, value.var = c(component ), na.rm = TRUE)

    names(nowComp) <- colnames
    names(prevComp) <- colnames

    nowComp$extr <- nowComp$full-nowComp$shapeintr
    prevComp$extr <- prevComp$full-prevComp$shapeintr

    res <- merge(nowComp, prevComp, by=c("month", "bucket"))
    res <- ddply(res, .(Year = format(month, "%Y"), bucket), .fun = function(x){
        if (tolower(aggr)=="sum")
            {
                colSums(x[,3:ncol(x)])
            } else {
                colMeans(x[,3:ncol(x)])
            }
    })
    names(res) <- gsub( "\\.x", "_now", names(res))
    names(res) <- gsub( "\\.y", "_prev", names(res))
    res <- subset(res, res$bucket!="OFFPEAK")
    res$extr_pct_now<- res$extr_now*100/res$full_now
    res$extr_pct_prev<- res$extr_prev*100/res$full_prev

    res <- res[, c("Year", "bucket", "extr_now", "extr_prev", "extr_pct_now", "extr_pct_prev", "full_now", "full_prev")]
    res$extr_diff <- res$extr_now- res$extr_prev
#    res[,3:ncol(res)] <- round(res[,3:ncol(res)], digits = 2)
    res[,3:ncol(res)] <- prettyNum(round(res[,3:ncol(res)], digits = 2), big.mark = ",", nsmall = 2, preserve.width = "individual", right=TRUE )
    names(res) <- gsub("_", "_", toupper(names(res)))
    res
}

## fetch yearly Heat Rate ATM Call option value from SecDb
fetchYrHR <- function(yr){
    begMonth <- max(nextMonth(asOfDate), as.Date(paste(yr,"-01-01", sep="" )))
    endMonth <- as.Date(paste(yr,"-12-31", sep=""))

    NumMth <- min(12, length(seq(begMonth, endMonth, by="1 month")))

    HROptNow <- secdb.HR.OptPremium(region="NEPOOL", loc="hub", gasCommod="NG ALGCG", gasLoc="GAS-DLY MEAN", bucket="5x16", freq="Daily", OptType="Call", StartDate=begMonth, NumMth=NumMth, Strike=0, HR="ATM", ExpRule="-1b", AsOfDate=asOfDate, Aggregate = TRUE)
    HROptNow$PwrOpt <- as.double(secdb.getOptionPremium(region="NEPOOL", loc="hub", bucket="5x16", ExpRule="-1b", Strike=HROptNow$power, StartDate=begMonth, NumMth=NumMth, AsOfDate=asOfDate, Aggregate=TRUE))
    HROptNow$AsOfDate <- asOfDate
    HROptNow$month <- NULL
    HROptNow$Year <- yr


    HROptPrev <- secdb.HR.OptPremium(region="NEPOOL", loc="hub", gasCommod="NG ALGCG", gasLoc="GAS-DLY MEAN", bucket="5x16", freq="Daily", OptType="Call", StartDate=begMonth, NumMth=NumMth, Strike=0, HR="ATM", ExpRule="-1b", AsOfDate=prevDate, Aggregate = TRUE)
    HROptPrev$PwrOpt <- as.double(secdb.getOptionPremium(region="NEPOOL", loc="hub", bucket="5x16", ExpRule="-1b", Strike=HROptPrev$power, StartDate=begMonth, NumMth=NumMth, AsOfDate=prevDate, Aggregate=TRUE))
    HROptPrev$AsOfDate <- prevDate
    HROptPrev$month <- NULL
    HROptPrev$Year <- yr
    ret <- rbind( HROptNow, HROptPrev )
}

## save csv file
save.monthly.Data <- function(allData){
    allData <- dcast(allData,month+bucket+reportdateid~reportgroup, sum, value.var = c("rnf"), na.rm = TRUE)
    names(allData) <- c("Month", "Bucket", "AsOfDate", "Flat Intr.", "Shaped Intr.", "Full Value")
    allData$AsOfDate <- as.Date(as.character(allData$AsOfDate), format="%Y%m%d")
    allData["Shaped Extr"] <- allData["Full Value"] - allData["Shaped Intr."]
    asDate <- unique(allData$AsOfDate)
#    allData[,4:ncol(allData)] <- prettyNum(round(allData[,4:ncol(allData)], digits = 2), big.mark = ",", nsmall = 2, preserve.width = "individual", right=TRUE )
    write.csv(allData, file=paste(format(asDate, "%Y_%m_%d"), "_Extr_Report.csv", sep=""))
}

## create email report
report.PDS.Extrinsic <- function( asOfDate, prevDate){
    componentsToCheck <- c("averagedeltamw", "rnf")
    compUnits <- c("MW", "$")
    compAggr <- c("mean", "sum")
    rLog("\n\n")
    setwd("S:/All/Structured Risk/NEPOOL/Position Management/position/simcube/Extrinsic/")
    rLog("Querying SimCube for Gen Extrinsic Data\n")
    genBooks <- as.array(read.csv( "Booknames.csv", strip.white = TRUE,header = FALSE)$V1)
    recList <- as.array(read.csv("EmailList.csv", strip.white = TRUE,header = FALSE)$V1)
    xNow <- get_Intr_vs_Extr_By_Book(asOfDate, region="New England", books = genBooks)
    xPrev <- get_Intr_vs_Extr_By_Book(prevDate, region="New England", books = genBooks)

    save.monthly.Data( xNow )
    allYears <- as.double(unique(format( xNow$month, "%Y")))
    rLog("Done Querying Simcube data \n")
    out <- ""
    for (cIdx in seq_along(componentsToCheck)){
        thisC <- componentsToCheck[cIdx]
        thisUnit <- compUnits[cIdx]

        thisCData <- fetchExtrinscForComponent(xNow, xPrev, thisC, aggr=compAggr[cIdx])
        out <- c(out, paste("Extrinsic Differnces: ", toupper(thisC), " (", thisUnit, "):\t"))

        text <-  ddply( thisCData, .(BUCKET), .fun=function(x){ data.frame( "extr_chg" =do.call( compAggr[cIdx], as.list(as.double(gsub("[,]","",x$EXTR_DIFF))))) } )
        out <- c(out, paste("Total extrinsic change for term : ", compUnits[cIdx]), prettyNum( text[text$BUCKET=="7X24", c("extr_chg")], big.mark = ",", nsmall =2), "\n", sep="")

        out <- c(out, capture.output(print(thisCData, quote=FALSE, right =TRUE)))

        out <- c( out, "\n" )
    }

    ## Fetch the HR ATM Call option values in secdb
    out <- c(out, "ATM 5x16 Call HR & Power option value from SecDb\n")
    allHROpt <- do.call( rbind, Map(fetchYrHR, allYears ))
    allHROpt <- allHROpt[c(ncol(allHROpt),ncol(allHROpt)-1,1:(ncol(allHROpt)-2))]
    allHROpt[,3:ncol(allHROpt)] <- prettyNum(round(allHROpt[,3:ncol(allHROpt)], digits = 2), big.mark = ",", nsmall = 2, preserve.width = "individual", right=TRUE )
    out <- c(out, capture.output(print(allHROpt, quote=FALSE, right=TRUE)))
    out <- c(out, "\n\n")


    out <- c(out, "\n\nContact: Ankur Salunkhe",
             "Procmon job: RMG/R/Reports/Energy/Trading/Congestion/monitor.PDS.Extrinsic")
    rLog("Email results")
    sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
              recList,
              paste("NEPOOL Gen Extrinsic Report for ", as.character(asOfDate), " vs ", as.character(prevDate)), out)
}

###  Create the oil burn report for a given unit. Conversion factor from SecDb for FO1, HO convFac = 5.82
report.Unit.Oil.PDS <- function( asOfDate, book = "NPKLEEN", convFac= 6.29 ){
    unitOil <- get_fuel_burn_data( asOfDate, books = book )

    names(unitOil) <- c("AS_Date", "BUCKET_PDS", "FUEL_TYPE", "MW_EXP", "MMBTU_EXP", "MMBTU_DELTA", "MONTH", "BUCKET")

    unitOil <- subset( unitOil, tolower(unitOil$BUCKET) != "offpeak")
    oilMths <- subset( unitOil, abs(unitOil$MMBTU_EXP)>0 & grepl("oil", unitOil$FUEL_TYPE, ignore.case=TRUE) & unitOil$BUCKET == "7X24" & !is.na(unitOil$MMBTU_EXP))$MONTH

    ## Get Expected MW
    powerExp <- dcast(unitOil, MONTH~BUCKET, sum, value.var=c("MW_EXP"), na.rm=TRUE)
    names(powerExp) <- paste(names(powerExp), "_MW_EXP", sep="")
    names(powerExp)[1] <- "MONTH"
    powerExp <- powerExp[,c(1,2,3,5,4)]

    ## Get oil percentage by Bucket
    fuelBurn <- dcast(unitOil, MONTH+BUCKET~FUEL_TYPE, sum, value.var=c("MMBTU_EXP"), na.rm=TRUE)
    names(fuelBurn) <- gsub( "NATURAL ", "", toupper(names(fuelBurn)))
    names(fuelBurn) <- gsub( "HEAT ", "", toupper(names(fuelBurn)))
    fuelBurn <- subset(fuelBurn, fuelBurn$MONTH %in% oilMths)
    fuelBurn$OIL_PCT <- fuelBurn$OIL*100/(fuelBurn$GAS+fuelBurn$OIL)

    ## Get Total oil and gas burn
    burn_ATC<- subset(fuelBurn, fuelBurn$BUCKET == "7X24")[,c("MONTH", "OIL", "GAS")]
    names(burn_ATC) <- c("MONTH", "7x24_OIL_MMBTU", "7x24_GAS_BURN")
    burn_ATC["7x24_Oil_BBL"] <- burn_ATC["7x24_OIL_MMBTU"] / convFac

    fuelBurn <- dcast(melt( fuelBurn[,c("MONTH", "BUCKET", "OIL_PCT")], id.vars=c("MONTH", "BUCKET")), MONTH~BUCKET+variable, value.var=c("value"), na.rm=TRUE )
    fuelBurn <- fuelBurn[,c(1,2,3,5,4)]

    ## merge and format all data to be reported
    fuelBurn <- merge( fuelBurn, burn_ATC[ c(1, 4, 2, 3)], by="MONTH")
    fuelBurn <- merge( fuelBurn, powerExp, by="MONTH")
    fuelBurn[,2:ncol(fuelBurn)] <- prettyNum(round(fuelBurn[,2:ncol(fuelBurn)], digits = 2), big.mark = ",", nsmall = 2, preserve.width = "individual", right=TRUE )
    out <- ""
    out <- c(out, paste( book, "oil burn data from PDS :\n"))
    out <- c(out, capture.output(print(fuelBurn, quote=FALSE, right=TRUE)))
    out <- c(out, "\n\n")

    out
}

options(width=600)  # make some room for the output
options(stringsAsFactors=FALSE)
require(methods); require(CEGbase); require(CEGterm); require(SecDb); require(PM); require(plyr);
require(PM); require(reshape2);require(RODBC)
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.simcube.R")


rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/monitor.PDS.Extrinsic.R")
rLog("Start at ", as.character(Sys.time()))
returnCode <- 0    # succeed = 0
asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b", calendar = "CPS-BAL"))
prevDate <- as.Date(secdb.dateRuleApply(asOfDate, "-1b", calendar = "CPS-BAL"))
report.PDS.Extrinsic(asOfDate, prevDate)
oilOutK <- report.Unit.Oil.PDS(asOfDate, book="NPKLEEN", convFac=6.29)
oilOutM <- report.Unit.Oil.PDS(asOfDate, book="MYSTIC7M", convFac=5.82)

out <- c(oilOutK, oilOutM, "\n\nContact: Ankur Salunkhe",
         "Procmon job: RMG/R/Reports/Energy/Trading/Congestion/monitor.PDS.Extrinsic")
rLog("Email results")
sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
          paste(c("ankur.salunkhe@constellation.com",
                  "kate.norman@constellation.com"), sep="", collapse=","),
          paste("NEPOOL Oil burn data for :  ", as.character(asOfDate), sep=""), out)

rLog("Done at ", as.character(Sys.time()))
if( interactive() ){
    print( returnCode )
} else {
    q( status = returnCode )
}
