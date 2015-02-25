# Return the books that make up a portfolio
#
#
#


###############################################################################
secdb.getBooksFromPortfolio <- function( portfolio, from="SecDb"  )
{
    books <- names(secdb.getValueType(portfolio, "Nodes")$Book)
    
    if( length(books) == 0 )
    {
        stop("No books found for portfolio: ", portfolio)
    }
    
    return( books )
}
