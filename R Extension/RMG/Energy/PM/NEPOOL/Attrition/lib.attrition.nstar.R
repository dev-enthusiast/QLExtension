#  
#
# NSTAR: http://www.nstar.com/business/energy_supplier/supply_wholesale.asp
#
# download_customer_info
# download_ICAP_tags
# get_customer_info_files
# read_customer_info_xls
# update_customer_info_archive
#

NSTAR <- new.env(hash=TRUE)

NSTAR$DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Attrition/NSTAR/"
NSTAR$ARCHIVE_CUSTOMER_INFO <- "customer_info_archive.xlsx"


#####################################################################
# Download customer counts for a given month
# month is an R Date
# http://www.nstar.com/docs3/energy_supplier/wholesale/customer-info-december-2011.xls
NSTAR$download_customer_info <- function(month=as.Date("2011-12-01"))
{
  month <- as.Date(format(month, "%Y-%m-01")) # always first of month
  mth <- tolower(format(month, "%B-%Y"))

  url <- paste("http://www.nstar.com/docs3/energy_supplier/wholesale/",
    "customer-info-", mth, ".xls", sep="")

  fileout <- paste(NSTAR$DIR, "CustomerCounts/Raw/", basename(url), sep="")

  res <- download.file(url, fileout, mode="wb")  
  
  if (res == 0) {
    rLog("Downloaded ", basename(url), "successfully!")
  } else {
    stop("FAILED download of ", url)
  }
}

#####################################################################
# Download customer counts
#
NSTAR$download_ICAP_tags <- function()
{
  url <- "http://www.nstar.com/docs3/energy_supplier/wholesale/icap-tags.xls"

  fileout <- paste(NSTAR$DIR, "ICAP_Tags/Raw/", basename(url), sep="")

  res <- download.file(url, fileout)
  
  if (res == 0) {
    rLog("Downloaded ", basename(url), "successfully!")
  } else {
    stop("FAILED download of ", url)
  }
}


#####################################################################
# get the filenames of all the files in the archive and label them 
# with the month
#
NSTAR$get_customer_info_files <- function()
{
  dir <- paste(NSTAR$DIR, "CustomerCounts/Raw/", sep="")
  files <- list.files(dir, full.names=TRUE, pattern="\\.xls$")

  aux <- gsub("customer-info-(.*)\\.xls", "\\1", basename(files))
  months <- as.Date(paste("1-", aux, sep=""), "%d-%B-%Y")

  names(files) <- as.character(months)

  files
}



#####################################################################
# Read one xls for one month
# 
NSTAR$read_customer_info_xls <- function(month=as.Date("2011-12-01"))
{
  files <- NSTAR$get_customer_info_files()

  filename <- files[as.character(month)]
  if (length(filename) == 0) {
    rLog("Cannot find file", basename(filename))
    return(NULL)
  }
  
  rLog("Reading file", basename(filename))
  wb <- loadWorkbook(filename)
  sheets <- getSheets(wb)

  res <- vector("list", length=length(sheets))
  for (i in 1:length(sheets)) {
    aux <- read.xlsx(filename, sheetIndex=i)
    names(aux) <- c("customer.class", "customer.count", "kwh")

    aux <- subset(aux, toupper(customer.class) != "TOTAL")
    aux <- aux[,1:3]
    res[[i]] <- cbind(sheet.name=names(sheets)[i], month=month, aux)
  }

  X <- do.call(rbind, res)

  rLog("Done")
  X
}


#####################################################################
# Maintain the archive with all the historical info by 
# appending new data to the old file
#
NSTAR$update_customer_info_archive <- function()
{
  files <- NSTAR$get_customer_info_files()
  toDo <- sort(names(files))  # which months need to do

  fname <- paste(NSTAR$DIR, NSTAR$ARCHIVE_CUSTOMER_INFO, sep="")
  if (file.exists(fname)) {
    A <- read.xlsx2(fname, sheetName="archive", colIndex=1:5,
      colClasses=c("character", "Date", "character", "numeric", "numeric"))
    existingMonths <- unique(A$month)
    toDo <- setdiff(toDo, format(existingMonths))
    if (length(toDo)==0) {
      rLog("No new data to archive, exiting.")
      return(invisible())
    }
  } else {
    A <- NULL      # if you start the archive from scratch
  }
    
  res <- vector("list", length=length(toDo))
  for (i in 1:length(toDo)) {
    res[[i]] <- NSTAR$read_customer_info_xls(as.Date(toDo[i]))
  }
  res <- do.call(rbind, res)
  A <- rbind(A, res)
  A$customer.count <- as.numeric(A$customer.count)
  A$kwh <- as.numeric(A$kwh)

  write.xlsx2(A, fname, sheetName="archive", row.names=FALSE)
  rLog("Wrote ", basename(fname)) 
}
