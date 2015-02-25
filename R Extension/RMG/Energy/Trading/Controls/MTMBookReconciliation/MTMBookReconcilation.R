###############################################################################
# UnflaggedBooks.R
#
# Author: e14600
#
memory.limit( 4095 )
require( RODBC )
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Utilities/sendemail.R")
source("H:/user/R/RMG/Utilities/write.xls.R")



###############################################################################
# Get all the books in grizzly 
# 
getGrizzlyBooks <- function()
{
    query = "select distinct book, acct_treatment from bookmgr.rmg_bookmappings"
   
    file.dsn <- "GRIZZLYP.dsn"
    file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
            ";UID=bookmgr_read;PWD=bookmgr_read;", sep="")  
    rLog("Connecting to Grizzly...")
    con <- odbcDriverConnect(connection.string)  
    
    data = sqlQuery( con, query )
    odbcClose( con )
    
    data = data[ do.call( order, data ), ]
    rownames( data ) = NULL
    
    return( data )
}


###############################################################################
# Get all the books in pnl
# 
getPNLBooks <- function()
{
    query = "select distinct ltrim(rtrim(book)) as book, ltrim(rtrim(accounting_method)) as acct_treatment from plr.VW_STRATEGY_ACCT_TREATMENT"
    
    file.dsn <- "PLRP.dsn"
    file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
            ";UID=plr_read;PWD=plr_read;", sep="")  
    rLog("Connecting to PNL...")
    con <- odbcDriverConnect(connection.string)  
    
    data = sqlQuery( con, query )
    odbcClose( con )
    
    data = data[ do.call( order, data ), ]
    rownames( data ) = NULL
    
    return( data )
}


###############################################################################
# Get all the books in pnl
# 
getControllersBooks <- function()
{
    query = "select book_name from bookmgr.vw_book_sor_all where cntrlr_book_flg = 'Y'"

    file.dsn <- "GRIZZLYP.dsn"
    file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
            ";UID=bookmgr_read;PWD=bookmgr_read;", sep="")  
    rLog("Connecting to Grizzly...")
    con <- odbcDriverConnect(connection.string)  
    
    data = sqlQuery( con, query )
    odbcClose( con )
    
    bookList = as.character( data$BOOK_NAME )
    
    return( bookList )
}



################################################################################
sendStatusEmail <- function( outputFile, notEqual, grizzlyMissing, pnlMissing )
{
    body = "Grizzly to PNL Accounting Treatment Reconcilation:"
    
    if( nrow( notEqual ) > 0 )
    {
        notEqualHeader = paste( "There are", nrow(notEqual),
                "mismatched books between Grizzly & PNL" )
        
        body = paste( body, notEqualHeader, sep="\n\n")
    }
    
    if( nrow( grizzlyMissing ) > 0 )
    {
        grizzlyHeader = paste( "There are", nrow(grizzlyMissing),
                "books in PNL that are missing or unassigned in Grizzly" )
        
        body = paste( body, grizzlyHeader, sep="\n\n")
    }
    
    if( nrow( pnlMissing ) > 0 )
    {
        pnlHeader = paste( "There are", nrow(pnlMissing),
                "books in Grizzly that are missing or unassigned in PNL" )
        
        body = paste( body, pnlHeader, sep="\n\n")
    }
    
    
    body = paste( body,
            "Please see the attached file for details.",
            "RAC, Risk Systems\nJohn.Scillieri@constellation.com", sep="\n\n")
    
    subject = paste( "Grizzly/PNL Rec", format( Sys.Date(), "%d%b%y" ) )
    
    toList = getEmailDistro("GrizzlyRec")
    
    sendEmail( from="DO-NOT-REPLY@constellation.com", to=toList, 
            subject, body, attachments = outputFile )
}



################################################################################
main <- function()
{
    outputFile = "S:/All/Risk/Data/VaR/prod/Controls/AccountingTreatmentRec.xls"
    file.remove( outputFile )
    
    grizzly = getGrizzlyBooks()
    grizzly$BOOK = as.character( grizzly$BOOK )
    grizzly$ACCT_TREATMENT = as.character( grizzly$ACCT_TREATMENT )
    
    pnl = getPNLBooks()
    pnl = pnl[ -which( pnl$BOOK %in% getControllersBooks() ),]
    
    pnl$BOOK = as.character( pnl$BOOK )
    pnl$ACCT_TREATMENT = as.character( pnl$ACCT_TREATMENT )
    pnl$ACCT_TREATMENT[which( pnl$ACCT_TREATMENT %in% "MTM - HOA Expense" )] = "Mark to Market"
    accrualAccountingTreatments = c( "Hedge", "Inventory", "Lease", "Modeling", "Normal" )   
    pnl$ACCT_TREATMENT[which( pnl$ACCT_TREATMENT %in% accrualAccountingTreatments)] = "Accrual"
    
    rec = merge( grizzly, pnl, all=TRUE, by="BOOK", suffixes=c("_GRIZZLY", "_PNL") )
    notEqual = rec[ which( rec$ACCT_TREATMENT_GRIZZLY != rec$ACCT_TREATMENT_PNL ), ] 
    grizzlyMissing = rec[ which( is.na( rec$ACCT_TREATMENT_GRIZZLY )), ]
    pnlMissing = rec[ which( is.na( rec$ACCT_TREATMENT_PNL )), ]

    if( nrow( notEqual ) > 0 )
        write.xls( notEqual, file=outputFile, sheetname="TreatmentNotEqual" )
    if( nrow( grizzlyMissing ) > 0 )
        write.xls( grizzlyMissing, file=outputFile, sheetname="MissingOrNullInGrizzly" )
    if( nrow( pnlMissing ) > 0 )
        write.xls( pnlMissing, file=outputFile, sheetname="MissingOrNullInPNL" )
    
    sendStatusEmail( outputFile, notEqual, grizzlyMissing, pnlMissing )
    
    rLog("Done.")
}


################################################################################
main()

