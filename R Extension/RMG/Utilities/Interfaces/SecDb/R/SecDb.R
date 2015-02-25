################################################################################
#
.onLoad <- function(lib, pkg) {
    cat("Loading RSecDbInterface... ")
    
    options(warn.FPU=FALSE)
    dyn.load(Sys.which("RSecDbInterface.dll"))
    options(warn.FPU=TRUE)
    
    cat("Loaded.\n")
}


################################################################################
secdb.setDatabase <- function( dbName )
{
    name = as.character(dbName)
    
    .Call("SetSecDbDatabase", name)
    
    return("Database Set")
}


################################################################################
secdb.convertTime <- function ( secDbTime )
{
    return( as.POSIXlt(ISOdatetime(1970,1,1,0,0,0, tz="GMT") + 
                            secDbTime, tz="") )
}


################################################################################
# 
#
secdb.getValueType <- function( securityName, valueTypeName, debug=FALSE )
{
    security = as.character(securityName)
    valueType = as.character(valueTypeName)
    
    if(debug)
    {
        cat("Calling GetValueType with", security, valueType, "\n")
    }
    returnValue = .Call("GetValueType", security, valueType)
    
    return(returnValue)
}


################################################################################
# 
#
secdb.evaluateSlang <- function( slangCode, debug=FALSE )
{
    slang = as.character(slangCode)
    
    if(debug)
    {
        cat("Calling EvalutateSlang with", slang, "\n")
    }
    returnValue = .Call("EvaluateSlang", slang)
    
    return(returnValue)
}


################################################################################
# 
#
secdb.invoke <- function( libName, funcName, ... )
{
    libraryName = as.character(libName)[1]
    functionName = as.character(funcName)[1]
    arguments = list(...)
    
    returnValue = .Call( "CallFunction", libraryName, functionName, 
            arguments )
    
    return(returnValue)
}


################################################################################
# 
#
secdb.dateRuleApply <- function( dateValue, ruleToApply )
{
    dateFormat = "%d%b%y"
    dateString = format(as.Date(dateValue), dateFormat)
    rule = as.character(ruleToApply)
    
    slangCode = paste( 'DateRuleApply(Date("', dateString, '"), "', 
            rule, '")', sep="")
    returnValue = secdb.evaluateSlang( slangCode )
    returnValue = secdb.convertTime(returnValue)
    return(returnValue)
}


################################################################################
# Return a data frame of booleans and time values, specifying the hours that 
# fall into that bucket
# 
secdb.getBucketMask <- function( region, bucket, startDate, endDate )
{
    asset = paste("Commod PWR", region, "Physical")
    
    # Add a day to each side of the window to make sure we get the needed range
    startString = format(as.Date(startDate)-1, "%d%b%y")
    endString = format(as.Date(endDate)+1, "%d%b%y")
    
    slang = paste( 'TCurveCreateFromValues(',
            'Frequency Definitions("', asset, '"), ',
            'Structure("', bucket, '", 1), ',
            'Date("', startString, '"), ',
            'Date("', endString, '"))', 
            sep="")
    
    hourMask = data.frame(secdb.evaluateSlang(slang))
    class(hourMask$value) = "logical"
    
    # if (Sys.getenv("tz")=="") Sys.setenv(TZ="EST5EDT") # Why do I need this?!

    if( is( startDate, "Date" ) )
    {
        startDate = as.POSIXct( paste(as.character(startDate), "00:00:00") ) 
    }
    
    if( is( endDate, "Date" ) )
    {
        endDate = as.POSIXct( paste(as.character(endDate), "23:59:59") ) 
    }
    
    # Do the filtering here in case they pass a POSIXct instead of a date
    hourMask = subset( hourMask, time >= startDate & time<=endDate )
    
    names(hourMask) = c("time", bucket)
    
    return(hourMask)
}


################################################################################
# Return the time ranges that match the bucket provided for the given region.
# 
secdb.getHoursInBucket <- function( region, bucket, startDate, endDate )
{
    hourMask = secdb.getBucketMask( region, bucket, startDate, endDate )
    
    hoursToKeep = hourMask[ which( hourMask[[bucket]] ), "time", drop=F]
    
    return(hoursToKeep)
}


################################################################################
# Return forward FX rates from SecDB
# 
secdb.getFXFwdCurve <- function( CurrencyFrom, CurrencyTo, asOfDate )
{
    FXCurveName <- paste(CurrencyTo,'/',CurrencyFrom,sep = "")
    SecDBLib    <- "_Lib Matlab Functions"
    SecDBFun    <- "MATLAB::GetDatedValue"
    ValueMethod <- "Forward Curve"
    FwdFX       <- as.data.frame(secdb.invoke(SecDBLib, SecDBFun, asOfDate,
                    ValueMethod,FXCurveName))
    return(FwdFX)
}

################################################################################
# Return discount curve from SecDB
# 
secdb.getDiscountFactor <- function( asOfDate, dates, currency="USD" )
{
    SecDBLib  <- "_Lib Matlab Functions"
    SecDBFun  <- "MATLAB::GetSelectedDF"
    ldays     <- lapply(as.list(as.character(dates)), as.Date)
    DF        <- data.frame(dates=dates,  DF=unlist(secdb.invoke(SecDBLib,
                            SecDBFun, asOfDate, ldays, currency)))
    return(DF)
}
## ################################################################################
## # Get elec prices
## # 
## secdb.getElecPrices <- function( asOfDate, startDate, endDate,
##   commodity, location, bucket, serviceType="Energy", useFutDates=FALSE )
## {
##   df <- expand.grid( asOfDate, startDate, endDate,
##     commodity, location, bucket, serviceType)
##   names(df) <- c("asOfDate", "startDate", "endDate", "commodity",
##                  "location", "bucket", "serviceType")
  
##   SecDBLib  <- "_Lib Matlab Functions"
##   SecDBFun  <- "MATLAB::GetFutCurve"

##   res <- vector("list", nrow(df))
##   for (i in 1:nrow(df)){
##     res[[i]] <- data.frame(i=i, secdb.invoke(SecDBLib, SecDBFun, df$asOfDate[i],
##       df$startDate[i], df$endDate[i], as.character(df$commodity[i]),
##       as.character(df$location[i]), as.character(df$bucket[i]),
##       as.character(df$serviceType[i]), useFutDates))
##   }

##   if (nrow(df)==1)
##     res <- res[[1]]
##   else
##     res <- do.call(rbind, res)

##   res <- merge(data.frame(i=1:nrow(df), df[,c("asOfDate","location", "bucket",
##     "serviceType")]), res)
##   res$i <- NULL
  
##   return(res)
## }

################################################################################
# Return a flag if a date is holiday or not.  
# 
secdb.isHoliday <- function(dates, calendar="CPS-BAL", skipWeekends=FALSE)
{
    ldays <- lapply(as.list(as.character(dates)), as.Date)
    aux <- secdb.invoke("_LIB RMG SecDB Bridge", "RMGSecDB::IsHoliday", 
            ldays, calendar, skipWeekends)
    return(unlist(aux)==1)
}


################################################################################
# Read the deltas for a given portfolio/book
# 
secdb.readDeltas <- function(asOfDate, portfolioBook)
{
    portbook  = as.character(portfolioBook)
    asOfDate <- as.Date(asOfDate)
    
    res <- secdb.invoke("_lib elec deltagamma report",
            "DeltaGamma::ReadDeltas", list(portbook), asOfDate, list(1))
    
    res = lapply(res, data.frame)
    
    return(res)
}


################################################################################
# Run a position report on a series of calculators.  Return a data.frame with 
# curve.name, contrac.dt, delta.  You can pass a vector of calculator names.
#
calculator.get.deltas <- function (group, trader, calc.name) 
{
    res <- secdb.invoke("_LIB RMG SecDB Bridge", "RMGSecDB::Get Delta Multi Calcs", 
            group, trader, as.list(calc.name))
    
    if( length( res[[1]] ) == 0 ) 
    {
        res <- as.data.frame( matrix( 0, nrow=0, ncol=3 ) )
        names( res ) <- c("curve.name", "contract.dt", "delta")
        return( res )
    }
    
    res <- matrix(unlist(res), ncol = 3, byrow = TRUE)
    res <- data.frame(res, stringsAsFactors = FALSE)
    res[, 1] <- toupper(res[, 1])
    res[, 2] <- as.Date(as.POSIXct("1970-01-01") + as.numeric(res[,2]))
    res[, 3] <- as.numeric(res[, 3])
    names(res) <- c("curve.name", "contract.dt", "delta")
    res <- aggregate(res$delta, list(curve.name = res$curve.name, 
                    contract.dt = res$contract.dt), sum) 
    names( res ) <- c("curve.name", "contract.dt", "delta")
    
    return( res )
}


################################################################################
# List the calculators available under a given group & trader name. 
# 
calculator.list <- function(group, trader)
{
    res <- secdb.invoke(
            "_lib commod calc utils", 
            "CalcUtils::Collect Calcs By Deal",
            trader, group )
    
    return(unlist(res))
}


################################################################################
tsdb.getSymbolInfo <- function( curveName, debug=FALSE )
{
    curve = as.character(curveName)
    
    if(debug)
    {
        cat("Calling GetSymbolInfo with ", curve, "\n" )
    }
    returnValue = .Call("GetSymbolInfo", curve)
    
    return(returnValue)
}


################################################################################
tsdb.futCurve <- function( curveName, asOfDate, startDate, endDate )
{
    asOfDate = format(asOfDate, "%d%b%y")
    startDate = format(startDate, "%d%b%y")
    endDate = format(endDate, "%d%b%y")
    
    slang = paste('TsdbFunc("futcurve", Date("', startDate, '"), Date("', 
            endDate, '"), "', curveName, '", Date("', asOfDate, '"))', sep="")
    
    curveData = data.frame( secdb.evaluateSlang(slang) )
    
    return(curveData)
}


################################################################################
tsdb.readCurve <- function( curveName, startDatetime, endDatetime, debug=FALSE )
{
    curve = as.character(curveName)
    
    if (class(startDatetime)[1]=="Date"){
      startDatetime <- as.POSIXct(paste(startDatetime, "00:00:00"))
    }
    
    if (class(endDatetime)[1]=="Date"){
      endDatetime <- as.POSIXct(paste(endDatetime, "23:59:00"))
    }
    
    if(debug){
        cat("Calling TSDBRead with ", curve, " ", start, " ", end, "\n")
    }
    
    returnValue <- secdb.invoke("_lib elec matlab dispatch fns", "TSDB::Read",
            curve, startDatetime, endDatetime)
    
    return( data.frame(returnValue) )
}


################################################################################
# Return a flat table with curve.name, contract.dt, pricing.dt, value(price)
# from tsdb.
#
tsdb.futStrip <- function( curveNames, histStart.dt, histEnd.dt, contract.dts,
        debug=FALSE )
{
    res <- NULL
    # Let's do the loops in SecDb if possible!   
    for (n in 1:length(curveNames))
    {
        symbol.root <- secdb.getValueType(curveNames[n], "tsdb symbol rule")
        mkt <- strsplit(curveNames[n], " ")[[1]][2]
        symbol.root <- gsub("!", mkt, symbol.root)
        for (m in 1:length(contract.dts))
        {
            yymm   <- format(contract.dts[m], "%y%m") 
            symbol <- gsub("\\^y2\\^m2", yymm, symbol.root)
            aux    <- secdb.invoke("_lib elec matlab dispatch fns",
                    "TSDB::Read", symbol, histStart.dt, histEnd.dt)
            
            # if you request things that don't exist
            if (!is.null(aux))
            {     
                res <- rbind( res, 
                        data.frame( curve.name=curveNames[n],
                                contract.dt = rep(contract.dts[m], length(aux$value)),
                                data.frame(aux) )
                        )
            }
        }
    }
    if (!is.null(res)){
        names(res)[3] <- "pricing.dt"
        res$curve.name <- as.character(res$curve.name)
    }
    return(res)
}

################################################################################
# Format a date to the MYY format used throughout SecDB.
# 
#
format.dateMYY <- function(dateIn, direction=1){
    
    monthGS <- 1:12
    names(monthGS) <- c("F","G","H","J","K","M","N","Q","U","V","X","Z")
    
    if (direction==1){               # Goldman -> as.Date
        letter <- substr(dateIn, 1, 1)
        ind <- as.numeric(monthGS[letter])
        yr  <- substr(dateIn, 2, 3)
        dateOut <- as.Date(paste(yr, ind, "01",sep="-"), format="%y-%m-%d")
    } else {
        if (direction == -1){          # as.Date -> Goldman
            aux <- format(dateIn, format="%m%Y")
            mth <- names(monthGS)[as.numeric(substr(aux,1,2))]
            yr  <- substr(aux, 5, 6)
            dateOut <- paste(mth, yr, sep="")
        } else {
            print("Unknown direction, check inputs.")
        }
    }
    return(dateOut)
}






