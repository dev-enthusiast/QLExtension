###############################################################################
# Ensure that you can use aprime before you go shooting yourself in the foot.
# 
# Author:	J. Scillieri
# Added:	APrimeReconcilation.R Initiated Mar 27, 2009
# 
library( RODBC )
source( "H:/user/R/RMG/Utilities/RLogger.R" )

printCurveDifferences <- function( final, counterparty )
{
    cpData = subset( finalCurveData, COUNTERPARTY==counterparty )
    cpData$COUNTERPARTY = NULL
    
    rLog(counterparty, "Curve Differences:")
    print( cpData )
    rLog("\n")
}



asOfDate = Sys.Date()-1
rLog( "Running with asOfDate:", as.character( asOfDate ) )


dsnFile = "RMGDBP.dsn"
dsnPath = "H:/user/R/RMG/Utilities/DSN/"
connection.string <- paste("FileDSN=", dsnPath, dsnFile, 
        ";UID=RMGDBUSER;PWD=RMGDBPWD_PROD;", sep="")  
aprimeCon <- odbcDriverConnect(connection.string)  

aprimeCurveQuery = paste(" SELECT upper(counterparty2) counterparty, 
				upper(commod_curve) commod_curve,
                SUM(position) aprime_delta
                FROM rmgsas.aprime_base
                WHERE asofdate = '", format( asOfDate, "%d%b%y"), "'
                group by counterparty2, commod_curve
                order by counterparty2, commod_curve", sep="")
aprimeCurveData = sqlQuery( aprimeCon, aprimeCurveQuery )
odbcClose( aprimeCon )


file.dsn <- "VCTRLP.dsn"
file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
        ";UID=vcentral_read;PWD=vcentral_read;", sep="")  
rLog("Connecting to VCentral...")
vcvCon <- odbcDriverConnect(connection.string)  


vcvCurveQuery = paste(" SELECT upper( counterparty ) counterparty,
				upper(location_nm) commod_curve,
                SUM(delta) vcv_delta
                FROM vcv.app_positions_detail
                WHERE valuation_date = '", format( asOfDate, "%d%b%y"), "'
                group by counterparty, location_nm
                order by counterparty, location_nm", sep="")
vcvCurveData = sqlQuery( vcvCon, vcvCurveQuery )
odbcClose( vcvCon )

finalCurveData = merge( aprimeCurveData, vcvCurveData, all=TRUE )
finalCurveData$APRIME_DELTA[which(is.na(finalCurveData$APRIME_DELTA))] = 0
finalCurveData$VCV_DELTA[which(is.na(finalCurveData$VCV_DELTA))] = 0
finalCurveData$DIFF = round( finalCurveData$APRIME_DELTA - finalCurveData$VCV_DELTA )
finalCurveData = finalCurveData[which(finalCurveData$DIFF != 0 & finalCurveData$VCV_DELTA != 0),]
row.names(finalCurveData) = NULL



nonMatchingList = as.character( sort( unique( finalCurveData$COUNTERPARTY ) ) )

#sink("C:/Documents and Settings/e14600/Desktop/aprime_position_rec.txt")
for( counterparty in nonMatchingList )
{
    printCurveDifferences( finalCurveData, counterparty )
}
#sink()


