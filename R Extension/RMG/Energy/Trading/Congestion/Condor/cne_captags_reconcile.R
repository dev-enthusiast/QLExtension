# Reconcile CNE captags
#
#
# Written by Adrian Dragulescu on 17-Feb-2012



#################################################################
#################################################################
options(width=500)  # make some room for the output
options(stringsAsFactors=FALSE)
require(methods)  
require(CEGbase)
require(reshape)
require(SecDb)

require(xlsx)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.capacity.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/CNE/Capacity/reconcile_cne_cap_tags.R")

returnCode <- 0
  
asOfDate <- Sys.Date()
# go back two months
month <- seq(as.Date(format(asOfDate, "%Y-%m-01")), by="-2 month", length.out=2)[2]

res <- run_report(month)

if (res == 0) {
  to <- paste(c("adrian.dragulescu@constellation.com", 
    "abizar.shahpurwala@constellation.com", "kate.norman@constellation.com", 
    "ranjan.ezra@constellation.com", "brandon.fong@constellation.com"), sep="", collapse=",")
  #to <- "adrian.dragulescu@constellation.com"
  from <- "OVERNIGHT_PM_REPORTS@constellation.com"

  out <- paste("See files in directory \n",
    "S:\\All\\Structured Risk\\NEPOOL\\ConstellationNewEnergy\\Capacity\\ISO.Reconciliation",
    "\n\n\n\nFor questions contact Adrian Dragulescu.",
    "\nProcmon process: /RMG/R/Reports/Energy/Trading/Congestion/cne_captags_reconcile",
    sep="")
  sendEmail(from, to, paste("Done CNE captag reconciliation for", format(month, "%b-%y")), out)
}



rLog(paste("Done at ", Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}



































## #filename   <- "C:/Temp/testxls.xls"
## filename    <- "C:/Temp/Downloads/myOntario.xls"
## readpasswd  <- "o"
## writepasswd <- "o"

## xls <- COMCreate("Excel.Application")
## xls[["Visible"]] <- TRUE

## books <- xls[["workbooks"]]

## # book <- books$open(filename)   
## book <- books$open(filename, NA, NA, NA, readpasswd, writepasswd)

## sheets <- book[["sheets"]]

## sheets$Count()  # number of sheets

## sheetnames <- NULL
## for (s in seq_len(sheets$Count())){
##   aux <- sheets$Item(as.integer(s))
##   sheetnames <- c(sheetnames, aux[["Name"]])
## }


## ind <- which(sheetnames == "(4) OUTPUT FOR PRICING")
## mySheet <- sheets$Item(as.integer(ind))  

## # run a macro
## xls$Run("Odelta")
## xls[["DisplayAlerts"]] <- 0

## # close this workbook
## book$Close()
## # quit excel, still leaves a rogue excel in place.
## xls$quit()








## r <- mySheet[["Cells"]]

## # extract cell [1,1]
## v <- r$Item(as.integer(1), as.integer(1))
## v[["Value"]]

## # extract cell [1,3]
## v <- r$Item(as.integer(1), as.integer(3))
## v[["Value"]]

## # close spreadsheet 
## book$Close()
## xls <- NULL
## gc()  # does not kill excel





## xls[["Visible"]]
## xls[["StandardFontSize"]]

## xls[["StartupPath"]]
## xls[["Path"]] #
## xls[["PathSeparator"]] # Check characters.	

## xls[["StatusBar"]] # VARIANT returned.

## xls[["SheetsInNewWorkbook"]]


## # Functions
## xls$CentimetersToPoints(20.)
## xls$CheckSpelling("Duncan")
## xls$CheckSpelling("Duncna")

## xls$Calculate()
## xls$CalculateFull()	


## print repr(xlwb) 
## print xlwb.FullName 
## xlws = xlwb.Sheets(1) # counts from 1, not from 0 print repr(xlws) 
## print xlws.Name 
## print xlws.Cells(1, 1) # that's A1
