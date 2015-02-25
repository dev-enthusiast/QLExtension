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
# Get all the books in grizzly with no risk reporting flag
# 
getAllUnflaggedBooks <- function()
{
    query = "select * from bookmgr.rmg_bookmappings where risk_reporting_flag is null"
    
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


################################################################################
sendStatusEmail <- function( outputFile, summaryTable, newlyAddedTable,
        removedTable, unmappedBookCount )
{
    headerLine = paste( "There are", unmappedBookCount, "books with no", 
            "Risk Reporting flag set in Grizzly." )
    
    body = headerLine
    
    if( nrow( newlyAddedTable ) > 0 )
    {
        newBookHeader = paste( "The following books are new to this report", 
                "since the last run:" )
        outputTable = data.frame( apply( newlyAddedTable, 2, as.character),
                check.names = FALSE )
        outputTable = capture.output( outputTable )
        
        body = paste( body, newBookHeader, 
                paste( outputTable, collapse="\n"), sep="\n\n")
    }
    
    if( nrow( removedTable ) > 0 )
    {
        removedBookHeader = paste( "The following books are no longer missing", 
                "their risk reporting flag:" )
        outputTable = data.frame( apply( removedTable, 2, as.character),
                check.names = FALSE )
        outputTable = capture.output( outputTable ) 
        
        body = paste( body, removedBookHeader, 
                paste( outputTable, collapse="\n"), sep="\n\n")
    }
    
    summaryOutput = capture.output( print.table(summaryTable[,-1]) )
    
    body = paste( body,
            "\nSummary Statistics:",
            paste( summaryOutput, collapse="\n"), 
            "Please see the attached file for details.",
            "RAC, Risk Systems\nJohn.Scillieri@constellation.com", sep="\n\n")
    
    subject = paste( "Unflagged Book Report", format( Sys.Date(), "%d%b%y" ) )
    
    toList = getEmailDistro("UnflaggedBookReport")
    
    sendEmail(from="DO-NOT-REPLY@constellation.com",
            to=toList, subject, body, attachments = outputFile )
}

################################################################################
formatBookTable <- function( data )
{
    columnsToKeep = c("BOOK", "OFFICE", "ACCT_TREATMENT", "ORGANIZATION", 
            "PORTFOLIO_MGR", "PM_REGION")
    formattedTable = data[ , columnsToKeep ]
    names( formattedTable ) = c( "Book", "Office", "Acct Treatment", "Org", 
            "Portfolio Mgr", "PM Region" )
    
    row.names( formattedTable ) = NULL
    
    return( formattedTable )
    
}


################################################################################
main <- function()
{
    options(width=300)
    DATA_FILE = "S:/All/Risk/Data/VaR/prod/Controls/UnflaggedBooks.RData"
    
    data = getAllUnflaggedBooks()
    
    if( nrow( data ) == 0 )
    {
        rLog("Sending email...")
        toList = getEmailDistro("UnflaggedBookReport")
        subject = paste( "Unflagged Book Report", format( Sys.Date(), "%d%b%y" ) )
        sendEmail( from = "DO-NOT-REPLY@constellation.com",
                to = toList, 
                subject = subject,
                body = "There are no books missing their Risk Reporting Flag" ) 
        
        rLog("Done.")
        return( NULL )
    }
    

    oldData = varLoad( "data", DATA_FILE )
    newlyAddedTable = formatBookTable( 
            data[-which( data$BOOK %in% oldData$BOOK ),] )
    removedTable = formatBookTable( 
            oldData[-which( oldData$BOOK %in% data$BOOK ),] )
    
    # We can save the new data now that we're done looking at the old stuff
    rLog("Saving data...")
    save( data, file = DATA_FILE )

    rLog("Formatting table & summary statistics...")
    forSummary = formatBookTable( data )
    
    summaryTable = summary( forSummary )
    
    forSummary[which(is.na(forSummary))] = ""
    toOutput = forSummary 
    
#    toOutput = apply( forSummary, 2, function(x){       
#                missing = which( is.na(x) ) 
#                x[ missing ] = "" 
#                return(x) }
#    )
#    toOutput = data.frame( toOutput, check.names = FALSE )     # has error when toOutput row number is 1
    
    rLog("Writing output file...")
    template = "S:/All/Risk/Software/R/prod/Energy/Trading/Controls/UnflaggedBookReport/template.xlsx"
    #outputFile = "S:/All/Risk/Software/R/prod/Energy/Trading/Controls/UnflaggedBookReport/UnflaggedBooks.xlsx"
    outputFile = paste("S:/All/Risk/Software/R/prod/Energy/Trading/Controls/UnflaggedBookReport/emails/UnflaggedBooks",
                       " asOf ", Sys.Date(),".xlsx",sep="")
    
    file.copy( template, outputFile, overwrite = TRUE )
    write.xls( toOutput, file = outputFile, "Sheet1" )
    
    rLog("Sending email...")
    sendStatusEmail( outputFile, summaryTable, newlyAddedTable, 
            removedTable, nrow( toOutput ) )

    rLog("Done.")
}


################################################################################
main()

