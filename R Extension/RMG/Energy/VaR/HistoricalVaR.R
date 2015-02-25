options=NULL
options$dirArchive <- "R:/archive/"
options$dir <- "S:/Risk/2006,Q4/Risk.Analytics/Historical.VaR/"
options$dirUnzip <- paste(options$dir, "reports/power/", sep = "")
setwd(options$dir)
options$fileName1 <- "gastrd"
options$fileName2 <- "gasacc"
options$fileName3 <- "upstrm"
options$fileName1ris <- paste(options$fileName1,".ris", sep = "")
options$fileName2ris <- paste(options$fileName2,".ris", sep = "")
options$fileName3ris <- paste(options$fileName3,".ris", sep = "")
options$startDate <- "01/01/2006"
options$endDate   <- "10/31/2006"

outData <- NULL
dateArray <- seq(as.Date(options$startDate, "%m/%d/%Y"), as.Date(options$endDate, "%m/%d/%Y"), by = 1)
for (nDate in 1:length(dateArray)) {
  workingDir <- paste(options$dirUnzip,format(dateArray[nDate],"%Y%m%d"),sep = "")
  if(format(as.Date(dateArray[nDate]), "%d") == "01" & file.exists(workingDir) == FALSE){
    fileCore <- paste("power.", format(dateArray[nDate],"%Y%m"), ".tar", sep = "")
    fileArchive <- paste(options$dirArchive, format(dateArray[nDate], "%Y%m"), "/reports/", 
                         fileCore, ".gz", sep = "")
    fileX <- paste(options$dir, fileCore, sep = "")
    fileXgz <- paste(fileX, ".gz", sep = "")
    file.copy(fileArchive, fileXgz)
    setwd(options$dir)
    system(paste("gunzip ", fileXgz, sep = ""))
    system(paste("tar -xvf ", fileX, sep = ""))
    file.remove(fileX)    
  }
  if(file.exists(workingDir) == TRUE) {
    setwd(workingDir)
    fileName1 <- paste(options$dirUnzip, format(dateArray[nDate],"%Y%m%d"), "/", options$fileName1ris, sep = "")
    fileName2 <- paste(options$dirUnzip, format(dateArray[nDate],"%Y%m%d"), "/", options$fileName2ris, sep = "")
    fileName3 <- paste(options$dirUnzip, format(dateArray[nDate],"%Y%m%d"), "/", options$fileName3ris, sep = "")
    if(file.exists(options$fileName1ris) == TRUE & file.exists(options$fileName2ris) == TRUE & 
       file.exists(options$fileName3ris) == TRUE) {
      outPutLine <- as.character(dateArray[nDate])
      outPutLine <- extractVar(options$fileName1ris,outPutLine)
      outPutLine <- extractVar(options$fileName2ris,outPutLine)
      outPutLine <- extractVar(options$fileName3ris,outPutLine)
      outData <- rbind(outData, outPutLine)
    }
    listFiles <- list.files(workingDir)
    if( length(listFiles) >0 ){
      indF <- which(listFiles %in% c(options$fileName1ris, options$fileName2ris, options$fileName3ris) )
      listFiles <- listFiles[-indF]
      if(length(listFiles) >0 ) {file.remove(listFiles)}
    }
  }    
}
colnames(outData) <- c("Date", paste(options$fileName1, "StdDev4"), paste(options$fileName1, "Pct95"), 
                       paste(options$fileName2, "StdDev4"), paste(options$fileName2, "Pct95"), 
                       paste(options$fileName3, "StdDev4"), paste(options$fileName3, "Pct95"))                       
write.table(outData, file = paste(options$dir,"/HistVar_", format(as.Date(options$startDate, "%m/%d/%Y"),"%Y%m%d"),"_",  
                                  format(as.Date(options$endDate, "%m/%d/%Y"),"%Y%m%d"), ".csv", sep = ""), 
            append = FALSE, sep = ",", row.names = FALSE, col.names = TRUE)

extractVar <- function(fileName, outPutLine) {
  readData <- readLines(fileName, n = -1)
  ind4S <- grep("StdDev4 -70b    ", readData)
  if(length(ind4S) == 1) {
    formatLength <- strsplit(readData[ind4S],"  ")
    outPutLine <- c(outPutLine, formatLength[[1]][length(formatLength[[1]])])
  } else {
    outPutLine <- c(outPutLine, NA)  
  }  
  ind95 <- grep("Pct95 -70b    ", readData)
  if(length(ind95) == 1) {  
    formatLength <- strsplit(readData[ind95],"  ")
    outPutLine <- c(outPutLine, formatLength[[1]][length(formatLength[[1]])])    
  } else {
    outPutLine <- c(outPutLine, NA)
  }  
  return(outPutLine)
}

