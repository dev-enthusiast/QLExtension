# Utilities to deal with node marking in SecDb
#
# NodalView
# .read_ExternalData_csv_file
# .read_ExternalData_xlsx_file
# update_nodalview_archive
# .write_ExternalData_file
#
#


###################################################################
# A constructor for a nodal view, to be used for archiving or to
#   constructNodeMark.
# Where view is a data.frame with columns c("month", "bucket",
#   "component", "value") where component has c("congestion", "loss%")
#   calculated relative to Hub
#
# 
NodalView <- function(ptid, delivPt, view, 
   method="PM", lastUpdate=Sys.Date(), nodeType="GEN",
   comments="")
{
  X <- list()
  X$ptid    <- ptid
  X$delivPt <- delivPt    # SecDb delivPt name
  if (method %in% c("PM", "UPLAN")) {
    X$method  <- method
  } else {
    stop("Unknown method to construct the node!")
  }
  X$lastUpdate <- lastUpdate
  X$nodeType   <- nodeType
  X$comments   <- comments
  
  # add the nodeView
  if (!all(c("loss%", "congestion") %in% unique(view$component)))
    stop("Don't have loss% or congestion component in nodeView!")
  X$view <- view
  
  structure(X, class="NodalView")
}


###################################################################
# return a list of data.frames with the node spread to parent
# 
.read_ExternalData_csv_file <- function(filename)
{
  x <- read.csv(filename, header=FALSE, skip=2)
  x <- x[-(2:3), ]
  x <- x[,-2]
  colnames(x) <- x[1,]
  x <- x[-1,]
  colnames(x)[1] <- "month"
  x[,1] <- as.Date(x[,1], "%m/%d/%Y")

  aux <- melt(x, id=1)
  aux$value <- as.numeric(aux$value)
  aux$bucket   <- gsub("(2X16H|5X16|7X8) .*", "\\1", aux$variable)
  aux$location <- gsub("(2X16H|5X16|7X8) (.*)", "\\2", aux$variable)
  res <- dlply(aux, .(location), function(x){
    aux <- data.frame(cast(x, month ~ bucket, I, fill=NA))
    colnames(aux) <- gsub("^X", "", colnames(aux))
    aux
  })

  res
}


###################################################################
# return a list of data.frames with the node spread to parent
#
.read_ExternalData_xlsx_file <- function(filename, sheetName="Spreads")
{
  require(xlsx)
  wb     <- loadWorkbook(filename)
  sheets <- getSheets(wb)

  sheet  <- sheets[[sheetName]]
  cnames <- readRows(sheet, startRow=3, endRow=3, startColumn=1)
  cnames <- as.vector(cnames)

  months <- readColumns(sheet, startColumn=1, endColumn=1, startRow=6,
    header=FALSE, colClasses="Date")
  colnames(months) <- "month"
    
  SS <- readColumns(sheet, startColumn=3, endColumn=length(cnames), startRow=6,
    header=FALSE, colClasses="numeric")
  if (any(is.na(SS)))
    stop("You have NAs in the data.  Please check!")
  colnames(SS) <- toupper(cnames[-(1:2)])
  SS <- cbind(months, SS)  # add the months

  # make the list
  aux <- melt(SS, id=1)
  aux$bucket   <- gsub("(2X16H|5X16|7X8) .*", "\\1", aux$variable)
  aux$location <- gsub("(2X16H|5X16|7X8) (.*)", "\\2", aux$variable)
  res <- dlply(aux, .(location), function(x){
    aux <- data.frame(cast(x, month ~ bucket, I, fill=NA))
    colnames(aux) <- gsub("^X", "", colnames(aux))
    aux
  })
  
  res
}


###################################################################
# Where X is a list of NodalView 
#
update_nodalview_archive <- function(X, do.save=FALSE)
{
  DIR <- "S:/All/Structured Risk/NEPOOL/Marks/Marks Automation/"
  
  # archive existing file
  filePMview <- paste(DIR, "PM_node_view.RData", sep="")
  if (file.exists(filePMview)){
    load(filePMview)   
    archiveFile <- paste(DIR, "Archive/PM_node_view_", Sys.Date(), 
                         ".RData", sep="")
    file.copy(filePMview, archiveFile)
    rLog("Archived", archiveFile)
  }

  for (nv in X) {
    if (nv$delivPt %in% names(pmView)) {
      rLog("Overwritting existing pmView for", nv$delivPt)
    } else {
      rLog("Adding", nv$delivPt)
    }

    if (class(nv) != "NodalView")
      stop("You're doing the wrong thing!")
    pmView[[nv$delivPt]] <- nv
  }
  
  # save the new PM calibration file
  if (do.save) {
    fileOut <- paste(DIR, "PM_node_view.RData", sep="")
    save(pmView, file=fileOut)
    rLog("Saved", fileOut)  
  }

  invisible()
}


###################################################################
# Where X is a list of data.frames with the names the SecDb delivPt
# Each data.frame has columns c("month", "2X16", "5X16", "7X8") with
#   the values representing the spread to parent(zone), so it's ready
#   to submit to SecDb
#
.write_ExternalData_file <- function(X, filePostfix="_AAD")
{
  # make a big data.frame by merging the lists
  nodes <- names(X)
  for (i in 1:length(X)) {
    if (i==1){
      aux <- X[[i]]
      colnames(aux)[2:4] <- paste(colnames(aux)[2:4], nodes[i])
      XX <- aux
    } else {
      aux <- X[[i]]
      colnames(aux)[2:4] <- paste(colnames(aux)[2:4], nodes[i])
      XX <- merge(XX, aux, by="month")
    }
  }

  # make the character matrix
  MM <- rbind(matrix("", ncol=ncol(XX), nrow=5), as.matrix(as.data.frame(XX)))
  MM <- cbind(matrix("", ncol=2, nrow=nrow(MM)), MM)
  MM[1, ] <- c("NEPOOL Marks Spreads", rep("", ncol(MM)-1))
  MM[2,1] <- format(Sys.Date(), "%m/%d/%Y")
  MM[3,]  <- c("Commods", colnames(MM)[-1])
  MM <- MM[,-3]
  MM[6:nrow(MM),1] <- format(XX$month, "%m/%d/%Y")
 
  fileName <- paste("S:/All/Structured Risk/NEPOOL/Marks/Sent Files/",
    "NEPOOL Marks Nodal Spreads ", format(Sys.Date(), "%m-%d-%y"), "_",
    filePostfix, ".csv", sep="") 
  write.table(MM, file=fileName, sep=",", col.names=FALSE, row.names=FALSE)
  rLog("Wrote file", fileName)
}



###################################################################
###################################################################
#
.test <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(plyr)
  require(reshape)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Marks/lib.mark.nodes.SecDb.R")

  asOfDate <- Sys.Date()-1
  filename <- paste("S:/All/Structured Risk/NEPOOL/Marks/GeneratorMarks/",
    "NEPOOL Nodal Basis 2012-09-04.xlsx", sep="")

  SS <- .read_ExternalData_xlsx_file(filename)
  lapply(SS, head)

  .write_ExternalData_file(SS, filePostfix="_TEST")
  

  filename <- paste("S:/All/Structured Risk/NEPOOL/Marks/Sent Files/",
    "NEPOOL Marks Nodal Spreads 09-19-12.csv", sep="")
  SS <- .read_ExternalData_csv_file(filename)
  lapply(SS, head)


  
  


  
}
