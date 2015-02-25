###############################################################################
# This function retrieves the portfolio hierarchy from CPSPROD as well as 
# the RMG Synthetic Mapping. It then uses that to build a complete mapping
# of portfolios to books.
#
#  options <- NULL
#  options$asOfDate <- as.Date("2007-04-02")
#  options$run.type <- c("Overnight")
#  options$source.dir <- "H:/user/R/RMG/"
#  options$run.name <- "UPSTREAM PORTFOLIO"  # if don't specify this field
#                                            # it will return all overnight portfolios
#  source("H:/user/R/RMG/Utilities/RLogger.R")
#  source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
#  x <-  get.portfolio.book(options)
#
require(RODBC)
require(SecDb)

get.portfolio.book <- function(options=NULL)
{
    if (length(options$asOfDate)==0){options$asOfDate <- Sys.Date()-1}
    if (length(options$run.type)==0){options$run.type <- c("Overnight")}
    if (length(options$source.dir)==0){options$source.dir <- "H:/user/R/RMG/Energy/VaR/"}
    
    file <- paste(options$source.dir,"Base/schedule.overnight.xls", sep="")
    con <- odbcConnectExcel(file)
    syntheticHierarchy <- sqlFetch(con, "RMG_R_VAR_SYNTHETIC_PORTFOLIO") 
    syntheticHierarchy$RUN_NAME = toupper(syntheticHierarchy$RUN_NAME)
    syntheticHierarchy$CHILD = toupper(syntheticHierarchy$CHILD)
    
    overnightRuns <- sqlFetch(con, "RMG_R_VAR_RUN")
    overnightRuns$RUN_NAME = toupper(overnightRuns$RUN_NAME)
    odbcClose(con)
    
    # If the caller requested a specific portfolio, don't parse out the whole hierarchy
    if (!is.null(options$run.name))
    {
        overnightRuns <- subset( overnightRuns, 
                overnightRuns$RUN_NAME == toupper(options$run.name) )
    }
    
    cpsprodHierarchy = get.portfolio.book.from.cpsprod( options$asOfDate )
    
    finalDataSet = .getBookList( overnightRuns, syntheticHierarchy, cpsprodHierarchy )
    
    # They should all be books at this point
    names(finalDataSet)[which(names(finalDataSet)=="CHILD")] = "BOOK"
    
    finalDataSet = unique(finalDataSet)
    
    return(finalDataSet)
}


###############################################################################
get.portfolio.book.from.cpsprod <- function(asOfDate) 
{
    require(RODBC)
    file.dsn <- "CPSPROD.dsn"
    file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
            ";UID=stratdatread;PWD=stratdatread;", sep="")  
    cat("Connecting to CPSPROD...\n")
    con <- odbcDriverConnect(connection.string)  
    
    query <- paste("Select RP.Name Parent, RP.Type Parent_Type, RC.Name Child, ",
            "RC.Type Child_Type, RC.SDB_Name Child_SDB_Name, ",      
            "RC.SDB_Type Child_SDB_Type, RC.RA_Type Child_RA_Type,  ",        
            "RC.RMSYS_Type Child_RMSYS_Type  ",    
            "FROM foitsox.CCG_Portfolio_all H, ",         
            "foitsox.CCG_Portfolio_ID_Ref RP, ", 
            "foitsox.CCG_Portfolio_ID_Ref RC  ", 
            "WHERE H.PARENT_ID = RP.ID AND  ",
            "H.Child_ID = RC.ID AND  ",  
            "H.Pricing_Date = to_date('", asOfDate, "', 'yyyy-mm-dd') ",
            "and parent_id <> -1  order by Parent, Child ", sep="")
    
    cat("Querying CPSPROD for portfolio/book information...\n")
    pHier <- sqlQuery(con, query)
    odbcClose(con)
    
    ind <- sapply(pHier , class) %in% c("factor", "logical")
    pHier[,ind] <- sapply(pHier[,ind], function(x){toupper(as.character(x))})
    return(pHier)
}


###############################################################################
# The worker function of the process. For each row in runDataSet, call
# .extractBookData to give a complete book listing and add it to the
# final result set.
#
.getBookList <- function( runDataSet, syntheticHierarchy, cpsprodHierarchy )
{
    # use this to store the final results of the expansion
    finalListing = NULL
    
    for( rowIndex in seq_len(nrow(runDataSet)) )
    {
        rowData = runDataSet[rowIndex, ]
        
        portfolio = toupper(rowData$RUN_NAME)
        
        bookData = .extractBookData( portfolio, rowData, 
                cpsprodHierarchy, syntheticHierarchy )
        
        if( is.null(bookData) )
        {
            stop("NULL bookData retrieved from .extractBookData. Find out why!")
        }
        
        rowData$SYSTEM_SOURCE = NULL
        rowData$WEIGHT = NULL
        rowData$BOOK = NULL
        bookData = merge(rowData, bookData)
        
        finalListing = rbind(finalListing, bookData)
    }
    
    return( finalListing )
}


###############################################################################
# This function extracts book data from the respective systems.
# It must return a data frame of children, weights and system sources respectively
#
.extractBookData <- function( portfolio, rowData, cpsprodHierarchy, syntheticHierarchy )
{
    systemSource = rowData$SYSTEM_SOURCE
    
    if( systemSource == "SECDB" )
    {
        #cat("Retrieving SecDb portfolio:", portfolio, "\n")
        bookList = .getPortfolioBooksFromSecDb( portfolio )
        weight = ifelse( length(rowData$WEIGHT)==0, 1, rowData$WEIGHT )
        
        bookData = data.frame( CHILD=bookList, WEIGHT = weight, SYSTEM_SOURCE="BOOK")
        
    } else if( systemSource == "CPSPROD" )
    {   
        #cat("Retrieving CPSProd portfolio:", portfolio, "\n")
        bookList = .getPortfolioBooksFromCPSPROD( portfolio, cpsprodHierarchy )
        weight = ifelse( length(rowData$WEIGHT)==0, 1, rowData$WEIGHT )
        
        bookData = data.frame( CHILD=bookList, WEIGHT = weight, SYSTEM_SOURCE="BOOK")
        
    } else if( systemSource == "SYNTHETIC" )
    {
        #cat("Retrieving Synthetic portfolio:", portfolio, "\n")
        bookData = .getSyntheticPortfolioBooks( portfolio, syntheticHierarchy, cpsprodHierarchy )
        if( length(rowData$WEIGHT)==0 && length(bookData$WEIGHT)==0 )
        {
            bookData$WEIGHT = 1
            
        }else if( length(rowData$WEIGHT)!=0 )
        { 
            bookData$WEIGHT = rowData$WEIGHT
        }
        
    } else if( systemSource == "BOOK" )
    {
        bookData = data.frame( CHILD=rowData$RUN_NAME, WEIGHT=1, SYSTEM_SOURCE="BOOK")
        
    } else 
    {
        stop( "Invalid system source in hierarchy:", systemSource )
    }
    
    return( bookData )
}


###############################################################################
.getPortfolioBooksFromCPSPROD <- function( portfolio, hierarchy )
{
    portfolioData = subset( hierarchy, PARENT==toupper(portfolio) )
    bookList = sort( unique(portfolioData$CHILD) )
    
    if( length(bookList) == 0 )
    {
        stop("No books found for portfolio: ", portfolio)
    }
    
    return( bookList )
}

.test.getPortfolioBooksFromCPSPROD <- function()
{    
    options <- NULL
    options$asOfDate <- as.Date("2008-03-07")
    hierarchy = get.portfolio.hierarchy.from.cpsprod(options)
    
    # Test a single level-depth portfolio
    joesBooks = c("JKCASH", "JKNG", "NY1ED", "NYOPTION", "NYTCC1", "NYTRADE")
    bookList = .getPortfolioBooksFromCPSPROD( "JKirkpatrick Trading Portfolio", hierarchy )
    checkEquals( bookList, joesBooks )
    
    # Test a multi-level portfolio
    jeffsBooks = c("DLPJM", "JA2ED", "JADOPT", "JAMOPT", "JANYPP", "JAPJM",
            "JAPNLXFR", "MBMOPT", "MBPJM", "MIDCNOPT", "MIDCO1ED", "SMECAR",  
            "SMNEPOOL", "SMNYPP", "SMPJMDOP", "SMPJMFIN", "SMPJMMOP", "SMPNLXFR" )
    bookList = .getPortfolioBooksFromCPSPROD( "JAddison Trading Portfolio", hierarchy )
    checkEquals( bookList, jeffsBooks )
    
    # Test an invalid portfolio
    checkException( .getPortfolioBooksFromCPSPROD("Foo Portfolio", hierarchy), silent=TRUE )
    
    invisible(NULL)
}


###############################################################################
.getPortfolioBooksFromSecDb <- function( portfolio )
{
    bookList = names(secdb.getValueType(portfolio, "Nodes")$Book)
    
    if( length(bookList) == 0 )
    {
        stop("No books found for portfolio: ", portfolio)
    }
    
    return( bookList )
}

.test.getPortfolioBooksFromSecDb <- function()
{    
    # Test a single level-depth portfolio
    joesBooks = c("JKCASH", "JKNG", "NY1ED", "NYOPTION", "NYTCC1", "NYTRADE")
    bookList = .getPortfolioBooksFromSecDb( "JKirkpatrick Trading Portfolio" )
    checkEquals( bookList, joesBooks )
    
    # Test a multi-level portfolio
    jeffsBooks = c("DLPJM", "JA2ED", "JADOPT", "JAMOPT", "JANYPP", "JAPJM",
            "JAPNLXFR", "MBMOPT", "MBPJM", "MIDCNOPT", "MIDCO1ED", "SMECAR",  
            "SMNEPOOL", "SMNYPP", "SMPJMDOP", "SMPJMFIN", "SMPJMMOP", "SMPNLXFR" )
    bookList = .getPortfolioBooksFromSecDb( "JAddison Trading Portfolio" )
    checkEquals( bookList, jeffsBooks )
    
    # Test an invalid portfolio
    checkException( .getPortfolioBooksFromSecDb("Foo Portfolio"), silent=TRUE )
    
    invisible(NULL)
}

###############################################################################
# Example:
# portfolio <- "NEPOOL Gen PORTFOLIO"
# .getPortfolioTreeFromSecDb(portfolio, tree=list())
#
.getPortfolioTreeFromSecDb <- function( portfolio, tree=list() )
{
  children   <- secdb.getValueType(portfolio, "Children")
  childNames <- names(children)
  
  for (i in seq_along(children)){
    if (!is.null(secdb.getValueType(childNames[i], "Book Name"))){
      tree[[childNames[i]]] <- 1
    } else {
      tree[[childNames[i]]] <- list()
      tree[[childNames[i]]] <- .getPortfolioTreeFromSecDb(childNames[i], tree[[childNames[i]]])
    }
  } 
 
  return( tree )
}


###############################################################################
.getSyntheticPortfolioBooks <- function( portfolio, syntheticHierarchy, cpsprodHierarchy )
{
    synthetics = subset( syntheticHierarchy, toupper(RUN_NAME)==portfolio )
    
    if( nrow(synthetics) == 0 )
    {
        stop("No synthetic mapping found for portfolio: ", portfolio)
    }
    
    bookData = NULL
    newData = NULL
    toBeProcessed = synthetics
    
    # take all books, store them separately, and remove them from the set
    bookIndex = which(toBeProcessed$SYSTEM_SOURCE=="BOOK")
    if( length(bookIndex) > 0 )
    {
        # Store just the books
        bookData = toBeProcessed[bookIndex,]
        
        # Remove them so we can continue to parse out the synthetics, secdb ports, etc
        toBeProcessed = toBeProcessed[-bookIndex,]
    }
    
    # for what's left in toBeProcessed...
    if( nrow(toBeProcessed) > 0 )
    {
        originalName = as.character(unique(toBeProcessed$RUN_NAME))
        
        toBeProcessed$RUN_NAME = toBeProcessed$CHILD
        toBeProcessed$CHILD = NULL
        
        newData = .getBookList( toBeProcessed, syntheticHierarchy, cpsprodHierarchy )
        newData$RUN_NAME = originalName
    }
    
    finalData = rbind( bookData, newData )
    
    return( finalData )
}



