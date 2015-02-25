# Download short term outages report from NEPOOL
#
#
# Written by Adrian Dragulescu on 20-May-2008


############################################################################
# Compare two ST Outages files 
#
.compareTwoShortTermFiles <- function(file1, file2)
{
  out <- c(paste("Comparing ",
    substr(file1, nchar(file1)-16, nchar(file1)-4), " with ",
    substr(file2, nchar(file2)-16, nchar(file2)-4), sep=""))      
  
  ST1 <- read.csv(file1)
  ST2 <- read.csv(file2)
  ind <- which(names(ST1) %in% c("h", "local.time", "local.hour.end"))
  colnames(ST1) <- gsub("\\.date$", "", colnames(ST1))
  colnames(ST2) <- gsub("\\.date$", "", colnames(ST2))
  
  aux1 <- unique(data.frame(file1=TRUE,
     appNumber=ST1[, c("application.number")]))
  aux2 <- unique(data.frame(file2=TRUE,
     appNumber=ST2[, c("application.number")]))
  MM <- merge(aux1, aux2, all=TRUE)

  newOutages <- MM$appNumber[is.na(MM$file1)]
  if (length(newOutages)>0){
    aux <- subset(ST2, application.number %in% newOutages)
    aux <- unique(aux[,c("station", "equipment.type", "voltage",
      "planned.start", "planned.end", "request.type")])
    aux <- aux[order(aux$planned.start), ]
    out <- c(out, "\n\nNew outages to the list:\n")
    out <- c(out, capture.output(print(aux, row.names=FALSE)))
  }
    
  rmOutages <- MM$appNumber[is.na(MM$file2)]
  if (length(rmOutages)>0){
    aux <- subset(ST1, application.number %in% rmOutages)
    aux <- unique(aux[,c("station", "equipment.type", "voltage",
      "planned.start", "planned.end", "request.type")])
    aux <- aux[order(aux$planned.start), ]
    out <- c(out, "\n\nRemoved outages from the list:\n")
    out <- c(out, capture.output(print(aux, row.names=FALSE)))
  }

  ST2$planned.start <- as.POSIXct(ST2$planned.start,
                                       format="%m/%d/%Y %H:%M:%S")
  aux <- subset(ST2, planned.start <= as.POSIXct(Sys.Date()+2))
  aux <- unique(aux[,c("station", "equipment.type", "voltage",
    "planned.start", "planned.end", "request.type")])
  aux <- aux[rev(order(aux$planned.start)), ]
  out <- c(out, "\n\nActive outages for tomorrow:\n")
  out <- c(out, capture.output(print(aux, row.names=FALSE)))

  return(out)  
}

############################################################################
# Download awards/settle prices directly from the NEPOOL ISO.
#
NEPOOL.download.ST.Outages <- function()
{             
  #con <- url("http://www.iso-ne.com/trans/ops/outages/generateCurrentCSV.action")
  con <- url(paste("http://www.iso-ne.com/transform/csv/outages?outageType=short-term&start=",
                   format(Sys.Date(), "%Y%m%d"), sep=""))
  res <- try(open(con), silent=T)
  if (class(res)[1] == "try-error"){
    rLog("Cannot open url.  Exiting.")
    return(1)
  }
  res <- NULL
  res <- try(read.csv(con, sep=",", skip=4))
  if (class(res)[1]=="error"){
    rLog("Cannot close url.  Exiting.")   
    return(1)
  }
  res <- res[-nrow(res), ]
  close(con)
  res <- res[-1,]
  colnames(res) <- tolower(colnames(res))

  res$actual.start <- as.POSIXct(strptime(res$actual.start,
     "%m/%d/%Y %H:%M:%S", tz="EST5EDT"))

  runHour <- as.numeric(format(Sys.time(), "%H"))
  if (runHour > 12 | runHour <6)
    return(99)                     # don't bother to save and email 
    
  fileout <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/",
    "Outages/st.outages.", format(Sys.time(), "%Y-%m-%d %H"), ".csv", sep="")
  write.csv(res, file=fileout, row.names=FALSE)
  rLog("Wrote outages to", fileout)

  dirOut <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Outages/"
  fileNameHeader <- "st.outages."
  files <- sort(list.files(dirOut, pattern=fileNameHeader))
  yday  <- grep(as.character(Sys.Date()-1), files)
  if (length(yday)>1){
    file1 <- paste(dirOut, files[yday[1]], sep="")  # first pull yesterday
  } else {
    file1 <- paste(dirOut, files[length(files)-1], sep="")
  }
  file2 <- paste(dirOut, files[length(files)], sep="")
  
  out <- .compareTwoShortTermFiles(file1, file2)
  if (!is.null(out)){
    rLog("Email results")
    sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
      getEmailDistro("st_outage_report"),
      "Short Term outages update", out)
  }
  
  return(99)  
}

############################################################################
# Clean-up old files from the directory.  Keep only files from 6AM.  
# zip them up by month
#
.cleanupFiles <- function()
{
  dirOut <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Outages/"
  setwd(dirOut)
  csvfiles  <- list.files(dirOut, pattern="csv$", full.names=TRUE)

  if (length(csvfiles) > 0){
    dateTime <- gsub(".*st\\.outages\\.(.*)\\.csv", "\\1", csvfiles)
    dateTime <- as.POSIXct(paste(dateTime, ":00:00", sep=""))
    hour <- as.POSIXlt(dateTime)$hour

    # keep only the first report in the day, not necessarily the 6AM one!
    gf  <- split(csvfiles, format(as.Date(dateTime)))
    lapply(gf, function(files){
      files <- sort(files)
      if (length(files)>1)
        unlink(files[2:length(files)])
    })

    # gzip all csv files, you can read them back with read.csv anyhow...
    ind <- which(as.Date(dateTime)!= Sys.Date())
    if (length(ind) > 0) {
      for (f in csvfiles[ind])
        res <- system(paste('gzip ', shQuote(basename(f))), intern=TRUE)
    }
  }
  
}


############################################################################
############################################################################
options(width=200)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
Sys.setenv(tz="")

rLog("Download short term outages")
rLog("Start at ", as.character(Sys.time()))
returnCode <- NEPOOL.download.ST.Outages()

rLog("Start cleanup")
.cleanupFiles()

rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = 0 )
}





  
  # any months that need zipping?  Zip all files in previous month.
  ## currentMonth <- format(Sys.time(), "%Y-%m")
  ## prevMonth <- format(as.Date(paste(currentMonth,"-01",sep=""))-1,"%Y-%m")
  ## monthsToArchive <- unique(format(dateTime, "%Y-%m"))
  ## ind.zip <- which(monthsToArchive < prevMonth)
  ## if (length(ind.zip) > 0){
  ##   for (ind in ind.zip){
  ##     thisMonth <- monthsToArchive[ind]
  ##     rLog("Zipping month", thisMonth)
  ##     filesMonth <- basename(files[grep(thisMonth, files)])
  ##     cmd <- paste('zip st.outages.', thisMonth, '.zip "',
  ##       paste(filesMonth, collapse='"  "'), '"', sep="")
  ##     res <- system(cmd, intern=TRUE)
  ##     if (length(res) == length(filesMonth)) unlink(filesMonth)       
  ##   }
  ## }
  
