#  Example on how to read/write excel files
#
#
#
# Adrian Dragulescu, 7/3/2013


#######################################################################
# Some advice:
# 1) If you can avoid it, don't store your data in Excel, it is a
#    terrible format (use .csv or .csv.gz)
# 2) If you want to read into R a really big spreadsheet with data in
#    a rectangular shape use package RODBC -- still the fastest.
# 3) Use package xlsx for everything else (Big spreadsheets are a pain!)
# 4) write.csv, read.csv should be considered too, as they are very
#    convenient.



#######################################################################
# Good method for reading big chunk of data.  May not work good 
# 
.read_with_RODBC <- function(filename)
{
  require(RODBC)
  on.exit(odbcCloseAll())

  con  <- odbcConnectExcel(filename)       # for .xls files
  #con  <- odbcConnectExcel2007(filename)  # for .xlsx files
  data <- sqlFetch(con, "Feb13")    # read the entire sheet "gasdeals"
  
  data  
}


#######################################################################
#######################################################################
#
.main <- function()
{  
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(plyr)
  require(reshape)
  
  source("H:/user/R/RMG/_Learning/2013-07-03_excel_interaction.R")

  
  filename <- "S:/All/Structured Risk/NEPOOL/Temporary/2013_load_by_region.xls"

  D1 <- .read_with_RODBC(filename)
  head(D1)

  #options(java.parameters="-Xmx1024m")  # you may need this for "big" files 
  require(xlsx)
  D2 <- read.xlsx2(filename, sheetName="Feb13")
  head(D2)
  
  D3 <- read.xlsx2(filename, sheetName="Feb13", startRow=3)
  head(D3)
  str(D3)
  
  D4 <- read.xlsx2(filename, sheetName="Feb13", startRow=3,
    colClasses=c("Date", "numeric", "character", "numeric"))
  head(D4)
  str(D4)
  # now it's ok!

  # write it out to an xlsx
  fileout <- "S:/All/Structured Risk/NEPOOL/Temporary/junk.xlsx"
  write.xlsx2(D4, file=fileout, sheetName="test", row.names=FALSE)

  #==============================================================
  # How to format the output?
  # have to get to lower details
  wb <- createWorkbook()
  sheet <- createSheet(wb, "test")
  headerStyle <- CellStyle(wb) + Fill(backgroundColor="wheat",
    foregroundColor="wheat", pattern="SOLID_FOREGROUND")  +
    Font(wb, isBold=TRUE) + Border() 
  cs1 <- CellStyle(wb) + Font(wb, isItalic=TRUE)           # rowcolumns
  cs2 <- CellStyle(wb) + Font(wb, color="blue")

  addDataFrame(D4, sheet, startRow=3, startColumn=2, colnamesStyle=headerStyle,
    rownamesStyle=cs1, colStyle=list(`2`=cs2, `3`=cs2))
  autoSizeColumn(sheet, 3:6) # fit to columns
  file2 <- "S:/All/Structured Risk/NEPOOL/Temporary/junk2.xlsx"
  saveWorkbook(wb, file2) 

  #==============================================================
  # How to do conditional formatting (even finer control)?
  # get even lower
  wb <- createWorkbook()
  sheet <- createSheet(wb, "test")

  headerStyle <- CellStyle(wb) + Fill(backgroundColor="wheat",
    foregroundColor="wheat", pattern="SOLID_FOREGROUND")  +
    Font(wb, isBold=TRUE) + Border() 
  cs1 <- CellStyle(wb) + Font(wb, isItalic=TRUE)           # rowcolumns
  cs2 <- CellStyle(wb) + Font(wb, color="blue")

  addDataFrame(D4, sheet, startRow=1, startColumn=1,
    row.names=FALSE, colnamesStyle=headerStyle,
    rownamesStyle=cs1, colStyle=list(`2`=cs2, `3`=cs2))
  autoSizeColumn(sheet, 1:4) # fit to columns

  cb <- CellBlock(sheet, 1, 1, nrow(D4), 5, create=FALSE)
  ind <- which(D4[,4] > -30000)
  CB.setFont(cb, Font(wb, color="red"), ind+1, 4)
  
  file3 <- "S:/All/Structured Risk/NEPOOL/Temporary/junk3.xlsx"
  saveWorkbook(wb, file3) 


  
}


