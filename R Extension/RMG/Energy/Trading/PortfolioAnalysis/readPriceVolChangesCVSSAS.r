
readPriceVolChangesCVSSAS <- function(Portfolio, Date)
{
#  browser()
  options$SASdir <- "S:/Risk/Projects/SAS/DATASTORE/SASIVaRProcm/"
  fileList <- list.files(options$SASdir)
  fileCorePortfolio <- paste(strsplit(toupper(Portfolio)," ")[[1]], sep = "", collapse = "-")
  indPortfolio <- grep(fileCorePortfolio, fileList)
#  fileCoreDate <- paste("HDV-vSOR_", toupper(as.character(format(Date, "%d%b%y"))), sep = "")
  fileCoreDate <- paste("HDV-vSOR_", toupper(as.character(format(Date, "%d%b%y"))), sep = "")
  indDate <- grep(fileCoreDate, fileList[indPortfolio])
  # Delta 
#  browser()
  indDateP <- grep(paste(fileCorePortfolio, "_price", sep = ""),  fileList[indPortfolio[indDate]])
  if(length(indDateP) == 1) {
    fileNameP <- paste(options$SASdir,fileList[indPortfolio[indDate[indDateP]]], sep = "")
    dataDelta <- read.csv(fileNameP)
    dataDelta$contract_month <- as.Date(dataDelta$contract_month,"%d%b%Y")
    cols <- c(1,3, c((length(colnames(dataDelta)) -69):length(colnames(dataDelta))))
    dataDelta <- dataDelta[,cols]
    colnames(dataDelta)[1:2] <- c("CURVE_NAME", "VALUATION_MONTH")      
  } else {
    dataDelta <- NULL 
  }
  # Vega
  indDateV <- grep(paste(fileCorePortfolio, "_vol", sep = ""),  fileList[indPortfolio[indDate]])
  if(length(indDateV) == 1) {
    fileNameV <- paste(options$SASdir, fileList[indPortfolio[indDate[indDateV]]], sep = "")
    dataVega <- read.csv(fileNameV)
    dataVega$contract_month <- as.Date(dataVega$contract_month,"%d%b%Y")
    cols <- c(1,3, c((length(colnames(dataVega)) -69):length(colnames(dataVega))))
    dataVega <- dataVega[,cols]
    colnames(dataVega)[1:2] <- c("CURVE_NAME", "VALUATION_MONTH")      
  } else {
    dataVega <- NULL 
  }     
  return(list(dataDelta, dataVega))
}