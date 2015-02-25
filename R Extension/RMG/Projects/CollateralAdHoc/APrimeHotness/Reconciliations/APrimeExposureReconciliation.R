###############################################################################
# Ensure that you can use aprime before you go shooting yourself in the foot.
# 
# Author:	J. Scillieri
# Added:	APrimeExposureReconcilation.R Initiated Mar 27, 2009
# 
library( RODBC )
source( "H:/user/R/RMG/Utilities/RLogger.R" )
source( "H:/user/R/RMG/Utilities/dollar.R" )


asOfDate = Sys.Date()-3
rLog( "Running with asOfDate:", as.character( asOfDate ) )


dsnFile = "RMGDBP.dsn"
dsnPath = "H:/user/R/RMG/Utilities/DSN/"
connection.string <- paste("FileDSN=", dsnPath, dsnFile, 
        ";UID=RMGDBUSER;PWD=RMGDBPWD_PROD;", sep="")  
aprimeCon <- odbcDriverConnect(connection.string)  

aprimeQuery = paste(" SELECT 
                upper(counterparty) counterparty, 
                SUM(EXPOSURE) aprime_exposure
                FROM rmgsas.aprime_base
                WHERE asofdate = '", format( asOfDate, "%d%b%y"), "'
                group by counterparty
                order by counterparty", sep="")
aprimeExposureData = sqlQuery( aprimeCon, aprimeQuery )
odbcClose( aprimeCon )


file.dsn <- "RaftEnergyCreditManual.dsn"
file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
        ";UID=ExecSPOnly;PWD=EnergyCredit_ExecSPOnly;", sep="")  

raftCon <- odbcDriverConnect(connection.string)  

raftQuery = paste(
        "select ",
        "rtrim( upper( Credit_Counterparty ) ) COUNTERPARTY, ",
        "sum( Credit_MTM + Credit_DNP ) RAFT_EXPOSURE ",
        "from energycreditarchive.dbo.TradeAndCollateralForSAS_NA where ", 
        "Credit_DataAsOfDate='", format( asOfDate, "%d%b%y" ), "' ",
        "group by Credit_Counterparty ",
        "order by Credit_Counterparty ",
        sep="" ) 

raftExposureData = sqlQuery( raftCon, raftQuery )
odbcClose( raftCon )



finalExposureData = merge( aprimeExposureData, raftExposureData, all=TRUE )
finalExposureData$DIFF = round( finalExposureData$APRIME_EXPOSURE - finalExposureData$RAFT_EXPOSURE )
finalExposureData = finalExposureData[which(finalExposureData$DIFF != 0 | 
                        (is.na(finalExposureData$RAFT_EXPOSURE) & !is.na(finalExposureData$APRIME_EXPOSURE)) |
                        (!is.na(finalExposureData$RAFT_EXPOSURE) & is.na(finalExposureData$APRIME_EXPOSURE))),]
row.names(finalExposureData) = NULL
finalExposureData$APRIME_EXPOSURE = dollar( finalExposureData$APRIME_EXPOSURE, 0 )
finalExposureData$RAFT_EXPOSURE = dollar( finalExposureData$RAFT_EXPOSURE, 0 )
finalExposureData$DIFF = dollar( finalExposureData$DIFF, 0 )

#sink("C:/Documents and Settings/e14600/Desktop/aprime_exposure_rec.txt")
print( finalExposureData )
#sink()


