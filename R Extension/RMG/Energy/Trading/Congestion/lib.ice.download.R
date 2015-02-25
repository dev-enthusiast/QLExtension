###############################################################################
# Adapted from Energy/Trading/Controls/Liquidity/ExtractBrokerData.R
# Added more functions for filtering 
#
# Author: e47187
#
# .archiveInputFiles  - append to the CSV file (overwrite the old data)
# .filter_files
# .downloadFromICE    - pull files from ICE's web site
# .extract_date_filename
# .fix_hub_names      
# .get_filename(days, tag)
# .filter_files       - not good.  needs to be rewritten 
# .replace_yyyy_mm_dd -
# .zipOldFiles        - save space and gzip older files
#
#

    ## colnames(aux) <- c('REGION', 'HUBS', 'PRODUCT', 'STRIP', 'BEGIN_DATE', 'END_DATE',
    ##   'PRICE', 'TIME', 'QUANTITY', 'TOTAL_VOLUME', 'ORDER_TYPE', 'REFERENCE_ID',
    ##   'PARENT_ID', 'MARKET_ID', 'NGX_VOLUME', 'IS_BLOCK_TRADE')


################################################################################
# Lib Namespace
#
ICE <- new.env(hash=TRUE)


################################################################################
# tag = c("POWER_TRADES", "GAS_TRADES", "icecleared_power", "ngxcleared_power") 
#
# Return a list with all the needed configuration parameters (not great design!)
#

ICE$BASE_DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/ICE/Raw/"

ICE$CONFIGURATION <- list(
                           
  POWER_TRADES = list(
      iceDIR = "EOD_Power_Enterprise_CSV/",
      fname  = "powerdeals_yyyy_mm_dd.dat",
      isGZ   = TRUE
    ),
                           
  GAS_TRADES = list(
      iceDIR = "EOD_Gas_Enterprise/",
      fname  = "EOD_gasenterprise_yyyy_mm_dd.xlsx",  # changed on 9/11/2014
      isGZ   = FALSE
    ),
                           
  ICECLEARED_POWER = list(
      iceDIR = "Settlement_Reports/Power/",
      fname  = "icecleared_power_yyyy_mm_dd.xlsx",  # changed on 12/13/2014
      isGZ   = FALSE
    ),
                          
  ICECLEARED_POWEROPTIONS = list(
      iceDIR = "Settlement_Reports/Power/",
      fname  = "icecleared_poweroptions_yyyy_mm_dd.xlsx",  # new from 12/15/2014
      isGZ   = FALSE
    ),
                          
  NGXCLEARED_POWER = list(
      iceDIR = "Settlement_Reports/Power/",
      fname  = "ngxcleared_power_yyyy_mm_dd.xlsx",  
      isGZ   = FALSE
    ),
                          
  ICECLEARED_GAS = list(
      iceDIR = "Settlement_Reports/Gas/",
      fname  = "icecleared_gas_yyyy_mm_dd.xls",
      isGZ   = FALSE
    ),
                          
  NGXCLEARED_GAS = list(
      iceDIR = "Settlement_Reports/Gas/",
      fname  = "ngxcleared_gas_yyyy_mm_dd.xls",
      isGZ   = FALSE
    )
                          
)


################################################################################
# save most recent POWER_TRADES data to an R file
# filterHub = "PJM", or filterHub = "^(Nepool|ISO-NE)"
#
ICE$.archiveInputFiles <- function( days, filterHub=NULL, filterTime=FALSE,
  tag="POWER_TRADES")
{
  config <- ICE$CONFIGURATION[[tag]]

  res <- lapply(days, function(day) {
    aux <- .read_file(day, tag)                 # from lib.ice.stats
    if (is.null(aux)) 
      return(aux)
    if (!is.null(filterHub))                    # pick only this hub
      aux <- aux[grepl(filterHub, aux$HUBS),]
    if (filterTime)                             # pick only actual trades
       aux <- aux[!is.na(aux$TIME),]
 
    aux
  })
  res <- do.call(rbind, res)
  
  # save the data to the archive
  hub <- if (grepl("Nepool", filterHub)) "nepool" else filterHub
  ARCHIVE_FILE <- paste(ICE$BASE_DIR, "../ice.", tag, ".", tolower(hub), ".RData", sep="") 
   dataAll <- res
 
  dataAll <- dataAll[order(dataAll$TIME), ]  
  dataAll <- tail(dataAll, 60000)
  save(dataAll, file=ARCHIVE_FILE)
  rLog("Wrote file", ARCHIVE_FILE)

  invisible()
}


################################################################################
# download a set of files corresponding to a given "tag" from ICE, for one day.
# A tag is a name in the CONFIGURATION list. 
#
ICE$.downloadFromICE <- function( day, tag )
{
  config <- ICE$CONFIGURATION[[tag]]
  if (is.null(config))
    stop("Unknown tag", tag)

  outDIR <- paste(ICE$BASE_DIR, config$iceDIR, sep="") 

  INPUT_FILE <- paste( "https://downloads.theice.com/", config$iceDIR,
    .replace_yyyy_mm_dd(config$fname, day), sep="")
                        
  OUTPUT_FILE <- paste(ICE$BASE_DIR, config$iceDIR, basename(INPUT_FILE), sep="")
  if ( file.exists(paste(OUTPUT_FILE, ifelse(config$isGZ, ".gz", ""), sep="") )){
    rLog("  File in archive.  Skipping day", format(day))
    return( NULL )
  }

  if (weekdays(day) %in% c("Saturday", "Sunday"))
    return( NULL )
         
  command = paste( "i:\\util\\wget --http-user=constellation --http-password=data ",
    "--no-check-certificate ", "--output-document=",
    gsub("/", "\\\\", paste('"', OUTPUT_FILE, '"', sep="")), " ", INPUT_FILE, sep="" ) 
    
  errorCode <- system( command )
    
  if( errorCode != 0 ) {
    cat("File Not Found... Skipping.\n")  # weekends and holidays are empty files!
    file.remove(OUTPUT_FILE)
  }
    
  return( errorCode )
}


###################################################################
# From
# "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/ICE/Raw/Power/powerdeals_2012_11_29.dat.gz"
# return as.Date("2012-11-29")
# It's vectorized
#
.extract_date_filename <- function( fname )
{
  aux <- gsub(".*_([[:digit:]]{4}_[[:digit:]]{2}_[[:digit:]]{2})\\..*",
    "\\1", basename(fname))
  as.Date(aux, "%Y_%m_%d")
}

###################################################################
# use more reasonal hub names
#
#
.fix_hub_names <- function(x)
{
  aux <- matrix(
    c("Nepool MH DA",                      "MassHub",
      "Nepool MH DA Off-Peak",             "MassHub",
      "Nepool MH DA Off-Peak Mini",        "MassHub",
      "Nepool MH DA Mini",                 "MassHub",
      "ISO-NE Maine DA Off-Peak",          "Maine",
      "ISO-NE Maine DA",                   "Maine",
      "ISO-NE NE-Mass DA",                 "NEMA",
      "ISO-NE NE-Mass DA Off-Peak",        "NEMA",
      "ISO-NE Con DA",                     "CT",
      "ISO-NE Con DA Off-Peak",            "CT",
      "ISO-NE New Hampshire DA",           "NH",
      "ISO-NE New Hampshire DA Off-Peak",  "NH",
      "ISO-NE SE-Mass DA",                 "SEMA",
      "ISO-NE SE-Mass DA Off-Peak",        "SEMA",
      "ISO-NE W Central Mass DA",          "WMA",
      "ISO-NE W Central Mass DA Off-Peak", "WMA"), ncol=2, byrow=TRUE,
      dimnames=list(NULL, c("HUBS", "Zone")))
  TT <- structure(aux[,2], names=aux[,1])

  y <- merge(data.frame(HUBS=x, ind=1:length(x)), data.frame(aux), all.x=TRUE)
  y <- y[order(y$ind),]
  

  y$Zone
}

###################################################################
# Get the filename for a given tag and day
#
.get_filename <- function( days, tag )
{
  config <- ICE$CONFIGURATION[[tag]]
  if (is.null(config)) stop("Unknown tag", tag)

  outDIR <- paste(ICE$BASE_DIR, config$iceDIR, sep="")

  filename <- paste(outDIR, sapply(days, function(x){
    .replace_yyyy_mm_dd(config$fname, x)
  }), ifelse(config$isGZ, ".gz", ""), sep="")

  # ICE changed the file name
  if ((toupper(tag) == "GAS_TRADES")) {
    filename <- ifelse(days < as.Date("2014-09-05"), 
      gsub("xlsx$", "xls", filename), filename)
  }

    
  filename
}


###################################################################
# loop over the files and extract what you need from the power
# trades files
#
ICE$.filter_files <- function(hub="Nepool MH Day-Ahead",
    strip="Next Day")
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/ICE/Raw/"
  files <- list.files(DIR, full.names=TRUE, pattern="^powerdeals")

  filter_one <- function(file=file, hub=hub, strip=strip){
    aux <- read.delim(file, sep="|")
#browser()    
    bux <- subset(aux, HUBS==hub & STRIP==strip)
    bux
  }
  res <- finalize(files, filter_one, hub=hub, strip=strip)
  
  res
}

################################################################################
# take a string x = "powerdeals_yyyy_mm_dd.dat.gz" and replace the yyyy, mm, dd
# with the values from the date.  So if day = 2012-11-29
# return "powerdeals_2012_11_29.dat.gz"
#
.replace_yyyy_mm_dd <- function(x, day)
{
  x1 <- gsub("yyyy", format(day, "%Y"), x)
  x2 <- gsub("mm", format(day, "%m"),   x1)
  x3 <- gsub("dd", format(day, "%d"),   x2)

  x3
}


################################################################################
# Good compression!  A factor of 10!
# 
ICE$.zipInputFiles <- function( tag )
{
  config <- ICE$CONFIGURATION[[tag]]
  if (is.null(config)) stop("Unknown tag", tag)

  outDIR <- paste(ICE$BASE_DIR, config$iceDIR, sep="") 

  files <- list.files(outDIR,  full.names=TRUE)
  filesNotGz <- files[grep("\\.gz$", basename(files), invert=TRUE)]
  
  if ( length(filesNotGz)>0 ) {
    for (fname in filesNotGz) {
      rLog("Zipping", fname)
      system(paste("I:/gnu/bin/gzip.exe", shQuote(fname)))
    }
  }

  if ( length(filesNotGz)>0 ) {
    rLog("Remove old dat files ...")
    unlink(filesNotGz)
  }
}



########################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.download.R")

  days <- seq(as.Date("2010-01-01"), as.Date("2010-12-31"), by="1 day")
  #BASE_DIR  <<- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/ICE/"

  tag <- "ICECLEARED_GAS"
  lapply(days, function(day) {
    try( ICE$.downloadFromICE(day, tag) )
  })



  ## i:\util\wget --http-user=constellation --http-password=data --no-check-certificate --output-document="S:\All\Structured Risk\NEPOOL\FTRs\ISODatabase\NEPOOL\ICE\Raw\EOD_Gas_Enterprise\EOD_gasenterprise_2014_09_05.xlsx" https://downloads.theice.com/EOD_Gas_Enterprise/EOD_gasenterprise_2014_09_05.xlsx


  ## curl -SL -k --user constellation:data https://downloads.theice.com/EOD_Gas_Enterprise/EOD_gasenterprise_2014_09_05.xls

  

  
  ## #============================================================
  ## # play with sqlite
  ## require(RSQLite)
  ## days <- seq(as.Date("2013-06-14"), as.Date("2013-07-01"), by="1 day")
  ## day <- days[1]
  ## tag <- "POWER_TRADES"

  ## res1 <- .read_file(days[1], tag)
  ## res2 <- .read_file(days[4], tag)

  ## ARCH <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/ICE/Raw/sqlite/power_trades.sqlite"
  ## drv <- dbDriver("SQLite")
  ## con <- dbConnect(drv, dbname=ARCH)
  ## dbWriteTable(con, "POWER_TRADES", res2)
  
  ## dbGetQuery(con, "select count(*) from POWER_TRADES")

  ## dbDisconnect(con)

  
  
}







  ## files <- .get_filename(days, tag)
  ## if (length(files)==0) {
  ##   rLog("No files downloaded from ICE for the days you want...  Exiting.")
  ##   return()                # nothing to do here ...
  ## }
  ## res <- vector("list", length=length(files))
  ## for (f in 1:length(files)){
  ##   if (!file.exists(files[f]))
  ##     next
  ##   rLog("Reading", files[f])
  ##   aux <- read.delim(files[f], sep="|", encoding="UTF-8")  # need the encoding arg 
  ##   if (!is.null(filterHub))                    # pick only this hub
  ##     aux <- aux[grepl(filterHub, aux$HUBS),]
  ##   if (filterTime){                             # pick only actual trades
  ##     aux <- aux[aux$TIME != "", ]
  ##     aux <- aux[!is.na(aux$TIME),]
  ##   }
  ##   res[[f]] <- aux
  ## }
  ## res$TIME <- as.POSIXct(res$TIME, format="%m/%d/%Y %I:%M:%S %p")
