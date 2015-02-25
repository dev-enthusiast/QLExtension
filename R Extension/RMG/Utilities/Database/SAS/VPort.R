###############################################################################
# VPort.R
#
# Code to access a VPort book hierarchy used in SAS. 
#
# Author: e14600
#

################################################################################
# External Libraries
#
library( RODBC )
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")


################################################################################
# File Namespace
#
VPort = new.env(hash=TRUE)


################################################################################
# Cached Data
#
VPort$vportHierarchy = NULL
VPort$oportHierarchy = NULL


################################################################################
VPort$getHierarchy <- function( asOfDate )
{
    if( is.null( VPort$vportHierarchy ) )
    {
        INPUT_FILE = paste( 
                "//ceg/cershare/All/Risk/data/VaR/prod/Portfolios/", 
                "vports.", asOfDate, ".RData", sep = "" )
        
        VPort$vportHierarchy = varLoad( "hierarchy", INPUT_FILE )
    }
    
    return( VPort$vportHierarchy )
}


################################################################################
VPort$booksForPortfolio <- function( portfolioName, asOfDate = Sys.Date()-1 )
{
    portfolio = toupper( portfolioName )
    
    hierarchy = VPort$getHierarchy( asOfDate )

    dataSubset = subset( hierarchy, toupper(VPORT) == portfolio )
    
    # Try the entry as an official portfolio...
    if( nrow( dataSubset ) == 0 )
    {
        dataSubset = data.frame( jobtype = "HDV-VSOR", VPORT = portfolio,
                NODE = portfolio, TYPE = "OPORT", Add_Sub = "+", 
                update_date = Sys.Date() )
    }

    bookList = VPort$.booksFromVPortData( dataSubset, asOfDate )
    
    bookList = sort( bookList )
    
    return( bookList )
}


################################################################################
VPort$.booksFromVPortData <- function( portfolioData, asOfDate )
{   
    # process adds
    includedPortfolios = subset( portfolioData, Add_Sub == "+")
    includedBooks = VPort$.extractBookData( includedPortfolios, asOfDate )
    
    # process subtracts
    removedPortfolios = subset( portfolioData, Add_Sub == "-")
    removedBooks = VPort$.extractBookData( removedPortfolios, asOfDate )
    
    # do a setdiff of the two
    finalBookList = unique( setdiff( includedBooks, removedBooks ) )
    
    return( finalBookList )
}


################################################################################
VPort$.extractBookData <- function( portfolioData, asOfDate )
{
    bookList = NULL
    
    for( rowIndex in seq_len( nrow( portfolioData ) ) )
    {
        rowData = portfolioData[rowIndex, ]
        
        portfolioType = toupper( rowData$TYPE )
        portfolio = toupper( rowData$NODE )
        
        if( portfolioType == "OPORT" )
        {               
            #cat("Retrieving OPORT:", portfolio, "\n")
            newBooks = VPort$.getBooksForOfficial( portfolio, asOfDate )
            
        } else if( portfolioType == "VPORT" )
        {
            #cat("Retrieving Synthetic portfolio:", portfolio, "\n")
            newBooks = VPort$booksForPortfolio( portfolio, asOfDate )
            
        } else if( portfolioType == "BOOK" )
        {
            newBooks = as.character( rowData$NODE ) 
            
        } else 
        {
            stop( "Invalid portfolio type in hierarchy:", portfolioType )
        }
        
        bookList = c( bookList, newBooks )
        
    }
    
    return( bookList )
}


################################################################################
VPort$.getBooksForOfficial <- function( portfolio, asOfDate )
{
    if( is.null( VPort$oportHierarchy ) )
    {
        VPort$oportHierarchy = get.portfolio.book.from.cpsprod( asOfDate ) 
    }
    
    bookData = subset( VPort$oportHierarchy, PARENT == portfolio )
    bookList = sort( unique( bookData$CHILD ) )
    
    return( bookList )
}









