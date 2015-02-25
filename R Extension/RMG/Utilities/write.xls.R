# Utility to export a data.frame to an Excel file (NO rownames!).
# Will convert everything to numbers/characters.
# Where x        - a data.frame to write
#       file     - the name of the xls output file,
#                  if file exists it will be overwritten
#       top.cell - in the form "A1", the upper-left corner of your data,
#                  only "A1" is implemented now.
#       visible  - TRUE or FALSE
#
# To do: - Do better than the empty spreadsheet.  Fix the saving issue.
#        - Allow more general cell referencing.
#
# Written by Adrian Dragulescu on 20-Oct-2006
#
# x <- data.frame(months = month.abb, no=1:12)
# file <- "C:/test1.xls"
# sheetname <- "first"
# source("H:/user/R/RMG/Utilities/write.xls.R")
# write.xls(x, file, sheetname, visible=FALSE)
source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
load.packages("rcom")

write.xls <- function(x, file, sheetname, top.cell="A1", 
        visible=FALSE, col.names = TRUE )
{
#  browser()
    col.Class <- sapply(x, class)
    ind.Date  <- col.Class %in% c("Date", "POSIX")
    x[,ind.Date] <- as.character(x[,ind.Date])    # make them character
    if (ncol(x)>256){x <- x[,1:256]}              # cut the data
    
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
    
    
    if (nargs()==2){sheetname <- "Sheet1"}  
    xls <- comCreateObject("Excel.Application")
    if (visible==TRUE){comSetProperty(xls, "Visible", TRUE)}
    if (!file.exists(file)){  # copy an empty template file 
        suffix = strsplit( basename(file), ".", fixed=TRUE )[[1]][2]
        if( suffix == "xlsx" )
        {
            file.copy("//ceg/cershare/All/Risk/Software/DO_NOT_DELETE.xlsx", file)
        }
        else
        {
            file.copy("//ceg/cershare/All/Risk/Software/DO_NOT_DELETE.xls", file)
        }
    }
    wb  <- comGetProperty(comGetProperty(xls, "Workbooks"), "Open", file)
    sheets <- comGetProperty(xls, "Sheets")
    noOfSheets <- comGetProperty(sheets, "Count")
    sheetNames <- NULL
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
        r <- comGetProperty(target.sheet, "Range", paste(xls.colnames[k], dataStart, sep=""),
                paste(xls.colnames[k], dataStop, sep=""))
        comSetProperty(r, "Value", as.matrix(x[,k]))
    }
    comSetProperty(wb, "Save", file)
    comInvoke(xls, "Quit")
    rm(xls)
    gc()
}






##   r <- comGetProperty(target.sheet, "Range", "A1", rend)   # get the range
##   comSetProperty(r, "Value", x)         # push the data to Excel

##   range.end <- function(top.cell, ncols, nrows){
##     aux <- ncols %/% 26
##     end.col <- LETTERS[ncols %% 26]
##     if (aux > 0){end.col <- paste(LETTERS[aux+1], end.col, sep="")}
##     return(paste(end.col, nrows, sep=""))
##   }
##   rend <- range.end(top.cell, ncol(x), nrow(x))



##   if (file.exists(file)){      # open the file
##     wb  <- comGetProperty(comGetProperty(xls, "Workbooks"), "Open", file)
##   } else {                     # create the file
##     wb <- comInvoke(comGetProperty(xls, "Workbooks"), "Add")
##   }

##   if (file.exists(file)){
##     comSetProperty(wb, "Save", file)
##   } else {
##     comSetProperty(wb, "SaveAs", file)    # save
##   }
