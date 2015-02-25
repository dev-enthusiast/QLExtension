main.TXT2CSV <- function(options){
  startDate <- as.Date(options$startDate, "%m/%d/%Y")
  endDate   <- as.Date(options$endDate, "%m/%d/%Y")
  dateArray <- seq(startDate, endDate, by = 1)
  dataFile <- NULL
  unzipFiles(options, dateArray)
  for (nDate in 1:length(dateArray)) {
    fileName <- paste(options$dir,"/", format(dateArray[nDate],"%Y%m%d"), "/", options$fileNameCore, sep = "")
    fileNameCSV <- paste(options$dir,"/", options$fileNameCore,"_", startDate, "_", endDate, ".csv", sep = "")
    if(file.exists(fileName) == TRUE) {
      readData <- readLines(fileName, n = -1)
      indTrader <- grep("Trader", readData) 
      formatLength <- strsplit(readData[indTrader[1]+ 1],"  ")
      nLengthChar <- nchar(formatLength[[1]])
      wInd <- nLengthChar[1]
      for (k in 2:length(nLengthChar)) 
        wInd <- c(wInd,-2,nLengthChar[k])
      indBreak <- grep(formatLength[[1]][1],readData)
      dataFileX <- NULL
      for (j in 1:length(indTrader)) {
        for (jx in 1:length(indBreak)) {
          if(indBreak[jx] >  indTrader[j]+1) {
            indF <- indBreak[jx]-1
            break
          }
        }
        ff = tempfile()
        aux <- readData[(indTrader[j]+2):indF]
        aux <- gsub("#"," ", aux)
        write.table(aux, file = ff, quote = FALSE, row.names = FALSE, col.names = FALSE)
        dataFile <- rbind(dataFile, cbind(as.character(dateArray[nDate]), read.fwf(ff, width = wInd))) 
        unlink(ff)                  
      }                    
    }
  }
  colnames(dataFile) <- c("Date", "Portfolio", "Trader", "PNL", "PNL Explanation", "Market Conditions", "Trading Strategies")
  write.table(dataFile, file = fileNameCSV, append = FALSE, sep = ",", row.names = FALSE, col.names = TRUE)  
}

unzipFiles <- function(options,dateArray) {
  for (nDate in 1:length(dateArray)) {
    fileDir <- paste(options$dir,"/", format(dateArray[nDate],"%Y%m%d"), sep = "")
    if(file.exists(fileDir) == FALSE) {
      fileX <- paste(options$dir,"/", "mgmt.", format(dateArray[nDate],"%Y%m"), ".tar", sep = "")
      system(paste("gunzip ", fileX, ".gz", sep = ""))
      system(paste("tar -xvf ", fileX, sep = ""))
    }    
  }
  return()
}