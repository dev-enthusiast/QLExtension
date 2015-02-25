# Steps necessary to update the calibrations for the nodes/zones marks
# 1) runs the matlab file: markNepoolSpread('R')
#
# 2) run the .refreshPMView function below
#
# You can have custom functions for a node see .makePMview.1216 for an 
# example.  Otherwise you get the default treatment.
#
# .add_to_pmView_file  - add a calibration to the pmView file
# .refreshPMView
# .rm_from_pmView_file - remove a node from the pmView file
#
# fwd.PMview.default
# transformerFun.12163 - mark GRAHAM  46  OLDT as child of GRAHAM.MIS

#########################################################################
# If you have a pmView that you calculated, save it in the archive.
# 
#
.add_to_pmView_file <- function(deliv.pt, pmView1, comments="",
   transformerName=NULL)
{
  rLog("DEPRECATED!  Use update_nodalview_archive from NEPOO/Marks/lib.mark.nodes.SecDb.R")
  
  if (is.null(pmView1) && is.null(transformerName))
    stop("pmView1 and transformerName cannot be both NULL!")
  
  if (!is.null(pmView1) && !is.null(transformerName))
    stop("pmView1 and transformerName cannot be both not NULL!") 
  
  rLog("Adding the pmView for", deliv.pt, "to archive.")
  dirMat <- "S:/All/Structured Risk/NEPOOL/Marks/Marks Automation/"
  # archive existing file
  filePMview <- paste(dirMat, "PM_node_view.RData", sep="")
  if (file.exists(filePMview)){
    load(filePMview)   # no need to load it, do only what's in xls!
    archiveFile <- paste(dirMat, "Archive/PM_node_view_", Sys.Date(), 
                         ".RData", sep="")
    file.copy(filePMview, archiveFile)
    rLog("Archived", archiveFile)
  }
  if (deliv.pt %in% names(pmView))
    rLog("Overwritting existing pmView for", deliv.pt)

  # one element of the pmView list 
  pmView[[deliv.pt]] <- list()
  pmView[[deliv.pt]]$view        <- pmView1
  pmView[[deliv.pt]]$lastUpdate  <- Sys.Date()
  pmView[[deliv.pt]]$comments    <- comments
  # no idea what this is for?!  AAD - 9/25/2012, remove it!
  # pmView[[deliv.pt]]$transformer <- transformerName  # this is the name of a function
  
  # save the new PM calibration file
  fileOut <- paste(dirMat, "PM_node_view.RData", sep="")
  save(pmView, file=fileOut)
  rLog("Saved", fileOut)  
}


#########################################################################
# Read the nodes to be marked with PM method, update (overwrite) the view
# and save the results to the archive file.
# This function should be run monthly, or every few weeks. 
# Can be run stand alone!
#
.refreshPMView <- function(maxMonth=as.Date("2017-12-01"))
{
  stop("DEPRECATED!  Needs to be reviewed!")

  maxMonth=as.Date("2018-12-01")
  
  # first manually archive the production xls by stamping it with today's date
  require(xlsReadWrite); require(CEGbase); require(FTR); require(PM)
  FTR.load.ISO.env("NEPOOL")
  
  dirMat <- "S:/All/Structured Risk/NEPOOL/Marks/Marks Automation/"
  fileIn <- paste(dirMat, "PM_node_mark.xls", sep="")
  options(warn=-1)
  rLog("Read the", fileIn)
  X <- read.xls(fileIn, sheet=1)  # sheet="Map"
  options(warn=0)
  
  X <- subset(X, toupper(method)=="PM")
  if (any(is.na(X$ptid))){
    rLog("These delivPt have no ptids.  Please update xls.")
    print(X[is.na(X$ptid),])
    rLog("The delivPt without ptids will be ingnored.")    
  }
  X <- X[!is.na(X$ptid),]
  X$delivPt <- toupper(X$delivPt)
  rownames(X) <- NULL
  
  pmView <- lapply(split(X, 1:nrow(X)), as.list)  
  names(pmView) <- X$delivPt
  pmView <- lapply(pmView, function(x){
    if (x$nodeType == "")
      x$nodeType <- "LOAD"   # let the default flow
    x})

  fom <- as.Date(secdb.dateRuleApply(Sys.Date(), "+1e+1d"))
  toMonths <- seq(fom, maxMonth, by="1 month")
  
  # calculate the forward PM view for all deliveryPts
  setLogLevel(2)
  for (dp in pmView){    # loop over all active locations
    cat("\n-----------------------------------------------")
    cat("\nWorking on", dp$delivPt)

    funName <- "fwd.PMview.default"
    if (dp$custom){    # unless you have a custom function
      funName <- paste("fwd.PMview.", dp$ptid, sep="")
      cat("\n    Calling custom function:", funName)
    }

    res <- NULL
    try(res <- eval(parse(text=paste(funName, "(dp, toMonths)", sep=""))))
    if (class(res)=="try-error"){
      rError("*********************************** FAILED!!")
      next
    }
    
    pmView[[dp$delivPt]]$view <- res
    pmView[[dp$delivPt]]$lastUpdate <- Sys.Date()
  }
  setLogLevel(3)  # back to the default
  
  # archive existing file
  filePMview <- paste(dirMat, "PM_node_view.RData", sep="")
  if (file.exists(filePMview)){
    #load(filePMview)   # no need to load it, do only what's in xls!
    archiveFile <- paste(dirMat, "PM_node_view_", Sys.Date(), 
                         ".RData", sep="")
    file.copy(filePMview, archiveFile)
    rLog("Archived", archiveFile)
  }

  # save the new PM calibration file
  fileOut <- paste(dirMat, "PM_node_view.RData", sep="")
  save(pmView, file=fileOut)
  rLog("Saved", fileOut)
}


#########################################################################
# remove a node from the pmView file.
# .rm_from_pmView_file("MYSTIC.MYS8 DA")
#
.rm_from_pmView_file <- function(deliv.pt)
{
  rLog("Removing", deliv.pt, "from pmView.Rdata file.")
  dirMat <- "S:/All/Structured Risk/NEPOOL/Marks/Marks Automation/"
  # archive existing file
  filePMview <- paste(dirMat, "PM_node_view.RData", sep="")
  if (file.exists(filePMview)){
    load(filePMview)   # no need to load it, do only what's in xls!
    archiveFile <- paste(dirMat, "Archive/PM_node_view_", Sys.Date(), 
                         ".RData", sep="")
    file.copy(filePMview, archiveFile)
    rLog("Archived", archiveFile)
  }
  if (!(deliv.pt %in% names(pmView))){
    rLog("Node", deliv.pt, "is NOT in the file.  Exiting.")
    return()
  }
  pmView[[deliv.pt]] <- NULL
  
  # save the new PM calibration file
  fileOut <- paste(dirMat, "PM_node_view.RData", sep="")
  save(pmView, file=fileOut)
  rLog("Saved", fileOut)  
  
}

#########################################################################
# Suggest how far to go when forecasting 
#
get_toMonths <- function()
{
  lastMonth <- as.Date(paste(as.numeric(format(Sys.Date(),"%Y"))+8,
      "-12-01", sep=""))    # 8 years out
  seq(nextMonth(), lastMonth, by="1 month")
}


#########################################################################
# dp is one element of the pmView list with at least
#   dp$ptid
#   dp$nodeType
#
fwd.PMview.default <- function(dp, toMonths=get_toMonths(), skipDates=NULL, noSims=10000)
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)

  res <- PM:::.extendPeriodically(res, toMonths)
  rownames(res) <- NULL

  return(res) 
}

#########################################################################
# Customize CL8.  Values get calculated with Marks/lib.sellers.choice.R
#
#
fwd.PMview.CL8 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{

  VV2 <- expand.grid.df(
      data.frame(month=toMonths), 
      data.frame(bucket=c("5X16", "2X16H", "7X8"),
      value =c(0.0),   
      component="congestion"))
  
  VV3 <- expand.grid(
    month=seq(as.Date("2014-01-01"), max(toMonths), by="1 month"), 
    bucket=c("5X16", "2X16H", "7X8"))
  VV3$mm <- as.numeric(format(VV3$month, "%m"))
  LF <- data.frame(
    component = "loss%",                
    mm    = 1:12, 
    value = c(-0.0441, -0.0593, -0.0502, -0.0517, -0.1016, -0.0536,
              -0.044, -0.0403, -0.0454, -0.036, -0.0494, -0.0437))
  VV3 <- merge(VV3, LF)
  VV3$mm <- NULL  
  VV3 <- VV3[order(VV3$month, VV3$bucket),]
  
  res <- rbind(VV2, VV3)
  res <- subset(res, month >= toMonths[1])
  
  res$year <- res$mon <- NULL
  rownames(res) <- NULL

  
  res 
}


#########################################################################
# Customize Maine-SC.  Values get calculated with
#   NEPOOL/Marks/lib.sellers.choice.R
#
fwd.PMview.MAINESC <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{

  VV2 <- expand.grid.df(
      data.frame(month=toMonths), 
      data.frame(bucket=c("5X16", "2X16H", "7X8"),
      value =c(0.0),   
      component="congestion"))
  
  VV3 <- expand.grid(
    month=seq(as.Date("2014-01-01"), max(toMonths), by="1 month"), 
    bucket=c("5X16", "2X16H", "7X8"))
  VV3$mm <- as.numeric(format(VV3$month, "%m"))
  LF <- data.frame(
    component = "loss%",                
    mm    = 1:12, 
    value = c(-0.0712, -0.0991, -0.0756, -0.1004, -0.1208, -0.1519,
              -0.1057, -0.0859, -0.1459, -0.1173, -0.0868, -0.0784))
  VV3 <- merge(VV3, LF)
  VV3$mm <- NULL  
  VV3 <- VV3[order(VV3$month, VV3$bucket),]
  
  res <- rbind(VV2, VV3)
  res <- subset(res, month >= toMonths[1])
  
  res$year <- res$mon <- NULL
  rownames(res) <- NULL

  
  res 
}



#########################################################################
# Customize PEMIGWAS.AYER
#
fwd.PMview.330 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- -0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- -0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="7X8")]   <- -0.0  # long term
  #res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$year > 2012)] <- -0.5  # Pt.Lepreau
  #res$value[which(res$component=="congestion" & res$bucket=="7X8"  & res$year > 2012)] <- -0.25  
  #res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$year > 2012)] <- -0.35  

  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -0.25    
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -0.15

  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.0
  res$value[which(res$component=="loss%" & res$bucket=="5X16")]  <- 0.0090
  res$value[which(res$component=="loss%" & res$bucket=="2X16H")] <- 0.0065
  res$value[which(res$component=="loss%" & res$year >= 2017)] <- -0.01     # for Northern Pass
  res$value[which(res$component=="loss%" & res$year >= 2017 & res$bucket=="5X16")] <- -0.00
  
  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}



#########################################################################
# Customize Rumford.AZIS
#
fwd.PMview.331 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- -0.25  # long term
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- -0.15  # long term
  res$value[which(res$component=="congestion" & res$bucket=="7X8")]  <- -0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$year > 2012)] <- -0.5  # Pt.Lepreau
  res$value[which(res$component=="congestion" & res$bucket=="7X8"  & res$year > 2012)] <- -0.25  
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$year > 2012)] <- -0.35  

  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -2.5    
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -1.0

  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.04
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.03
  res$value[which(res$component=="loss%") & res$mon %in% c(10,11)] <- -0.03 
  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}



#########################################################################
# Customize Bethlehem, biomass 15.5 MW, Sank's deal 08/31/2010
#
fwd.PMview.337 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)

  res <- PM:::.extendPeriodically(res, toMonths)
  
  # loss factors ... pretty volatile
  ind <- which(res$component=="loss%")
  res$value[ind] <- -0.0265

  rownames(res) <- NULL
  return(res) 
}

#########################################################################
# Customize UN.BPT_RESC14 BPTR
#  Bridgeport Wheelabrador, ct wood/trash  58 MW! 
#
fwd.PMview.349 <- function(dp, toMonths=get_toMonths(), noSims=2000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- 0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% c(1:3,11:12))] <- 0.25   
  res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% 4:10)] <- 0.50   
  res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% 7:8)]  <- 0.85   
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% 4:10)] <- 0.44   
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% 7:8)]  <- 0.65   
  res$value[which(res$component=="congestion" & res$bucket=="7X8"   & res$mon %in% 4:9)]  <- 0.00
  
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% 4:10 & res$year==2013)] <- 0.65  
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% 7:8  & res$year==2013)] <- 1.00   
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% 4:10 & res$year==2013)] <- 0.55  
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% 7:8  & res$year==2013)] <- 0.85   

  
#browser()  
  # loss factors 
  res$value[which(res$component=="loss%")] <- 0.0
  res$value[which(res$component=="loss%" & res$bucket=="5X16"  & res$mon %in% 5:9)] <- 0.0105 
  res$value[which(res$component=="loss%" & res$bucket=="5X16"  & res$mon %in% 7:8)] <- 0.0125 
  res$value[which(res$component=="loss%" & res$bucket=="2X16H" & res$mon %in% 5:9)] <- 0.0075 
  res$value[which(res$component=="loss%" & res$bucket=="7X8")]  <- -0.01   
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$mon %in% 5:9)] <- -0.0025 

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}


#########################################################################
# Customize Brayton1
#
fwd.PMview.350 <- function(dp, toMonths=get_toMonths(), noSims=2000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- 0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- -0.10   
  res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$year==2013)] <- 0.00 
  res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$year==2014)] <- 0.00 
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- -0.07   
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$year %in% c(2013,2014))] <- 0.00
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$year==2013 & res$mon %in% c(4,5,10,11))] <- 0.10  
  res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- 0.00   
  res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$year==2013 & res$mon %in% c(4,5,10,11))] <- 0.20
  res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$year==2014 & res$mon %in% c(4,5,10,11))] <- 0.15

  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.025
  res$value[which(res$component=="loss%" & res$bucket=="5X16")]  <- -0.0254 
  res$value[which(res$component=="loss%" & res$bucket=="2X16H")] <- -0.0235   
  res$value[which(res$component=="loss%" & res$bucket=="7X8")]   <- -0.0225 

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}



#########################################################################
# Customize UN.ASHLAND 34.5BRID, ptid=357
# responds positively for NE EW congestion
# VY retirement will make NH plants run more, lower the LF component
#
fwd.PMview.357 <- function(dp, toMonths=get_toMonths(), noSims=2000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- 0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% 4:5)]   <- 0.00   
  res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% 10:11)] <- 0.00   
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% 4:5)]   <- 0.00   
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% 10:11)] <- 0.00   
  res$value[which(res$component=="congestion" & res$bucket=="7X8"   & res$mon %in% 4:9)]   <- 0.0   
#browser()
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- 0.0
  res$value[which(res$component=="loss%" & res$bucket=="5X16")]  <- 0.0115 
  res$value[which(res$component=="loss%" & res$bucket=="2X16H")] <- 0.0075   
  res$value[which(res$component=="loss%" & res$bucket=="7X8")]   <- 0.0020   

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}



#########################################################################
# Customize Dartmouth
# What happened in Nov-2010?  A lot of Sema outages, pushed the loss
# factors high 2%.
#
fwd.PMview.388 <- function(dp, toMonths=get_toMonths(), noSims=2000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- -0.10  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% 4:9)] <- -0.40   
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% 4:9)] <- -0.25   
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.01  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}


#########################################################################
# Customize Covanta Jonesboro, 20.5 MW wood/refuse, in BHE
#
fwd.PMview.446 <- function(...)
{
  fwd.PMview.536(...)
}


#########################################################################
# Customize Livermore AEI to Bigelow
#
fwd.PMview.463 <- function(...)
{
  fwd.PMview.590(...)
}


#########################################################################
# Customize Milstone
#
fwd.PMview.484 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion")]  <- 0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- 0.25  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5,10,11))] <- 1.00
  
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- 0.25  # long term
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5,10,11))] <- 0.75

  res$value[which(res$component=="congestion" & res$bucket=="7X8")]  <- -0.00  # long term
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.024 
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.030
  # res$value[which(res$component=="loss%") & res$mon %in% c(10,11)] <- -0.04
  
  #res$value[which(res$component=="loss%" & res$year == 2013)] <- -0.055 
  #res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year == 2013)] <- -0.045
  
  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Milford
#
fwd.PMview.486 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion")]  <- 0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- 0.25  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(12,1,2))] <- 1.35
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(12,1,2))] <- 0.90
  res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(12,1,2))]  <- 0.5  
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.00 
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.00
  
  #res$value[which(res$component=="loss%" & res$year == 2013)] <- -0.055 
  #res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year == 2013)] <- -0.045
  
  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Merimack 2, 490
#
fwd.PMview.490 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion")]  <- 0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- 0.25  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(12,1,2))] <- 1.35
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(12,1,2))] <- 0.90
  res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(12,1,2))]  <- 0.5  
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.02 
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.02
  res$value[which(res$component=="loss%" & res$mon %in% c(12,1,2))] <- -0.04
  
  #res$value[which(res$component=="loss%" & res$year == 2013)] <- -0.055 
  #res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year == 2013)] <- -0.045
  
  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Masspower
#
fwd.PMview.497 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2006-12-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)

  res <- PM:::.extendPeriodically(res, toMonths)
  
  # cong component up after NEEWS, only 5 years.
  ind <- which(res$component=="congestion" & res$bucket=="2X16H" &
    res$month >= as.Date("2013-01-01") & res$month <= as.Date("2018-01-01"))
  res$value[ind] <- res$value[ind] + 0.35
  ind <- which(res$component=="congestion" & res$bucket=="5X16" &
    res$month >= as.Date("2013-01-01") & res$month <= as.Date("2018-01-01"))
  res$value[ind] <- res$value[ind] + 0.50
  ind <- which(res$component=="congestion" & res$bucket=="7X8" &
    res$month >= as.Date("2013-01-01") & res$month <= as.Date("2018-01-01"))
  res$value[ind] <- res$value[ind] + 0.30

  # loss factors have been increasing since 2004 ...
  ind <- which(res$component=="loss%")
  res$value[ind] <- -0.005  
  
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Mystic7 = Mystic9
#
fwd.PMview.502 <- function(...)
{
  fwd.PMview.1616(...)
}



#########################################################################
# Customize Bellingham 507
#
fwd.PMview.507 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- 0.00   # long term
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- 0.10   # long term
  res$value[which(res$component=="congestion" & res$bucket=="7X8")]   <- 0.05   # long term

  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -0.35    
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -0.25

  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.021
  res$value[which(res$component=="loss%" & res$bucket=="2X16H")] <- -0.0195
  res$value[which(res$component=="loss%" & res$bucket=="7X8")]   <- -0.0175
  #res$value[which(res$component=="loss%") & res$mon %in% c(10,11)]   <- -0.04

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}


#########################################################################
# Customize Newington 508
#
fwd.PMview.508 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- 0.00   # long term
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- 0.00   # long term
  res$value[which(res$component=="congestion" & res$bucket=="7X8")]   <- 0.00   # long term

  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -0.35    
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -0.18

  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.03
  res$value[which(res$component=="loss%" & res$bucket=="2X16H")] <- -0.025
  res$value[which(res$component=="loss%" & res$bucket=="7X8")]   <- -0.0225
  #res$value[which(res$component=="loss%") & res$mon %in% c(10,11)]   <- -0.04

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}


#########################################################################
# Customize Chemical.Perc DA, 21 MW 
#
fwd.PMview.536 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- -0.25  # long term
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- -0.15  # long term
  res$value[which(res$component=="congestion" & res$bucket=="7X8")]  <- -0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$year > 2012)] <- -0.5  # Pt.Lepreau
  res$value[which(res$component=="congestion" & res$bucket=="7X8"  & res$year > 2012)] <- -0.0  
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$year > 2012)] <- -0.35  

  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -1.5    
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -0.75

  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.06 
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.05
  res$value[which(res$component=="loss%") & res$mon %in% c(10,11)]   <- -0.04
  
  res$value[which(res$component=="loss%" & res$year == 2013)] <- -0.055 
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year == 2013)] <- -0.045
  
  res$value[which(res$component=="loss%" & res$year == 2014)] <- -0.0525 
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year == 2013)] <- -0.0425

  res$value[which(res$component=="loss%" & res$year == 2015)] <- -0.050 
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year == 2015)] <- -0.04  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}



#########################################################################
# Customize Pilgrim
#
fwd.PMview.537 <- function(dp, toMonths=get_toMonths(), noSims=2000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- 0.00  # long term
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16")] <- -0.10  # long term
  ## # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))]   <- 0.80   
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(6,7,8))] <- 0.50
  res$value[which(res$component=="congestion" & res$mon %in% c(1,2,12))] <- 1.15
  ## res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- -0.05 # long term
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(1,2,12))] <- 0.65
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- 0.40
  ## res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- -0.05  
  ## res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(4,5))] <- -0.35
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.0125  
  res$value[which(res$component=="loss%" & res$mon %in% c(12,1,2,3))] <- -0.01
  #res$value[which(res$component=="loss%" & res$month==as.Date("2013-04-01"))] <- -0.014

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}


#########################################################################
# Customize Spring Street.  12 MW, Southern ME
# goes by name: EcoMaine
#
fwd.PMview.542 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion" & res$bucket=="5X16")] <- -0.0  # long term
  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -0.0   
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(6,7,8))] <- -0.0
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(1,2,12))] <- 0.60
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- -0.0 # long term
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(1,2,12))] <- 0.30
  res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- 0.00  # some value before MPRP
  res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(1,2,12))] <- 0.25  

  
  # loss factors 
  res$value[which(res$component=="loss%" & res$bucket=="5X16")]  <- -0.035
  res$value[which(res$component=="loss%" & res$bucket=="5X16" & res$year==2013)]  <- -0.0350
  res$value[which(res$component=="loss%" & res$bucket=="5X16" & res$year==2014)]  <- -0.0350
  res$value[which(res$component=="loss%" & res$bucket=="5X16" & res$year==2015)]  <- -0.0340
  res$value[which(res$component=="loss%" & res$bucket=="5X16" & res$year>=2016)]  <- -0.0325
  res$value[which(res$component=="loss%" & res$bucket=="5X16" & res$mon %in% c(1,2,12))]  <- -0.0425
  
  res$value[which(res$component=="loss%" & res$bucket=="2X16H")] <- -0.035
  res$value[which(res$component=="loss%" & res$bucket=="2X16H" & res$year==2013)]  <- -0.030
  res$value[which(res$component=="loss%" & res$bucket=="2X16H" & res$year==2014)]  <- -0.030
  res$value[which(res$component=="loss%" & res$bucket=="2X16H" & res$year==2015)]  <- -0.030
  res$value[which(res$component=="loss%" & res$bucket=="2X16H" & res$year>=2016)]  <- -0.030
  res$value[which(res$component=="loss%" & res$bucket=="2X16H" & res$mon %in% c(1,2,12))]  <- -0.037
  
  res$value[which(res$component=="loss%" & res$bucket=="7X8")]   <- -0.0275
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year==2013)]  <- -0.0265
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year==2014)]  <- -0.0265
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year==2015)]  <- -0.0265
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$mon %in% c(1,2,12))]  <- -0.035
 
  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Ward Hill (Wheelabrator North Andover)
# Loss factors correlate with Seabrook outage
#
fwd.PMview.547 <- function(dp, toMonths=get_toMonths(), noSims=2000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion")] <- 0.0  # long term
  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -0.4    
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(6,7,8))] <- -0.0
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(10,11))] <- -0.2
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -0.15
  res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- -0.0

  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.0235
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.02

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}




#########################################################################
# Customize Seabrook.  We have CL8 marked at Seabrook $-0.40
#
fwd.PMview.555 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion" )] <- -0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16")] <- -0.0  # long term
  # assume EW congestion during Apr-May, Oct
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -0.25    
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(6,7,8))] <- -0.25
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(10,11))] <- -0.25
  ## res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -0.25
  ## res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- -0.2

  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.03       
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.0225

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}

#########################################################################
# Customize SouthMeadow SO11.  
#
fwd.PMview.572 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion" )] <- -0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16")] <- -0.0  # long term
  # assume EW congestion during Apr-May, Oct
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -0.25    
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(6,7,8))] <- -0.25
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(10,11))] <- -0.25
  ## res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -0.25
  ## res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- -0.2

  
  # loss factors 
  res$value[which(res$component=="loss%")] <- 0.00       
  #res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- 0.0225

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}



#########################################################################
# Customize South Meadows.
#
fwd.PMview.580 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion")] <- 0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(7,8))] <- 0.25
  
  # loss factors 
  res$value[which(res$component=="loss%" & res$bucket=="5X16")] <- 0.0115
  res$value[which(res$component=="loss%" & res$bucket=="5X16" & res$year >= 2013)] <- 0.0095
  res$value[which(res$component=="loss%" & res$bucket=="5X16" & res$year >= 2014)] <- 0.0080
  res$value[which(res$component=="loss%" & res$bucket=="5X16" & res$year >= 2015)] <- 0.0065
  res$value[which(res$component=="loss%" & res$bucket=="2X16H")] <- 0.0075
  res$value[which(res$component=="loss%" & res$bucket=="2X16H" & res$year > 2013)] <- 0.0050
  res$value[which(res$component=="loss%" & res$bucket=="2X16H" & res$year > 2015)] <- 0.0025
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- 0.00
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year==2013)] <- -0.0015
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year==2014)] <- -0.0025
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year==2015)] <- -0.0035
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year>=2015)] <- -0.0045


  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}



#########################################################################
# Customize Bigelow
#
fwd.PMview.590 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=NULL)
{
  fwd.PMview.12551(list(ptid=12551, nodeType="GEN"), toMonths, noSims=noSims,
    skipDates=skipDates)
}



#########################################################################
# Customize Tamworth, biomass 19.9 MW, Sank's deal 08/31/2010
#
fwd.PMview.592 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)

  res <- PM:::.extendPeriodically(res, toMonths)
  
  # loss factors ... pretty volatile
  ind <- which(res$component=="loss%")
  res$value[ind] <- -0.0265

  rownames(res) <- NULL
  return(res) 
}



#########################################################################
# Customize AES Thames
# When Thames will come back it will push the loss factor even lower!
#
fwd.PMview.594 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion")] <- 0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(1,2,6))]  <- 0.15
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(7,8))]    <- 0.35
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(1,2,6))] <- 0.05
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(7,8))]   <- 0.10

  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.001       
  res$value[which(res$component=="loss%" & res$bucket=="2X16H")] <- -0.002
  res$value[which(res$component=="loss%" & res$bucket=="7X8")]   <- -0.02

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize AES Thames
# When Thames will come back it will push the loss factor even lower!
#
fwd.PMview.594 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion")] <- 0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(1,2,6))]  <- 0.15
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(7,8))]    <- 0.35
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(1,2,6))] <- 0.05
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(7,8))]   <- 0.10

  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.001       
  res$value[which(res$component=="loss%" & res$bucket=="2X16H")] <- -0.002
  res$value[which(res$component=="loss%" & res$bucket=="7X8")]   <- -0.02

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}



#########################################################################
# Customize VERGENNE.VERD for F15-1Y-R1
#
fwd.PMview.598 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=1000)
  
  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # shave a bit for VY tanking when NY exports are high
  res$value[which(res$component=="congestion")] <- 0.00      # long term
  res$value[which(res$component=="congestion" & res$mon %in% c( 1, 2) & res$bucket=="5X16" )] <- -0.90  
  res$value[which(res$component=="congestion" & res$mon %in% c(11, 12, 1, 2) & res$bucket=="2X16H" )] <- -0.55 
  res$value[which(res$component=="congestion" & res$mon %in% c(11, 12, 1, 2) & res$bucket=="7X8" )] <- -0.35 
  
  # loss factors ...
  ind <- which(res$component=="loss%")
  res$value[ind] <- -0.0075
  res$value[which(res$component=="loss%" & res$mon %in% c(11, 12, 1, 2) )] <- -0.015
  res$value[which(res$component=="loss%" & res$mon %in% c(3) )] <- -0.0125


  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}



#########################################################################
# Customize West Enfield
#
fwd.PMview.616 <- function(dp, toMonths=get_toMonths(), noSims=2000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- 0.00      # long term
  # assume EW congestion during Apr-May, Oct
  #res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))]   <- -0.80   
  #res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(6,7,8))] <- -0.50
  #res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(10,11))] <- -0.55
  #res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- -0.0 # long term
  #res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -0.75
  #res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- 0.0  
  #res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(4,5))] <- -0.35
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.06  
  ## res$value[which(res$component=="loss%") & res$mon %in% c(6,7,8,9)]   <- -0.065 
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.045
  ## res$value[which(res$component=="loss%" & res$mon %in% c(6,7,8,9) & res$bucket=="7X8")] <- -0.055
  ## res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.07
  ## res$value[which(res$component=="loss%" & res$mon %in% c(6,7,8,9) & res$bucket=="7X8")] <- -0.055

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}


#########################################################################
# Customize Whitefield
#
fwd.PMview.618 <- function(dp, toMonths=get_toMonths(), noSims=2000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- 0.00      # long term
  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))]   <- -0.80   
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(6,7,8))] <- -0.50
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(10,11))] <- -0.55
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- -0.0 # long term
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -0.75
  res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- 0.0  
  #res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(4,5))] <- -0.35
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.02  
  ## res$value[which(res$component=="loss%") & res$mon %in% c(6,7,8,9)]   <- -0.065 
  ## res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.07
  ## res$value[which(res$component=="loss%" & res$mon %in% c(6,7,8,9) & res$bucket=="7X8")] <- -0.055
  ## res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.07
  ## res$value[which(res$component=="loss%" & res$mon %in% c(6,7,8,9) & res$bucket=="7X8")] <- -0.055

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}


#########################################################################
# Customize Wyman Hydro 1.  Nothern Maine, 27.4 MW. 
#
fwd.PMview.636 <- function(dp, toMonths=get_toMonths(), noSims=2000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- 0.00      # long term
  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))]   <- -1.85   
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(6,7,8))] <- -0.50
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(10,11))] <- -0.95
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- -0.0 # long term
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -0.75
  res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- 0.10  
  #res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(4,5))] <- -0.35
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.08  
  res$value[which(res$component=="loss%") & res$mon %in% c(6,7,8,9)]   <- -0.065 
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.07
  res$value[which(res$component=="loss%" & res$mon %in% c(6,7,8,9) & res$bucket=="7X8")] <- -0.055
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.07
  res$value[which(res$component=="loss%" & res$mon %in% c(6,7,8,9) & res$bucket=="7X8")] <- -0.055

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}



#########################################################################
# Customize Dighton
#
fwd.PMview.1005 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)

  res <- PM:::.extendPeriodically(res, toMonths)
  
  # loss factors have been increasing since 2004 ...
  ind <- which(res$component=="loss%")
  res$value[ind] <- -0.02
  ind <- which(res$component=="loss%" & res$month >= as.Date("2018-01-01"))
  res$value[ind] <- -0.02 + 0.007   # assume HQ line goes online in 2018

  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Berkshire
#
fwd.PMview.1086 <- function(dp, toMonths=get_toMonths(), noSims=2000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- 0.00  # long term
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16")] <- 0.20   
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$year==2012)] <- 1.15   
  ## res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$year==2012)] <- 1.00   
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% 6:10 & res$year==2013)] <- 3.00   
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% 4:5  & res$year==2013)] <- 4.10   
  ## res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% 6:10 & res$year==2013)] <- 2.25   
  ## res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% 4:5  & res$year==2013)] <- 2.75
  
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% 6:10 & res$year==2014)] <- 0.75   
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% 4:5  & res$year==2014)] <- 1.00   
  ## res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% 6:10 & res$year==2014)] <- 0.40   
  ## res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% 4:5  & res$year==2014)] <- 0.75   
  ## res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$year %in% 2012:2013)] <- 0.25 
  ## res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$year %in% 2014)] <- 0.0 

  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.005
  res$value[which(res$component=="loss%" & res$bucket=="5X16") ]  <- -0.0095
  res$value[which(res$component=="loss%" & res$bucket=="2X16H") ] <- -0.0075
  
  ## res$value[which(res$component=="loss%" & res$bucket=="5X16"  & res$year %in% 2012:2013)] <- 0.008  
  ## res$value[which(res$component=="loss%" & res$bucket=="2X16H" & res$year %in% 2012:2013)] <- 0.005  
  ## res$value[which(res$component=="loss%" & res$bucket=="7X8"   & res$year %in% 2012:2013)]  <- 0.004
  
  ## res$value[which(res$component=="loss%" & res$bucket=="5X16"  & res$year %in% 2014)] <- 0.004  
  ## res$value[which(res$component=="loss%" & res$bucket=="2X16H" & res$year %in% 2014)] <- 0.002  
  ## res$value[which(res$component=="loss%" & res$bucket=="7X8"   & res$year %in% 2014)] <- 0.001
  
  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}



#########################################################################
# Customize Stonybrook
# Have the Cal14 ftr from stonybrook to nema, 50 MW
# What if there is no nema cong??
#
fwd.PMview.1185 <- function(dp, toMonths=get_toMonths(), noSims=2000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- 0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16")] <- 0.0  
  res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% 1:2)]  <- -1.00   
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% 1:2)]  <- -0.50   
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.0045  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}



#########################################################################
# Customize Meadowbrook
#
fwd.PMview.1188 <- function(dp, toMonths=get_toMonths(), noSims=2000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- -0.10  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16")] <- -0.15   
  res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% 6:10)] <- -0.30   
  res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% 4:5)]  <- -1.00   
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% 6:10)] <- -0.20   
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% 4:5)]  <- -0.65   
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.005  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}


#########################################################################
# Customize MIS:  Take loss factors to come back to -5.5 around 2015 or
# so starting from 2009-11-01.  
#
fwd.PMview.1216 <- function(dp, toMonths=get_toMonths(), noSims=10000)
{
  # some brutal negative congestion in these days.  Remove them.
  skipDates <- seq(as.Date("2008-06-27"), as.Date("2008-07-23"), by="1 day")

  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)  # a lot of sims for MIS

  res <- PM:::.extendPeriodically(res, toMonths)
  rownames(res) <- NULL
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  
  # Make sure there is no "big" jump between recent history and 
  # the nearby future in the congestion component.  
  # res <- PM:::.smoothJoinPMView(res, dp$ptid, histPeriods=3, lambda=4/365,
  #   buckets=c("2X16H", "7X8", "5X16"))
 
  # congestion
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- -0.25  # long term
  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -3.5    
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -2.0

  
  ## # make losses go back to a -5.5% level in 2015 and over
  ## # using an exponential decay function
  ## res <- split(res, res$component)  
  ## fLF <- split(res[["loss%"]], res[["loss%"]]$bucket)
  
  ## for (b in 1:length(fLF)){   # needs to be reviewed as 2015 aproaches!!!
  ##   fLF[[b]] <- PM:::.expDecay(fLF[[b]], as.Date("2009-11-01"),
  ##     1/(2.5*365), -0.055, tVariable="month", value="value")
  ## }
  ## fLF <- do.call(rbind, fLF)
  ## res[["loss%"]] <- fLF
  ## res <- do.call(rbind, res)
  # loss factors
  
  res$value[which(res$component=="loss%")] <- -0.08       
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.06
  res$value[which(res$component=="loss%") & res$mon %in% 10]   <- -0.055 
  
  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}


#########################################################################
# Customize Tiverton
#
fwd.PMview.1226 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- -0.00  # long term
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16")] <- -0.05   
  res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% c(1,2,12))] <- 3.00   
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% 4:5)]  <- -0.00   
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(1,2,12))] <- 1.80   
  res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(1,2,12))] <- 1.50   
  ## res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% 4:5)]  <- -0.65   
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.025  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res

}


#########################################################################
# Customize Wallingford
#
fwd.PMview.1376 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  skipDates <- c(seq(as.Date("2003-01-01"), as.Date("2007-05-31"),
    by="1 day"), seq(as.Date("2010-05-01"), as.Date("2010-05-15"),
    by="1 day"))
  rLog("Using custom function for 1342.")
  rLog("Removing dates from", as.character(skipDates[1]), "to",
       as.character(skipDates[length(skipDates)]))
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)
  
  res <- PM:::.extendPeriodically(res, toMonths)

  # loss factors have been increasing since 2004 ...
  ind <- which(res$component=="loss%")
  res$value[ind] <- 0.01
  
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Mystic8 != Mystic9
#
fwd.PMview.1478 <- function(dp, toMonths=get_toMonths(), noSims=1000)
{
  VV1 <- expand.grid.df(
      data.frame(month=seq(toMonths[1], as.Date("2013-12-01"), by="1 month")), 
      data.frame(bucket=c("5X16", "2X16H", "7X8"),
      value =c(-0.30, -0.12, -0.02),   
      component="congestion"))
  VV2 <- expand.grid.df(
      data.frame(month=seq(as.Date("2014-01-01"), as.Date("2025-12-01"), by="1 month")), 
      data.frame(bucket=c("5X16", "2X16H", "7X8"),
      value =c(-0.20, -0.08, -0.02),   
      component="congestion"))
  VV3 <- expand.grid.df(
      data.frame(month=toMonths), 
      data.frame(bucket=c("5X16", "2X16H", "7X8"),
      value =c(-0.017),   
      component="loss%"))
  res <- rbind(VV1, VV2, VV3)

  res
}


#########################################################################
# Customize Mystic9
#
fwd.PMview.1616 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=c(seq(as.Date("2003-01-01"), as.Date("2007-12-31"),
    by="1 day"), seq(as.Date("2010-05-01"), by="1 day", length.out=7)))
{
  VV0 <- expand.grid.df(
      data.frame(month=seq(as.Date("2012-01-01"), as.Date("2012-12-01"), by="1 month")), 
      data.frame(bucket=c("5X16", "2X16H", "7X8"),
      value =c(-0.30, -0.12, -0.02),   
      component="congestion"))
  VV0$value <- c(
  -0.90, -0.75,	-0.55,	-0.95,	-1.04,	-0.45,	-0.42,	-0.45,	-0.55,	-0.95,	-0.85,	-0.52,
  -0.45, -0.32,	-0.21,	-0.42,	-0.42,	-0.12,	-0.12,	-0.12,	-0.24,	-0.42,	-0.46,	-0.18,
  -0.22, -0.12,	-0.02,	-0.22,	-0.22,	-0.02,	-0.02,	-0.02,	-0.12,	-0.22,	-0.18,	-0.02)
  VV1 <- expand.grid.df(
      data.frame(month=seq(as.Date("2013-01-01"), as.Date("2013-12-01"), by="1 month")), 
      data.frame(bucket=c("5X16", "2X16H", "7X8"),
      value =c(-0.30, -0.12, -0.02),   
      component="congestion"))
  VV2 <- expand.grid.df(
      data.frame(month=seq(as.Date("2014-01-01"), as.Date("2025-12-01"), by="1 month")), 
      data.frame(bucket=c("5X16", "2X16H", "7X8"),
      value =c(-0.20, -0.08, -0.02),   
      component="congestion"))
  VV3 <- expand.grid.df(
      data.frame(month=toMonths), 
      data.frame(bucket=c("5X16", "2X16H", "7X8"),
      value =c(-0.017),   
      component="loss%"))
  res <- rbind(VV0, VV1, VV2, VV3)

  res <- subset(res, month >= toMonths[1])
  
  return(res) 
}

#########################################################################
# Customize Granite Ridge
#
fwd.PMview.1625 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=NULL)
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)
  
  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion" & res$bucket=="5X16")] <- -0.15  # long terrm, onpeak
  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))]   <- -0.30   
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(6,7,8))] <- -0.30
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(10,11))] <- -0.30
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- -0.05 # long term
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))]   <- -0.20
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(10,11))] <- -0.20
  res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- -0.02  
  res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(4,5,10,11))] <- -0.15
  res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(7,8))] <- -0.10
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.027  
#  res$value[which(res$component=="loss%" & res$mon %in% c(5,6,7,8,9))] <- -0.0275
  res$value[which(res$component=="loss%" & res$bucket=="7X8")]   <- -0.021
  res$value[which(res$component=="loss%" & res$bucket=="2X16H")] <- -0.024

  res$year <- res$mon <- NULL
  rownames(res) <- NULL

  return(res) 
}


#########################################################################
# Customize RISE
#
fwd.PMview.1630 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=NULL)
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)
  
  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- 0.0  # long terrm, onpeak
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5,10,11)& res$year %in% c(2013:2015) )] <- 0.30
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(7,8) & res$year %in% c(2013:2015))] <- 0.15
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(1,2) & res$year %in% c(2013:2016))] <- 3.0
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(4,5,10,11)& res$year %in% c(2013:2015) )] <- 0.30
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(7,8) & res$year %in% c(2013:2015))] <- 0.15
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(1,2) & res$year %in% c(2013:2016))] <- 2.0
  res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(4,5,10,11)& res$year %in% c(2013:2015) )] <- 0.30
  res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(7,8) & res$year %in% c(2013:2015))] <- 0.15
  res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(1,2) & res$year %in% c(2013:2016))] <- 1.15
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.028
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.013
  res$value[which(res$component=="loss%" & res$mon %in% c(1, 2))] <- -0.01

  # custom prompt Month
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon==5 & res$year==2014 )]  <- 1.9
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon==5 & res$year==2014 )] <- 1.8
  res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon==5 & res$year==2014 )]   <- 1.8
  
  
  res$year <- res$mon <- NULL
  rownames(res) <- NULL

  return(res) 
}


#########################################################################
# Customize Edgar (Fore River)
#
fwd.PMview.1691 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=c(seq(as.Date("2003-01-01"), as.Date("2007-12-31"),
    by="1 day"), as.Date("2008-06-10")))
{
##   res <- makePMview(dp$ptid, nodeType=dp$nodeType,
##     skipDates=skipDates, noSims=10000)

##   # take extra 0.05 off for when the hub blows up
##   ind <- which(res$component=="congestion" & res$bucket=="5X16")
##   res$value[ind] <- res$value[ind] - 0.05     
##   ind <- which(res$component=="congestion" & res$bucket=="2X16H")
##   res$value[ind] <- res$value[ind] - 0.03     
  
##   res <- PM:::.extendPeriodically(res, toMonths)
  
##   # add 10c for less EW congestion after 2014
##   ind <- which(res$component=="congestion" & res$bucket=="5X16"
##                & res$month > as.Date("2014-01-01"))
##   res$value[ind] <- pmin(res$value[ind] + 0.10, 0)
  
##   # loss factors 
##   ind <- which(res$component=="loss%")
##   res$value[ind] <- -0.0275   
  
##   rownames(res) <- NULL
  VV1 <- expand.grid.df(
      data.frame(month=seq(toMonths[1], as.Date("2013-12-01"), by="1 month")), 
      data.frame(bucket=c("5X16", "2X16H", "7X8"),
      value =c(-0.39, -0.06, -0.02),   
      component="congestion"))
  VV2 <- expand.grid.df(
      data.frame(month=seq(as.Date("2014-01-01"), as.Date("2025-12-01"), by="1 month")), 
      data.frame(bucket=c("5X16", "2X16H", "7X8"),
      value =c(-0.29, -0.06, -0.02),   
      component="congestion"))
  VV3 <- expand.grid.df(
      data.frame(month=toMonths), 
      data.frame(bucket=c("5X16", "2X16H", "7X8"),
      value =c(-0.0275),   
      component="loss%"))
  res <- rbind(VV1, VV2, VV3)
  
  return(res) 
}


#########################################################################
# Customize Maine
#
fwd.PMview.4001 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion" & res$bucket=="5X16")] <- 0.25  # long term
  # assume EW congestion during Dec, Jan, Feb
  res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% c(1,2,12))] <- 1.85   
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- 0.10 
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(1,2,12))] <- 1.00 
  res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- 0.35   
  res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(1,2,12))] <- 1.25   
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.032  
  res$value[which(res$component=="loss%" & res$mon %in% c(1,2,12))] <- -0.01
  res$value[which(res$component=="loss%" & res$mon %in% c(7,8,9))] <- -0.06
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.04
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$mon %in% c(1,2,12))] <- -0.02
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$mon %in% c(7,8,9))]  <- -0.05

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}


#########################################################################
# Customize VT
#
fwd.PMview.4003 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- 0.0  # long term
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16")] <- 0.25  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% c(1,2,12))] <- -1.75   
  ## res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- 0.10 
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(1,2,12))] <- -0.85 
  ## res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- 0.35   
  res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(1,2,12))] <- -1.00   
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- 0.00  
  #res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- 0.0025
  #res$value[which(res$component=="loss%" & res$bucket=="2X16H")] <- 0.004
  res$value[which(res$component=="loss%" & res$mon %in% c(6,7,8)  & res$bucket=="5X16")]  <-  0.0060
  res$value[which(res$component=="loss%" & res$mon %in% c(1,2,3,12) & res$bucket=="5X16")]  <- -0.0090
  res$value[which(res$component=="loss%" & res$mon %in% c(1,2,3,12) & res$bucket=="2X16H")] <- -0.0120
  res$value[which(res$component=="loss%" & res$mon %in% c(1,2,3,12) & res$bucket=="7X8")]   <- -0.0110
  res$value[which(res$component=="loss%" & res$mon %in% c(4))]   <- -0.0065
  res$value[which(res$component=="loss%" & res$mon %in% c(5))]   <- -0.0055
  res$value[which(res$component=="loss%" & res$mon %in% c(11))]  <- -0.0055          
            
  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}


#########################################################################
# Customize Salsbury
#
fwd.PMview.4010 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- 0.00  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16")] <- 0.00  # long term
  # assume EW congestion during Dec, Jan, Feb
  ## res$value[which(res$component=="congestion" & res$bucket=="5X16"  & res$mon %in% c(1,2,12))] <- 1.85   
  ## res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- 0.10 
  ## res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(1,2,12))] <- 1.00 
  ## res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- 0.35   
  ## res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(1,2,12))] <- 1.25   
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.05  
  ## res$value[which(res$component=="loss%" & res$mon %in% c(1,2,12))] <- -0.01
  ## res$value[which(res$component=="loss%" & res$mon %in% c(7,8,9))] <- -0.06
  ## res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.04
  ## res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$mon %in% c(1,2,12))] <- -0.02
  ## res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$mon %in% c(7,8,9))]  <- -0.05

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}


#########################################################################
# Customize Roseton
# 2010-11-18:  Update LossFactors based on data since May
#
fwd.PMview.4011 <- function(dp, toMonths=get_toMonths(), noSims=100,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion, set at the F11-1Y FTR auction results
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- 0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(1,2))] <- -4.0   
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- 0.00  # long term
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(1,2))] <- -2.0 
  res$value[which(res$component=="congestion" & res$bucket=="7X8")]   <- 0.00  # long term
  res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(1,2))]  <- -1.0   
  
  # loss factors ... pretty volatile
  res$value[which(res$component=="loss%" & res$bucket=="5X16")]  <- -0.025
  res$value[which(res$component=="loss%" & res$bucket=="5X16" & res$mon %in% c(5:9))]  <- -0.005
  res$value[which(res$component=="loss%" & res$bucket=="5X16" & res$mon %in% c(1:2))]  <- -0.035
  res$value[which(res$component=="loss%" & res$bucket=="2X16H")] <- -0.0225
  res$value[which(res$component=="loss%" & res$bucket=="2X16H" & res$mon %in% c(5:9))]  <- -0.015
  res$value[which(res$component=="loss%" & res$bucket=="2X16H" & res$mon %in% c(1:2))]  <- -0.035
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.019
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$mon %in% c(5:9))]  <- -0.01
  
  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize LD.GRAHAM46
#
fwd.PMview.4078 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))
  
  # congestion
  res$value[which(res$component=="congestion")] <- 0.00      # long term
  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))]   <- -0.20   
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(6,7,8))] <- -0.15
  #res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(10,11))] <- -0.55
  #res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- -0.0 # long term
  #res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -0.75
  #res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- 0.0  
  #res$value[which(res$component=="congestion" & res$bucket=="7X8" & res$mon %in% c(4,5))] <- -0.35
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.06  
  ## res$value[which(res$component=="loss%") & res$mon %in% c(6,7,8,9)]   <- -0.065 
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.045
  ## res$value[which(res$component=="loss%" & res$mon %in% c(6,7,8,9) & res$bucket=="7X8")] <- -0.055
  ## res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.07
  ## res$value[which(res$component=="loss%" & res$mon %in% c(6,7,8,9) & res$bucket=="7X8")] <- -0.055

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  
  res
}



#########################################################################
# Customize Newcastle
#
fwd.PMview.4129 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=NULL)
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)
  
  res <- PM:::.extendPeriodically(res, toMonths)

  # take extra 0.25 off
  ind <- which(res$component=="congestion" & res$bucket=="5X16")
  res$value[ind] <- res$value[ind] - 0.25
  
  # loss factors 
  ind <- which(res$component=="loss%")
  res$value[ind] <- -0.05               # be a bit conservative
  
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Bucksport.  We have CinCapV marked at Bucksport $-something
#
fwd.PMview.4140 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- -0.85  # long term
  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -3.5    
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -1.5

  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.08       
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.07
  res$value[which(res$component=="loss%") & res$mon %in% 10]   <- -0.055 
  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Winslow
#
fwd.PMview.4142 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=NULL)
{
  fwd.PMview.4172(dp, toMonths, noSims=10000,
    skipDates=NULL)
}


#########################################################################
# Customize GUILFORD and Greenville
#
fwd.PMview.4150 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- -0.5  # long term
  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -2.5    
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -1.0

  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.06 
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.05
  res$value[which(res$component=="loss%") & res$mon %in% c(10,11)]   <- -0.04 
  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Bigelow
#
fwd.PMview.4153 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=NULL)
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)
  
  res <- PM:::.extendPeriodically(res, toMonths)

  # loss factors 
  ind <- which(res$component=="loss%")
  res$value[ind] <- -0.083               # be a bit conservative
  
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Scott Paper (Sappi), SD Warren
# Make ptid = 1107, SCOT_PAP.SOMR DA the same in the hardcoded page!
#
fwd.PMview.4156 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- -0.0  # long term
  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -0.0    
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -0.0

  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.025 
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.03
  #res$value[which(res$component=="loss%") & res$mon %in% c(10,11)]   <- -0.04 
  
  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}




#########################################################################
# Customize Gulf Island
#
fwd.PMview.4162 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=NULL)
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)
  
  res <- PM:::.extendPeriodically(res, toMonths)

  # loss factors 
  ind <- which(res$component=="loss%")
  res$value[ind] <- -0.05               # be a bit conservative
  
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Norway
#
fwd.PMview.4170 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=NULL)
{
  fwd.PMview.4172(list(ptid=4172, nodeType="GEN"), toMonths, noSims=noSims,
    skipDates=skipDates)
}


#########################################################################
# Customize Hotel_Rd
#
fwd.PMview.4171 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=NULL)
{
  fwd.PMview.4172(list(ptid=4172, nodeType="GEN"), toMonths, noSims=noSims,
    skipDates=skipDates)
}


#########################################################################
# Customize Lewstn
#
fwd.PMview.4172 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=NULL)
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)
  
  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- -0.15  # long term
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- -0.05  # long term
  res$value[which(res$component=="congestion" & res$bucket=="7X8")]  <- -0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$year > 2012)] <- -0.0  # Pt.Lepreau
  res$value[which(res$component=="congestion" & res$bucket=="7X8"  & res$year > 2012)] <- -0.0  
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$year > 2012)] <- -0.0  

  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -0.75    
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -0.25
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.0275
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.0275
  res$value[which(res$component=="loss%") & res$mon %in% c(10,11)]   <- -0.0375
  
  res$value[which(res$component=="loss%" & res$year == 2013)] <- -0.0275 
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year == 2013)] <- -0.02
  
  res$value[which(res$component=="loss%" & res$year == 2014)] <- -0.025 
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year == 2013)] <- -0.02

  res$value[which(res$component=="loss%" & res$year == 2015)] <- -0.0225 
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year == 2015)] <- -0.02  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Quaker Hill
#
fwd.PMview.4190 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=NULL)
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)
  
  res <- PM:::.extendPeriodically(res, toMonths)

  # take extra 0.40 off
  ind <- which(res$component=="congestion")
  res$value[ind] <- res$value[ind] - 0.40
  
  # loss factors 
  ind <- which(res$component=="loss%")
  res$value[ind] <- -0.02               # be a bit conservative
  
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Lewstn
#
fwd.PMview.4192 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=NULL)
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)
  
  res <- PM:::.extendPeriodically(res, toMonths)

  # loss factors 
  ind <- which(res$component=="loss%")
  res$value[ind] <- -0.03               # be a bit conservative
  
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Pawtucket
#
fwd.PMview.4217 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=NULL)
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=10000)
  
  res <- PM:::.extendPeriodically(res, toMonths)

  # loss factors have been increasing since 2006 ...
  ind <- which(res$component=="loss%")
  res$value[ind] <- -0.003               # be a bit conservative
  
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize INDSTLPK.4242 DA  -- New Bedford load in SEMA  
# 
#
fwd.PMview.4242 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion")]  <- 0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- 0.25  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(12,1,2))] <- 2.25
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(12,1,2))] <- 1.75
  res$value[which(res$component=="congestion" & res$bucket=="7X8"  & res$mon %in% c(12,1,2))] <- 1.75  
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- 0.005 
  #res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.00
  
 
  
  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize BEAVR_PD.4291 for F14  
# Milford - 0.10
#
fwd.PMview.4291 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion")]  <- 0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- 0.25  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(12,1,2))] <- 1.00
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(12,1,2))] <- 0.75
  res$value[which(res$component=="congestion" & res$bucket=="7X8")]  <- -0.00  
  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.00 
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.00
  
  #res$value[which(res$component=="loss%" & res$year == 2013)] <- -0.055 
  #res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$year == 2013)] <- -0.045
  
  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}



#########################################################################
# Customize BeanHill, we got an FTR from Plainfield to BeanHill
#
fwd.PMview.4475 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion")] <- 0  # no cong long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(1,2,12))] <- -0.40
  
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in%  c(1,2,12))] <- -0.40
 
  res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- 0.00

  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.01
  ## res$value[which(res$component=="loss%" & res$bucket=="5X16"  & res$mon %in% c(6,7,8))] <- -0.0125
  ## res$value[which(res$component=="loss%" & res$bucket=="2X16H" & res$mon %in% c(6,7,8))] <- -0.0125
  ## res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.0125
  ## res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$mon %in% c(6,7,8))] <- -0.02
  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}




#########################################################################
# Customize Newton 4885
#
fwd.PMview.4885 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion")] <- 0  # no cong long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$year %in% 2011:2013)] <- 0.50
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$year %in% 2013)] <- 0.40
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$year %in% 2014)] <- 0.30
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$year %in% 2015)] <- 0.20
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$year %in% 2016)] <- 0.10
  
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$year %in% 2011:2013)] <- 0.10
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$year %in% 2013:2014)] <- 0.10
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$year %in% 2014:2015)] <- 0.05

  res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- 0.05

  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.001
  ## res$value[which(res$component=="loss%" & res$bucket=="5X16"  & res$mon %in% c(6,7,8))] <- -0.0125
  ## res$value[which(res$component=="loss%" & res$bucket=="2X16H" & res$mon %in% c(6,7,8))] <- -0.0125
  ## res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.0125
  ## res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$mon %in% c(6,7,8))] <- -0.02
  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}



#########################################################################
# Customize Tunnel
#
fwd.PMview.10034 <- function(dp, toMonths=get_toMonths(), noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion")] <- 0  # no cong long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$year %in% 2011:2013)] <- 0.50
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$year %in% 2013)] <- 0.40
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$year %in% 2014)] <- 0.30
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$year %in% 2015)] <- 0.20
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$year %in% 2016)] <- 0.10
  
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$year %in% 2011:2013)] <- 0.10
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$year %in% 2013:2014)] <- 0.10
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$year %in% 2014:2015)] <- 0.05

  res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- -0.05

  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.0035
  res$value[which(res$component=="loss%" & res$bucket=="5X16"  & res$mon %in% c(6,7,8))] <- -0.0125
  res$value[which(res$component=="loss%" & res$bucket=="2X16H" & res$mon %in% c(6,7,8))] <- -0.0125
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.0125
  res$value[which(res$component=="loss%" & res$bucket=="7X8" & res$mon %in% c(6,7,8))] <- -0.02
  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Cape Wind.  Barnstable is not a pnode anymore. 
# use Oak St.
fwd.PMview.10571 <- function(dp, toMonth, noSims=10000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2008-12-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)
  
  res <- PM:::.extendPeriodically(res, toMonths)
  rownames(res) <- NULL
  
  # take congestion down about a buck after 2016 based on UPLAN
  # after the HVDC line is in
  ind <- which(res$component=="congestion" & res$month >= "2016-01-01")
  res$value[ind] <- res$value[ind] - 1
  
  # loss factors will get lower after Cape is added
  ind <- which(res$component=="loss%" & res$month >= "2011-01-01")
  res$value[ind] <- -0.01
  ind <- which(res$component=="loss%" & res$month >= "2015-01-01")
  res$value[ind] <- -0.15    # if some HVDC lines get built
  
  return(res) 
}


#########################################################################
# Customize Colburn, NEMA
#
fwd.PMview.10831 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  # mark it like Newton
  res <- fwd.PMview.4885(list(ptid=4885, nodeType="LOAD"), toMonths,
    noSims=noSims, skipDates=skipDates)

  res$value[which(res$component=="loss%")] <- 0.0025

  res
}


#########################################################################
# Customize Kibby
#
fwd.PMview.12551 <- function(dp, toMonths=get_toMonths(), noSims=2000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion")]  <- -0.00  # long term
  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5,10))] <- -0.00    
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5,10))] <- -0.00

  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.055
  ## res$value[which(res$component=="loss%" & res$bucket=="7X8")]   <- -0.003
  ## res$value[which(res$component=="loss%" & res$bucket=="5X16")]  <- 0.0095
  ## res$value[which(res$component=="loss%" & res$bucket=="2X16H")] <- 0.0075  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL

  res   
}



#########################################################################
# Customize Pemigwas, in NH!
#
fwd.PMview.14211 <- function(dp, toMonths=get_toMonths(), noSims=2000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion")]  <- -0.05  # long term
  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5,10))] <- -0.40    
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5,10))] <- -0.30

  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.0
  res$value[which(res$component=="loss%" & res$bucket=="7X8")]   <- -0.003
  res$value[which(res$component=="loss%" & res$bucket=="5X16")]  <- 0.0095
  res$value[which(res$component=="loss%" & res$bucket=="2X16H")] <- 0.0075  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL

  res   
}

#########################################################################
# Customize Kleen
#
fwd.PMview.14614 <- function(dp, toMonths=get_toMonths(), noSims=2000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(4491, nodeType=dp$nodeType,    # <-- FIX ME (until we have 1 year of Kleen)
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion")] <- 0.0  # before Cal 14
  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% 5:9)] <- 0.2
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(7,8))] <- 0.4
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$mon %in% c(7,8))] <- 0.15
  res$value[which(res$component=="congestion" & res$bucket=="7X8")] <- 0.0

  
  # loss factors 
  res$value[which(res$component=="loss%")] <- 0.0  # before Cal 14
  res$value[which(res$component=="loss%" & res$bucket=="5X16")]  <- -0.005 
  res$value[which(res$component=="loss%" & res$bucket=="2X16H")] <- -0.005
  res$value[which(res$component=="loss%" & res$bucket=="7X8")]   <- -0.015

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}



#########################################################################
# Customize Stetson.  
#
fwd.PMview.15464 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(dp$ptid, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- -0.25  # long term
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- -0.15  # long term
  res$value[which(res$component=="congestion" & res$bucket=="7X8")]  <- -0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$year > 2012)] <- -0.5  # Pt.Lepreau
  res$value[which(res$component=="congestion" & res$bucket=="7X8"  & res$year > 2012)] <- -0.25  
  res$value[which(res$component=="congestion" & res$bucket=="2X16H" & res$year > 2012)] <- -0.35  


  # assume EW congestion during Apr-May, Oct
  res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -1.00    
  res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -0.50

  
  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.08   # pushed it down 1% on 12/20/2013 for the MTM nazis!
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.065
  res$value[which(res$component=="loss%") & res$mon %in% c(10,11)]   <- -0.055
  res$value[which(res$component=="loss%") & res$mon %in% c(1,2,6,7,8)] <- -0.10
  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Plainfield CT.  Start point for FTR
#
fwd.PMview.15509 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  # mark it like BeanHill!
  fwd.PMview.4475(list(ptid=4475, nodeType="GEN"), toMonths,
    noSims=noSims, skipDates=skipDates)
}

#########################################################################
# Customize Berlin Burgress wood plant
#
fwd.PMview.16653 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  res <- makePMview(350, nodeType=dp$nodeType,
    skipDates=skipDates, noSims=noSims)

  res <- PM:::.extendPeriodically(res, toMonths)
  res$year <- as.numeric(format(res$month, "%Y"))
  res$mon  <- as.numeric(format(res$month, "%m"))

  # congestion
  res$value[which(res$component=="congestion" & res$bucket=="5X16")]  <- 0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="2X16H")] <- 0.0  # long term
  res$value[which(res$component=="congestion" & res$bucket=="7X8")]   <- 0.0  # long term
  #res$value[which(res$component=="congestion" & res$bucket=="5X16" & res$mon %in% c(4,5))] <- -1.00    
  #res$value[which(res$component=="congestion" & res$bucket=="2X16H"& res$mon %in% c(4,5))] <- -0.50

  # loss factors 
  res$value[which(res$component=="loss%")] <- -0.025   
  res$value[which(res$component=="loss%" & res$bucket=="7X8")] <- -0.0225
  #res$value[which(res$component=="loss%") & res$mon %in% c(10,11)]   <- -0.05 
  

  res$year <- res$mon <- NULL
  rownames(res) <- NULL
  return(res) 
}


#########################################################################
# Customize Rollins Wind.  
#
fwd.PMview.37175 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  # mark it like Stetson!
  fwd.PMview.15464(list(ptid=15464, nodeType="GEN"), toMonths,
    noSims=noSims, skipDates=skipDates)
}



#########################################################################
# ForeRiver1
#
fwd.PMview.40327 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  # mark it like old one
  fwd.PMview.1691(list(ptid=1691, nodeType="GEN"), toMonths,
    noSims=noSims, skipDates=skipDates)
}



#########################################################################
# Customize Rollins Wind.  
#
fwd.PMview.40328 <- function(dp, toMonths=get_toMonths(), noSims=1000,
  skipDates=seq(as.Date("2003-01-01"), as.Date("2007-05-31"), by="1 day"))
{
  # mark it like Stetson!
  fwd.PMview.1691(list(ptid=1691, nodeType="GEN"), toMonths,
    noSims=noSims, skipDates=skipDates)
}


#########################################################################
# Customize GRAHAM.40338 DA the current MIS as the load 4078
#
fwd.PMview.40338 <- function(...)
{
  fwd.PMview.4078(...)
}










##   # get historical settle prices for congestion relative to Hub
##   endDate   <- as.Date(format(Sys.Date(), "%Y-%m-01"))-1  # end of current month
##   startDate <- seq(endDate+1, by="-12 months", length.out=2)[2] # last 1 years
##   symbol   <- paste("nepool_smd_da_", ptid,
##     "_congcomp - nepool_smd_da_4000_congcomp", sep="")
##   aux <- ntplot.bucketizeByMonth(symbol, c("5X16"), startDate, endDate)
##   hSP <- data.frame(cast(aux, month ~ bucket, I, fill=NA))
##   hSP <- zoo(hSP[,-1], hSP[,1])
##   hSP <- hSP[, c("X5X16")]
##   lasthSP <- mean(tail(hSP,3))    # last 3 months!

##   ind <- which(res$component=="congestion" & res$bucket=="5X16")
##   CC <- res[ind,]
##   fwdMean  <- mean(CC$value[1:12])
##   CC$value <- CC$value - fwdMean + lasthSP  # rescale to last value
  
##   bux <- PM:::.expDecay(CC, tail(index(hSP),1), 5/365, fwdMean,
##                  tVariable="month", value="value")
##   #plot(res[ind,"value"], type="o", ylim=c(-5,-2));lines(bux$value, col="blue")
##   res[ind,] <- bux
  
