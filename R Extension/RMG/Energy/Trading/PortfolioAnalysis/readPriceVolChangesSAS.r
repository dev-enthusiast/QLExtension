
readPriceVolChangesSAS <- function(Portfolio, Date)
{
#  browser()
  options$SASdir <- "S:/Risk/Projects/SAS/DATASTORE/SASIVaRProcm/"
  fileList <- list.files(options$SASdir)
  fileCorePortfolio <- paste(strsplit(toupper(Portfolio)," ")[[1]], sep = "", collapse = "-")
  indPortfolio <- grep(fileCorePortfolio, fileList)
#  fileCoreDate <- paste("HDV-vSOR_", toupper(as.character(format(Date, "%d%b%y"))), sep = "")
  fileCoreDate <- paste("HDV-vSORX_", toupper(as.character(format(Date, "%d%b%y"))), sep = "")
  indDate <- grep(fileCoreDate, fileList[indPortfolio])
  if(length(indDate) == 1) {
    fileName <- paste(options$SASdir,fileList[indPortfolio[indDate]], sep = "")
  }
  require(xlsReadWrite)
  # Delta
  dataDelta <- read.xls( fileName, colNames = TRUE, sheet = "Prc_Chgs", type = "data.frame", from = 1, colClasses = NA)  
  dataDelta$contract_month <- as.Date(dataDelta$contract_month,"%d%b%Y")
  cols <- c(1,3, c((length(names(dataDelta)) -69):length(names(dataDelta))))
  dataDelta <- dataDelta[,cols]
  colnames(dataDelta)[1:2] <- c("CURVE_NAME", "VALUATION_MONTH")  
  # Vega
  dataVega  <- read.xls( fileName, colNames = TRUE, sheet = "Vol_Chgs", type = "data.frame", from = 1, colClasses = NA)
  dataVega$contract_month <- as.Date(dataVega$contract_month,"%d%b%Y")
  cols <- c(1,3, c((length(names(dataVega)) -69):length(names(dataVega))))
  dataVega <- dataVega[,cols]
  colnames(dataVega)[1:2] <- c("CURVE_NAME", "VALUATION_MONTH")  
   
  return(list(dataDelta, dataVega))
#  return(dataVega)
}