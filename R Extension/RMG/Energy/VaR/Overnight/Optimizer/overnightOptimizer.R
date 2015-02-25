###############################################################################
# overnightOptimizer.R
#
# This file runs the overnight var optimization process
#


################################################################################
# Required library files
source("H:/user/R/RMG/Energy/Tools/OptimizerEngine.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Utilities/rLogger.R") 
source("H:/user/R/RMG/Utilities/sendEmail.R")
source("H:/user/R/RMG/Utilities/lib.excel.functions.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/Optimizer/BestTradeOptimizer.R")


################################################################################
# File Namespace
#
overnightOptimizer = new.env(hash=TRUE)


################################################################################
# CONSTANTS
#
overnightOptimizer$LIMIT_DIR = "//ceg/cershare/All/Risk/Software/R/prod/Energy/VaR/Overnight/Optimizer/limits/"


################################################################################
overnightOptimizer$main <- function( )
{
    options(width=300)
    
    asOfDate   <- overnightUtils.getAsOfDate()
    options = overnightUtils.loadOvernightParameters(asOfDate, FALSE)
    
    limitFiles = list.files( overnightOptimizer$LIMIT_DIR, full.names=TRUE )
    
    for( limitFile in limitFiles )
    {
        portfolio = gsub( ".xls", "", basename(limitFile) )
        
        ivarFile = paste( options$save$overnight.dir, "/", portfolio, "/IVaR.", 
                basename(limitFile), sep="" )
        
        
        standardFile = paste( options$save$overnight.dir, "/", portfolio,
                "/", portfolio, ".optimized.xls", sep="" )
        outputValue = overnightOptimizer$.writeStandardOutput( ivarFile, limitFile, standardFile )
        
        
        bestTradeFile = paste( options$save$overnight.dir, "/", portfolio, "/", 
                        portfolio, ".bytrade.txt", sep="" )
        overnightOptimizer$.writeBestTradeOutput( ivarFile, limitFile, bestTradeFile )
        
        
        overnightOptimizer$.sendEmail( portfolio, outputValue, standardFile, bestTradeFile )
    }
    
    return(NULL)
}


################################################################################
overnightOptimizer$.writeStandardOutput <- function( ivarFile, limitFile, outputFile )
{
    output = OptimizerEngine$main( ivarFile, limitFile, FALSE )
    
    output$data$contract.dt.start = as.character(output$data$contract.dt.start)
    output$data$contract.dt.end = as.character(output$data$contract.dt.end)    

    XLS$write.xls(output$data, outputFile, "optimized")
    
    outputValue = dollar(output$optimized - output$original)
    outputData = data.frame(label="VaR Change:", value=outputValue)
    cell = paste("A", (nrow(output$data)+3), sep="")
    XLS$write.xls(outputData, outputFile, "optimized", top.cell=cell, col.names = FALSE)
    
    return( outputValue )
}


################################################################################
overnightOptimizer$.writeBestTradeOutput <- function( ivarFile, limitFile, outputFile )
{
    output = BestTradeOptimizer$findBestTrades( ivarFile, limitFile, sasVaR=NULL )

    sink(outputFile)
    BestTradeOptimizer$printResults(output)
    sink()
    
    invisible( NULL )
}


################################################################################
overnightOptimizer$.sendEmail <- function( portfolio, outputValue, standardFile, bestTradeFile )
{
    rLog("Sending Email...")
    
    portfolioName = gsub(".", " ", portfolio, fixed=TRUE)
    portfolioName = overnightOptimizer$.totitle(portfolioName)
    
    time = paste("Time:", Sys.time())
    
    valueLine = paste( "The RMG VaR Optimizer has calculated a VaR reduction of:\n\n", 
            outputValue, "\n\n", 
            "Please see the attached Excel file for details.", sep="" )
    
    bodyOfEmail = paste( time, valueLine, 
            "CEG Risk Management Group\nAnalytics, Trading Controls\n", sep="\n\n")
    
    subject = paste( "VaR Optimization Output", portfolioName, sep=" - ")
    
    reportName = paste( "optimizer.", portfolio, sep="" )
    toList = getEmailDistro( reportName )
    #toList = "john.scillieri@constellation.com"
    
    sendEmail(from="DO-NOT-REPLY@constellation.com",
            to=toList, subject, bodyOfEmail, attachments=c( standardFile, bestTradeFile ) )
}


################################################################################
overnightOptimizer$.totitle <- function(x) 
{
    s <- strsplit(tolower(x), " ")
    capFirstLetters = lapply( lapply(s, substring, 1, 1), toupper)
    restOfWords = lapply( s, substring, 2 )
    fullWords = NULL
    for( wordIndex in seq_along(restOfWords) )
    {
        fullWords[[wordIndex]] = paste( capFirstLetters[[wordIndex]], 
                restOfWords[[wordIndex]], sep="", collapse=" " )
    }
    
    return(fullWords)
}


################################################################################
overnightOptimizer$main()








