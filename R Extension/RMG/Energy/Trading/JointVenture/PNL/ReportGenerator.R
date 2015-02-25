###############################################################################
## ReportGenerator.R
##
## Process:
##         For each type { PNL, Price, Price Change, Position }:
##             Read in data set
##             Iterate through all row specs to generate ROW_SPEC column in data
##             For each COLUMN_SPEC
##                 get data subset
##                 cast data: ROW_SPEC ~ . 
##                 agg.fun in cast is by type:
##                         PNL = sum
##                         Price = average
##                         Price Change = average
##                         Position = sum
##                 append Type and COLUMN_SPEC columns to results
##                 rbind results to final table
##         cast final table: ROW_SPEC + Type ~ COLUMN_SPEC
##
## Author: e14600
##
source( "H:/user/R/RMG/Utilities/RLogger.R" )
source( "H:/user/R/RMG/Utilities/load.R" )
source( "H:/user/R/RMG/Utilities/write.xls.R" )
source( "H:/user/R/RMG/Energy/Trading/JointVenture/PNL/PNLCalculator.R" )
source( "H:/user/R/RMG/Energy/Trading/JointVenture/PNL/ReportSpecification.R" )


################################################################################
# File Namespace
#
ReportGenerator = new.env(hash=TRUE)


################################################################################
ReportGenerator$forPortfolio <- function( portfolio, asOfDate )
{
    rLog( "Generating Report For:", portfolio, "-", as.character( asOfDate ) )
    
    pqData = PNLCalculator$pnlData( portfolio, asOfDate ) 
    dayOnDayPNL = pqData[[ 1 ]]
    newTradePNL = pqData[[ 2 ]]
    
    OUTPUT_FILE = paste( "S:/All/Risk/Reports/TradingControlReports/PNL/", 
            portfolio, " PNL.", as.character( asOfDate ), ".xls", sep="")
    
    if( !is.null( newTradePNL ) )
    {
        newTradeTable = .createTable( newTradePNL )
        write.xls( newTradeTable, file = OUTPUT_FILE, sheetname = "New Trade PNL" )
    }
    
    if( !is.null( dayOnDayPNL ) )
    {       
        dayOnDayTable = .createTable( dayOnDayPNL )
        write.xls( dayOnDayTable, file = OUTPUT_FILE, sheetname = "Day On Day PNL" )
    }
    
    summaryTable = .createSummaryTable( dayOnDayPNL, newTradePNL )
    write.xls( summaryTable, file = OUTPUT_FILE, sheetname = "PNL Summary", 
            col.names = FALSE)
    
    rLog( "Done. Data written to:", OUTPUT_FILE )
    
    invisible( OUTPUT_FILE )
}


################################################################################
.createTable <- function( data )
{
    if( is.null( data ) ) return( NULL )
    
    data = .appendRowSpecifications( data )
    
    # We only want to chart data that has a supplied row specification
    data = data[ which( !is.na(data$ROW_SPEC) ), ]
    
    finalData = NULL
    for( typeIndex in seq_along( TYPE_SPEC ) )
    {
        typeName = names( TYPE_SPEC[typeIndex] )
        
        typeData = .getDataForType( data, typeName )
        
        finalData = rbind( finalData, typeData )
    }
    
    finalTable = .layoutFinalTable( finalData )
    
    return( finalTable )
}


################################################################################
.appendRowSpecifications <- function( data )
{
    data$ROW_SPEC = NA
    for( index in seq_along( ROW_SPEC ) )
    {
        spec = ROW_SPEC[index]
        
        filter = eval( parse( text=ROW_SPEC[[index]] ) )
        
        data$ROW_SPEC[ filter ] = names(spec)
    }
    
    return( data )
}


################################################################################
.layoutFinalTable <- function( finalData )
{
    finalTable = cast( finalData, ROW_SPEC + Type ~ COLUMN_SPEC, fill = 0 )
    
    # Lay out the columns how they're supposed to be
    columnsPresent = intersect( names(COLUMN_SPEC), names( finalTable ) )
    finalTable = finalTable[ , c( "ROW_SPEC", "Type", columnsPresent)]
    
    # Create a table to use to put the rows in the correct order
    orderingTable = data.frame( 
            index = seq_len( length(ROW_SPEC) * length(TYPE_SPEC) ), 
            ROW_SPEC = rep( names(ROW_SPEC), each=length(TYPE_SPEC) ), 
            Type = rep( names(TYPE_SPEC), length(ROW_SPEC) ) 
            )
    
    # Swap the rows how they're supposed to be    
    finalTable = merge( finalTable, orderingTable, sort=FALSE )
    finalTable = finalTable[order(finalTable$index),]
    finalTable$index = NULL
    
    # Remove the unneeded row labels
    unneededRows = which( seq_len( nrow( finalTable ) ) %% length(TYPE_SPEC) != 1 )
    finalTable$ROW_SPEC[ unneededRows ] = ""
    
    return( finalTable )
}


################################################################################
.getDataForType <- function( data, typeName )
{
    typeAggFunction = eval( parse( text=TYPE_SPEC[[ typeName ]] ) )
    
    finalData = NULL
    for( index in seq_along( COLUMN_SPEC ) )
    {   
        spec = COLUMN_SPEC[index]
        
        tempData = subset( data, eval( parse( text=spec ) ) )
        
        if( nrow( tempData ) == 0 ) { next }
        
        names(tempData)[ which( names( tempData ) == typeName ) ] = "value"
        dataToBind = cast( tempData, ROW_SPEC ~ . , typeAggFunction )
        names( dataToBind ) = c( "ROW_SPEC", "value")
        
        dataToBind$Type = typeName
        dataToBind$COLUMN_SPEC = names( spec )
        
        finalData = rbind( finalData, dataToBind )
    }
    
    return( finalData )
}


################################################################################
.createSummaryTable <- function( dayOnDayPNL, newTradePNL )
{
    totalPNL = sum( dayOnDayPNL$pnl ) + sum( newTradePNL$pnl )
    rLog( "Approximate Total Daily PnL:", dollar( totalPNL ) )
    rLog( "  From DeltaP * Q:", dollar( sum( dayOnDayPNL$pnl ) ) )    
    rLog( "  From new trades:", dollar( sum( newTradePNL$pnl ) ) )
    
    labels = c( 
            "Approximate Total PnL:", 
            "  From DeltaP * Q:", 
            "  From new trades:" )
    values = c( 
            dollar( totalPNL ),  
            dollar( sum( dayOnDayPNL$pnl ) ),
            dollar( sum( newTradePNL$pnl ) ) )
    table = data.frame( labels, values )
    
    return( table )
}



