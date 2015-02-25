readPriceVolChangesRData <- function(Portfolio, Date)
{
  source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
  package.names <- c("RODBC")
  load.packages(package.names)
  
#  browser()
  dataDelta <- NULL
  dataVega  <- NULL  
  fileName <- paste("S:/All/Risk/Reports/VaR/prod/", as.character(Date), 
                             "/rmg.mark.orman/IVaR.rmg.mark.orman.xls", sep = "")
  
  if( file.exists(fileName) == TRUE ) { 
    con <- odbcConnectExcel(fileName)
    dataPriceVolChange <- sqlFetch(con, "Changes")
     odbcCloseAll()
    colnames(dataPriceVolChange)[1:3] <- c("curve.name", "contract.dt", "vol.type")
    dataPriceVolChange$vol.type <- as.character(dataPriceVolChange$vol.type)
    indNA <- which(is.na(dataPriceVolChange$vol.type))
    if (length(indNA) > 0 ) {
      colnames(dataPriceVolChange)[1:2] <- c("CURVE_NAME", "VALUATION_MONTH")
      cols <- c(1,2,5:(length(colnames(dataPriceVolChange)) - 1))
      dataDelta <- dataPriceVolChange[indNA, cols]
      dataVega <- dataPriceVolChange[-indNA, cols]
    }  
  } 
  return(list(dataDelta[,1:72], dataVega[,1:72]))
}                               
