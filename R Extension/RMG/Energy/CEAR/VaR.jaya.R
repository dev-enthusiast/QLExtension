# Utility to calculate Jaya's VaR before he submits the bids. 
#
#
# Written by Adrian Dragulescu on 30-Nov-2007
library(SecDb)

################################################################################
# Example
#  region = "NYPP"
#  deliveryPoint = "DUNW"
#  serviceType = "Energy"
#  tenor = as.Date("2008-06-01")
#  megaWatts = 100
#
convertMWtoMWH <- function( region, deliveryPoint, serviceType, tenor, megaWatts )
{
    curves = secdb.invoke(
            "_Lib Vcentral delivery point", "VCN::EML2CML",
            region, deliveryPoint, serviceType, "7x24" )
    
    buckets = sapply( sapply( lapply( curves, strsplit, " " ), "[", 1 ), "[", 3 )

    endMonth = as.Date( secdb.dateRuleApply( tenor, "+1mJ") )
    
    allData = NULL
    for( bucketIndex in seq_along( buckets ) )
    {
        bucket = buckets[bucketIndex]
        curve = curves[bucketIndex]
        
        hoursCurve = data.frame( secdb.invoke( "_Lib NTPLOT Functions", 
                        "NTPlot::BucketHours", bucket, region, "Day", "Sum", TRUE) )
        
        validHours = subset( hoursCurve, date >= tenor & date < endMonth )
        hours = sum( validHours$value )
        
        rowData = data.frame( curve.name = toupper( curve ), contract.dt = tenor, hours = hours )
        allData = rbind( allData, rowData )
    }
    
    allData$position = allData$hours * megaWatts
    allData$vol.type = "NA"
    allData$hours = NULL
    
    return( allData )
}


################################################################################
getNewPositionData <- function( csvFile )
{
    newTrades = read.csv( csvFile )
    newTrades$tenor = as.Date( newTrades$tenor, "%m/%d/%Y")
    
    allTradeData = NULL
    for( tradeIndex in seq_len( nrow( newTrades ) ) )
    {
        tradeData = newTrades[ tradeIndex, ]
        positionData = convertMWtoMWH( tradeData$region, tradeData$deliveryPoint, 
                tradeData$serviceType, tradeData$tenor, tradeData$megaWatts )
        allTradeData = rbind(allTradeData, positionData)
    }
    
    return( allTradeData )
}


#######################################################################
source("S:/All/Risk/Software/R/prod/Energy/VaR/Overnight/overnightUtils.R")
options <- overnightUtils.loadOvernightParameters("2008-08-22")

source("S:/All/Risk/Software/R/prod/Energy/VaR/Base/get.portfolio.book.R")
run.all <-  get.portfolio.book(options)
sort(unique(run.all$RUN_NAME))
run <- NULL

run$run.name <- "EAST PM CONGESTION TRD PORT"
run$specification <- subset(run.all, RUN_NAME == toupper(run$run.name))

# YOU NEED TO REMOVE NYTCCTMP if it's in run$specification!
#run$specification <- run$specification[-8,] 

QQ.old <- aggregate.run.positions(run, options)

# go to generate.positions.R and return data.frame data with the new positions.
data = getNewPositionData( "H:/user/R/RMG/Energy/CEAR/auctionPositions.csv")

# add the new positions
QQ <- rbind(QQ.old, data)
QQ <- aggregate(QQ$position, list(QQ$curve.name, QQ$contract.dt,
                                  QQ$vol.type), sum)
names(QQ) <- c("curve.name", "contract.dt", "vol.type", "position")
QQ$curve.name  <- toupper(as.character(QQ$curve.name))
QQ$vol.type    <- as.character(QQ$vol.type)
QQ$contract.dt <- as.Date(QQ$contract.dt)
QQ <- QQ[order(QQ$curve.name, QQ$contract.dt), ]  # need to sort the quantity data!

# put a meaningful name here
run$run.name <- "EAST PM CONGESTION TRD PORT"

source("H:/user/R/RMG/Energy/VaR/Base/main.run.VaR.R")
main.run.VaR(run, options, QQ.old)


#############################################################################
#############################################################################
#############################################################################


## filename <- "H:/user/R/RMG/Energy/CEAR/jaya.nepool.Apr08.xls"
## con    <- odbcConnectExcel(filename)
## tables <- sqlTables(con) 
## data   <- sqlFetch(con, "Sheet1")
## odbcCloseAll()

## ind <- which(is.na(data[,"Source Commod"]))
## data <- data[-ind,]
## data <- data[, c("Source Commod", "Sink Comod", "MW", "Class")]
## DD <- rbind(cbind(as.character(data[,"Source Commod"]), -data[, "MW"]),
##             cbind(as.character(data[,"Sink Comod"]),     data[, "MW"]))
## DD <- data.frame(DD)
## names(DD) <- c("curve.name", "position")
## DD$curve.name <- toupper(as.character(DD$curve.name))
## DD$position   <- as.numeric(as.character(DD$position) )
## DD <- aggregate(DD$position, list(DD$curve.name), sum)
## names(DD) <- c("curve.name", "position")

## DD$mult  <- NA
## ind.peak <- grep("5X16", DD$curve.name)
## DD$mult[ind.peak]  <- 336
## ind.2x16 <- grep("2X16", DD$curve.name)
## DD$mult[ind.2x16]  <- 128
## ind.7x8 <- grep("7X8", DD$curve.name)
## DD$mult[ind.7x8]  <- 224
## ind.off <- grep("OFFPEAK", DD$curve.name)
## DD$mult[ind.off]  <- 224 + 128

## DD <- subset(DD, position != 0)
## DD$position <- DD$position * DD$mult

## #DD$curve.name <- gsub("PWX 2X16 INDEX", "PWX 2X16H INDEX", DD$curve.name)
## DD$mult <- NULL
## DDD <- expand.grid(curve.name = DD$curve.name,
##   contract.dt=seq(as.Date("2008-01-01"), length.out=1, by="month"))
## DDD$curve.name <- toupper(as.character(DDD$curve.name))
## DDD <- merge(DDD, DD, all=T)
## DDD$vol.type <- "NA"
