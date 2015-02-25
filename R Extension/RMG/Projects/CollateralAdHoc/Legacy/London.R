########## Try a batch  ########################################################
memory.limit(4095)
library(RODBC)
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/dollar.R")

columnsWeNeed = c( "ShortName", "Netting Agreement", "CSA", 
        "CEG Threshold Amt Used (USD)", "Cpty Threshold Amt Used (USD)",
        "Cash Held", "LC Held", "Cash Posted", "LC Posted", 
        "MTMPlusAccrualPlusARAP", "MTM")

allReportData = read.csv("C:/Documents and Settings/e14600/Desktop/CollateralAdHoc/AllReportData.csv", check.names=FALSE)
allReportData = allReportData[, columnsWeNeed]


noUKData = read.csv("C:/Documents and Settings/e14600/Desktop/CollateralAdHoc/NoUKData.csv", check.names=FALSE )
noUKData = noUKData[, columnsWeNeed]

mtmDifference = sum( allReportData$MTM, na.rm=TRUE) - 
        sum( noUKData$MTM, na.rm=TRUE )
rLog("MTM Difference:", dollar( mtmDifference) )

exposureDifference = sum( allReportData$MTMPlusAccrualPlusARAP, na.rm=TRUE) - 
        sum( noUKData$MTMPlusAccrualPlusARAP, na.rm=TRUE )
rLog("Exposure Difference:", dollar( exposureDifference) )

# New Posted = max( E-T, 0 )
# CC = Posted - New Posted

noUKToMerge = noUKData[,c( "ShortName", "Netting Agreement", "CSA", 
                "MTMPlusAccrualPlusARAP", "MTM")]
allData = merge( allReportData, noUKToMerge, all=TRUE,
        by = c("ShortName", "Netting Agreement", "CSA"), 
        suffixes = c(".all", ".noUK") )

allData = allData[ which(allData$CSA != "" ), ]

allData$MTMPlusAccrualPlusARAP.all[which(is.na(allData$MTMPlusAccrualPlusARAP.all))] = 0 
allData$MTMPlusAccrualPlusARAP.noUK[which(is.na(allData$MTMPlusAccrualPlusARAP.noUK))] = 0
allData$"Cash Held"[which(is.na(allData$"Cash Held"))] = 0 
allData$"LC Held"[which(is.na(allData$"LC Held"))] = 0 
allData$"Cash Posted"[which(is.na(allData$"Cash Posted"))] = 0 
allData$"LC Posted"[which(is.na(allData$"LC Posted"))] = 0 

allData$"Exposure Diff" = allData$MTMPlusAccrualPlusARAP.noUK - allData$MTMPlusAccrualPlusARAP.all


write.csv( allData, row.names=FALSE, 
        file="C:/Documents and Settings/e14600/Desktop/CollateralAdHoc/UKAdjustedCollateral.csv")
