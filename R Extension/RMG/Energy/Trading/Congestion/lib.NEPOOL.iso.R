################################################################################
# External Libraries
#
# Transmission projects listing
# http://www.iso-ne.com/committees/comm_wkgrps/prtcpnts_comm/pac/projects/
# username: sys_studies, password: min98ute
#
# Excellent ISO presentations on transmission issues:
# I've put them all at: S:\All\Structured Risk\NEPOOL\Nepool System Studies\
# http://www.iso-ne.com/trans/sys_studies/rsp_docs/pres/2007/ (and other years)
#
# COO report from 
# http://www.iso-ne.com/committees/comm_wkgrps/prtcpnts_comm/prtcpnts/mtrls/2010/jan82010/index.html
#
# Get generation queue from here:
# http://www.iso-ne.com/genrtion_resrcs/nwgen_inter/status/
#
# Get the Congestion Revenue Summary.  2009 FTR Monthly Summary. 
# http://www.iso-ne.com/markets/othrmkts_data/conrev_summ/index.html
#
# .downloadGenericInterfaceConstraints
# .make.FTR.transmission.outages.RData()  - read the pdfs and make an RData file
# .make.LT.FTR.auction.bids.RData()  
# customer_and_asset_info
#
## URL = Structure( "Host",        "misftp.iso-ne.com",
##                  "UserName",    "600035611",
##                  "Password",    "sibt0aau",
##                );



################################################################################
# File Namespace, constants
#
NEPOOL <- new.env(hash=TRUE)
NEPOOL$options$bid.file.dir <- paste("S:/All/Structured Risk/NEPOOL/FTRs/",
                                     "Auction Data - Bids & Results/", sep="")
NEPOOL$options$awards.dir   <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/"

NEPOOL$mkt    <- "PWX"
NEPOOL$region <- "NEPOOL"
NEPOOL$hist.start.dt <- as.POSIXlt("2003-11-01 00:00:00")   # start with this winter
NEPOOL$analysis.start.dt <- as.Date("2006-01-01")

################################################################################
################################################################################
# Run this every month
#
.main <- function()
{
  require(CEGbase)
  require(FTR)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.iso.R")
  
  FTR.load.ISO.env("NEPOOL")

  NEPOOL$.makeMAP()                  # add the latest pnode table, SecDb nodes
  NEPOOL$.makeSecDbDelivPt.MAP()     # for marking the curves, parent/child 

  NEPOOL$customer_and_asset_info()   # update the customer & asset info  

  
}



########################################################################
# NOTE: some of the delivPt may no longer be available for trading.
# however some old positions may still remain. 
#
NEPOOL$.makeSecDbDelivPt.MAP <- function()
{
  # add the location name, NEPOOL
  aux <- data.frame(parentDelivPt=toupper(unlist(secdb.getValueType(
    "Bal Close PWR NEPOOL PM", "Relative Flag Codes"))),
    delivPt=toupper(unlist(secdb.getValueType(
    "Bal Close PWR NEPOOL PM", "Locations"))))
  aux <- cbind(mkt="PWX", aux)
  res <- aux
  
  # add the MCC location name, NEPOOL
  aux <- data.frame(parentDelivPt=toupper(unlist(secdb.getValueType(
    "Bal Close PWR NEPOOL MCC", "Relative Flag Codes"))),
    delivPt=toupper(unlist(secdb.getValueType(
    "Bal Close PWR NEPOOL MCC", "Locations"))))
  aux <- cbind(mkt="PWX", aux)
  res <- rbind(res, aux)
  
  # add the location name, NYPP
  aux <- data.frame(parentDelivPt=toupper(unlist(secdb.getValueType(
    "Bal Close PWR NYPP PM", "Relative Flag Codes"))),
    delivPt=toupper(unlist(secdb.getValueType(
    "Bal Close PWR NYPP PM", "Locations"))))
  aux <- cbind(mkt="PWY", aux)
  res <- rbind(res, aux)
  
  # add the MCC location name, NYPP
  aux <- data.frame(parentDelivPt=toupper(unlist(secdb.getValueType(
    "Bal Close PWR NYPP MCC", "Relative Flag Codes"))),
    delivPt=toupper(unlist(secdb.getValueType(
    "Bal Close PWR NYPP MCC", "Locations"))))
  aux <- cbind(mkt="PWY", aux)
  res <- rbind(res, aux)

  # map the empty parents to themselves.
  ind <- which(res$parentDelivPt=="")
  res[ind, "parentDelivPt"] <- res[ind, "delivPt"]

  res <- unique(res)  # there are some duplicates!

  
  MAP_DP <- res  # map delivery points
  comment(MAP_DP) <- paste("Map of SecDb delivery points.  Last update",
                          Sys.Date())

#  fname <- "H:/user/R/RMG/Utilities/Interfaces/PM/data/MAP_DP.RData"
  fname <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/MAP_DP.RData"
  save(MAP_DP, file=fname)
  rLog("Wrote file:", fname)
}


################################################################################
# Make tsdb price summaries with realized congestion
# TODO: There are two warnings I need to investigate.
#   probably with first cbind.
# Where: nodes    - a vector of ptids
#        start.dt - a POSIXct date
#
NEPOOL$.makePriceSummaries <- function(nodes, start.dt=NULL, save=FALSE)
{ 
  tsdb.symbols <- paste("NEPOOL_SMD_DA_", nodes, "_CongComp", sep="")

  if (is.null(start.dt)){
    start.dt <- NEPOOL$hist.start.dt
  } else {
    start.dt <- as.POSIXct(paste(start.dt, "00:00:00")) 
  }
  end.dt   <- as.POSIXct(paste(Sys.Date()-1, "23:00:00")) 
  
  ind.ONPEAK  <- secdb.getHoursInBucket("NEPOOL", "peak", start.dt, end.dt)
  ind.OFFPEAK <- secdb.getHoursInBucket("NEPOOL", "offpeak", start.dt, end.dt)
  
  rLog("Read prices from tsdb")
  hP <- list(ONPEAK=NULL, OFFPEAK=NULL)
  packets  <- trunc(length(nodes)/100) + 1
  for (p in 1:packets){
    rLog(paste("Working on packet", p, "of", packets))
    ind <- ((p-1)*100+1) : min(p*100, length(nodes))
    symbols <- tsdb.symbols[ind]
    hPP <- FTR.load.tsdb.symbols(symbols, start.dt, end.dt)
    colnames(hPP) <- gsub("_CongComp", "",
                          gsub("NEPOOL_SMD_DA_", "", colnames(hPP)))

    ind.NA <- apply(is.na(hPP), 2, sum)   # check for missing values!
    if (any(ind.NA > 0)){
      missing <- c(missing, ind.NA[which(ind.NA>0)])
      print(ind.NA[which(ind.NA>0)])
    }
    
    aux <- hPP[as.character(ind.ONPEAK$time),]
    aux <- aggregate(aux, format(index(aux), "%Y-%m-%d"), mean )
    if (p==1){
      hP$ONPEAK <- aux
    } else {hP$ONPEAK <- cbind(hP$ONPEAK, aux)}
    
    aux <- hPP[as.character(ind.OFFPEAK$time),]
    aux <- aggregate(aux, format(index(aux), "%Y-%m-%d"), mean )
    if (p==1){
      hP$OFFPEAK <- aux 
    } else {hP$OFFPEAK <- cbind(hP$OFFPEAK, aux)}
  }
  index(hP$ONPEAK)    <- as.Date(index(hP$ONPEAK))
  colnames(hP$ONPEAK) <- paste("ONPEAK.", colnames(hP$ONPEAK), sep="")
  
  index(hP$OFFPEAK) <- as.Date(index(hP$OFFPEAK))
  colnames(hP$OFFPEAK) <- paste("OFFPEAK.", colnames(hP$OFFPEAK), sep="")
  
  hP <- cbind(hP[["ONPEAK"]], hP[["OFFPEAK"]])

  if (save){
    filename <- paste(NEPOOL$options$awards.dir, "RData/hPrices.RData", sep="")
    save(hP, file=filename)
  } else {
    return(hP)
  }
}

################################################################################
# Read the ISO awards
#
NEPOOL$.loadAwards <- function()
{
  rLog("Loading historical awards")
  filename <- paste(NEPOOL$options$awards.dir, "RData/ftrslt.RData", sep="")
  load(filename)
  rLog("Done.")
  return(AWD)
}

################################################################################
# Read the ISO clearing price file
#
NEPOOL$.loadClearingPrice <- function()
{
  rLog("Loading historical clearing prices")
  filename <- paste(NEPOOL$options$awards.dir, "RData/ftrcp.RData", sep="")
  load(filename)
  rLog("Done.")
  return(CP)
}

################################################################################
# Read the settled prices file
#
NEPOOL$.loadSettlePrice <- function()
{
  rLog("Loading historical clearing prices")
  filename <- paste(NEPOOL$options$awards.dir, "RData/ftrsp.RData", sep="")
  load(filename)
  rLog("Done.")
  return(SP)
}

################################################################################
# Read the congestion settle prices aggregated daily, peak and offpeak
#
NEPOOL$.loadHistoricalPrice <- function()
{
  rLog("Loading historical realized congestion prices")
  filename <- paste(NEPOOL$options$awards.dir, "RData/hPrices.RData", sep="")
  load(filename)
  rLog("Done.")
  return(hP)
}

################################################################################
# Load the NEPOOL MAP
#
NEPOOL$.loadMAP <- function()
{
  rLog("Loading the MAP")
  filename <- paste(NEPOOL$options$awards.dir, "RData/map.RData", sep="")
  load(filename)
  rLog("Done.")
  return(MAP)
}

################################################################################
# Load the NEPOOL MAP
#
NEPOOL$.loadMAP_DP <- function()
{
  rLog("Loading the MAP_DP data")
  filename <- paste(NEPOOL$options$awards.dir, "RData/MAP_DP.RData", sep="")
  load(filename)
  rLog("Done.")
  return(MAP_DP)
}


################################################################################
# Load the NEPOOL LT_FTR_BIDS
#
NEPOOL$.loadLT_FTR_BIDS <- function()
{
  rLog("Loading LT_FTR_BIDS")
  filename <- paste(NEPOOL$options$awards.dir, "RData/LT_FTR_BIDS.RData", sep="")
  load(filename)
  rLog("Done.")
  return(LT_FTR_BIDS)
}





################################################################################
# Make the Settle Price archive.
# Returns a data.frame with columns: ptid, month, class.type, settle.price
#
NEPOOL$.makeSettlePriceArchive <- function()
{
  aux <- aggregate(NEPOOL$hP, format(as.Date(index(NEPOOL$hP)),
                                            "%Y-%m-01"), mean, na.rm=TRUE)
  aux <- melt(as.matrix(aux))
  bux <- strsplit(as.character(aux[,2]), "\\.")
  aux$class.type <- sapply(bux, function(x){x[1]})
  aux$ptid <- as.numeric(sapply(bux, function(x){x[2]}))
  aux <- aux[,-2]
  
  names(aux)[1:2] <- c("month", "settle.price")
  SP <- na.omit(aux)
  SP$month <- as.Date(as.character(SP$month))
  
  filename <- paste(dir.csv, "RData/ftrsp.RData", sep="")
  save(SP, file=filename)
}


################################################################################
# Make the NEPOOL MAP, read from SecDB the report manager, clean it a bit, 
# and intersect with the latest pnode table from NEPOOL.
# Get this from http://www.iso-ne.com/stlmnts/stlmnt_mod_info/index.html
# EPTF = pool transmission facility, non-PTF = non pool transmission facility. 
NEPOOL$.makeMAP <- function()
{
  rootdir  <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/"
  DIR <- paste(rootdir, "PnodeTable/Raw/", sep="")
  lfiles   <- sort(list.files(DIR, pattern="pnode_table"), decreasing=T)
  filename <- paste(DIR, lfiles[1], sep="")  # use the last one!
  rLog("Loading the node map from:", filename)
  con      <- odbcConnectExcel(filename)
  tables   <- sqlTables(con) 
  data     <- sqlFetch(con, "New England")
  odbcCloseAll()

  # remove the rows with all NA's
  ind <- !apply(is.na(data), 1, all)  # this gives a warning in R 2.8.1
  data <- data[ind,]
  data <- data[,c("Node/Unit ID", "RSP Area", "Node Name", "Zone ID",
                  "Dispatch Zone", "PTF Status")]
  names(data) <- c("ptid", "area", "iso.name", "zone.id",
                   "dispatch.zone", "PTF.status")
  data$area       <- as.character(data$area)
  data$iso.name   <- as.character(data$iso.name)
  data$in.pnode.table <- TRUE
  
  MAP <- FTR:::FTR.ReportManager("NEPOOL")
  MAP <- merge(MAP, data, all.x=TRUE)
  MAP$secdb.name <- MAP$name                # save this for later
  
  # fix the SecDB names according to the latest pnode table!
  ind <- which(MAP$name != MAP$iso.name)
  if (length(ind)>0) MAP$name[ind] <- MAP$iso.name[ind]
  MAP$iso.name <- NULL                      # remove this
  

  # fix the SecDB zone names according to the latest pnode table!
  ind <- which(MAP$zone != as.character(MAP$zone.id))
  if (length(ind)>0) MAP$zone[ind] <- as.character(MAP$zone.id[ind])
  
  ind <- grep("^HC\\.", MAP$name)
  MAP$area[ind] <- "HUB"

  # add the map of ptids to utilities
  aux <- read.csv(paste(rootdir, "ptidToUtilityName.csv", sep=""),
                  comment.char="#")
  aux$utility <- gsub(" *$", "", aux$utility)
  aux$ptid <- as.numeric(aux$ptid)
  MAP <- merge(MAP, aux[,c("ptid", "utility")], by="ptid", all.x=TRUE)

  # add the MCC location name
  aux <- data.frame( location=toupper(unlist(secdb.getValueType(
    "Bal Close PWR NEPOOL MCC","Relative Flag Codes"))),
    mcc.deliv.pt=toupper(unlist(secdb.getValueType(
    "Bal Close PWR NEPOOL MCC", "Locations"))))
  aux$location <- gsub(" DA$", "", aux$location)
  MAP <- merge(MAP, aux, all.x=TRUE)
  MAP <- MAP[order(MAP$ptid),]

  rLog("Saving the NEPOOL map to the DB RData folder")
  filename <- paste(rootdir, "RData/map.RData", sep="")
  save(MAP, file=filename)
  rLog("Done.")
}

################################################################################
# Check if a set of pathts is allowed (the nodes are in the latest pnode table)
# Where exclude can be a vector of ptids
#
NEPOOL$in.PNODE.table <- function(paths, ISO.env, exclude=NULL)
{
  aux <- unique(paths[,c("path.no", "source.ptid", "sink.ptid")])
  aux <- melt(aux, id=1)
  colnames(aux)[3] <- "ptid"
  aux$ptid <- as.numeric(aux$ptid)

  aux <- merge(aux, ISO.env$MAP[,c("ptid", "in.pnode.table")], all.x=TRUE)
  ind.out <- which(!aux$in.pnode.table)
  if (length(ind.out)>0){
    rLog("These paths are not valid and will be removed:")
    paths.no.out <- sort(unique(aux$path.no[ind.out]))
    rLog(paste(paths.no.out))
    paths <- subset(paths, !(path.no %in% paths.no.out))
  } else {
    rLog("All nodes are in pnode table.")
  }

  if (!is.null(exclude)){
    ind.out <- sort(unique(subset(aux, ptid %in% exclude)$path.no))
    paths <- subset(paths, !(path.no %in% ind.out))
  }
  return(paths)
}

##########################################################################
# Make the binding constraints map.  Read the table Zach has prepared. 
# 
NEPOOL$.make.BC.MAP <- function()
{
  rLog("Reading the binding constraints map ...")
  con  <- odbcConnectExcel("S:/All/Risk/Data/FTRs/NEPOOL/Constraint Key.xls")
  data <- sqlFetch(con, "ConstraintPivot")
  odbcCloseAll()
  data <- data[,c(1,3)]
  data <- data.frame(sapply(data, as.character))
  colnames(data) <- tolower(colnames(data))
  rLog("Done")

  return(data)
}


##########################################################################
# Read the UPLAN interface definition
# 
NEPOOL$.get.INTERFACES <- function()
{
  rLog("Reading the UPLAN interfaces definition ...")
  INT <- read.csv("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/map.interfaces.csv")
  BUS <- read.csv("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/map.bus_pnode.csv")
  BUS <- BUS[order(BUS$Bus.ID),]  
  rownames(BUS) <- NULL
  
  LN  <- read.csv("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/map.lines_buses.csv")
  LN  <- LN[, c("Name", "Start.Bus", "End.Bus")]
  LN  <- subset(LN, Start.Bus != End.Bus)
  
  # map the source bus
  aux <- BUS
  names(aux)[which(names(aux) == "Bus.Name")]   <- "FromBusName"
  names(aux)[which(names(aux) == "Pnode.ID")] <- "source.ptid"
  INT <- merge(INT, aux[,c("FromBusName", "source.ptid")], all.x=T)
  # map the sink bus
  aux <- BUS
  names(aux)[which(names(aux) == "Bus.Name")]   <- "ToBusName"
  names(aux)[which(names(aux) == "Pnode.ID")] <- "sink.ptid"
  INT <- merge(INT, aux[,c("ToBusName", "sink.ptid")], all.x=T)

  INT <- INT[,c("Interface.Name", "Direction", "FromBusName", "ToBusName",
                "source.ptid", "sink.ptid")]
  INT <- INT[order(INT$Interface.Name), ]    # some have NA ptids

  # remove the interfaces that are missing both the source & sink
  ind <- which(is.na(INT$sink.ptid) & is.na(INT$source.ptid))
  INT <- INT[-ind, ]
  
  # add next sink.ptid on the line ... 
  aux  <- INT[is.na(INT$sink.ptid), ]   # missing sinks
  bux  <- LN; names(bux)[2:3] <- c("ToBusName", "ToBusName2")
  aux  <- merge(aux, bux[,c("ToBusName", "ToBusName2")], all.x=T)
  aux  <- aux[!is.na(aux$ToBusName2),]
  aux$sink.ptid <- NULL 
  bux  <- BUS
  names(bux)[which(names(bux) == "Bus.Name")]  <- "ToBusName2"
  names(bux)[which(names(bux) == "Pnode.ID")]  <- "sink.ptid"
  aux <- merge(aux, bux[,c("ToBusName2", "sink.ptid")], all.x=T)
  aux <- aux[,c("Interface.Name", "Direction","FromBusName",
                "ToBusName2", "source.ptid", "sink.ptid")]
  names(aux)[4] <- "ToBusName"
  aux <- aux[!is.na(aux$sink.ptid), ]
  aux <- unique(aux)
  aux <- subset(aux, source.ptid != sink.ptid)
  INT <- INT[!is.na(INT$sink.ptid), ]  # remove them
  INT <- rbind(INT, aux)               # add the new ones
 
  
  # add next source.ptid on the line ... 
  aux  <- INT[is.na(INT$source.ptid), ]   # missing sinks
  bux  <- LN; names(bux)[2:3] <- c("FromBusName", "FromBusName2")
  aux  <- merge(aux, bux[,c("FromBusName", "FromBusName2")], all.x=T)
  aux  <- aux[!is.na(aux$FromBusName2),]
  aux$source.ptid <- NULL 
  bux  <- BUS
  names(bux)[which(names(bux) == "Bus.Name")]  <- "FromBusName2"
  names(bux)[which(names(bux) == "Pnode.ID")]  <- "source.ptid"
  aux <- merge(aux, bux[,c("FromBusName2", "source.ptid")], all.x=T)
  aux <- aux[,c("Interface.Name", "Direction","FromBusName2",
                "ToBusName", "source.ptid", "sink.ptid")]
  names(aux)[3] <- "FromBusName"
  aux <- aux[!is.na(aux$source.ptid), ]
  aux <- unique(aux)
  aux <- subset(aux, source.ptid != sink.ptid)
  INT <- INT[!is.na(INT$source.ptid), ]  # remove them
  INT <- rbind(INT, aux)                 # add the new ones 

  INT <- subset(INT, source.ptid != sink.ptid)
  INT <- INT[order(INT$Interface.Name), ]
  rownames(INT) <- NULL

  rLog("Done")

  # I think this got saved as RData/INTERFACES.RData
  
  return(INT)
}


##########################################################################
# Download this info from NEPOOL web site and create an RData set
# 
NEPOOL$.downloadGenericInterfaceConstraints <- function()
{
  #rootURL  <- "http://www.iso-ne.com/markets/hrly_data/support_docs/"
  rootURL <- "http://www.iso-ne.com/static-assets/documents/markets/hrly_data/support_docs/"
  rootDir  <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/SupportDocuments/"
  
  fileName <- "generic_interface_constraints.xlsx"
  linkURL  <- paste(rootURL, fileName, sep="")
  file1 <- paste(rootDir, Sys.Date(), "_", fileName, sep="")  
  download.file(linkURL, file1, quiet=TRUE, mode="wb") 

  fileName <- "contingency_definitions.xlsx"
  linkURL  <- paste(rootURL, fileName, sep="")
  file2 <- paste(rootDir, Sys.Date(), "_", fileName, sep="")  
  download.file(linkURL, file2, quiet=TRUE, mode="wb") 
  
  fileName <- "line_and_zbr_definitions.xlsx"
  linkURL  <- paste(rootURL, fileName, sep="")
  file3 <- paste(rootDir, Sys.Date(), "_", fileName, sep="")  
  download.file(linkURL, file3, quiet=TRUE, mode="wb") 

  #require(xlsReadWrite)
  require(xlsx)
  # save the interfaces  ---------------------------------------------
  DD <- read.xlsx2(file1, sheetIndex=1, startRow=3, header=TRUE)
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
}



##########################################################################
# Download this info from NEPOOL web site and create RData sets
# Also Ted has older versions in folder S:\Data\NEPOOLrealTime\WebPages.
# useful to see if there are some customers no longer active ...
#
NEPOOL$customer_and_asset_info <- function()
{
  # download the latest file
  # url <- "http://www.iso-ne.com/support/asset_info/customer_and_asset_information.xls"
  # url <- "http://www.iso-ne.com/support/asset_info/p_s_monthly_lists_new.xls"
  url <- paste("http://www.iso-ne.com/static-assets/documents/",
    format(prevMonth(), "%Y/%m"), "/p_s_monthly_list_new.xls", sep="")
  DIROUT <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/AssetInfo/Raw/"
  fileout <- paste(DIROUT, format(Sys.Date()), "_", basename(url), sep="")

  res <- download.file(url, fileout, mode="wb")  
  if (res == 0) {
    rLog("Downloaded ", basename(url), "successfully!")
  } else {
    stop("FAILED download of ", url)
  }

  # make the RData sets
  require(xlsx)
  DIR_RDATA <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/"
  
  LL <- read.xlsx2(fileout, sheetName="Loads", startRow=3)
  LL[,c(1,3)] <- sapply(LL[,c(1,3)], as.numeric)
  loadAssets <- LL[,1:4]
  loadAssets <- unique(loadAssets)  # not sure why they are not unique!

  fileout <- paste(DIR_RDATA, "loadAssets.RData", sep="")
  save(loadAssets, file=fileout)
  rLog("Saved ", fileout)  
}



#########################################################################
#########################################################################
# Moved this section to FTR.load.ISO.env ... 
#NEPOOL$MAP      <- NEPOOL$.loadMAP()
#NEPOOL$HRS      <- FTR.bucketHours("NEPOOL", end.dt=as.POSIXlt("2017-12-31 23:00:00"))
#NEPOOL$HRS.AUCT <- FTR.countHoursAuction("NEPOOL")  
#NEPOOL$AWD      <- NEPOOL$.loadAwards()       
#NEPOOL$CP       <- NEPOOL$.loadClearingPrice()
#NEPOOL$hP       <- NEPOOL$.loadHistoricalPrice()
#NEPOOL$SP       <- NEPOOL$.loadSettlePrice()
#NEPOOL$MAP.BC   <- NEPOOL$.make.BC.MAP()

nfind <- function( x ) {
  if (class(x) == "numeric") {
    return( subset(NEPOOL$MAP, ptid == x))
  } else { 
    return( NEPOOL$MAP[grep(toupper(x), NEPOOL$MAP$name),] )
  }
}




















###################################################################################
###################################################################################
#                                OLD/UNUSED STUFF BELOW
###################################################################################

##   # expand graphs    # DON'T DO IT HERE
##   X <- res
##   names(X)[2:3] <- c("parent", "child")
##   EX <- expandGraphs(X[,2:3])

##   names(X)[2:3] <- c("level1", "level0")  
##   mapDP <- merge(X, EX)
##   mapDP <- mapDP[,rev(order(names(mapDP)))] 
  

## ##########################################################################
## # Do the folders automatically...
## # 
## NEPOOL$.makeMonthFolders <- function(month)
## {
##   root <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/"
##   mon  <- format(month, "%Y-%m")
##   root <- paste(root, mon, sep="")

##   my.mkdir <- function(dir, root){
##     if (file.access(paste(root, dir, sep=""))!=0){
##     dir.create(paste(root, dir, sep=""))
##    }
##   }
##   dir.list <- c("", "/Plots", "/Constraints", "/History", "/Reports", "/Reports/Awards",
##                 "/Reports/Bids")
##   for (i in seq_along(dir.list)){
##     my.mkdir(dir.list[i], root)
##   }
## }
