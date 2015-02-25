# Tools to operate on Excel spreadsheets using COM objects.
# See the last function for examples
#
# problems treating NA's ... 
#
#
# Written by AAD, JB on 18-Feb-2008 

require(rcom); require(RODBC)

source("H:/user/R/RMG/Utilities/RLogger.R")


##############################################################################
# File Namespace
#
XLS <- new.env(hash=TRUE)

##############################################################################
# Set certain properties to a selected range. Where sheet is a COM object with 
# the sheet that you want to modify.  
#
XLS$set.property <- function(sheet = NULL,
  propertyList = list(range=c("A1", "C1"), Font = FALSE, FontSize = 10,
  FontColorIndex = 1, BackgroundColorIndex = 6, Comment = NULL)){

  if (!comIsValidHandle(sheet)){
    stop("Cannot set properties, the sheet is not a valid COM object.")
  }
  if (!is.list(propertyList[[1]])){propertyList <- list(propertyList)}
  
  # Loop over the property List
  for (i in 1:length(propertyList)){ 
    CP <- propertyList[[i]]
    r  <- comGetProperty(sheet, "Range", CP$range[1], CP$range[2])
    if (!r$Activate())stop("Cannot activate the range")

    fp <- comGetProperty(r, "Font")
    if ("FontSize" %in% names(CP)){ comSetProperty(fp, "size", CP$FontSize)}
    if ("Font" %in% names(CP)){ comSetProperty(fp, CP$Font, TRUE) }
    if ("FontColorIndex" %in% names(CP)){
      comSetProperty(fp, "ColorIndex", CP$FontColorIndex) }
    if ("BackgroundColorIndex" %in% names(CP)){
      aux <- comGetProperty(r, "Interior")
      aux[["ColorIndex"]] <- CP$BackgroundColorIndex
      aux[["Pattern"]] <- "xlSolid"
    }
    if ("DrawlineSide" %in% names(CP)){   # does not work now...
      aux <- paste("Borders(xlEdge", CP$DrawlineSide, ")", sep="")
      bux <- eval(parse(text=paste('comGetProperty(r, "', aux, '")', sep="")))
    }
    if ("NumberFormat" %in% names(CP)){
      comSetProperty(r, "NumberFormat", CP$NumberFormat)
    } 
  }  
}


#########################################################################
# use the RODBC package to read a sheet
# 
XLS$read.sheet <- function(filename, sheetname)
{
  con    <- odbcConnectExcel(filename)
  data   <- sqlFetch(con, sheetname)
  odbcCloseAll()

  return(data)
}
#########################################################################
# Insert a data.frame x to an xls
# file needs to be a full path name.
XLS$write.xls <- function(x, file, sheetname="Sheet1", top.cell="A1", 
        visible=FALSE, col.names = TRUE, propertyList=NULL)
{
#  browser()
    col.Class <- sapply(x, class)
    ind.Date  <- col.Class %in% c("Date", "POSIX")
    x[,ind.Date] <- as.character(x[,ind.Date])    # make them character
    if (ncol(x)>256){
      cat("Only 256 columns will be exported.\n")
      x <- x[,1:256]}     
    if (nrow(x)>63999){
      cat("Only 64,000 rows will be exported.\n")  
      x <- x[,1:63999]
    }              
    
    splitTopCells = strsplit(top.cell, "")[[1]]
    numberOffset = regexpr("[0-9]+", top.cell)
    startingRow = splitTopCells[numberOffset:length(splitTopCells)]
    startingRow = paste(startingRow, collapse="")
    startingColumn = splitTopCells[1:(numberOffset-1)]
    startingColumn = paste(startingColumn, collapse="")
    
    xls.colnames <- c(LETTERS,t(outer(LETTERS,LETTERS,"paste",sep="")))
    columnOffset = which(xls.colnames==startingColumn)
    xls.colnames = xls.colnames[columnOffset:(columnOffset+ncol(x)-1)]
    last.xls.colname <- xls.colnames[ncol(x)]
    
    xls <- comCreateObject("Excel.Application")
    if (visible==TRUE){comSetProperty(xls, "Visible", TRUE)}
    if (!file.exists(file)){  # copy an empty template file 
      file.copy("//ceg/cershare/All/Risk/Software/DO_NOT_DELETE.xls", file)
    }
    wb  <- comGetProperty(comGetProperty(xls, "Workbooks"), "Open", file)
    if (is.null(wb))
      stop("Cannot open the file.  Please check filepath.")
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
    
    # write the colnames  
    if( col.names )
    {
        r <- comGetProperty(target.sheet, "Range", top.cell, 
                paste(last.xls.colname,startingRow, sep=""))
        comSetProperty(r, "Value", colnames(x))         
    }else
    {
        startingRow = as.character(as.numeric(startingRow)-1)
    }
    
    # write one column at a time
    dataStart = as.character(as.numeric(startingRow)+1)
    dataStop = as.character(nrow(x)+as.numeric(startingRow))

    for (k in 1:ncol(x)){
      r <- comGetProperty(target.sheet, "Range", paste(xls.colnames[k],
        dataStart, sep=""), paste(xls.colnames[k], dataStop, sep=""))
      comSetProperty(r, "Value", as.matrix(x[,k]))
    }

    # set the cell property list if any 
    if (!is.null(propertyList)){
       try(XLS$set.property(target.sheet, propertyList))}
    
    comSetProperty(wb, "Save", file)
    comInvoke(xls, "Quit")
    rm(xls)
}




########################################################################
#  Function to delete a sheet from a file
#  input filepath, and name of the sheet to be deleted
#
XLS$delete.sheet <- function(filepath=NULL, sheetname=NULL)
{
  xls <- comCreateObject("Excel.Application")
  wb  <- comGetProperty(comGetProperty(xls, "Workbooks"), "Open", filepath)
  sheets <- comGetProperty(xls, "Sheets") 
  comSetProperty(xls, "Visible", TRUE) 
  sheet.to.delete <- comGetProperty(sheets, "Item", sheetname)
  if( !is.null(sheet.to.delete) )
  {
      sheet.to.delete$Activate()
      sheet.to.delete$Delete()
  }
  comSetProperty(wb, "Save", filepath)      
  comInvoke(xls, "Quit")
  rm(xls)  
}


########################################################################
# Rename a sheet of a spreadsheet
#
XLS$rename.sheet <- function(filepath, oldname, newname)
{
  xls <- comCreateObject("Excel.Application")
  wb  <- comGetProperty(comGetProperty(xls, "Workbooks"), "Open", filepath)
  sheets <- comGetProperty(xls, "Sheets")  
  sheet.to.rename <- comGetProperty(sheets, "Item", oldname)
  sheet.to.rename[["Name"]] <- newname
  comSetProperty(wb, "Save", filepath)      
  comInvoke(xls, "Quit")
  rm(xls)
}
  
#############################################################################
#       This one works the same as write.xls()
#
XLS$add.sheet <- function(parentfile, parent.sheetname, child.sheetname)
{   
  #add merged table to a new sheet of the parent file
  xls <- comCreateObject("Excel.Application")
  wb  <- comGetProperty(comGetProperty(xls, "Workbooks"), "Open", parentfile)
      comSetProperty(xls, "Visible", TRUE)
      sheets <- comGetProperty(xls, "Sheets")
      new.sheet <- sheets$Add()
      new.sheet[["Name"]] <- child.sheetname
      xls.colnames <- c(LETTERS,t(outer(LETTERS,LETTERS,"paste",sep="")))
      headers = colnames(p_c.new)
      h.range.end =paste(xls.colnames[length(headers)],"1",sep ="")
      h.range <- new.sheet[["Range", "A1", h.range.end]]
      h.range$Select()
      comSetProperty(h.range, "Value", as.matrix(headers))
      
      s.range.end <- paste(xls.colnames[ncol(p_c.new)],nrow(p_c.new)+1, sep ="")
      s.range <- new.sheet[["Range", "A2", s.range.end]]
      s.range$Select()
      
      comSetProperty(s.range, "Value", as.matrix(p_c.new))
      comSetProperty(wb, "Save", parentfile)
      
  }
 
###############################################################################  
# Will add a table (given as dataframe) to a selected range.
XLS$add.table <- function(range, table.data)
{
  if(class(range) == "COMObject" ){
    if(range$Activate()== TRUE){
       comSetProperty(range, "Value", as.matrix(table.data))
    } else{
       rLog("Cannot activate the given range. Operation failed")
    }
  }
}


#########################################################################
# 
# 
XLS$open.xls <- function(filename){

}

#########################################################################
# Advance the top.cell by a certain number of row, columns
# 
XLS$advance.cell.coordinates <- function(from.cell="A1", add.rows=0, add.cols=0)
{
  xls.colnames <- c(LETTERS,t(outer(LETTERS,LETTERS,"paste",sep="")))[1:256]
  
  splitTopCell = strsplit(from.cell, "")[[1]]
  startRow <- as.numeric(paste(grep("[[:digit:]]", splitTopCell, value=TRUE),
    sep="", collapse=""))
  startColumn <- paste(grep("[[:alpha:]]", splitTopCell, value=TRUE), sep="",
                       collapse="")
  
  toRow    <- startRow + add.rows
  toColumn <- xls.colnames[match(startColumn, xls.colnames)+add.cols]

  return(paste(toColumn, toRow, sep="", collapse=""))
}


#########################################################################
# Example of how to use the library ...
# 
XLS$.demo.lib.excel.functions <- function(){

MM <- data.frame(letters = month.abb[1:4],
                 num     = 1:4,
                 dates   = seq(Sys.Date(), length.out=4, by="day"))

filename  <- "C:/Temp/A.xls"
sheetname <- "Sheet1"

#source("H:/user/R/RMG/Utilities/lib.excel.functions.R")

# create some cell properties
CP <- NULL
CP$range <- c("B1", "B5")    # start, end of range
CP$Font   <- "Bold"
CP$FontSize   <- 14
CP$FontColorIndex <- 9        # brown
CP$BackgroundColorIndex <- 6  # yellow=6, magenta=7
#CP$rawlineSide <- "Right"   # c("Top", "Bottom", "Right", "Left")
CP$Comment <- "R is easy"
CP$NumberFormat <- "0.00_);[Red](0.00)"   # valid Excel number formats
#CP$NumberFormat <- "#,##0.00_);[Red](#,##0.00)"
#                   "mm/dd/yy;@"   # for dates
CP <- list(CP)

XLS$write.xls(MM, filename, sheetname, propertyList=CP)


}



################################################################################
## # will sort table on a sheet according to column A by ascending order
## excel.sort <-function(sheet){  #argument "sheet" is an Excel
## worksheet to be sorted
##   #not quite sure how to do this in Excel directly. It's better
## to use R to do the job.
##   }
  
