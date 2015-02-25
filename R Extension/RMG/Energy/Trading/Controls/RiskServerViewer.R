###############################################################################
# RiskServerViewer.R
# 
# Author: e14600


################################################################################
# CONFIGURATION PARAMETERS
#
PORTFOLIO = "JKirkpatrick Trading Portfolio"


################################################################################
library(reshape)
source("H:/user/R/RMG/Utilities/rLogger.R") 
source("H:/user/R/RMG/Utilities/load.R") 
source("H:/user/R/RMG/Utilities/sendEmail.R")
source("H:/user/R/RMG/Utilities/Database/R/LiveData.R")
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Projects/PV/Term.R")


###############################################################################
readPositionData <- function(StartingPositions)
{    
    options = NULL
    options$run.name = PORTFOLIO
    bookList = get.portfolio.book( options )$BOOK
    
    livePositions = LiveData$cpsprodPositions()
    
    bookData = subset(livePositions, 
            livePositions$book %in% bookList )
                    #& contract.dt < as.Date("2009-01-01") )
    
    bookData$book = NULL
    names(bookData)[3]="value"
    bookData$value = round(bookData$value)
    
    StartingPositions = subset(StartingPositions, 
            StartingPositions$book %in% bookList )
    
    StartingPositions$book = NULL
    names(StartingPositions)[3]="value"
    StartingPositions$value = round(StartingPositions$value)
    
    allData = merge(bookData, StartingPositions, all=TRUE, 
            na.fill=0, by=c("curve.name", "contract.dt"))
    allData$difference = round(allData$value.y - allData$value.x)
    allData = allData[ -(which(allData$difference==0)), ]

    allData$value.x = allData$value.y = NULL
    allData$curve.name = as.character(allData$curve.name)
    allData$contract.dt = as.character(allData$contract.dt)
    names(allData) = c("curve.name", "contract.dt", "value")
    
    allData  = cast(allData, curve.name + contract.dt ~ ., 
            fun.aggregate=sum, fill=0)
    names(allData) = c("curve.name", "contract.dt", "value")
    allData = allData[ -(which(allData$value==0)), ]
    
    allData$curve.name = gsub("COMMOD ", "", allData$curve.name, fixed=TRUE)
    
    changeMatrix = cast(allData,  curve.name ~ contract.dt , 
            fun.aggregate=sum, fill=0)
    
    names(changeMatrix) = c(" ", format(as.Date(names(changeMatrix)[-1]), "%b %y"))
    
    return( changeMatrix )
}

startingFile = paste("S:/All/Risk/Software/R/prod/Energy/Trading/Controls/",
        "StartingPositions.", Sys.Date(), ".RData", sep="")
load(startingFile)
names(StartingPositions) = c("book", "curve.name", "contract.dt", "value")

while( TRUE )
{
    cat(rep("\n", 50))
    pd = readPositionData(StartingPositions)
    cat("\n")
    print(pd)
    cat("\nDone:", as.character(Sys.time()), "\n")
    flush.console()
    Sys.sleep( 5 * 60 )
}


