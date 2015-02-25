# Basic function to write to xls 
#
#

###################################################################
#
.writeSheet.df <- function(target.sheet, x, top.cell, col.names)
{
    col.Class    <- sapply(x, class)
    ind.Date     <- col.Class %in% c("Date")
    ind.DateTime <- col.Class %in% c("POSIX")
    if (length(ind.Date)>0)
      x[,ind.Date] <- as.numeric(x[,ind.Date]) + 25569
    
    if (ncol(x)>256){
      cat("Only 256 columns will be exported.\n")
      x <- x[,1:256]}     
    if (nrow(x)>63999){
      cat("Only 64,000 rows will be exported.\n")  
      x <- x[,1:63999]
    }
    
    splitTopCells  <- strsplit(top.cell, "")[[1]]
    numberOffset   <- regexpr("[0-9]+", top.cell)
    startingRow    <- splitTopCells[numberOffset:length(splitTopCells)]
    startingRow    <- paste(startingRow, collapse="")
    startingColumn <- splitTopCells[1:(numberOffset-1)]
    startingColumn <- paste(startingColumn, collapse="")
    
    xls.colnames <- c(LETTERS,t(outer(LETTERS,LETTERS,"paste",sep="")))
    columnOffset <- which(xls.colnames==startingColumn)
    xls.colnames <- xls.colnames[columnOffset:(columnOffset+ncol(x)-1)]
    last.xls.colname <- xls.colnames[ncol(x)]

    # write the colnames  
    if( col.names ){
        r <- comGetProperty(target.sheet, "Range", top.cell, 
                paste(last.xls.colname,startingRow, sep=""))
        comSetProperty(r, "Value", colnames(x))         
    } else {
        startingRow = as.character(as.numeric(startingRow)-1)
    }
    
    # write one column at a time
    dataStart <- as.character(as.numeric(startingRow)+1)
    dataStop  <- as.character(nrow(x)+as.numeric(startingRow))
    for (k in 1:ncol(x)){
      r <- comGetProperty(target.sheet, "Range", paste(xls.colnames[k],
        dataStart, sep=""), paste(xls.colnames[k], dataStop, sep=""))
      comSetProperty(r, "Value", as.matrix(x[,k]))
      if (k %in% which(ind.Date))
        comSetProperty(r, "NumberFormat", "m/d/yyyy")
      if (k %in% ind.DateTime)
        comSetProperty(r, "NumberFormat", "m/d/yyyy HH:MM:SS")
    }

    return(invisible())     
}

###################################################################
#
xlsWriteSheet <- function(x, file, sheetname="Sheet1", top.cell="A1", 
  visible=FALSE, col.names=TRUE, clearContents=TRUE, propertyList=NULL)
{
    # somehow the com server leaves an Excel zombie on the
    # machine.  I get the PID to kill it before exit. 
    TL1  <- system('tasklist /fi "IMAGENAME eq excel.exe"',
     show.output.on.console=FALSE, intern=TRUE)
    if (length(TL1)>3){
      pid1 <- as.numeric(substr(TL1[4:length(TL1)], 28, 33))
    } else {
      pid1 <- NULL
    }
    # create an xls object
    xls <- comCreateObject("Excel.Application")
    TL2 <- system('tasklist /fi "IMAGENAME eq excel.exe"',
     show.output.on.console=FALSE, intern=TRUE)
    pid2 <- as.numeric(substr(TL2[4:length(TL2)], 28, 33))
    pid  <- setdiff(pid2, pid1)
      
    if (visible==TRUE){comSetProperty(xls, "Visible", TRUE)}
    if (!file.exists(file)){  # copy an empty template file 
      file.copy(paste(R.home(),"/library/CEGxls/DO_NOT_DELETE.xls",
        sep=""), file)
    }
    wb  <- comGetProperty(comGetProperty(xls, "Workbooks"), "Open", file)
    if (is.null(wb))
      stop(cat("Cannot open the file ", file, ".  Please check filepath.",
               sep=""))
    sheets <- comGetProperty(xls, "Sheets")
    noOfSheets <- comGetProperty(sheets, "Count")
    sheetNames <- NULL
    if (is.null(noOfSheets))
      stop("Problem getting the xls sheets.")
    for (s in 1:noOfSheets){
      sheet <- comGetProperty(sheets, "Item", s)
      sheetNames <- c(sheetNames, comGetProperty(sheet, "Name"))
    }
    lastSheet  <- comGetProperty(sheets, "Item", noOfSheets)

    if (!(sheetname %in% sheetNames)){    # add a new sheet  
      target.sheet <- comInvoke(sheets, "Add")
      comSetProperty(target.sheet, "Name", sheetname)
      ##     comSetProperty(target.sheet, "move", lastSheet)
      ##     comSetProperty(lastSheet, "move", target.sheet)
    }
    target.sheet <- comGetProperty(sheets, "Item", sheetname)
    comInvoke(target.sheet, "Activate")

    if (clearContents){
      r <- comGetProperty(target.sheet, "Cells")      
      comInvoke(r, "ClearContents")
    }
    
    if (class(x)=="data.frame")
      .writeSheet.df(target.sheet, x, top.cell, col.names)
   
    # set the cell property list if any 
    if (!is.null(propertyList)){
       try(xlsSetProperty(target.sheet, propertyList))}
    
    comSetProperty(wb, "Save", file)
    comInvoke(xls, "Quit")
    rm(xls)

    # somehow the com server leaves the process alive!
    if (length(pid)>0){
      aux <- system(paste('taskkill /f /fi "pid eq ', pid,'"', sep=""),
             show.output.on.console=FALSE, intern=TRUE)
    } else {
      aux <- system("taskkill /f /im Excel.exe",
             show.output.on.console=FALSE, intern=TRUE) # kill'em all
    }

  return(invisible())
}
