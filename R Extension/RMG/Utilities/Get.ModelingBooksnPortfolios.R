# Extract modeling books and associated portfolios

library( SecDb )
library( RODBC )

asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(),'-1b'))

file.dsn          <- "RaftEnergyCreditManual.dsn"
file.dsn.path     <- "H:/user/R/RMG/Utilities/DSN/"
connection.string <- paste( "FileDSN=", file.dsn.path, file.dsn,
                            ";UID=ExecSPOnly;PWD=EnergyCredit_ExecSPOnly;", sep="" )

con               <- odbcDriverConnect(connection.string)
query             <- "select	* from EnergyCreditStaging.dbo.PNL_AccountingMethod
                      where UPPER(accounting_method) = UPPER('modeling')"
Books             <- as.character( sqlQuery( con, query )$BOOK )
odbcClose(con)

dsnFile           <- "CPSPROD.dsn"
dsnPath           <- "H:/user/R/RMG/Utilities/DSN/"
connection.string <- paste( "FileDSN=", dsnPath, dsnFile,
                            ";UID=foitsoxread;PWD=foitsoxread;", sep="" )
con               <- odbcDriverConnect(connection.string)

for( b in seq_along(Books) )
{
    Query    <- paste( 'SELECT ID, TYPE ',
                       'FROM FoitSOX.ccg_portfolio_id_ref ',
                       "WHERE  Name = '", Books[b], "'",
                       sep = '');
    R1       <- sqlQuery( con, Query )

    Query    <- paste( 'SELECT Parent_ID ',
                       'FROM FoitSOX.ccg_portfolio_all ',
                       "WHERE  Child_ID = '", R1$ID, "'",
                       "AND Pricing_Date = to_date('", format(asOfDate, "%d%b%y"), "')",
                       sep = '');
    R2       <- sqlQuery( con, Query )

    Query    <- paste( 'SELECT ID, NAME, TYPE ',
                       'FROM FoitSOX.ccg_portfolio_id_ref ',
                       "WHERE ID in ('", paste(as.character(R2$PARENT_ID), collapse="','"),"')",
                       sep = '');
    Record   <- sqlQuery( con, Query )
    Idx      <- which( Record$TYPE == 'PORTFOLIO' )

    Books[b] <- paste(Books[b], " % '", paste(as.character(Record$NAME[Idx]), collapse="','"), "'", sep='')
 }

odbcClose( con )

Books