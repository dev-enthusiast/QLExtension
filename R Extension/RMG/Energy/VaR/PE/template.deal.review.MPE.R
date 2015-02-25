# Template for calculating MPE for DR.
#
# 
#

rm(list=ls())
library(SecDb)

####################################################################
# Run using a bunch of calculators 
#
runName  <- "_DR.Williamson" 
asOfDate <- as.Date("2008-02-01")
group    <- "CPS Trading Group"
trader   <- "Williamson"


(calc.names <- calculator.list(group, trader))
calc.names  <- calc.names[c(3,5,6)]
#calc.names  <- calc.names[c(3)]

source("H:/user/R/RMG/Energy/VaR/PE/InteractivePECalc.R") 
QQ <- InteractivePECalc$makePositionsFromCalcs(asOfDate, group, trader,
                                                     calc.names)
InteractivePECalc$withPositionData( QQ, runName, asOfDate )



####################################################################
# Run using a portfolio/book combination position
#
runName  <- "VENERE.SA" 
asOfDate <- as.Date("2008-02-25")
run <- NULL
run$run.name <- "VENERE SA"

source("H:/user/R/RMG/Energy/VaR/PE/InteractivePECalc.R") 
QQ <- InteractivePECalc$makePositionsFromPortfolios(asOfDate, run)

InteractivePECalc$withPositionData( QQ, runName, asOfDate )



####################################################################
# Run using a counterparty position
#
asOfDate <- as.Date("2008-02-21")
run <- NULL
run$run.name <- "_test"
run$counterparty <- "MITSUI"

# if you want detailed diagnostics on simulated curves
run$extras$plots <- TRUE

source("H:/user/R/RMG/Energy/VaR/PE/InteractivePECalc.R") 
QQ <- PEUtils$getPositionsForCounterparty(run$counterparty)

InteractivePECalc$withPositionData( QQ, run, asOfDate )


####################################################################
# Run using portfolio positions by ICR
#
asOfDate <- as.Date("2008-03-06")
run.name <- "FREIGHT ACCRUAL PORTFOLIO.ICR5"

source("H:/user/R/RMG/Energy/VaR/PE/InteractivePECalc.R")
PE$SRC_DIR <- "H:/user/R/RMG/Energy/VaR/PE/"
run <- PEUtils$setParmsRun( run.name )

source("H:/user/R/RMG/Energy/VaR/PE/PEUtils.R")
PE$SRC_DIR <- "H:/user/R/RMG/Energy/VaR/PE/"
QQ <- PEUtils$getPositionsForRunName( asOfDate, run )

# if you want detailed diagnostics on simulated curves
run$extras$plots <- TRUE

source("H:/user/R/RMG/Energy/VaR/PE/InteractivePECalc.R")
InteractivePECalc$withPositionData( QQ, run, asOfDate )



#  write.csv(QQ, file="C:/temp/positions.csv", row.names=FALSE)      




####################################################################
####################################################################
####################################################################
source("H:/user/R/RMG/TimeUtilities/dateGS.R")
require(RODBC)
con    <- odbcConnectExcel("H:/Temporary/position.xls")
data   <- sqlFetch(con, "Sheet1")
odbcCloseAll()

QQ <- melt(data, id=1)
QQ$curve.name  <- paste("COMMOD COL ", QQ$variable, sep="")
aux <- substr(QQ$Commod, 4, 6)

QQ$contract.dt <- format.dateMYY(aux, direction=1)
names(QQ)[3] <- "position"
QQ <- QQ[,-(1:2)]
QQ <- subset(QQ, position != 0 )
QQ$curve.name <- toupper(QQ$curve.name)
QQ$curve.name <- gsub(" $", "", QQ$curve.name)
QQ$contract.dt <- as.Date(QQ$contract.dt)

  dir <- "//ceg/cershare/All/Risk/Data/VaR/prod/Prices/"
  filename <- paste(dir, "hPrices.", asOfDate, ".RData", sep="")
  load(filename)
  QQ <- merge(QQ, hP, all.x=T)
  names(QQ)[3:4] <- c("position", "F0")



