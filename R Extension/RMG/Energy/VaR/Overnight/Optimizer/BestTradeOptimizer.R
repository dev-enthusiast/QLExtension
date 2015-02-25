###############################################################################
# BestTradeOptimizer.R
#
# This file runs the overnight var optimization process
#


################################################################################
# Required library files
source("H:/user/R/RMG/Energy/Tools/OptimizerEngine.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
source("H:/user/R/RMG/Projects/PV/Term.R")
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Utilities/rLogger.R") 
source("H:/user/R/RMG/Utilities/sendEmail.R")



################################################################################
# File Namespace
#
BestTradeOptimizer = new.env(hash=TRUE)


################################################################################
# CONSTANTS
#
BestTradeOptimizer$LIMIT_DIR = "//ceg/cershare/All/Risk/Software/R/prod/Energy/VaR/Overnight/Optimizer/limits/"


################################################################################
BestTradeOptimizer$main <- function( )
{
    options(width=300)
    
    asOfDate   <- overnightUtils.getAsOfDate()
    options = overnightUtils.loadOvernightParameters(asOfDate, FALSE)
    
    limitFiles = list.files( BestTradeOptimizer$LIMIT_DIR, full.names=TRUE )
    
    for( limitFile in limitFiles )
    {
        portfolio = gsub( ".xls", "", basename(limitFile) )
        
        ivarFile = paste( options$save$overnight.dir, "/", portfolio, "/IVaR.", 
                basename(limitFile), sep="" )
        
        output = BestTradeOptimizer$findBestTrades( ivarFile, limitFile, sasVaR=NULL )
        
        outputFile = paste( options$save$overnight.dir, "/", portfolio, 
                "/", portfolio, ".bytrade.txt", sep="" )
        sink(outputFile)
        printResults(output)
        sink()
    }
    
    invisible(output)
}


################################################################################
BestTradeOptimizer$findBestTrades <- function( ivarFile, limitsFile, sasVaR=NULL )
{
    # read the ivar info from spreadsheet
    ivarData = OptimizerEngine$getIVaRFromFile( ivarFile ) 
    
    # read the limits info from spreadsheet
    LL    <- OptimizerEngine$getLimitsFromFile( ivarData, limitsFile )
    
    # now we have to run over all the combinations
    uniqueTrades = as.character(unique(LL$comments))
    
    numTrades = BestTradeOptimizer$getMaxTrades( length(uniqueTrades) )
    rLog("Finding the maximum", numTrades, "best trades...")
    numTrades = 1:numTrades
    
    combinationList = lapply(numTrades, combn, x=length(uniqueTrades), simplify=FALSE)
    combinationList = unlist(combinationList, recursive=FALSE)
    
    results = NULL
    for( combinationIndex in seq_along(combinationList) )
    {
        combination = combinationList[[combinationIndex]]
        rLog("\n\nRunning", combinationIndex, "of", length(combinationList))
        
        tradeList = uniqueTrades[combination]
        print(tradeList)
        
        optimizedResults = try( BestTradeOptimizer$runTradeSubset( tradeList, LL, ivarData, sasVaR ) )
        
        if (class(optimizedResults)!="try-error")
        {
            if( length(results) < length(combination) )
            {
                results[[length(combination)]] = optimizedResults
                
            }else if( results[[length(combination)]]$difference < optimizedResults$difference )
            {
                results[[length(combination)]] = optimizedResults
            }
            
        } else 
        {
            cat("Failed individual run = ", tradeList, ".\n", sep="")
        }
    }
    
    return(results)
}


################################################################################
BestTradeOptimizer$runTradeSubset <- function( tradeList, LL, ivarData, sasVaR )
{
    currentLL = subset(LL, LL$comments %in% tradeList)
    
    aux <- OptimizerEngine$getExtendedLimits( currentLL )
    LL.ext <- aux[[1]]
    LL     <- aux[[2]]
    
    data <- merge(LL.ext, ivarData, sort=FALSE, all.y=TRUE)  
    
    optimizedResults = OptimizerEngine$optimizeDataSet( data, LL, sasVaR )
    
    return( optimizedResults )
}


################################################################################
# The purpose of this is to run the best trade optimizer with a decent number
# of trades that wont take the code 10 hours to finish. The logic is, if we
# can optimize over the whole set, great, if not, do a 'logical' subset.
#
BestTradeOptimizer$getMaxTrades <- function( numUniqueTrades )
{
    optimalNumber = numUniqueTrades
    
    #2000 runs in about a half an hour on a decent machine as of Feb 08
    while( sum( choose( numUniqueTrades, (1:optimalNumber) ) ) > 2000 )
    {
        optimalNumber = optimalNumber - 1
    }
    
    return( optimalNumber )
}


################################################################################
BestTradeOptimizer$printResults <- function( resultSet )
{
    for( resultIndex in seq_along(resultSet) )
    {
        results = resultSet[[resultIndex]]
        
        rLog( "\n\nBest", resultIndex, "Trade(s)...\nVaR:", 
                dollar(results$original), "->", 
                dollar(results$optimized), "=", 
                dollar(results$difference) )
        results$data$term = NA
        results$data$term = Term$fromDatePair(results$data$contract.dt.start, results$data$contract.dt.end)
        results$data$change = formatC(results$data$change, big.mark=",", format="d")
        
        print( results$data[, c("comments", "curve.name", "term", "change")] )
    }
    invisible(NULL)
}

