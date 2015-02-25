# Library to read runs from PDS DHI Runs
# Current functionality involves plotting impled shape for a given commod.
# Additional functionality to be added is check Generation dispatch shaping, Gen Payoff, etc by bucket/hour
# readPdsDhi                - Read a PDS Shi file into a Data frame
# .getRiskFreeRate          - get Risk free rate from secdb
# getImplVolandOptPrcForCommod - Get implied vol and option cost from PDS price Sim
# getImpliedHourlyShape     - Get implied hourly shape for a given commodity
# filterSimForMthBuc        - Helper function to filter PDS sim by bucket and month

require(plyr)
require(RQuantLib)
require(CEGbase)
require(SecDb)
require(PM)
require(lattice)
require(reshape2)
require(zoo)
require(latticeExtra)

#Read PDS DHI file and return a Data Frame
readPdsDhi <- function(filename)
{
    dhiRaw <- read.csv(filename)
    dhiRaw$Date <- as.Date(dhiRaw$Date, origin = "1899-12-30" )
    tempTime <- paste( as.character(dhiRaw$Date), as.character(dhiRaw$Hour), " ")
    dhiRaw$Time <- as.POSIXct(tempTime, "%Y-%m-%d %H", tz="EST" )
    dhiRaw
}

#Risk Free Rate Calculation
.getRiskFreeRate <- function(discountFactor, timeToMat){
    return(-log(discountFactor) / timeToMat)
}

#Get implied vol and option price from PDS prices for the commod in question. Col num indicates the commod to process
getImplVolandOptPrcForCommod <- function(dhiDat, month = NULL, colNum = 4, Moneyness = c( 0.5, 1, 1.5 ), optType = "Call", asDate = Sys.Date(), Bucket = "7x24", region = "NEPOOL"){
    simData <- filterSimForMthBuc(dhiDat, month, Bucket, region, colNum)
#    browser()
    simData$Price <- simData$Agg1
    x <- as.data.frame(simData)
    fwd <- mean(simData$Price)
    Df <- secdb.getDiscountFactor(asDate, month)

    YTM <- as.integer((month-asDate))/365.25
    RiskFreeRate <- .getRiskFreeRate(Df[, 2], YTM)
    optList <- c()
    volList <- c()
    for (thisMon in Moneyness){
        strike <- thisMon*fwd
        dailyAvg <- ddply(simData, .(Date = as.Date(Time, format = "%Y-%m", tz = "EST"), Sim), summarise, DlyPrc = mean(Price))
        if (optType == "Call"){
            optPay <- mean(pmax(dailyAvg$DlyPrc-strike, 0))
            optList <- c(optList, optPay)
        } else {
            optPay <- mean(pmax(strike-dailyAvg$DlyPrc, 0))
            optList <- c(optList, optPay)
        }
        implV <- EuropeanOptionImpliedVolatility(tolower(optType), value = optPay, strike = strike, underlying = fwd, riskFreeRate = RiskFreeRate, maturity = YTM, dividendYield = 0, volatility = 0.4)
        volList <- c(volList, implV$impliedVol)
    }
    retList <- list("Fwd" = fwd, "OptCost" = optList, "ImplVol" = volList, Moneyness = Moneyness)
}

#
getHrlyGenData <- function(dhiDat, mth=NULL, colNum = 8, unitCol = 4, unitize = FALSE, allSims = TRUE, asData = asDate, bucket = bucket, region = "NEPOOL", pcts=c(0.95, 0.05), sumColumns = TRUE){
    ## if (unitize)
    ##     colNums <- c(colNum, unitCol)
    ## else
    ##     colNums <- colNum

    if (sumColumns){
        simData <- filterSimForMthBuc(dhiDat, mth, bucket, region, colNum, sumColumns = sumColumns )
        simData$Gen <- simData$sum
    }

    if (unitize){
        simDataUnit <- filterSimForMthBuc(dhiDat, mth, bucket, region, unitCol, sumColumns = sumColumns )
        if (sumColumns)
            simDataUnit$Agg1 <- simDataUnit$sum
        simData$Gen = simData$Gen/simDataUnit$Agg1
    }
    simData$Gen[is.na(simData$Gen)] <- 0
    toUseIdx <- which(pmatch(names(simData), "Gen", nomatch=0)>0)
    retList <- getHourlyStatsForCol(simData, colNum = toUseIdx, pcts = pcts, allSims = allSims)
    retList
}

#Get hourly implied hourly shape for a given month from PDS simulations
getImpliedHourlyShape <- function(dhiDat, mth = NULL, commod = "HR", pwrCol = 4, gasCol = 7, pcts = c(0.95, 0.05), bucket = "7x24", region = "NEPOOL", asDate = Sys.Date(), moneyness=c(0.75, 1, 1.25), GetOption = FALSE, allSims = FALSE){
    simData <- filterSimForMthBuc(dhiDat, mth, bucket, region, c(pwrCol, gasCol))
    simData$Power <- simData$Agg1
    simData$Gas <- simData$Agg2
    simData$HR <- simData$Power/simData$Gas


    if (tolower(commod)=="hr")
        toUseIdx <- which(pmatch(names(simData), "HR", nomatch=0)>0)
    if (tolower(commod)=="gas")
        toUseIdx <- which(pmatch(names(simData), "Gas", nomatch=0)>0)
    if (tolower(commod)=="power")
        toUseIdx <- which(pmatch(names(simData), "Power", nomatch=0)>0)

    retList <- getHourlyStatsForCol(simData, colNum = toUseIdx, pcts = pcts, allSims = allSims)

    if (GetOption){
        ##Get Call Option Data
        OptDataC <- getImplVolandOptPrcForCommod(simData, mth, colNum = toUseIdx, Moneyness = moneyness, Bucket = bucket, region = region, optType = "Call", asDate = asDate)

        ##Get Put Option Data
        OptDataP <- getImplVolandOptPrcForCommod(simData, mth, colNum = toUseIdx, Moneyness = moneyness, Bucket=bucket, region = region, optType = "Put", asDate = asDate)

        retList <- c(retList, "Call" = list(OptDataC), "Put" = list(OptDataP))
    }
    retList
}


# Get hourly sim data for a column of the DHI data frame
getHourlyStatsForCol <- function( simData, colNum = 4, pcts = c(0.95, 0.05), allSims = FALSE ){
    ##Get hourly avg and percentile of Commodity
    getPctsHR <- function(x, pcts = c(0.95, 0.05), colNum = length(x)){
        ##    browser()
        if (!length(pcts) == 0){
            thisP <- as.data.frame.list(quantile(x[, colNum], pcts, na.rm = TRUE))
            names(thisP) <- paste0("P", pcts*100)
        }
        thisP$Mean <- mean(x[, colNum])
        thisP
    }

    HrlyHR <- ddply(simData, c("Hour"), .fun = getPctsHR, pcts, colNum)
    retList <- list("Shaping" = HrlyHR)
    if(allSims){
        BySim <- ddply(simData, c("Hour", "Sim"), .fun = function(x, col){
            c("ShapeBySim" = mean(x[[col]]))
          }, colNum)

        retList <- c(retList, "BySim"= list(BySim))
    }
}

#Filter the dhi data frame for a given month bucket and column
filterSimForMthBuc <- function(dhiDat, month = NULL, bucket, region, columns, sumColumns = FALSE){
    ##browser()
    if (!is.null(month)){
        dhiDat <- subset(dhiDat, as.Date(paste(format(Date, "%Y-%m"), "-01", sep = ""))==month)
    }

    buckethrs <- secdb.getHoursInBucket(region, bucket, min(dhiDat$Date), max(dhiDat$Date))
    dhiDat <- subset(dhiDat, Time %in% buckethrs$time)
    colName <- names(dhiDat)
    sIdx <- which(pmatch(colName, "Sim", nomatch = 0 )>0)
    tIdx <- which(pmatch( colName, "Time", nomatch = 0 )>0)
    hIdx <- which(pmatch( colName, "Hour", nomatch = 0 )>0)
    dIdx <- which(pmatch( colName, "Date", nomatch = 0 )>0)

    simData <- dhiDat[, c(sIdx, tIdx, hIdx, dIdx), drop = FALSE]
    if (sumColumns)
        simData$sum <- 0
    ##    browser()
    for (cIx in 1:length(columns)){
        temp <- as.matrix(dhiDat[, columns[cIx]])
        temp[temp == "-"] <- 0
        dhiDat[, columns[cIx]] <- as.double(temp)

        simData[[paste("Agg", cIx, sep = "")]] <- dhiDat[, columns[cIx]]
        if (sumColumns)
            simData$sum <- simData$sum + simData[[paste("Agg", cIx, sep = "")]]
    }

    simData
}

dumpUnitData <- function(dhiDat, pnlCol = pnlCol, genCol = genCol, mth = mth, asDate = asDate, bucket = "7x24", region = region, unitName="Portfolio" ){

    ## Get hourly gen unit payoff
    res <- getHrlyGenData(dhiDat, colNum = pnlCol, unitCol = genCol, unitize = TRUE, mth = mth, allSims = TRUE, asData = asDate, bucket=bucket, region = region, sumColumns = TRUE )
    ytMax <- (max(res$BySim$ShapeBySim)%/%10+1)*10
    ytMin <- (min(res$BySim$ShapeBySim)%/%10-1)*10
    yTicks <- seq(ytMin, ytMax, 10)
v
    my.plot <- bwplot(ShapeBySim~Hour,
                      res$BySim,horizontal=FALSE, xlab="Hour Ending", ylab = "$/MWh Gen payoff",
                      main = paste( "PDS Gen Payoff for", unitName),
                      scales =list( y= list(at = yTicks)),
                      panel = function(..., box.ratio){
                          panel.violin(..., col = "transparent", varwidth = FALSE, box.ratio = box.ratio)
                          panel.abline(h=yTicks, lty = "dotted", col = "grey")
                          panel.bwplot(..., fill = NULL, box.ratio = .1)
                      })

    trellis.device(device = "jpeg", filename = paste(unitName, "_GenPayoffByHour", ".jpeg", sep = ""))
    print(my.plot)
    dev.off()
    ## Get hourly unit dispatch
    res <- getHrlyGenData(dhiDat, colNum = genCol, unitCol = genCol, unitize = FALSE, mth = mth, allSims = TRUE, asData = asDate, bucket=bucket, region = region,sumColumns = TRUE )
    ytMax <- (max(res$BySim$ShapeBySim)%/%100+1)*100
    ytMin <- (min(res$BySim$ShapeBySim)%/%100-1)*100
    yTicks <- seq(ytMin, ytMax, 100)

    my.plot <- bwplot(ShapeBySim~Hour,
                      res$BySim,horizontal=FALSE, xlab="Hour Ending", ylab = "MW Gen",
                      main = paste( "PDS Gen Levels for", unitName),
                      scales =list( y= list(at = yTicks)),
                      panel = function(..., box.ratio){
                          panel.violin(..., col = "transparent", varwidth = FALSE, box.ratio = box.ratio)
                          panel.abline(h=yTicks, lty = "dotted", col = "grey")
                          panel.bwplot(..., fill = NULL, box.ratio = .1)
                      })
    trellis.device(device = "jpeg", filename = paste(unitName, "_GenExpByHour", ".jpeg", sep = ""))
    print(my.plot)
    dev.off()

    ## Get hourly unit revenue in dollars
    res <- getHrlyGenData(dhiDat, colNum = pnlCol, unitCol = genCol, unitize = FALSE, mth = mth, allSims = TRUE, asData = asDate, bucket=bucket, region = region,sumColumns = TRUE )
    ytMax <- (max(res$BySim$ShapeBySim)%/%100+1)*100
    ytMin <- (min(res$BySim$ShapeBySim)%/%100-1)*100
    yTicks <- seq(ytMin, ytMax, 100)
    my.plot <- bwplot(ShapeBySim~Hour,
                      res$BySim,horizontal=FALSE, xlab="Hour Ending", ylab = "$ Revenue",
                      main = paste( "PDS Gen Revenue for", unitName),
                      scales =list( y= list(at = yTicks)),
                      panel = function(..., box.ratio){
                          panel.violin(..., col = "transparent", varwidth = FALSE, box.ratio = box.ratio)
                          panel.abline(h=yTicks, lty = "dotted", col = "grey")
                          panel.bwplot(..., fill = NULL, box.ratio = .1)
                      })
    trellis.device(device = "jpeg", filename = paste(unitName, "_GenRevByHour", ".jpeg", sep = ""))
    print(my.plot)
    dev.off()
}

getHistHeatRateMth <- function(pwrSym="NEPOOL_LMP_DA_HUB", gasSym="GasDailyMean_ALGCG", bucket="7x24", month=prevMonth(Sys.Date()), lookback=2, season=TRUE, seasonMths=c(1,2,12)){

    currMth <- as.double(format(month,"%mm"))
    if (month<Sys.Date())
        month <- secdb.dateRuleApply( month,"J-1Y")
    if (!season)
        seasonMths <-currMth
    seasonMths <- sort(seasonMths)
    ret <- c()
    allHr <- c()
    ##Aggregate histroical data for each historical month in the season
    for (sMth in seasonMths){
        begYr <- as.Date(format(month, "%Y-01-01"))
        while (begYr>Sys.Date())
            begYr <- as.Date(secdb.dateRuleApply(begYr, "+J-12c"))
        thisDt <- as.Date(format(secdb.dateRuleApply(begYr, paste("J+", sMth-1, "m", sep="")), "%Y-%m-01"))
        if (thisDt>Sys.Date())
            thisDt <- as.Date(secdb.dateRuleApply(thisDt, "+J-12c"))
        for (bkYr in 0:(lookback-1)){                # Loop over look back years
#            browser()
            thisDt <-as.Date(format(secdb.dateRuleApply(thisDt, paste("-", bkYr*12, "c", sep="")), "%Y-%m-01"))
            print(paste("Processing Month: ",thisDt,sep=""))
            pwr <- read.zoo(tsdb.readCurve(pwrSym, thisDt, nextMonth(thisDt)-1), tz="EST")
            gas <- read.zoo(tsdb.readCurve(gasSym, thisDt-1, nextMonth(thisDt)-1))

            gas <- upsample_hourly(gas, rule = "gas")
            hr <- merge(gas, pwr, all=FALSE)
#            buckethrs <- read.zoo(secdb.getHoursInBucket(region, bucket, min(as.Date(hr$time)), max(as.Date(hr$time))-1))
#            hr <- merge(hr, buckethrs, all=FALSE)
            colnames(hr) <- c("gas", "pwr")
            hr$HR <- hr$pwr/hr$gas
            allHr <- rbind(allHr, hr)
        }
    }

    AvgHr <- as.data.frame(aggregate(hr, format(time(hr), "%H"), mean))
    ## Stupid hack to make HE0 into HE24
    rownames(AvgHr)[1] <- "24"
    AvgHr <- rbind(AvgHr[-1,], AvgHr[1,])
    AvgHr$Hour <- rownames(AvgHr)
    ret <- c(ret, list( "All"=allHr, "AvgHr"=AvgHr))
    ret
}

##Compare PDS simulations with history
plotHistvsSimHeatRate <- function( dhiDat, pwrSym, gasSym, pwrCol=4, gasCol=7,  month=nextMonth(Sys.Date()), lookback=2, season=TRUE, seasonMths=c(1,2,12), bucket="7x16", region="NEPOOL"){
    histHR <- getHistHeatRateMth(pwrSym, gasSym, month = month, lookback=lookback, season=season, seasonMths=seasonMths)
    simData <- filterSimForMthBuc(dhiDat, month, bucket, region, c(pwrCol, gasCol))
    simData$Power <- simData$Agg1
    simData$Gas <- simData$Agg2
    simData <- ddply(simData, c("Date", "Sim"), .fun=function(x){
            ret <- list()
            ret$Gas <- mean(x$Gas)
            ret$Power <- mean(x$Power)
            as.data.frame(ret)
    })
    simData$HR <- simData$Power/simData$Gas
    histAll <- histHR$All
    bucketHrs <- read.zoo(secdb.getHoursInBucket(region, bucket, min(time(histAll)), max(time(histAll))-1))
    histAll <- merge(histAll,bucketHrs, all=FALSE)
    histAll <- aggregate(histAll, as.Date(cut(time(histAll), "days")), mean)
    histAll <- as.data.frame(histAll)
    histAll$HR <- histAll$pwr/histAll$gas

    ## plot the scatter plot of gas vs HR
    plot.new()
    ytMax <- (max(simData$HR)%/%2.5+1)*2.5
    ytMin <- (min(simData$HR)%/%2.5-1)*2.5
    yTicks <- seq(ytMin, ytMax, 2.5)
    xtMax <- (max(simData$Gas)%/%5+1)*5
    xtMin <- (min(simData$Gas)%/%5-1)*5
    xTicks <- seq(xtMin, xtMax, 5)
    filename <-paste("HistVsSimHR_Gas_", ".pdf", sep = "")
    my.plot <- xyplot( HR~Gas, simData, scales =list( y= list(at = yTicks), x=list(at=xTicks)),grid=TRUE
                     ,panel = function(x,y,...){
                         panel.xyplot(x, y, col="red", type=c("p","g") )
                         panel.xyplot(histAll$gas,histAll$HR,
                                      col="blue", type="p", cex=1.25)
                      })
    pdf(filename)
#    trellis.device(device = "pdf", filename=filename)
    print(my.plot)
    dev.off()

    ## plot the scatter plot of Power vs HR
    plot.new()
    ytMax <- (max(simData$HR)%/%2.5+1)*2.5
    ytMin <- (min(simData$HR)%/%2.5-1)*2.5
    yTicks <- seq(ytMin, ytMax, 2.5)
    xtMax <- (max(simData$Power)%/%50+1)*50
    xtMin <- (min(simData$Power)%/%50-1)*50
    xTicks <- seq(xtMin, xtMax, 50)
    filename <- paste("HistVsSimHR_Power_", ".pdf", sep = "")
    my.plot <- xyplot( HR~Power, simData, scales =list( y= list(at = yTicks), x=list(at=xTicks)),grid=TRUE
                     ,panel = function(x,y,...){
                         panel.xyplot(x, y, col="red", type=c("p","g") )
                         panel.xyplot(histAll$pwr,histAll$HR,
                                      col="blue", type="p", cex=1.25)
                      })
    pdf(filename)
    ##Plot cdfs for power gas and HeatRate
    print(my.plot)
    dev.off()

    my.plot <- plot.CDF(as.data.frame(list(value=simData$HR)), title="HR")
    pdf("HR_CDF.pdf")
    print(my.plot)
    dev.off()

    my.plot <- plot.CDF(as.data.frame(list(value=simData$Power)), title=paste("Power",bucket,sep="."))
    pdf("Power_CDF.pdf")
    print(my.plot)
    dev.off()

    my.plot <- plot.CDF(as.data.frame(list(value=simData$Gas)), title="Gas")
    pdf("Gas_CDF.pdf")
    print(my.plot)
    dev.off()

}
# Main function to act as a demo and test
.main <- function(){

    source("h:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/PDS/libReadPDsDHI.R")
    source("h:/user/R/RMG/Utilities/Graphics/plot.CDF.R")
    #dirName <<- "S://Strategies//User//Ankur//NEPOOL//Portfolio//"
#    dirName <<- "S://Strategies//User//Ankur//NEPOOL//Oil//Mystic7//"
    dirName <<- "S://Strategies//User//Ankur//NEPOOL//GranRidge//2014_11_04//"
    setwd(dirName)
    #dhiDat <- readPdsDhi( paste( dirName,'MysticFuel.csv', sep="" ))
    ## Fetch data from DHI file into a data frame
    dhiDat <- readPdsDhi(paste( dirName, "GranRidge.csv", sep = "" ))

    ##Look at the data frame column names to figure out which are the Power and Gas columns we want to use. Not immediately obvoius so need to do this.
    names(dhiDat)
    asDate <- as.Date("2014-11-03")
    mth <- as.Date("2015-01-01")
    region <- "NEPOOL"

    ## Option cost and vol from PDS simulation for differnt moneyneess
    res <- getImplVolandOptPrcForCommod(dhiDat, colNum = 4, Moneyness = c(0.5, 1, 1.5), month = mth, Bucket = "5x16")

    ## save figures for a given unit
    pnlCol <- c(13)
    genCol <- pnlCol-1
    unitName <- "GranRidge dec14"
    dumpUnitData( dhiDat, pnlCol = pnlCol, genCol = genCol, mth = mth, asDate = asDate, bucket = "7x24", region = region, unitName = unitName )


    ## Section to plot the implied shape of the selected commods. We save the plots of selected percentiles in the working directory
###    commods <- c("HR","Power")
    commods <- c("HR")
    units <- c(" ", "$/MWh", "$/MMBTu")
    pcts <- c(0.95, 0.5, 0.05)
    bucket <- "7x24"
    ##    genUnit <- "Mystic7"
    genUnit <- "GrantRdg"
    pwrCol <- 4
    gasCol <- 8
    option <- TRUE
    pwrSym <- "NEPOOL_LMP_DA_HUB"
    gasSym <- "GasDailyMean_ALGCG"

    histHR <- getHistHeatRateMth(pwrSym, gasSym, month = mth, lookback=2, season=TRUE, seasonMths=c(1,2,12))
    plotHistvsSimHeatRate( dhiDat, pwrSym, gasSym, pwrCol=pwrCol, gasCol=gasCol, month=mth, lookback=2, season=TRUE, seasonMths=c(1,2,12))
    for (cIdx in 1:length(commods)){
        ## Get implied hourly shape for commod.
        commodByHour <- getImpliedHourlyShape(dhiDat, commod = commods[cIdx], mth = mth, pwrCol = pwrCol, gasCol = gasCol, pcts = pcts, bucket = bucket, asDate = asDate, allSims = TRUE, GetOption = option)
        meltedDt <- melt(commodByHour$Shaping, id("Hour"), variable.name = "PctileType", value.name = "Pctile")
        meltedDt <- rename(meltedDt, c("value"="Pctile", "variable"="PctileType"))
        ## Plot of the selcted percentiles
        plot.new()
        my.plot <- xyplot(Pctile~Hour, meltedDt, groups = PctileType, type=c("l", "g"), auto.key = TRUE, xlab = "Hour Ending",
                          ylab = units[cIdx], main=paste("PDS", commods[cIdx], "Hourly Shaping for", genUnit),
                          scales =list( x = list(at = c(0,5,8,10,15,20,23))),
                          panel = function(...){
                              panel.abline(v=c(8, 23), lty = "dotted", col = "black")
                              panel.xyplot(...)
                          })

        trellis.device(device = "jpeg", filename = paste(genUnit, "Shape", commods[cIdx], ".jpeg", sep = ""))
        print(my.plot)
        dev.off()

        ## plot violin plot by hour to check distribution by hour
        plot.new()
#        pdf("test.pdf")
        my.plot <- bwplot(ShapeBySim~Hour,
                          commodByHour$BySim,horizontal=FALSE, xlab="Hour Ending", ylab = "Shaping",
                          main = paste( "PDS Hourly Shape for", commods[cIdx]),
###               scales =list( y= list(at = yTicks)),
                          panel = function(..., box.ratio){
                              panel.violin(..., col = "transparent", varwidth = FALSE, box.ratio = box.ratio)
                              panel.abline( lty = "dotted", col = "grey")
                              panel.bwplot(..., fill = NULL, box.ratio = .1)
                              panel.xyplot(histHR$Hour, histHR$HR, type="l")
                          })
        trellis.device(device = pdf("xxx.pdf"), filename = paste(genUnit, "Shape", commods[cIdx], ".pdf", sep = ""))
        print(my.plot)
        dev.off()
    }
}
##         bwplot(ShapeBySim~Hour, res$BySim,horizontal=FALSE, panel = function(..., box.ratio){
##             panel.violin(..., col = "transparent", varwidth = FALSE, box.ratio = box.ratio)
##             panel.bwplot(..., fill = NULL, box.ratio = .1)
##         })

##         bwplot(ShapeBySim~Hour, res$BySim,horizontal=FALSE, panel = function(..., box.ratio){
##             panel.violin(..., col = "transparent", varwidth = FALSE, box.ratio = box.ratio)
##             panel.bwplot(..., fill = NULL, box.ratio = .1)
##         })
##     pcs <- c( 0.95, 0.5, 0.05 )
##     tiles <- as.vector(quantile( commodByHour$BySim$ShapeBySim, pcts))
##     x <- data.frame( pcs, tiles )
##     ecdfplot(~ShapeBySim, commodByHour$BySim,
##              panel = function(...){
##                  panel.ecdfplot(..., type=c( "s"))
##                  panel.abline(v=mean(commodByHour$BySim$ShapeBySim), lty="dotted", col ="red")
##                  panel.dotplot(tiles, pcs, type = "p",horizontal = TRUE,
## #                               ltext(x=tiles,y=pcs,labels=sprintf( "%.2f", tiles ),pos=1,offset=1,cex=0.8))
##                                ltext(x=tiles,y=pcs,labels=sprintf( "%.2f", tiles ),pos=1,offset=1,cex=0.8))
##              })


##     xyplot( pcs~tiles,x, type = "p",
##            panel = function(x,y, ...){
##                panel.xyplot(ltext(x=x,y=y,labels=tiles,pos=1,offset=1,cex=0.8))
##            })


#histogram( ~ShapeBySim, x, xlab="$/MWh", type = "density", n = 100,
#          panel = function(x, ... ){
#              panel.histogram(x, ... )
#              panel.mathdensity(dmath = dnorm, col= "black", kernel ="triangular", n=100, args = list(mean=mean(x), sd=sd(x)))
#              panel.densityplot( x )
#          })
#densityplot( ~ShapeBySim, x, kernel="cosine")





## #####--------------old Stuff---------------------------
##     ## Get hourly gen data
##     res <- getHrlyGenData(dhiDat, colNum = pnlCol, unitCol = genCol, unitize = TRUE, mth = mth, allSims = TRUE, asData = asDate, bucket = "7x24", region = region,sumColumns = TRUE )
##     ytMax <- (max(res$BySim$ShapeBySim)%/%10+1)*10
##     ytMin <- (min(res$BySim$ShapeBySim)%/%10-1)*10
##     yTicks <- seq(ytMin, ytMax, 10)

##     bwplot(ShapeBySim~Hour,
##            res$BySim,horizontal=FALSE, xlab="Hour Ending", ylab = "$/MWh Gen payoff",
##            main = paste( "PDS NEPOOL unit Gen Payoff for", unit),
##            scales =list( y= list(at = yTicks)),
##            panel = function(..., box.ratio){
##         panel.violin(..., col = "transparent", varwidth = FALSE, box.ratio = box.ratio)
##         panel.abline(h=yTicks, lty = "dotted", col = "grey")
##         panel.bwplot(..., fill = NULL, box.ratio = .1)
##     })

##     res <- getHrlyGenData(dhiDat, colNum = pnlCol, unitCol = genCol, unitize = FALSE, mth = mth, allSims = TRUE, asData = asDate, bucket = "7x24", region = region,sumColumns = TRUE )
##     ytMax <- (max(res$BySim$ShapeBySim)%/%100+1)*100
##     ytMin <- (min(res$BySim$ShapeBySim)%/%100-1)*100
##     yTicks <- seq(ytMin, ytMax, 100)

##     bwplot(ShapeBySim~Hour,
##            res$BySim,horizontal=FALSE, xlab="Hour Ending", ylab = "MW Gen",
##            main = paste( "PDS NEPOOL unit Gen Levels for", unit),
##            scales =list( y= list(at = yTicks)),
##            panel = function(..., box.ratio){
##         panel.violin(..., col = "transparent", varwidth = FALSE, box.ratio = box.ratio)
##         panel.abline(h=yTicks, lty = "dotted", col = "grey")
##         panel.bwplot(..., fill = NULL, box.ratio = .1)
##     })


##     ytMax <- (max(res$BySim$ShapeBySim)%/%100+1)*100
##     ytMin <- (min(res$BySim$ShapeBySim)%/%100-1)*100
##     yTicks <- seq(ytMin, ytMax, 100)

##     bwplot(ShapeBySim~Hour,
##            res$BySim,horizontal=FALSE, xlab="Hour Ending", ylab = "$/MWh Gen payoff",
##            main = paste( "PDS NEPOOL unit Gen Payoff for", unit),
##            scales =list( y= list(at = yTicks)),
##            panel = function(..., box.ratio){
##                panel.violin(..., col = "transparent", varwidth = FALSE, box.ratio = box.ratio)
##                panel.abline(h=yTicks, lty = "dotted", col = "grey")
##                panel.bwplot(..., fill = NULL, box.ratio = .1)
##            })

###------------------old CDF stuff
##     pcs <- c( 0.95, 0.75, 0.5, 0.25, 0.05 )
##     tiles <- as.double(quantile(simData$HR, pcs))
##     plot.new()
##     ef <- ecdf(simData$HR)
##     meanData <- mean(simData$HR)
##     lattice.options(panel.error=NULL)
##     my.plot <- ecdfplot(~HR, simData,
##              panel = function(x,...){
##                  panel.ecdfplot(x, type=c( "s"))
##                  panel.abline(v=meanData, lty="dotted", col ="red")
##                  panel.dotplot(tiles, pcs, type = "p",horizontal = TRUE)
##                  panel.xyplot(tiles, pcs, type = "p",horizontal = TRUE)
##                  panel.text(x=tiles,y=pcs,labels=sprintf("%.f%%: %.2f", pcs*100, tiles),pos=1,offset=1,cex=0.8)
##                  panel.text(x=meanData,y=ef(meanData),labels=sprintf("Mean: %.2f", meanData),pos=1,offset=-18,cex=0.9,col="red")
##                               ## panel=function(x,y,...){
##                            ##        panel.superpose(...)
##                            ##        ltext(x=x,y=y,labels=sprintf("%.f%%: %.2f", y*100, x),pos=1,offset=1,cex=0.8)
##                            ## })
## #                             ltext(x=tiles,y=pcs,labels=sprintf("%.f%%: %.2f", pcs*100, tiles),pos=1,offset=1,cex=0.8))
## #                             ltext(x=tiles,y=pcs,pos=1,offset=1,cex=0.8))
## #                             ltext(x=tiles,y=as.vector(pcs),labels=sprintf( "%.2f", tiles ),pos=1,offset=1,cex=0.8))
##              })

##     dev.off()

##     plot.new()
##     tiles <- as.vector(quantile(simData$Gas, pcts))
##     my.plot <- ecdfplot(~Gas, simData,
##              panel = function(...){
##                  panel.ecdfplot(..., type=c( "s"))
##                  panel.abline(v=mean(simData$Gas), lty="dotted", col ="red")
##                  panel.dotplot(tiles, pcs, type = "p",horizontal = TRUE,
## ###                               ltext(x=tiles,y=pcs,labels=sprintf( "%.2f", tiles ),pos=1,offset=1,cex=0.8))
##                                ltext(x=tiles,y=pcs,labels=sprintf( "%.2f", tiles ),pos=1,offset=1,cex=0.8))
##              })
##     pdf("Gas_CDF.pdf")
##     print(my.plot)
##     dev.off()

##     plot.new()
##     tiles <- as.vector(quantile(simData$Power, pcts))
##     my.plot <- ecdfplot(~Power, simData,
##              panel = function(...){
##                  panel.ecdfplot(..., type=c( "s"))
##                  panel.abline(v=mean(simData$Power), lty="dotted", col ="red")
##                  panel.dotplot(tiles, pcs, type = "p",horizontal = TRUE,
## ###                               ltext(x=tiles,y=pcs,labels=sprintf( "%.2f", tiles ),pos=1,offset=1,cex=0.8))
##                                ltext(x=tiles,y=pcs,labels=sprintf( "%.2f", tiles ),pos=1,offset=1,cex=0.8))
##              })
##     pdf("Power_CDF.pdf")
##     print(my.plot)
##     dev.off()
