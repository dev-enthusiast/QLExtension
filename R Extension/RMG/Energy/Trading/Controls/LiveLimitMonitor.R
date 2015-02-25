################################################################################
# This program will display a matrix of either positions or changes in 
# positions for a given book. It will pull the live position data if 
# requested and can be run on a set of books for a given portfolio.
#
# Configuration parameters (change these to modify the program):
#	PORTFOLIO - the port you'd like to pull deltas for
#	


################################################################################
# CONFIGURATION PARAMETERS
#
PORTFOLIO = "East Power Trading Portfolio"
DISTRO_LIST = c( 
        "john.scillieri", "adrian.dragulescu", 
        "joseph.deluzio", "margot.everett" )


################################################################################
library(reshape)
source("H:/user/R/RMG/Utilities/rLogger.R") 
source("H:/user/R/RMG/Utilities/load.R") 
source("H:/user/R/RMG/Utilities/sendEmail.R")
source("H:/user/R/RMG/Utilities/Database/R/LiveData.R")
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Projects/PV/Term.R")


###############################################################################
main <- function()
{  
    data = readPositionData()
    
    limits = readLimits()
    
    failedLimits = checkAllLimits(data, limits)
    
    emailResults( failedLimits )
    
    invisible(data)
}


###############################################################################
readPositionData <- function()
{    
    options = NULL
    options$run.name = PORTFOLIO
    bookList = get.portfolio.book( options )$BOOK
    
    livePositions = LiveData$cpsprodPositions()
    
    bookData = subset(livePositions, 
            livePositions$book %in% bookList &
                    contract.dt < as.Date("2009-01-01") )
    
    bookData$book = NULL
    names(bookData)[3]="value"
    bookData$value = round(bookData$value)
    
    changeMatrix = cast(bookData, curve.name ~ contract.dt, 
            fun.aggregate=sum, fill=0)
    
    return( changeMatrix )
}


###############################################################################
getPositionValue <- function( data, curveName, term )
{
    startDate = Term$startDate(term)
    endDate = as.Date( format(Term$endDate(term), "%Y-%m-01") )
    dateRange = as.character( seq(startDate, endDate, by="month") )
    
    dataToCheck = subset(data, curve.name == curveName )
    dataToCheck = dataToCheck[, dateRange]
    
    current = sum(dataToCheck)
    
    return(current)
}


###############################################################################
underLimit <- function( current, limit )
{
    if( limit > 0 )
    {
        underLimit = ( current < limit )   
        
    } else if( limit < 0 )
    {
        underLimit = ( current > limit )  
        
    } else
    {
        error("ZERO LIMIT ENTERED. EXITING.")
    }
    
    return( underLimit )
}


###############################################################################
readLimits <- function()
{
    limits = data.frame(
            curveName=c("COMMOD PWJ 5X16 WEST PHYSICAL"),
            term=c("CAL 08"),
            limit=c(-6000000) )
    
    return(limits)
}


###############################################################################
checkAllLimits <- function( data, limits )
{
    failedLimits = NULL
    
    for( limitIndex in seq_len(nrow(limits)) )
    {
        limit = limits[limitIndex,]
        
        current = getPositionValue( data, limit$curveName, limit$term )
        
        rLog("Checking ", limit$curveName, " (", limit$term, ") Limit:", 
                limit$limit, " Value: ", current, sep="")
        
        if( !underLimit( current, limit$limit ) )
        {
            limit$current = current
            failedLimits = rbind(failedLimits, limit)
        }
    }
    
    return(failedLimits)
}


###############################################################################
# takes a data frame and if nrow > 0, emails results to a list.
#
emailResults <- function( results )
{
    if( is.null(results) )
    {
        return(NULL)
    }
    
    # Give the data frame some room to run
    options(width=300)
    
    rLog("Sending Email...")
    body = paste("Time:", Sys.time())
    body = paste( body, 
                    paste( capture.output( format(results, justify="left") ), 
                    collapse="\n" ), 
            "CCG RMG Analytics\nJohn.Scillieri@constellation.com", sep="\n\n")
    
    subject = paste("LIMIT VIOLATION NOTICE -", PORTFOLIO )
    toList = DISTRO_LIST
    toList = "john.scillieri"
    toList = paste( paste(toList, "@constellation.com", sep=""), collapse=", ")
    
    sendEmail(from="DO-NOT-REPLY@constellation.com",
            to=toList, subject, body)
    
    rLog(subject)
}


###############################################################################
start = Sys.time()

data = main()

outputFile = paste("H:/", gsub(" ", ".", PORTFOLIO), ".csv", sep="")
write.csv( data, file=outputFile, row.names=FALSE )

print(data)
print(Sys.time()-start)



