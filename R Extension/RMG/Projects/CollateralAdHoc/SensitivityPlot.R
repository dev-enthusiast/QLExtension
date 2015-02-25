###############################################################################
# TODO: Add comment
# 
# Author: e14600


################################################################################
readCollateralValue <- function( file )
{
    data = read.csv( file )
    collateral = sum( data$CASHFLOW_CHANGE[which(data$MARGINED==TRUE)], 
            na.rm=TRUE )
    return( collateral )
}


################################################################################
BASE_PATH = "S:/All/Risk/Collateral Scenarios/Reports/"

startDate = as.Date( '2008-11-10' )
endDate = as.Date( '2009-01-12' )

dateRange = seq( startDate, endDate, by = "day" )
sigmaFiles = c( "Dn8s.csv", "Dn4s.csv", "Dn2s.csv", "Dn1s.csv", "Epsilon.csv",
        "Up1s.csv", "Up2s.csv", "Up4s.csv", "Up8s.csv" )
sigmaValues = c( -8, -4, -2, -1, 0, 1, 2, 4, 8 )


allData = NULL

for( date in as.character( dateRange ) )
{
    for( file in sigmaFiles )
    {
        inputFile = paste( BASE_PATH, date, "/", file, sep = "" )
        if( !file.exists( inputFile ) ) next 
        print( inputFile )
        
        index = which( sigmaFiles == file )
        sigmaValue = sigmaValues[ index ]
        collateralValue = readCollateralValue( inputFile )
        
        rowData = data.frame( date = date, sigma = sigmaValue, 
                collateralChange = collateralValue )
        allData = rbind( allData, rowData )
    }
}


allData$date = as.Date( allData$date )
wireframe( abs(collateralChange/1000000) ~ sigma * as.numeric(date), allData, drape=TRUE, aspect=c(-1,1), colorkey=TRUE )





BASE_PATH = "S:/All/Risk/Collateral Scenarios/Reports/"

startDate = as.Date( '2008-11-10' )
endDate = Sys.Date()-1

dateRange = seq( startDate, endDate, by = "day" )


allData = NULL

for( date in as.character( dateRange ) )
{
    
    inputFile = paste( BASE_PATH, date, "/Epsilon.csv", sep = "" )
    if( !file.exists( inputFile ) ) next 
    print( inputFile )
    
    data = read.csv( inputFile )
    power = sum( data$POWER[which(data$MARGINED==TRUE & data$CASHFLOW_CHANGE!=0)], 
            na.rm=TRUE )
    if( power == 0 )
    {
        power = sum( data$POWER.PEAK[which(data$MARGINED==TRUE & data$CASHFLOW_CHANGE!=0)], 
                        na.rm=TRUE ) + 
                sum( data$POWER.OFFPEAK[which(data$MARGINED==TRUE & data$CASHFLOW_CHANGE!=0)], 
                        na.rm=TRUE )
    }
    gas = sum( data$GAS[which(data$MARGINED==TRUE & data$CASHFLOW_CHANGE!=0)], 
            na.rm=TRUE )
    international = sum( data$INTERNATIONAL.COAL[which(data$MARGINED==TRUE & data$CASHFLOW_CHANGE!=0)], 
            na.rm=TRUE )
    domestic = sum( data$DOMESTIC.COAL[which(data$MARGINED==TRUE & data$CASHFLOW_CHANGE!=0)], 
            na.rm=TRUE )
    
    rowData = data.frame( date = date, power = power,
            gas = gas, coal = international+domestic )
    allData = rbind( allData, rowData )
}


allData$date = as.Date( allData$date )
write.csv(allData, file="C:/Documents and Settings/e14600/Desktop/margined.csv")


allData = NULL

for( date in as.character( dateRange ) )
{
    
    inputFile = paste( BASE_PATH, date, "/Epsilon.csv", sep = "" )
    if( !file.exists( inputFile ) ) next 
    print( inputFile )
    
    data = read.csv( inputFile )
    power = sum( data$POWER[which(data$MARGINED==TRUE)], 
            na.rm=TRUE )
    if( power == 0 )
    {
        power = sum( data$POWER.PEAK[which(data$MARGINED==TRUE )], 
                        na.rm=TRUE ) + 
                sum( data$POWER.OFFPEAK[which(data$MARGINED==TRUE )], 
                        na.rm=TRUE )
    }
    gas = sum( data$GAS[which(data$MARGINED==TRUE)], 
            na.rm=TRUE )
    international = sum( data$INTERNATIONAL.COAL[which(data$MARGINED==TRUE )], 
            na.rm=TRUE )
    domestic = sum( data$DOMESTIC.COAL[which(data$MARGINED==TRUE)], 
            na.rm=TRUE )
    
    rowData = data.frame( date = date, power = power,
            gas = gas, coal = international+domestic )
    allData = rbind( allData, rowData )
}


allData$date = as.Date( allData$date )
write.csv(allData, file="C:/Documents and Settings/e14600/Desktop/marginable.csv")












