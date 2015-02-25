# Automate the CNE mark submission.  Write aux_nepool.xlsx file every night.
#
# insert_nepool_hedge_costs
# insert_nepool_offpeak_split
# writexlsx_buckets_gas_close
#
# Written by Adrian Dragulescu on 3-Feb-2010
#


###########################################################################
# Create the aux_nepool.xlsx spreadsheet.
# Called from Condor/cne_nepool_mark_download.R
#
writexlsx_buckets_gas_close <- function(filename, NN, GG, AGT, SC)
{
  ratio <- NN
  ratio$value <- NULL  
  ratio <- cast(ratio, month ~ bucket, I, fill=0, 
    subset=location=="MASSHUB" & bucket %in% c("OFFPEAK", "2X16H", "7X8"),
                value="secdbValue")
  ratio[, "2X16H"] <- ratio[, "2X16H"]/ratio[, "OFFPEAK"]
  ratio[, "7X8"] <- ratio[, "7X8"]/ratio[, "OFFPEAK"]
  
  wb <- loadWorkbook(filename)
  sheets <- getSheets(wb)
  dateFormat <- CellStyle(wb) +  DataFormat("m/d/yyyy")

  rLog("add the ratio")
  sheet <- sheets[["OffpeakSplit"]]
  rows <- getRows(sheet)
  if (length(rows)>0)   # remove rows ...
    lapply(rows, function(x){.jcall(sheet, "V", "removeRow", x)})
  noRows <- nrow(ratio) + 4
  rows  <- createRow(sheet, rowIndex=1:noRows)
  cells <- createCell(rows, colIndex=1:3)
  for (ic in 1:3)
    mapply(setCellValue, cells[5:noRows,ic], ratio[,ic])
  lapply(cells[5:noRows,1], setCellStyle, dateFormat)
  setCellValue(cells[[4,2]], "2X16H")
  setCellValue(cells[[4,3]], "7X8")
  setCellValue(cells[[1,2]], paste("Last updated:", Sys.time()))
  autoSizeColumn(sheet, 1)

  rLog("add the gas")
  sheet <- sheets[["gas"]]
  rows <- getRows(sheet)
  if (length(rows)>0)   # remove rows ...
    lapply(rows, function(x){.jcall(sheet, "V", "removeRow", x)})
  GG <- GG[, c("month", "value")]
  noRows <- nrow(GG) + 4
  rows  <- createRow(sheet, rowIndex=1:noRows)
  cells <- createCell(rows, colIndex=1:3)
  for (ic in 1:2)
    mapply(setCellValue, cells[5:noRows,ic], GG[,ic])
  #dateFormat <- createCellStyle(wb, dataFormat="m/d/yyyy")
  lapply(cells[5:noRows,1], setCellStyle, dateFormat)
  setCellValue(cells[[4,2]], "HH Gas")
  setCellValue(cells[[1,2]], paste("Last updated:", Sys.time()))
  autoSizeColumn(sheet, 1)
  
  rLog("add the power")
  sheet <- sheets[["power"]]
  rows <- getRows(sheet)
  if (length(rows)>0)   # remove rows ...
    lapply(rows, function(x){.jcall(sheet, "V", "removeRow", x)})
  res <- NN
  res$value <- NULL  # bug in reshape!
  res <- cast(res, month ~ bucket + location, I, value="secdbValue",
    subset=bucket %in% c("PEAK", "OFFPEAK"))
  aux <- paste("PEAK", c("CT", "MASSHUB", "ME", "NEMA", "NH", "RI",
    "SEMA", "VT", "WCMA"), sep="_")
  cnames <- c("month", aux, gsub("^PEAK", "OFFPEAK", aux))
  res <- res[,cnames]

  # agt
  agt <- subset(AGT, pricing.dt == max(AGT$pricing.dt))
  agt <- cast(agt, contract.dt ~ curve.name, I, fill=NA)
  colnames(agt) <- gsub("COMMOD NG ", "", colnames(agt))
  colnames(agt)[1] <- "month"
  res <- merge(res, agt, by="month", all=TRUE)
  # sc
  sc <- cast(SC, month ~ bucket + location, I, value="secdbValue")
  sc <- sc[,c("month", "PEAK_SC", "OFFPEAK_SC")]
  res <- merge(res, sc, by="month", all.x=TRUE)
  
  noRows <- nrow(res) + 4
  rows  <- createRow(sheet, rowIndex=1:noRows)
  cells <- createCell(rows, colIndex=1:ncol(res))
  for (ic in 1:ncol(res))
    mapply(setCellValue, cells[5:noRows,ic], res[,ic])
  #dateFormat <- createCellStyle(wb, dataFormat="m/d/yyyy")
  lapply(cells[5:noRows,1], setCellStyle, dateFormat)
  mapply(setCellValue, cells[4, 1:ncol(res)], colnames(res))
  setCellValue(cells[[1,2]], paste("Last updated:", Sys.time()))
  autoSizeColumn(sheet, 1)
  
  saveWorkbook(wb, filename)

  
}



















## ###########################################################################
## # Add gas closing prices to the xls
## #
## insert_gas_prices <- function(xls, GG, filename, visible=FALSE)
## {
##   err <- ""
##   rLog("Insert gas prices ", filename)
##   readpasswd  <- "ne"
##   writepasswd <- "ne"

##   xls[["Visible"]] <- visible
##   readOnly <- FALSE

##   books <- xls[["workbooks"]]
##   book <- books$open(filename, NA, readOnly, NA, readpasswd, writepasswd)
##   sheets <- book[["sheets"]]
  
##   sheetnames <- NULL
##   for (s in seq_len(sheets$Count())){
##     aux <- sheets$Item(as.integer(s))
##     sheetnames <- c(sheetnames, aux[["Name"]])
##   }

##   ind <- which(sheetnames == "current mids")
##   if (length(ind)!=1)
##     return("Cannot find sheet 'closing mids'!")
##   mySheet <- sheets$Item(as.integer(ind))  

##   res <- cast(GG, month ~ bucket + location, I, value="value")
  
##   r <- mySheet$Range("M16:M145")
##   r$ClearContents()                 # clear the old data!
##   r <- mySheet[["Cells"]]

##   voffset <- 0
##   repeat {
##     v <- as.Date(as.numeric(r$Item(as.integer(16+voffset),
##       as.integer(1))[["Value"]]), origin="1899-12-30")
##     if (v == res$month[1])
##       break
##     voffset <- voffset + 1
##   }
  
##   for (ir in 1:nrow(res)){
##     v <- r$Item(as.integer(ir+16+voffset-1), as.integer(12)) # add Current
##     v[["Value"]] <- res[ir,2]
##     v <- r$Item(as.integer(ir+16+voffset-1), as.integer(13)) # add Close
##     v[["Value"]] <- res[ir,2]
##   }
##   v <- r$Item(as.integer(13), as.integer(14))    # add timestamp
##   v[["Value"]] <- paste("Last Updated:", Sys.time())

##   book$Save()
##   book$Close()
  
##   rLog("Done!")
##   return(err)
## }





## ###########################################################################
## # Add NEPOOL closing prices to the xls
## #
## insert_nepool_energy_prices <- function(xls, NN, filename, visible=FALSE)
## {
##   err <- ""
##   rLog("Insert Peak/Offpeak prices ", filename)
##   readpasswd  <- "ne"
##   writepasswd <- "ne"

##   xls[["Visible"]] <- visible
##   readOnly <- FALSE

##   books <- xls[["workbooks"]]
##   book <- books$open(filename, NA, readOnly, NA, readpasswd, writepasswd)
##   sheets <- book[["sheets"]]
  
##   sheetnames <- NULL
##   for (s in seq_len(sheets$Count())){
##     aux <- sheets$Item(as.integer(s))
##     sheetnames <- c(sheetnames, aux[["Name"]])
##   }

##   ind <- which(sheetnames == "closing mids")
##   if (length(ind)!=1)
##     return("Cannot find sheet 'closing mids'!")
##   mySheet <- sheets$Item(as.integer(ind))  

##   res <- cast(NN, month ~ bucket + location, I, value="secdbValue",
##     subset=bucket %in% c("PEAK", "OFFPEAK"))
##   aux <- paste("PEAK", c("CT", "HUB", "MAINE", "NEMA", "NH", "RI",
##     "SEMA", "VT", "WMA"), sep="_")
##   cnames <- c("month", aux, gsub("^PEAK", "OFFPEAK", aux))
##   res <- res[,cnames]
  
##   r <- mySheet$Range("A3:S132")
##   r$ClearContents()                 # clear the old data!
##   r <- mySheet[["Cells"]]
  
##   for (ir in 1:nrow(res)){        # add the dates
##       v <- r$Item(as.integer(ir+2), as.integer(1))
##       v[["Value"]] <- as.character(res[ir, 1])
##   }
##   for (ic in 2:ncol(res)){                  # add the values
##     for (ir in 1:nrow(res)){
##       v <- r$Item(as.integer(ir+2), as.integer(ic))
##       v[["Value"]] <- res[ir, ic]
##     }
##   }
##   v <- r$Item(as.integer(1), as.integer(4))    # add timestamp
##   v[["Value"]] <- paste("Last Updated:", Sys.time())

##   book$Save()
##   book$Close()
  
##   rLog("Done!")
##   return(err)
## }


## ###########################################################################
## # Add NEPOOL hedge costs
## #
## insert_nepool_hedge_costs <- function(xls, HC, filename, visible=FALSE)
## {
##   err <- ""
##   rLog("Insert Peak/Offpeak prices ", filename)
##   readpasswd  <- "ne"
##   writepasswd <- "ne"

##   xls[["Visible"]] <- visible
##   readOnly <- FALSE

##   books <- xls[["workbooks"]]
##   book <- books$open(filename, NA, readOnly, NA, readpasswd, writepasswd)
##   sheets <- book[["sheets"]]
  
##   sheetnames <- NULL
##   for (s in seq_len(sheets$Count())){
##     aux <- sheets$Item(as.integer(s))
##     sheetnames <- c(sheetnames, aux[["Name"]])
##   }

##   ind <- which(sheetnames == "hedge costs")
##   if (length(ind)!=1)
##     return("Cannot find sheet 'hedge costs'!")
##   mySheet <- sheets$Item(as.integer(ind))  

##   res <- cast(HC, month ~ bucket + location, I, value="hedgeCost",
##     subset=bucket %in% c("PEAK", "OFFPEAK"))
##   aux <- paste("PEAK", c("CT", "HUB", "MAINE", "NEMA", "NH", "RI",
##     "SEMA", "VT", "WMA"), sep="_")
##   cnames <- c("month", aux, gsub("^PEAK", "OFFPEAK", aux))
##   res <- res[,cnames]
  
##   r <- mySheet$Range("A3:S132")
##   r$ClearContents()                 # clear the old data!
##   r <- mySheet[["Cells"]]
  
##   for (ir in 1:nrow(res)){        # add the dates
##       v <- r$Item(as.integer(ir+2), as.integer(1))
##       v[["Value"]] <- as.character(res[ir, 1])
##   }
##   for (ic in 2:ncol(res)){                  # add the values
##     for (ir in 1:nrow(res)){
##       v <- r$Item(as.integer(ir+2), as.integer(ic))
##       v[["Value"]] <- res[ir, ic]
##     }
##   }
##   v <- r$Item(as.integer(1), as.integer(4))    # add timestamp
##   v[["Value"]] <- paste("Last Updated:", Sys.time())

##   book$Save()
##   book$Close()
  
##   rLog("Done!")
##   return(err)
## }




## ###########################################################################
## # Add NEPOOL 7x8, 2x16H split.
## #
## insert_nepool_offpeak_split <- function(xls, NN, filename, visible=FALSE)
## {
##   err <- ""
##   rLog("Insert 2x16H, 7x8 ratio into ", filename)
##   readpasswd  <- "ne"
##   writepasswd <- "ne"

##   xls[["Visible"]] <- visible
##   readOnly <- FALSE

##   books <- xls[["workbooks"]]
##   book <- books$open(filename, NA, readOnly, NA, readpasswd, writepasswd)
##   sheets <- book[["sheets"]]
  
##   sheetnames <- NULL
##   for (s in seq_len(sheets$Count())){
##     aux <- sheets$Item(as.integer(s))
##     sheetnames <- c(sheetnames, aux[["Name"]])
##   }

##   ind <- which(sheetnames == "OffpeakSplit")
##   if (length(ind)!=1)
##     return("Cannot find sheet 'OffpeakSplit'!")
##   mySheet <- sheets$Item(as.integer(ind))  
  
##   r <- mySheet[["Cells"]]
##   r$ClearContents()                 # clear the sheet

##   ratio <- NN
##   ratio$value <- NULL  
##   ratio <- cast(ratio, month ~ bucket, I, fill=0, 
##     subset=location=="HUB" & bucket %in% c("OFFPEAK", "2X16H", "7X8"),
##                 value="secdbValue")
##   ratio[, "2X16H"] <- ratio[, "2X16H"]/ratio[, "OFFPEAK"]
##   ratio[, "7X8"] <- ratio[, "7X8"]/ratio[, "OFFPEAK"]
  
##   for (ic in 1:(ncol(ratio)-1)){    # add colnames
##       v <- r$Item(as.integer(3), as.integer(ic))
##       v[["Value"]] <- names(ratio)[ic]
##   }
##   for (ir in 1:nrow(ratio)){        # add the dates
##       v <- r$Item(as.integer(ir+3), as.integer(1))
##       v[["Value"]] <- as.character(ratio[ir, 1])
##   }
##   for (ic in 2:3){                  # add the values
##     for (ir in 1:nrow(ratio)){
##       v <- r$Item(as.integer(ir+3), as.integer(ic))
##       v[["Value"]] <- ratio[ir, ic]
##     }
##   }
##   v <- r$Item(as.integer(1), as.integer(1))    # add timestamp
##   v[["Value"]] <- paste("Last Updated:", Sys.time())

##   book$Save()
##   book$Close()
  
##   rLog("Done!")
##   return(err)
## }













## ###########################################################################
## # Submit Ontario 
## #
## submit_ontario <- function(asOfDate, PP, visible=FALSE, 
##   filename="C:/Temp/Downloads/myOntario.xls")
## {
##   rLog("Submitting Ontario marks for", as.character(asOfDate))
##   readpasswd  <- "o"
##   writepasswd <- "o"

##   xls <- COMCreate("Excel.Application")
##   xls[["Visible"]] <- visible

##   books <- xls[["workbooks"]]
##   book <- books$open(filename, NA, NA, NA, readpasswd, writepasswd)

##   sheetnames <- NULL
##   for (s in seq_len(sheets$Count())){
##     aux <- sheets$Item(as.integer(s))
##     sheetnames <- c(sheetnames, aux[["Name"]])
##   }

##   xls$Run("Odelta")         # run macro
##   xls[["DisplayAlerts"]] <- 0
  
##   ind <- which(sheetnames == "(4) OUTPUT FOR PRICING")
##   if (length(ind)!=1)
##     return("Cannot find sheet '(4) OUTPUT FOR PRICING'!")
##   mySheet <- sheets$Item(as.integer(ind))  

##   sheets <- book[["sheets"]]
## }










## ###########################################################################
## # Add NEPOOL closing prices to the xls
## #
## get_nepool_offpeak_split <- function(asOfDate, startDate, endDate)
## {
##   rLog("Grab closing prices for", region, "as of", as.character(asOfDate))
##   commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
##   location    <- c("HUB")
##   bucket      <- c("OFFPEAK", "2X16H", "7X8")
##   serviceType <- "ENERGY"
##   useFutDates <- FALSE
  
##   curveData <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
##     commodity, location, bucket, serviceType, useFutDates)

##   # 2X16H can be Inf at end of months
##   curveData$value[!is.finite(curveData$value)] <- 0   
##   ratio <- cast(curveData, month ~ bucket, I, fill=NA)
  
##   ratio[, "2X16H"] <- ratio[, "2X16H"]/ratio[, "OFFPEAK"]
##   ratio[, "7X8"] <- ratio[, "7X8"]/ratio[, "OFFPEAK"]
  
##   return(ratio)
## }


## #################################################################
## #################################################################
## options(width=500)  # make some room for the output
## options(stringsAsFactors=FALSE)
## require(methods)   # not sure why this does not load!
## require(CEGbase)
## require(CEGterm)
## require(reshape)
## require(RDCOMClient)
## require(SecDb)
## Sys.setenv(tz="")

## rLog(paste("Start proces at", Sys.time()))
## returnCode <- 0    # succeed = 0

## asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-0b"))   # faster?!
## rLog(paste("Start CNE data insert for", as.character(asOfDate)))

## startDate <- as.Date(format(asOfDate, "%Y-%m-01"))
## endDate   <- as.Date("2017-12-01")

## filename <- "C:/Temp/NEPOOLFwdPrice_test.xls"

## ratio <- get_nepool_offpeak_split(asOfDate, startDate, endDate)
## err <- insert_nepool_offpeak_split(ratio, filename, visible=TRUE)



## rLog(paste("Done at ", Sys.time()))

## if( interactive() ){
##   print( returnCode )
## } else {
##   q( status = returnCode )
## }






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



##   res <- cast(OO, month ~ bucket + location, I, value="value",
##     subset=bucket %in% c("PEAK", "OFFPEAK"))
##   cnames <- c("month", "PEAK_ONT", "OFFPEAK_ONT")
##   res <- res[,cnames]


