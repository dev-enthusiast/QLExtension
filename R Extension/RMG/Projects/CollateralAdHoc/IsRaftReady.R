################################################################################
# Is the Raft mega report (and derivative data) ready to be pulled?
# 
# Returns 0 if ready, 1 if not (per Procmon status codes).
#
library( RODBC )

query = paste("declare @IsDataReady int;",
        "exec EnergyCreditManual.dbo.CPSIsDataReady 'TradeAndCollateralForSAS_NA', @IsDataReady output;",
        "select @IsDataReady" )

file.dsn <- "RaftEnergyCreditManual.dsn"
file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
        ";UID=ExecSPOnly;PWD=EnergyCredit_ExecSPOnly;", sep="")  

con <- odbcDriverConnect(connection.string)  
result = sqlQuery( con, query )
odbcClose( con )

# Sucess is 1 in the DB and 0 in the shell so we need to flip them
returnCode = ifelse( result$"", 0, 1 )

if( interactive() )
{
    print( returnCode )
}else
{
    q( status = returnCode )
}

