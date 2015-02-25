#########################################################################
# Advance the top.cell by a certain number of row, columns
# 
.advance.cell.coordinates <- function(from.cell="A1", add.rows=0, add.cols=0)
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
