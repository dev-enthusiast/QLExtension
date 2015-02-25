main.PDF2CSVa <- function(options){
##  browser()
  fileName <- options$file
  pathName <- options$dir
  filePathName <- paste(pathName, fileName, sep = "")
  fileNameComp <- strsplit(fileName,"\\.")
  fileDate <- fileNameComp[[1]][1]
  fileTXT <- paste(pathName,fileDate,".txt",sep = "")

  if(file.exists(fileTXT) == FALSE){
##  command <- paste("S:/Risk/Software/Utilities/pdftotext/pdftotext", filePathName, sep = " ")
    command <- paste("S:/Risk/Software/Utilities/Xpdf/pdftotext", "-layout", filePathName, sep = " ")
    system(command)
  }

  fileDate <- paste(substr(fileDate,1,2),"/",substr(fileDate,3,4),"/",substr(fileDate,5,6), sep = "" )
  fileDate <- as.Date(fileDate,"%m/%d/%y")
  pathNameDate <- paste(pathName, fileDate, "/", sep = "") 
  dir.create(pathNameDate)

  filePathName <- paste(pathName, fileNameComp[[1]][1], ".txt", sep = "")
  readData <- readLines(filePathName, n = -1)
  readDataX <- gsub("\\(", "", readData)
  readDataX <- gsub("\\)", "", readDataX)
  readDataX <- gsub(" +", " ", readDataX)
##  browser()
  ListAllTables <- ListTable(readDataX)
  TitleString <- c("Natural Gas", "Basis Swap")
  for(i in 1:length(TitleString)) {
    indTable <- grep(TitleString[i], ListAllTables, ignore.case = TRUE)
    for(j in 1:length(indTable)) {
##      browser()
      currentTable <- ListAllTables[indTable[j]]
##      currentTable <- sub("\\(","",currentTable)
##      currentTable <- sub("\\)","",currentTable)
##      currentTable <- sub(" *$","",currentTable)
      indStr <- grep(currentTable, readDataX) 
      data2file = NULL
      for(k in 1:length(indStr))
##      for(k in 4:4)
        data2file <- tableFill(readData, indStr[k], data2file)
##      browser()  
      aux <- gsub(",", "", apply(data2file, 2, as.character))
      aux <- gsub(" ", "", aux)
      bux <- matrix(aux, ncol=ncol(data2file))
      cux <- data.frame(bux[,1], apply(bux[,2:ncol(bux)], 2, as.numeric))
      colnames(cux) <- colnames(data2file) 
      tableNameX <- sub("/", " ", ListAllTables[indTable[j]]) 
      tableNameX <- sub("for", "", tableNameX)
      tableNameX <- sub(' *$', '', tableNameX)      
      fileNameCSV <- paste(pathNameDate, tableNameX, ".csv" , sep = "") 
      write.table(cux, file = fileNameCSV, append = FALSE, sep = ",", 
              row.names = FALSE, col.names = TRUE)                
    }
  }                                                          
}                                                   
############################ function ######################################
tableFill <- function(readData, indIn, data2file){
  j <- indIn + 1
  indI <- j
  nElementMax <- 0
  while (checkStr(readData[j])[[1]] == TRUE){
    j <- j + 1
    if( checkStr(readData[j])[[2]] > nElementMax)
      nElementMax <- checkStr(readData[j])[[2]]
  }  
  indF = j - 1

  if ( indF >= indI && nElementMax >= 10 ){
    tableHeader <- getHeader(readData,indIn)
    readData[indI:indF] <- gsub("\\+", " ", readData[indI:indF])
    readData[indI:indF] <- gsub("- ", " -", readData[indI:indF])
    ff = tempfile()
    write.table(readData[indI:indF],file = ff, quote = FALSE, row.names = FALSE, col.names = FALSE)
    wInd <- tableHeader[[2]][1] + tableHeader[[3]][1]
##    browser()
    for( i in 2:length(tableHeader[[2]])){
      indSum <- 0
      for (j in 1:length(wInd))
        indSum <- indSum +abs(wInd[j])  
      wInd <- c(wInd, indSum - tableHeader[[2]][i], tableHeader[[3]][i])
    }  
    data2fileX <- read.fwf(ff, width = wInd)
##    browser() 
    if(length(tableHeader[[1]]) > length(tableHeader[[2]])) {
      titleLine <- sub(' *$', '', readData[indIn])
      titleLine <- strsplit(titleLine," ")
      lastEl <- titleLine[[1]][length(titleLine[[1]])]
      data2fileX <- cbind(lastEl, data2fileX)      
    } 
    colnames(data2fileX) <- tableHeader[[1]]    
    if (is.null(data2file))
      data2file <- data2fileX
    else
      data2file <- rbind(data2file, data2fileX) 
  } 
  return(data2file) 
}

############################################################################
getTitleStr <- function(titleStr){
##  browser()
  titleStr <- paste(titleStr," ")
  titleStrNS <- gsub(" +", " ", titleStr)
  titleStrS  <- strsplit(titleStrNS, " ")
  titleEl<- NULL
  for (i in 1:length(titleStrS[[1]])){
    if(nchar(titleStrS[[1]][i]) > 1 )
      titleEl <- c(titleEl, titleStrS[[1]][i])
  }
  titleElU <- unique(titleEl)
  indTitle <- NULL
  for (i in 1:length(titleElU)){
    nEl <- length(strsplit(titleStr, titleElU[i])[[1]]) - 1
    indTitle <- c(indTitle, nchar(strsplit(titleStr, titleElU[i])[[1]][1]))
    if (nEl > 1){
      for (j in 2:nEl){
        indX <- nchar(strsplit(titleStr, titleElU[i])[[1]][j]) + 
                indTitle[length(indTitle)] + nchar(titleElU[i])
        indTitle <- c(indTitle,indX) 
      }
    }
  }
  indTitle <- sort(indTitle)   
  return(list(titleEl,indTitle))  
}

############################ function ######################################
getHeader <- function(readData, indIn) {
  iTitle <- indIn -1
  while (length(grep("INTEREST", readData[iTitle])) == 0 && iTitle > 10 )   
    iTitle <- iTitle -1  
    
  strX <- gsub(" +", " ", readData[indIn + 1])
  strX <- strsplit(strX," ")
  firstField <- strX[[1]][1]
  if(firstField == "")
    firstField <- strX[[1]][2]
  firstField <-sub("\,", "\.", firstField)
  headerName <- "DATE"
  headerInd  <- 1
##  browser()
  titleStr1 <- getTitleStr(readData[iTitle-1])
  titleStr2 <- getTitleStr(readData[iTitle])
##  browser()
  for (i in 1:length(titleStr2[[2]])){
    for( j in 1:length(titleStr1[[2]])){
      if (abs( titleStr2[[2]][i] - titleStr1[[2]][j]) < 5){
        titleStr2[[1]][i] <- paste(titleStr1[[1]][j], "_", titleStr2[[1]][i], sep = "")
        break  
      }
    }
  }
##  browser()
  if(is.na(as.numeric(firstField)) == FALSE){
    headerName <- c(headerName, "STRIKE", titleStr2[[1]])
    headerInd <- c(headerInd, titleStr2[[2]])

    headerWidth  <- c(8, 8, 7, 6, 6, 6, 7, 7, 7, 7, 6, 6, 6)
    headerIndOff <- c(1, 0, 0,-1,-1,-1, 2,-1,-1, 0, 0, 1, 0)
    headerInd <- headerInd + headerIndOff                   
  } else {
    headerName <- c(headerName, titleStr2[[1]]) 
    headerInd  <- c(headerInd, titleStr2[[2]])        
       
    headerWidth  <- c(8, 7, 7, 7, 7, 7, 7, 6, 6, 7, 7, 7)
    headerIndOff <- c(0, 0, 0, 0,-2,-1,-1, 0, 0, 1,-1, 0)
    headerInd <- headerInd + headerIndOff 
  }
  return(list(headerName,headerInd,headerWidth))
}


##################################### function ###############################                  
ListTable <- function(readData) {
  indPrice <- grep("SETTLE",readData) + 2
  indPrice <- refineInd(readData,indPrice)
  indTotal <- grep("total",readData, ignore.case =TRUE) + 1                  
  indTotal <- refineInd(readData,indTotal)
  indTable <- union(indPrice,indTotal)
  ListTableOut <- sub(" *$", "", readData[indTable])
  for( i in 1:length(ListTableOut)) {
    strX <- strsplit(ListTableOut[i]," ")
    lastElement <- strX[[1]][length(strX[[1]])]
    if( is.na(as.Date(lastElement, "%b.%d" )) == FALSE && length(strX[[1]]) > 1) {
      ListTableOut[i] <- sub(lastElement,"", ListTableOut[i])
    }
  }     
  ListTableOut <- unique(ListTableOut)
  return(ListTableOut)
}

##################################### function ###############################                  
refineInd <- function(readData,ind){
  indX <- NULL
  m  <- 0
  for (i in 1:length(ind)){
    if(nchar(strsplit(readData[ind[i]], " ")[[1]][1]) > 0 &&
       length(grep("----", readData[ind[i]])) == 0  && 
       length(grep("\\*", readData[ind[i]])) == 0 && 
       nchar(readData[ind[i]]) > 5  &&
       length(grep("total", readData[ind[i]], ignore.case =TRUE)) == 0){
      m <- m + 1     
      indX[m] <- ind[i]    
    }                                                                                         
  }
  return(indX)  
}

################################ function ###################################
checkStr <- function(strIn){
  strIn <- gsub(" +", " ", strIn)
  strIn <- strsplit(strIn," ")
  nElement <- length(strIn[[1]])
  if( length(grep("TOTAL",strIn,ignore.case = TRUE )) == 0  && nElement > 4)
    bFlag = TRUE              
  else
    bFlag = FALSE
  return(list(bFlag,nElement))  
}      

