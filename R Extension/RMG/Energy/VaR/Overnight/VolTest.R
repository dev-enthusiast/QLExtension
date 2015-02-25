###############################################################################
# VolTest.R
#
# Author: John Scillieri
# 
library(RODBC)
library(reshape)
source("H:/user/R/RMG/Utilities/load.R")

asOfDate <- Sys.Date()-1


################################################################################
# Pull the most 
getLatestVol <- function( curve.name, contract.dt, type="M" )
{
    
    file.dsn <- "CPSPROD.dsn"
    file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", file.dsn.path, file.dsn,
            ";UID=stratdatread;PWD=stratdatread;", sep="")
    con <- odbcDriverConnect(connection.string)
    
    volQuery <- paste(
            "SELECT * ",
            "from foitsox.sdb_bs_volatility where ",
            "upper(COMMOD_CURVE) = '", toupper(curve.name), "' ",
            "and CONTRACT_DATE = to_date('", contract.dt, "', 'YYYY-MM-DD') ",
            "and TYPE='", type, "' ",
            "and strike_pct=0.5 ",
            "order by PRICING_DATE DESC",
            sep="")
    volData   <- sqlQuery(con, volQuery, max=1)
    odbcClose(con)
    return(volData)
}



############## GET VCENTRAL VEGA POSITIONS #####################################
vegaFile = paste("S:/All/Risk/Data/VaR/prod/Positions/vega.positions.", 
        asOfDate, ".RData", sep="")

currentPositions = varLoad("vegaPositions", vegaFile) 
currentPositions = currentPositions[ order(currentPositions$curve.name, 
                currentPositions$contract.dt), ]



################## GET VOL DATA FROM CPSPROD ###################################
file.dsn <- "CPSPROD.dsn"
file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
connection.string <- paste("FileDSN=", file.dsn.path, file.dsn,
        ";UID=stratdatread;PWD=stratdatread;", sep="")
con <- odbcDriverConnect(connection.string)

volQuery <- paste(
        "SELECT COMMOD_CURVE, CONTRACT_DATE, TYPE, VOLATILITY ",
        "from foitsox.sdb_bs_volatility where ",
        "pricing_date=to_date('", asOfDate, "','YYYY-MM-DD') ",
        "and strike_pct=0.5",
        sep="")
volData   <- sqlQuery(con, volQuery)
names(volData) = c("curve.name", "contract.dt", "vol.type", "value")
volData$contract.dt = as.Date(volData$contract.dt)
volData$curve.name = toupper(volData$curve.name)
volData = unique(volData)
volData = volData[ order(volData$curve.name, 
                volData$contract.dt, volData$vol.type), ]

odbcClose(con)



################# IDENTIFY MISSING VALUES ######################################
volCurves = unique(volData[, c("curve.name", "contract.dt", "vol.type")])
volCurves$hasVol=TRUE

vegaCurves = unique(currentPositions[,c("curve.name", "contract.dt", "vol.type")])

mergedData = merge(vegaCurves, volCurves, all.x=TRUE)
missingData = mergedData[which(is.na(mergedData$hasVol)),]
missingData$hasVol = NULL 
rownames(missingData) = NULL
missingData = missingData[do.call(order,missingData),]


aggPos = currentPositions
aggPos$book = NULL
#aggPos$contract.dt = NULL
names(aggPos)[length(aggPos)] = "value"
aggPos = cast(aggPos, curve.name + contract.dt + vol.type~., sum)
names(aggPos)[length(aggPos)] = "vega"


missingData = merge(missingData, aggPos)


write.csv(missingData, file=paste("H:/missingVols.",asOfDate,".csv",sep=""),
        row.names=FALSE)
print(missingData)
print("Done.")





