# Given a set of bids in a standard format in a spreadsheet
#
#
#

FTR.makeUploadXls <- function(bids.xls, file="upload.bids.xls", region="NEPOOL")
{
  if (dirname(file)==".")  # in case you forgot to give the full path
    file <- paste(getwd(), file, sep="/")

  if (file.exists(file))   # delete the file if it exists
    unlink(file)

  if (toupper(region) == "NEPOOL"){
    .make.NEPOOL.upload.sheet(bids.xls, file)
  }
  if (toupper(region) == "NYPP"){
    .make.NYPP.upload.sheet(bids.xls, file)
  }
  
  .insert.bids.xls(bids.xls, file)
  .insert.paths(bids.xls, file)
  
  cat("Wrote the xls.\n")   
}

#===============================================================
.make.NYPP.upload.sheet <- function(bids.xls, file)
{
  MM <- bids.xls[, c("bid.no", "mw", "bid.price.month",
                     "source.ptid", "sink.ptid")]
  colnames(MM)   <- c("bid.no", "mw", "bid.price", "POI", "POW")
  MM$request     <- "U"
  MM$bid.no      <- 1:nrow(MM)    
  MM$mw          <- ceiling(MM$mw)            # round upwards
  MM$bid.price   <- round(MM$bid.price, 2)
  MM$total.price <- MM$mw * MM$bid.price
  MM <- MM[,c(1:3, 7, 4:6)]                   # put them in the right order
    
  xlsWriteSheet(MM, file, "ISOupload", col.names=TRUE)
  rLog("")
  rLog("For NYISO, please upload the sheet ISOupload from spreadsheet:")
  rLog(paste("   ", file, sep=""))
}

.make.NEPOOL.upload.sheet <- function(bids.xls, file, do.checks=TRUE)
{
  rLog("DEPRECATED.  Use write_ftrbids_csv_file!")
}


#===============================================================
# this function needs exactly the right inputs!
#
write_ftrbids_csv_file <- function(bids.xls, file, do.checks=TRUE)
{
  if (!(all(c("start.dt", "end.dt", "class.type", "buy.sell",
        "source.name", "sink.name", "mw", "bid.price.month") %in% names(bids.xls))))
    stop("Missing some columns in bids.xls")
  
  if (do.checks){
    .check_bids(bids.xls)
  }
  
  MM <- matrix("", nrow=(nrow(bids.xls)+3), ncol=14)
  MM[1,1:6] <- c("C","FTR","FTRBID", "ISONE", format(Sys.Date(), "%Y/%m/%d"),
                 format(Sys.time(), "%H:00 %p"))
  MM[2,1:14] <- c("I","FTRBID", "BID", "VERSION", "PARTICIPANT", "MARKET",
    "BEGIN", "END", "CLASS", "BUYSELL", "SOURCE", "SINK", "MW", "PRICE")
  MM[3:(nrow(MM)-1), 1:6] <- rep(c("D", "FTRBID", "BID","1",
    "CONSTELLATION ENERGY COMMODITI", "A"), each=(nrow(MM)-3))
  MM[3:(nrow(MM)-1),7]  <- format(as.Date(bids.xls$start.dt), "%Y/%m/%d")
  MM[3:(nrow(MM)-1),8]  <- format(as.Date(bids.xls$end.dt), "%Y/%m/%d")
  MM[3:(nrow(MM)-1),9]  <- as.character(bids.xls$class.type)
  MM[3:(nrow(MM)-1),10] <- as.character(bids.xls$buy.sell)
  MM[3:(nrow(MM)-1),11] <- as.character(bids.xls$source.name)
  MM[3:(nrow(MM)-1),12] <- as.character(bids.xls$sink.name)
  MM[3:(nrow(MM)-1),13] <- as.character(round(bids.xls$mw,1))
  MM[3:(nrow(MM)-1),14] <- as.character(round(bids.xls$bid.price.month,2))
  
  MM[nrow(MM),1:3] <- c("C", "END OF REPORT", as.character(nrow(MM)))

  filecsv <- gsub("xls", "csv", file)
  write.table(MM, file=filecsv, row.names=F, col.names=FALSE, sep=",",
              quote=FALSE)
  
  # remove extra commas from the csv file
  aux <- readLines(filecsv)
  aux[1] <- gsub(",,,,,,,,", "", aux[1])
  aux[length(aux)] <- gsub(",,,,,,,,,,,", "", aux[length(aux)])
  writeLines(aux, filecsv)

  rLog("")
  rLog("The bid file ready for NEPOOL upload is saved at:")
  rLog(paste("   ", getwd(), "/", file, sep=""))  
}


