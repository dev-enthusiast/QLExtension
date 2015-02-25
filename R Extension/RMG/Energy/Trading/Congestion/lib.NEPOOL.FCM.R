# Utilitites to download and archive FCM summary results
#
# download_all_FCM_files
# download_ARA_summary
# download_CSOB_summary
# download_MRA_summary
# update_archive_RData  - archive with clearing prices
# .get_month
# .process_chunk
# .ara_xls_to_tabular  
# .mra_xls_to_tabular
# .csob_xls_to_tabular




################################################################
# ca is a CapacityAuction
#
download_all_FCM_files <- function(year)
{
  BASE_URL <- "http://www.iso-ne.com/markets/othrmkts_data/fcm/cal_results/"
  DIR_OUT  <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FCM/Auctions/Raw/"
  rLog("All files downloaded to", DIR_OUT)
  
  new.mra  <- download_MRA_summary( year, BASE_URL, DIR_OUT ) 

  new.ara  <- download_ARA_summary( year, BASE_URL, DIR_OUT )
  
  new.csob <- download_CSOB_summary( year, BASE_URL, DIR_OUT )

  c(new.mra, new.ara, new.csob)
}


################################################################
# All the files in a year.
# year = 2012
# http://www.iso-ne.com/markets/othrmkts_data/fcm/cal_results/ccp12/ara122/ara2cp2011_12.xls
#
download_ARA_summary <- function(year, BASE_URL, DIR_OUT)
{  
  yy  <- substring(year, 3, 4) 
  url <- paste(BASE_URL, "ccp", yy, "/ara", yy, "1/index.html", sep="")
  links <- .getLinksInPage(url, pattern="ara(.*)\\.xls")
  
  url <- paste(BASE_URL, "ccp", yy, "/ara", yy, "2/index.html", sep="")
  links <- c(links, .getLinksInPage(url, pattern="ara(.*)\\.xls"))

  url <- paste(BASE_URL, "ccp", yy, "/ara", yy, "3/index.html", sep="")
  links <- c(links, .getLinksInPage(url, pattern="ara(.*)\\.xls"))

  .download_and_archive(links, DIR_OUT=DIR_OUT)
}


################################################################
# All the files in a year.
# year = 2012
# CSO ARA bilaterals (annual)
download_CSOB_summary <- function(year, BASE_URL, DIR_OUT)
{
  yy  <- substring(year, 3, 4) 

  url <- paste(BASE_URL, "ccp", yy, "/bilat", yy, "/index.html", sep="")
  links <- .getLinksInPage(url, pattern="bil(.*)\\.xls")
  
  .download_and_archive(links, DIR_OUT=DIR_OUT)
}


################################################################
# All the files in a year.
# year = 2012
#
download_MRA_summary <- function(year, BASE_URL, DIR_OUT)
{
  yy  <- substring(year, 3, 4) 
  url <- paste(BASE_URL,  "ccp", yy, "/mra", yy, "/index.html",
    sep="")
  
  links <- .getLinksInPage(url, pattern="_ra\\.xls")    # the monthly reconfig 
  links <- c(links,
           .getLinksInPage(url, pattern="_bil\\.xls"))  # the monthly bilateral 
  
  .download_and_archive(links, DIR_OUT=DIR_OUT)
}


################################################################
# this has clearing prices and volumes only
#
update_archive_RData <- function()
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FCM/Auctions/Raw/"
  FILE_ARCHIVE <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/",
    "fcm_summary.RData", sep="")
  if ( file.exists(FILE_ARCHIVE) ) {
    load(FILE_ARCHIVE)
    filesIn <- sort(unique(FCM$filename))
  } else {
    FCM <- NULL      
    filesIn <- NULL
  }

  allFiles <- list.files(DIR, full.names=TRUE)
  filesToProcess <- setdiff(basename(allFiles), filesIn)

  if (length(filesToProcess) > 0) {
    
    rLog("Working on MRA's")
    files.mra <- grep("_ra\\.", basename(filesToProcess), value=TRUE)
    mra <- lapply(files.mra, .mra_xls_to_tabular, DIR)
    mra <- do.call(rbind, mra)

    rLog("Working on ARA's")
    files.ara <- grep("ara", basename(filesToProcess), value=TRUE)
    ara <- lapply(files.ara, .ara_xls_to_tabular, DIR)    
    ara <- do.call(rbind, ara)
    
    rLog("Working on Monthly bilaterals")
    files.bil <- grep("_bil\\.", basename(filesToProcess), value=TRUE)
    csob <- lapply(files.bil, .csob_xls_to_tabular, DIR)
    csob <- do.call(rbind, csob)
    
    all <- rbind(mra, ara, csob)
    
    ## # do some cleanup
    ## ind <- which(all$column %in% c("Start Date", "End Date"))
    ## all[ind, "value"] <- format(as.Date(as.numeric(all[ind, "value"]) -
    ##   25569, origin="1970-01-01"))

    FCM <- rbind(FCM, all)
    rownames(FCM) <- NULL
    
    save(FCM, file=FILE_ARCHIVE)
    rLog("Saved ", FILE_ARCHIVE)
    write.csv(FCM, row.names=FALSE, 
      file="S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/CSV/fcm_summary.csv")

    out <- all
  } else {
    rLog("No new file to process.  Exiting.")
    out <- NULL
  }
  
  out
}



################################################################
# try to figure out the ARA info from filename & first entry
#
.get_ara_info <- function(filename, txt)
{
  #browser()
  if (filename == "araresultsposting.xls") {
    ara <- "2"
  } else {
    ara <- gsub(".*ara([[:digit:]]).*", "\\1", filename)
  }
  if ( !(ara %in% c("1", "2", "3")) ) {  # try to figure it out from the text!
    words <- c("first", "second", "third")
    isMatch <- sapply(words, function(x){grepl(x, txt)})
    if ( any(isMatch) ) {
      ara <- which( isMatch )
    } else {            
      stop("CANNOT EXTRACT THE ARA NUMBER!")
    }
  }
  ara <- paste("ARA", ara, sep="")
  
  isBilateral <- grepl("bilateral", txt)
  if (isBilateral) {
    x <- gsub(".*period\\.([[:digit:]])\\..*", "\\1", txt)
    bilateralPeriod <- as.numeric(x)
    if (filename %in% c("cp2011_12_ara3_bil_rev1.xls"))
      bilateralPeriod <- 1
    if ( is.na(bilateralPeriod) ) {
      stop("CANNOT EXTRACT THE BILATERAL PERIOD!")
    }
  } else {
    bilateralPeriod <- NA
  }

  ccp <- gsub(".*commitment\\.period\\.([[:digit:]]{4}\\.[[:digit:]]{4}).*", "\\1", txt)
  if ( length(ccp)==1 ) {
    ccp <- gsub("\\.", "-", ccp)
  } else {
    ccp <- NA
  }

  c(ara=ara, isBilateral=isBilateral, bilateralPeriod=bilateralPeriod,
    capabilityPeriod=ccp)
}


################################################################
# figure out the month from the filename 
#
.get_month <- function(filename, txt)
{
  fname <- gsub("(.*)\\.xls*", "\\1", filename)
  if ( grepl("bil", fname) ) {  # the new format
    mmmyyyy <- strsplit(fname, "_")[[1]]
    aux <- paste("1", mmmyyyy[1], mmmyyyy[2], sep="")
    
  } else if (fname == "january_ra") {   # can't beat this!
    aux <- "1jan2013"
    
  } else {
    aux <- strsplit(fname, "_")[[1]][1]
    aux <- paste("1", aux, sep="")
  }
  formats <- c("%d%b%Y", "%d%B%Y", "%d%b%Y", "1sept%Y")

  for (fmt in formats) {
    month <- tryCatch(as.Date(aux, fmt), error=function(e) NA)
    if ( !is.na(month) )
      break
  }
    
  if ( is.na(month) ) {
    stop("  don't know how to convert to a month!", aux)
  } else {
    rLog("  for month ", format(month))
  }

  month
}


################################################################
# takes all the spreadsheet
#
.process_chunk <- function(x, section="Capacity Zone",
                           nextSection="External Interface")
{
  indStart <- grep(section, x[,1]) + 1  # colnames
  if (length(indStart)==2) 
    indStart <- indStart[2]  # skip the section header
  
  if (nextSection == "") {
    indEnd <- nrow(x)                      # end of data
  } else {
    indEnd <- grep(nextSection, x[,1]) -1  # end of section
  }
  if (length(indEnd)==2) 
    indEnd <- indEnd[1]  # skip the section header
  
  if ( (length(indStart) + length(indEnd)) == 0)  {
    stop("CANNOT FIGURE OUT FIRST SECTION!")
  }
  
  cnames  <- x[indStart-1,]
  res <- NULL
  for (ind.row in indStart:indEnd) {
    rLog("  working on row", ind.row)
    bux <- data.frame(section=section, rowIdx=ind.row,
      column=as.character(cnames), value=as.character(x[ind.row,]) )
    res <- rbind(res, bux)
  }

  res <- subset(res, value != "NA")
#  browser()
  
  res
}


################################################################
# both for ara and the ara bilateral files!
#
.ara_xls_to_tabular <- function(filename, DIR)
{
  rLog("reading", filename)
  aux <- read.xlsx(paste(DIR, filename, sep=""), sheetIndex=1)  

  info <- .get_ara_info(filename, tolower(colnames(aux)[1]))

  if (filename %in% c("cp2010_11_ara3_bilateral1.xls",
                      "ara3_cp2010_2011_summary.xls",
                      "araresultsposting.xls",
                      "cp2011_12_ara2_bilateral1.xls")) {
    section3 <- "Start Date"
  } else {
    section3 <- "Total"
  }

  cz <- .process_chunk(aux, section="Capacity Zone",
                       nextSection="External Interface")
  ei <- .process_chunk(aux, section="External Interface",
                       nextSection=section3)
  or <- .process_chunk(aux, section=section3,
                       nextSection="")
  or$section <- "Total"  # override it, so you don't have Start Date

  
#browser()  
  res <- data.frame(auction=info[["ara"]], month=NA,
    capabilityPeriod=info[["capabilityPeriod"]],
    isBilateral=info[["isBilateral"]],
    bilateralPeriod=info[["bilateralPeriod"]], 
    rbind(cz, ei, or), filename=filename)
  
  res
}


################################################################
# for monthly reconfig xls
.mra_xls_to_tabular <- function(filename, DIR)
{
  rLog("reading", filename)
  aux <- read.xlsx(paste(DIR, filename, sep=""), sheetIndex=1)  

  month <- .get_month(filename, colnames(aux)[1])

  section3 <- "Total"
  
  cz <- .process_chunk(aux, section="Capacity Zone",
                       nextSection="External Interface")
  ei <- .process_chunk(aux, section="External Interface",
                       nextSection=section3)
  or <- .process_chunk(aux, section=section3,
                       nextSection="")
  or$section <- "Total"  # override it, so you don't have Start Date

  res <- cbind(auction="MRA", month=month,
    capabilityPeriod=CapabilityPeriod(month),
    isBilateral=FALSE, bilateralPeriod=NA, 
    rbind(cz, ei, or), filename=filename)
  
  res
}

################################################################
# for monthly bilaterals xls
#
.csob_xls_to_tabular <- function(filename, DIR)
{
  rLog("reading", filename)
  aux <- read.xlsx(paste(DIR, filename, sep=""), sheetIndex=1)

  month <- .get_month(filename, colnames(aux)[1])

  if (filename %in% c("june2010_bil.xls")) {
    section3 <- "Start Date"
  } else {
    section3 <- "Total"
  }

  cz <- .process_chunk(aux, section="Capacity Zone",
                       nextSection="External Interface")
  ei <- .process_chunk(aux, section="External Interface",
                       nextSection=section3)
  or <- .process_chunk(aux, section=section3,
                       nextSection="")
  or$section <- "Total"  # override it, so you don't have Start Date

  res <- cbind(auction="MonthlyBilateral", month=month,
    capabilityPeriod=CapabilityPeriod(month),
    isBilateral=TRUE, bilateralPeriod=NA,            
    rbind(cz, ei, or), filename=filename)
  
  res
}




################################################################
################################################################
# 
.test_FCM <- function()
{
  require(CEGbase)
  require(reshape)
  require(lattice)
  require(xlsx)

  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Capacity/lib.capacity.auctions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.downloads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FCM.R")

  #BASE_URL <<- "http://www.iso-ne.com/markets/othrmkts_data/fcm/cal_results/"
  #DIR_OUT  <<- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FCM/Auctions/Raw/"

  yearStart <- as.numeric(format(Sys.Date(), "%Y"))-1
  ccp <- seq(yearStart, yearStart+6, by=1)
  for (year in ccp) {
    rLog("Working on auction starting", year)
    download_all_FCM_files(year)
  }



  # archive them
  update_archive_RData()


  FILE_ARCHIVE <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/",
    "fcm_summary.RData", sep="")
  load(FILE_ARCHIVE)

  
}
