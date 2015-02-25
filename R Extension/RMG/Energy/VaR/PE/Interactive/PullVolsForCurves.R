###############################################################################
# PullVolsForCurves.R
# 
# TODO: Add comment
#
# Author: John Scillieri
# 
library(RODBC)


pullVolCurve <- function( curveName, asOfDate )
{
    file.dsn <- "CPSPROD.dsn"
    file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", file.dsn.path, file.dsn,
            ";UID=stratdatread;PWD=stratdatread;", sep="")
    con <- odbcDriverConnect(connection.string)
    
    volQuery <- paste(
            "SELECT COMMOD_CURVE, CONTRACT_DATE, TYPE, VOLATILITY ",
            "from foitsox.sdb_bs_volatility where ",
            "pricing_date=to_date('", asOfDate, "','YYYY-MM-DD') ",
            "and upper(COMMOD_CURVE)='", toupper(curveName), "' ",
            "and strike_pct=0.5",
            sep="")
    volData   <- sqlQuery(con, volQuery)
    names(volData) = c("curve.name", "contract.dt", "type", "value")
    volData$contract.dt = as.Date(volData$contract.dt)
    volData$curve.name = toupper(volData$curve.name)
    volData = unique(volData)
    volData = volData[ order(volData$curve.name, 
                    volData$contract.dt, volData$type), ]
    
    odbcClose(con)
    
    return( volData )
}       
## 
## curve1 = 'COMMOD PWJ 2X16H METED PHYSICAL' 
## curve1 = 'COMMOD PWJ 5X16 METED PHYSICAL' 
## #curve1 = 'COMMOD PWJ 7X8 METED PHYSICAL' 
## #curve1 = 'COMMOD PWJ ICAP MAACSC PHYSICAL' 
## 
## asOfDate = secdb.dateRuleApply(Sys.Date(), "-1b")
## 
## curve1Data = pullVolCurve(curve1, asOfDate)
## dim(curve1Data)
##  
## names(curve1Data) = c("curve.name", "contract.dt", "type", asOfDate)
## mCurve1 = subset(curve1Data, type=="M")
## 
## 
## plot(x=mCurve1$contract.dt, mCurve1$asOfDate, type="l", 
##         main=curve1, col="blue" )

tsdb.futcurve <- function( curveName, asOfDate, startDate, endDate )
{
    asOfDate = format(asOfDate, "%d%b%y")
    startDate = format(startDate, "%d%b%y")
    endDate = format(endDate, "%d%b%y")
    
    slang = paste('TsdbFunc("futcurve", Date("', startDate, '"), Date("', 
            endDate, '"), "', curveName, '", Date("', asOfDate, '"))', sep="")
    
    curveData = data.frame( secdb.evaluateSlang(slang) )
}


asOfDate = as.Date(secdb.dateRuleApply(Sys.Date(),"-1b"))
startDate = asOfDate
endDate = as.Date("2021-12-31")

curve1Vol = "VOL_PWJ_2X16H_METED_PHYS"
curve2Vol = "VOL_PWJ_5X16_METED_PHYS"
curve3Vol = "VOL_PWJ_7X8_METED_PHYS"
curve4Vol = "VOL_PWJ_ICAP_MAACSC_PHYS"

vol1Data = tsdb.futcurve( curve1Vol, asOfDate, startDate, endDate )
vol2Data = tsdb.futcurve( curve2Vol, asOfDate, startDate, endDate )
vol3Data = tsdb.futcurve( curve3Vol, asOfDate, startDate, endDate )
vol4Data = tsdb.futcurve( curve4Vol, asOfDate, startDate, endDate )

final = merge(vol1Data, vol2Data, by=c("date"))
final = merge(final, vol3Data, by="date")
final = merge(final, vol4Data, by="date")
