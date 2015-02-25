# Deal with transmission outages, etc. 
#
# .add_line_name - a helper function
# .add_special_lines   - add a few ones that I think are important
# .calculate_interface_slack  
# .download_long_term_transmission_outages
# .download_interface_definition
# .get_outages_on_interfaces  - does the merge with the Interface definition
# .find_new interfaces        - have new interfaces been added to the list?
# .outages_byDay_toText    - make it ready for displaying (long term outages)
# .outages_byHour_toText   - short term outages
# .read_outage_file        - for one day
# .update_rdata_interfaces_file 
# search_st_outages - search the zip archives for patterns


############################################################################
# Add interface slacks 
# 
# 
# 
.add_interface_slack <- function(outEx)
{
  interfaceNames <- sort(unique( outEx$name ))
  name <- interfaceNames
  
  # get rid of the (AAD watch) etc.
  #interfaceNames <- gsub("\\(.*\\)", "", interfaceNames)
  interfaceNames <- gsub("((\\w|-)*).*", "\\1", interfaceNames)

  # get rid of the _, -, " " which are not part of the tsdb symbol
  interfaceNames <- gsub("(_|-| )", "", interfaceNames)
  

  slack <- .calculate_interface_slack( interfaceNames )

  aux <- merge(data.frame(name=name, interfaceNames=interfaceNames),
               slack)

  outEx <- merge(outEx, aux[, c("name", "slack")], all.x=TRUE)

  
  outEx
}



############################################################################
# Add line names to the outage files (short & long term)
# needed so you can merge with the Interface definition data.frame
# You go from equipment description e.g. "377: SUROWIEC" to line = "377"
# 
.add_line_name <- function(lineOO)
{
  #Add the line name from the equipment.description
  aux <- strsplit(lineOO$equipment.description, ":")
  lineOO$line <- sapply(aux, "[[", 1)
  # sometimes they put a description "WOODSTCK 221 to RUMFD_IP",
  # "SEABROOK 394-1 to W_AMESBY", "ROOD_AVE 1448 to MANCHSTR"
  # deal with this separately!
  ind <- grep(" to ", lineOO$line)
  if (length(ind)>0){
    lineOO$line[ind] <- gsub(".* (.*) to .*", "\\1", lineOO$line[ind])
  }

  return(lineOO)
}


############################################################################
# Add a few lines that are not on interfaces, but important
# 
#
.add_special_lines <- function(INTF)
{
  fname <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Outages/Extra/extra_lines_monitored.xlsx"
  extras <- read.xlsx2(fname, sheetName="Extras")
  colnames(extras)[1] <- "short name"
  extras$name <- extras$"short name"
  
  INTF <- rbind(INTF, extras[, c("short name", "name", "line", "derate")])

  
  INTF
}


############################################################################
# Use the last available data to calculate the interface slack (interface limit - flow)
# @param interfaceNames a vector of names
#
.calculate_interface_slack <- function( interfaceNames )
{
  startDt <- as.POSIXct(paste(Sys.Date()-1, "01:00:00"))
  endDt <- as.POSIXct(paste(Sys.Date()+2, "00:00:00"))

  suppressWarnings(res <- lapply(interfaceNames, function(iname){
    symbs <- c(paste("nepl_da_inter_", iname, "_limit", sep=""),
               paste("nepl_da_inter_", iname, "_mw", sep=""))
    limit <- tsdb.readCurve(symbs[1], startDt, endDt)
    mw    <- tsdb.readCurve(symbs[2], startDt, endDt)
    
    return(min(limit$value - mw$value))  
  }))

  slack <- unlist(res)
  slack <- ifelse (slack > 50000, Inf, slack)   # ignore if too big, ISO not enforcing it
  
  data.frame(interfaceNames = interfaceNames,
             slack = slack) 
}


############################################################################
# Compare two ST Outages files 
#
.download_long_term_transmission_outages <- function(asOfDate=Sys.Date(), save=TRUE, 
  dirout="S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Outages/LongTerm/")
{
  #con <- url("http://www.iso-ne.com/trans/ops/outages/long_term/generateCurrentLtorCSV.action")
  con <- url(paste("http://www.iso-ne.com/transform/csv/outages?outageType=long-term&start=",
             format(asOfDate, "%Y%m%d"), sep=""))
  res <- try(open(con), silent=T)
  if (class(res)[1] == "try-error"){
    rLog("Cannot open url.  Exiting.")
    return(1)
  }
  on.exit(close(con))
  res <- NULL
  res <- try(read.csv(con, sep=",", skip=4))
  if (class(res)[1]=="error"){
    rLog("Cannot close url.  Exiting.")   
    return(1)
  }
  res <- res[-nrow(res), ]
  res <- res[-1,]
  colnames(res) <- tolower(colnames(res))

  res$actual.start <- as.Date(strptime(res$actual.start,
     "%m/%d/%Y %H:%M:%S", tz="EST5EDT"))
  res$actual.end <- as.Date(strptime(res$actual.end,
     "%m/%d/%Y %H:%M:%S", tz="EST5EDT"))
  res$planned.start <- as.Date(strptime(res$planned.start,
     "%m/%d/%Y %H:%M:%S", tz="EST5EDT"))
  res$planned.end <- as.Date(strptime(res$planned.end,
     "%m/%d/%Y %H:%M:%S", tz="EST5EDT"))

  if (save){
    fileout <- paste(dirout, "lt.outages.", format(Sys.time(), "%Y-%m-%d"),
      ".csv", sep="")
    write.csv(res, file=fileout, row.names=FALSE)
    rLog("Wrote LT outages to", fileout)
  }


  return(res)
}


############################################################################
# Download the current interface definition 
# 
.download_interface_definition <- function()
{
  rootURL <- "http://www.iso-ne.com/static-assets/documents/markets/hrly_data/support_docs/"
  rootDir  <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/SupportDocuments/Interfaces/"
  
  fileName <- "generic_interface_constraints.xlsx"
  linkURL  <- paste(rootURL, fileName, sep="")
  file1 <- paste(rootDir, Sys.Date(), "_", fileName, sep="")  
  download.file(linkURL, file1, quiet=TRUE, mode="wb") 

  return(file1)
}


############################################################################
# frequency can be "hour" or "day"
#
.get_outages_on_interfaces <- function(lineOO, frequency="hour")
{
  library(xlsx)
  
  if (!(frequency %in% c("hour", "day")))
    stop("Wrong frequency for the outage report")

  # monitor P1/P2
  ind <- grep("P1_P2_T", lineOO$station)
  if (length(ind) > 0) 
    lineOO[ind,"line"] <- "P1_P2_T"
  
  
  # load latest interface definition done with
  # NEPOOL$.downloadGenericInterfaceConstraints
  fileIn <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/",
                   "RData/INTERFACES.RData", sep="")
  load(fileIn)  # loads data.frame INTF
  colnames(INTF) <- tolower(colnames(INTF))

  # add known derates
  fname <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Outages/Extra/extra_lines_monitored.xlsx"
  derates <- read.xlsx2(fname, sheetName="Derates")
  INTF <- merge(INTF, derates, all=TRUE)
  INTF$derate[is.na(INTF$derate)] <- ""
  
  
  # add the lines I found important 
  INTF <- .add_special_lines(INTF)
  
  DD <- merge(lineOO, INTF, by="line", all.x=TRUE)
  DD <- DD[!is.na(DD$name),]  
  sDD <- split(DD, DD$`short name`)
  
  # expand the outages by hours/days
  outEx <- vector("list", length(sDD))
  for (i in names(sDD)){       # loop over interfaces
    rLog("Working on ", i)
    aux <- sDD[[i]]

    auxEx <- vector("list", length=nrow(aux))
    for (r in 1:nrow(aux)){   # loop over line out
      startTime <- aux[r, "planned.start"]
      if (!is.na(aux[r, "actual.start"]))
        startTime <- aux[r, "actual.start"]
      endTime <- aux[r, "planned.end"]
      if (!is.na(aux[r, "actual.end"]))
        endTime <- aux[r, "actual.end"]
      if (endTime < startTime )
        endTime <- startTime      # sometimes data is inconsistent!
    
      auxEx[[r]] <- data.frame(
        equipment.description=aux[r, "equipment.description"],
        period=seq(startTime, endTime, by=paste("1", frequency)),
        voltage=aux[r, "voltage"],
        name=aux[r, "short name"],
        derate=aux[r, "derate"])
    }
    auxEx <- do.call(rbind, auxEx)
    outEx[[i]] <- auxEx
  }
  outEx <- do.call(rbind, outEx)
  rownames(outEx) <- NULL
  
  return(outEx)  
  
}


############################################################################
# Compare the last two files for new entries
# @return a vector of Strings with the short names of the new interfaces or NULL
#    if no new interface is found
#
.find_new_interfaces <- function()
{
  DIR  <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/SupportDocuments/Interfaces/"

  files <- list.files(DIR, full.names=TRUE)
  files <- tail(sort(files), 2)

  rLog("reading ", files[1])
  tb1 <- read.xlsx2(files[1], sheetIndex=1, startRow=3)
  tb1 <- tb1[nchar(tb1[,2]) > 0,]
  tb1 <- data.frame(day1=substring(basename(files[1]), 1, 10),
                    short.name=tb1[,"Short.Name"])
  
  rLog("reading ", files[2])
  tb2 <- read.xlsx2(files[2], sheetIndex=1, startRow=3)
  tb2 <- tb2[nchar(tb2[,2]) > 0,] 
  tb2 <- data.frame(day2=substring(basename(files[2]), 1, 10),
                    short.name=tb2[,"Short.Name"])

  tb <- merge(tb1, tb2, all=TRUE)

  ind <- which( is.na(tb[,"day1"]) )

  res <- NULL
  if (length(ind) > 0) 
     res <- tb[ind,"short.name"]
  
  
  res
}


############################################################################
# for long term outages.  For each day "." for outage, " " for nothing 
#
.outages_byDay_toText <- function(DF, startDate=NULL, endDate=NULL)
{
  DF$period <- as.Date(DF$period)
  #DF <- subset(DF, period >= startDate & period <= endDate)
  DF <- unique(DF[,c("equipment.description", "voltage", "name", "period")])
  DF <- cbind(DF, fill=".")

  TT <- cast(DF, equipment.description + voltage + name ~ period, I,
    fill=" ", value="fill")
  
  return(TT)

}


############################################################################
# for short term outages.  For each day "00-23" for duration of outage
# @param startTime
#
.outages_byHour_toText <- function(DF,
   startTime=as.POSIXct(paste(Sys.Date(), "00:00:00")),
   endTime=as.POSIXct(paste(Sys.Date(), "00:00:00")) + (7*24-1)*3600,
   style="range")
{
  DF <- subset(DF, period >= startTime & period <= endTime)
  
  days <- as.Date(seq(startTime, endTime, by="1 day"))
  TT <- data.frame(
    period=seq(startTime, endTime, by="1 hour"))
  if (nrow(TT) %% 24 != 0)
    stop(paste("No of hours between startTime and endTime",
               "is not a multiple of 24."))

  aux <- merge(DF, TT, all.y=TRUE)
  if (style=="dots"){                 # the else branch is better!
    aux$value <- 1
    aux <- cast(aux, name + equipment.description + voltage + derate + slack ~ period,
      length, fill=0)
    out <- t(apply(aux[,4:ncol(aux)], 1,
      function(x){
        x[x>=1] <- "."
        x[x==0] <- " "
        x <- matrix(x, ncol=24, byrow=TRUE)
        x <- apply(x, 1, paste, sep="", collapse="")
      }))
    if (ncol(out) != length(days))
      stop("Something is wrong!  Did you pass full days?!")
    colnames(out) <- as.character(days)
    out <- data.frame(aux[,1:3], out)
  } else {
   #aux <- merge(DF, TT, all.y=TRUE)
   aux$hour <- format(aux$period, "%H")
   aux$day  <- format(aux$period, "%Y-%m-%d")
   calcRange <- function(x){
     paste(range(x), sep="", collapse="-")}
   aux <- cast(aux, name + equipment.description + voltage + derate + slack ~ day,
      calcRange, fill="", value="hour")
   aux <- subset(aux, !is.na(aux$name)) # from the merge of DF and TT
   out <- data.frame(aux)
  }

  colnames(out) <- gsub("^X", "", colnames(out))
  
  return(out)
}


############################################################################
# Read line outages for one day
#
.read_outage_file <- function(day = Sys.Date())
{
  dirOut <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Outages/"
  setwd(dirOut)
  files  <- sort(list.files(dirOut, pattern=as.character(day), full.names=TRUE))

  lastFile <- files[length(files)]
  if (length(lastFile)==0) 
    stop("Cannot find an outage file for day ", day)

  rLog("Reading file:", lastFile)
  OO <- read.csv(lastFile)                      # all outages
  lineOO <- subset(OO, equipment.type=="Line")  # line outages
  if (nrow(lineOO)==0)
    return("Empty outage file!")

  # deal with different ISO format prior to 8/23/2014 
  ind <- grep("\\.date$", colnames(lineOO))
  if (length(ind) > 0)
    colnames(lineOO) <- gsub("\\.date$", "", colnames(lineOO))

  
  lineOO <- lineOO[,c("station", "equipment.description", "voltage",
    "planned.start", "planned.end", "actual.start",
    "actual.end", "status")]

  # replace MM:SS with 00:00 in the outage times.
  lineOO$planned.start <- gsub("[[:digit:]]{2}:[[:digit:]]{2}$",
    "00:00", lineOO$planned.start)
  lineOO$planned.end <- gsub("[[:digit:]]{2}:[[:digit:]]{2}$",
    "00:00", lineOO$planned.end)
  lineOO$actual.start <- gsub("[[:digit:]]{2}:[[:digit:]]{2}$",
    "00:00", lineOO$actual.start)

  # convert to POISXct
  lineOO$planned.start <- as.POSIXct(lineOO$planned.start,
                                          "%m/%d/%Y %H:%M:%S", tz="")
  lineOO$planned.end <- as.POSIXct(lineOO$planned.end,
                                          "%m/%d/%Y %H:%M:%S", tz="")
  lineOO$actual.start <- as.POSIXct(lineOO$actual.start,
                                          "%Y-%m-%d %H:%M:%S", tz="")
  
  lineOO <- .add_line_name(lineOO)
    
  lineOO
}



############################################################################
# Search zipped files for patterns
#
search_st_outages <- function(startDt=as.Date("2009-03-01"),
  endDt=Sys.Date(), equipmentDescription=".", station=".",
  voltage=".")
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Outages/"

  files <- list.files(DIR, pattern="gz$", full.names=FALSE)
  days  <- as.Date(gsub(".*st\\.outages\\.(.*) [[:digit:]]{2}\\.csv\\.gz",
                        "\\1", files))
  ind <- which(days >= startDt & days <= endDt)
  if (length(ind) == 0)
    stop("No outages found in the dates you requested!")

  files <- files[ind]
  
  OO <- NULL
  for (i in seq_along(files)){
    rLog("Reading ", files[i])    
    aux <- read.csv(paste(DIR, files[i], sep=""))
    if (equipmentDescription != ".") 
      aux <- aux[grepl(equipmentDescription, aux$equipment.description),]
    if (station != ".")
      aux <- aux[grepl(station, aux$station),]
    if (voltage != ".")
      aux <- aux[grepl(voltage, aux$voltage),]
      
    OO[[i]] <- aux
  }
  OO <- do.call(rbind, OO)
  
  OO
}


############################################################################
# Write the RData file
#
.update_rdata_interfaces_file <- function()
{
  library(xlsx)
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/SupportDocuments/Interfaces"
  fname <- list.files(DIR, full.names=TRUE)
  fname <- fname[length(fname)]  # pick the last one
  
  DD <- read.xlsx2(fname, sheetIndex=1, startRow=3, header=TRUE)
  DD <- DD[,1:6]
  DD <- DD[DD[,2]!="", ]
  #names(DD) <- DD[1,];  DD <- DD[-1,]

  # clean the less sign
  DD[,"Boundary.Lines.Equation"] <- gsub(" *<$", "", DD[,"Boundary.Lines.Equation"])
  aux <- strsplit(DD[,"Boundary.Lines.Equation"], " *\\+ *")
  names(aux) <- DD[,"Short.Name"]
  aux <- melt(aux); names(aux) <- c("Line", "Short.Name")
  INTF  <- merge(DD[,c("Short.Name", "Name")], aux, by="Short.Name")
  colnames(INTF)[1] <- "Short Name"
  
  fileOut <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/",
                   "RData/INTERFACES.RData", sep="")
  save(INTF, file=fileOut)
  rLog("Saved ", fileOut)
## > head(INTF)
##   Short Name                       Name        Line
## 1     BEL1-2   Bellingham Units 1 and 2 Bell_303 1X
## 2     BEL1-2   Bellingham Units 1 and 2 Bell_303 2X
## 3       BERK Berkshire Power Generation Berk_Pwr 1X
## 4     BH-EXP        Bangor Hydro Export         203
## 5     BH-EXP        Bangor Hydro Export          86
## 6     BH-EXP        Bangor Hydro Export         254



}



############################################################################
############################################################################
# 
#
.test <- function()
{
  require(CEGbase)
  require(reshape)
  library(xlsx)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.transmission.R")

  # look at the short term outages for a previous day!
  day <- as.Date("2014-05-29")
  #day <- Sys.Date()
  lineOO <- .read_outage_file(day = day)
  outEx  <- .get_outages_on_interfaces(lineOO, frequency="hour")
  out <- .outages_byHour_toText(outEx, startTime = as.POSIXct(paste(day, "01:00:00")),
                                endTime = as.POSIXct(paste(day+8, "00:00:00")))
  out 

  

  
  res <- search_st_outages()

  startDt <- as.Date("2012-03-02")
  endDt   <- as.Date("2012-03-03")
  aux <- search_st_outages(startDt, endDt)
  subset(aux, company.1 == "NSTAR" & grepl("03/03/2012", aux$planned.start.date))
  subset(aux, grepl("03/15/2012", aux$planned.start.date))
  


  
  
  
}




