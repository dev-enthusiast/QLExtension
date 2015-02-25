# Procmon runs this file.
#
# The spreadsheet PM_node_mark.xls is used ONLY for the hardcoded nodes!  PM marked
# nodes work from the PM_node_view.RData.  You can manipulate it with  
# PM:::.rm_from_pmView_file and PM:::.add_to_pmView_file
#
# Not used anymore ...
# To refresh Matlab WN view,
#   - archive the S:\All\Structured Risk\NEPOOL\Marks\Marks Automation\matData.mat
#     file
#   - run H:\Matlabrunexe "markNepoolSpreads('R')" on a matlab server.
#
# To refresh PM view, call .refreshPMView() from 
#   Utilities/Interfaces/PM/R/libMarkNodePMView.R
#
# Matlab reads the "LIW: NEPOOL Spreads Data" container
#
# getNodesMarkedByPM
# getHardCodedNodes
# reshapeSpreadsDF
# writeCSVfile
#
#



##############################################################################
# Some nodes are marked by PM.  In the future more nodes will be marked
# this way.  
#
getNodesMarkedByPM <- function(asOfDate)
{
  rLog("\nGet the nodes marked by PM ...")

  dirMat <- "S:/All/Structured Risk/NEPOOL/Marks/Marks Automation/"
  filePMview <- paste(dirMat, "PM_node_view.RData", sep="")
  load(filePMview)

  rLog("Loading MAP_DP ... ")
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/MAP_DP.RData")
  
  startDate <- as.Date(secdb.dateRuleApply(asOfDate, "+1e+1d"))
  
  rLog("Mark", length(pmView), "delivery points with PM method")
  MM.PM <- vector("list", length=length(pmView))
  names(MM.PM) <- names(pmView)
  for (dp in names(pmView)){
    rLog("Working on", dp)
    aux <- constructNodeMark(pmView[[dp]]$view, asOfDate=asOfDate,
      startDate, endDate=as.Date("2018-12-01"))
    aux <- cbind(delivPt=dp, subset(aux, component=="nodeMark"))
    aux$component=NULL
    aux <- spreadToParent(aux, asOfDate=asOfDate,
                          maxMonth=as.Date("2018-12-01"),
                          MAP_DP=MAP_DP)
    
    MM.PM[[dp]] <- aux
  }
  MM.PM <- do.call(rbind, MM.PM)
    
  return(MM.PM)
}

##############################################################################
# There are some nodes that are marked fixed to another nodes (not necessarily
# with the same SecDb parent) see spreadsheet.
# Where marksRef are the marks already calculated for asOfDate.
# 
# Check that the end.dt in the xls matches with what you want!
#
getHardCodedNodes <- function(asOfDate, MM, matrix=FALSE)
{
  rLog("\nGet the hard coded nodes ...")
  dirMat <- "S:/All/Structured Risk/NEPOOL/Marks/Marks Automation/"
  fileIn <- paste(dirMat, "PM_node_mark.xls", sep="")
  X <- read.xlsx(fileIn, sheetIndex=2) 
  X <- subset(X, delivPt !="")    # you get some stray nodes
  
  sprdToParent  <- PM:::.expandSpreadRule(X, asOfDate)
  
  uDelivPt <- unique(sprdToParent[, c("delivPt", "parentDelivPt")])
  delivPtWithMarkedParents <- subset(uDelivPt, parentDelivPt %in% MM$delivPt) 
  delivPtWithoutMarkedParents <- subset(uDelivPt,!(parentDelivPt %in% MM$delivPt))
  
  # for the delivPts that have parents in MM already updated for asOfDate
  parentMark <- subset(MM, delivPt %in% delivPtWithMarkedParents$parentDelivPt)
  parentMark <- parentMark[,c("month", "bucket", "delivPt", "childMark")]
  names(parentMark)[3:4] <- c("parentDelivPt", "parentMark")

  aux <- subset(sprdToParent, delivPt %in% delivPtWithMarkedParents$delivPt)  
  nodeMarks.HC <- addSpreadToParent(aux, parentMark, matrix=FALSE) # Hard Coded ones
  nodeMarks.HC$parentDelivPt <- NULL   # use parents from SecDb for marking...
  if (nrow(nodeMarks.HC) > 0) {
    nodeSpreads.HC <- spreadToParent(nodeMarks.HC, asOfDate=asOfDate,
      maxMonth=as.Date("2018-12-01"))
  } else {
    nodeSpreads.HC <- NULL
  }

  
  # for the delivPts that don't have parents in MM already, they MUST be in SecDb
  # this includes all the zonals and the hub
  if (nrow(delivPtWithoutMarkedParents)>0){
    aux <- subset(sprdToParent, delivPt %in% delivPtWithoutMarkedParents$delivPt)
    nodesOther <- addSpreadToParent(aux, asOfDate=asOfDate, matrix=FALSE)
    names(nodesOther)[which(names(nodesOther)=="value")] <- "childMark"
    names(nodesOther)[which(names(nodesOther)=="addedValue")] <- "value"

    aux <- nodesOther
    aux$value  <- aux$childMark         # use parents from SecDb for marking...
    nodesOther <- spreadToParent(aux[,c("delivPt", "month", "bucket", "value")],
      asOfDate=asOfDate, maxMonth=as.Date("2018-12-01"))    
    nodeSpreads.HC <- rbind(nodeSpreads.HC, nodesOther)
  }
    
  # reshape if requested
  if (matrix)
    nodeSpreads.HC <- reshapeSpreadsDF(nodeSpreads.HC)
  
  return(nodeSpreads.HC)
}


##############################################################################
# Take a DF and reshape it into a matrix ready for submission.
# 
reshapeSpreadsDF <- function(MM)
{
  res <- cast(MM, month ~ bucket + delivPt, I, fill=NA)
  if ((ncol(res)-1)%%3!=0)
    stop("You're missing a bucket somewhere!")

  # order them by delivPt not by bucket
  res <- res[,c(1, as.numeric(matrix(2:ncol(res), byrow=TRUE, nrow=3)))]
  rnames <- format(res[,1], "%m/%d/%Y")  # the format for ExternalData!
  res  <- res[,-1]
  rownames(res) <- rnames
  cnames <- names(res)
  res <- as.matrix(data.frame(res))
  colnames(res) <- cnames
  colnames(res) <- sub("_{1}", " ", colnames(res)) # space after buckets
  
  return(res)
}


##############################################################################
# Given a matrix, write the CSV file for submission to External Data.
# 
writeCSVfile <- function(MM, asOfDate, fileName)
{
  rLog("Final table has", ncol(MM), "columns.")

  MM <- rbind(matrix("", ncol=ncol(MM), nrow=5), MM)
  MM <- cbind(matrix("", ncol=2, nrow=nrow(MM)), MM)
  MM[1, ] <- c("NEPOOL Marks Spreads", rep("", ncol(MM)-1))
  MM[2,1] <- format(asOfDate, "%m/%d/%Y")
  MM[3,]  <- c("Commods", colnames(MM)[-1])

  MM[6:nrow(MM),1] <- rownames(MM)[6:nrow(MM)]

  # write the output to a csv for submission ...
  if (missing(fileName)){
    fileName <- paste("S:/All/Structured Risk/NEPOOL/Marks/Sent Files/",
    "NEPOOL Marks Nodal Spreads ", format(asOfDate, "%m-%d-%y"), ".csv", sep="")
  }

  write.table(MM, file=fileName, sep=",", col.names=FALSE, row.names=FALSE)
  rLog("Wrote file", fileName)

  return(invisible(MM))
}


##############################################################################
##############################################################################
options(stringsAsFactors=FALSE)
#require(xlsReadWrite)
require(reshape)
require(CEGbase); require(SecDb); require(PM)
require(xlsx)
asOfDate <- Sys.Date() # you can only send marks for current day!
userName <- Sys.getenv("username")
rLog(paste("Submitting marks for:", asOfDate))
returnCode <- 0        

# if run before 6pm, make if fail so it reruns
hour <- format(Sys.time(), "%H")
if (hour < 18)
  returnCode <- 1   # FAIL



# read the nodes marked by PM
MM <- getNodesMarkedByPM(asOfDate)

 
# add the hardcoded spreads 
MM.hardCoded <- getHardCodedNodes(asOfDate, MM, matrix=FALSE)
MM <- subset(MM, !(delivPt %in% unique(MM.hardCoded$delivPt))) # HardCoded overwites everything
MM <- rbind(MM, MM.hardCoded)


# reshape the spreads into a matrix ready for submission
res <- reshapeSpreadsDF(MM)
# res <- res[,grep(" GRAHAM.OLDT DA", colnames(res))]

# write the Matrix of marks to CSV
fileName <- paste("S:/All/Structured Risk/NEPOOL/Marks/Sent Files/",
  "NEPOOL Marks Nodal Spreads ", format(asOfDate, "%m-%d-%y"), "H",
  hour, ".csv", sep="")
writeCSVfile(res, asOfDate, fileName)

if (file.exists(fileName)){
  rLog("Sending email...")
  subject <- paste("Emailing: NEPOOL Marks Nodal Spreads ",
  format(asOfDate, "%m-%d-%y"), ".csv", sep="")
  body    <- paste(userName, "is sending", fileName, "to ExternalData.")
  blatVal <- sendEmail(from=paste("adrian.dragulescu@constellation.com", sep=""),
    to="External.Data2@constellation.com",
#    to="adrian.dragulescu@constellation.com",
    subject, body, attachments=fileName)
}

if (blatVal != 0){
  returnCode <- 1   # it failed
  rLog("\n\nEmail NOT sent! Email NOT sent!")
}
rLog("Closing this window in 10 seconds ...")
Sys.sleep(10)


if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}












# read Matlab WN output -- no longer used 12/26/2011
# MM <- try(readMatWNOutput(asOfDate, matrix=FALSE, maxMonth=as.Date("2018-12-01")))

# add the nodes marked by PM
## MM.PM <- getNodesMarkedByPM(asOfDate)
## if (class(MM) != "try-error") {
##   MM <- subset(MM, !(delivPt %in% unique(MM.PM$delivPt))) # PM view overwrites WN
##   MM <- rbind(MM, MM.PM)  # append the PM ones
## } else {
##   MM <- MM.PM
## }



## ##############################################################################
## # Read Matlab WN output.  If not there, try to run Matlab locally.
## # Return a matrix with the spreads to zone. 
## # If marks=TRUE, return the new marks by adding the parent marks (which should
## # be already updated by Bucht)
## #
## readMatWNOutput <- function(asOfDate, userName=Sys.getenv("username"),
##   matrix=FALSE, maxMonth=as.Date("2017-12-01"))
## {
##   rLog(paste("Read Matlab WeatherNorm file ..."))

##   dateMat  <- format(asOfDate, "%d%b%y")
##   fileNameXls <- paste(dateMat, "Spreads.xls", sep="")
##   fileNameXlsx <- paste(dateMat, "Spreads.xlsx", sep="")
##   dirMat   <- "S:/All/Structured Risk/NEPOOL/Marks/Marks Automation/"
##   fileMatXls  <- paste(dirMat, fileNameXls,  sep="")
##   fileMatXlsx <- paste(dirMat, fileNameXlsx, sep="")
  
##   if (file.exists(fileMatXls)) {   # old R 2.10.1 way, prior to 2011/11/15
##     require(xlsReadWrite)
##     options(warn=-1)
##     MM <- read.xls(fileMatXls, sheet=1)
##     options(warn=0)
##     cnames <- MM[2, 3:ncol(MM)]
##     rnames <- format(as.Date(as.numeric(MM[5:nrow(MM),1]), origin="1899-12-30"),
##                          "%m/%d/%Y")
##     MM <- as.numeric(unlist(MM[5:nrow(MM),3:ncol(MM)]))
##     MM <- matrix(MM, ncol=length(cnames), nrow=length(rnames),
##                   dimnames=list(rnames, cnames))
##   } else if (file.exists(fileMatXlsx)) {
##     require(xlsx)
##     MM <- read.xlsx(fileMatXlsx, sheetIndex=1, header=FALSE)
##     cnames <- MM[3, 2:ncol(MM)]
##     rnames <- format(as.Date(as.numeric(MM[4:nrow(MM),1]), origin="1899-12-30"),
##                          "%m/%d/%Y")
##     MM <- as.numeric(unlist(MM[4:nrow(MM),2:ncol(MM)]))
##     MM <- matrix(MM, ncol=length(cnames), nrow=length(rnames),
##                   dimnames=list(rnames, cnames))
##   } else {
##     rLog(paste("Cannot find the WN matlab file:", fileMatXlsx))
##   }
  
##   # delete cash month from the WN output
##   if (rnames[1]==format(asOfDate, "%m/01/%Y"))
##     MM <- MM[-1,]

##   if (!matrix){
##     aux  <- melt(MM)
##     indS <- as.numeric(regexpr(" ", aux$X2))
##     aux$bucket  <- toupper(substr(aux$X2, 1, indS-1))
##     aux$delivPt <- toupper(substr(aux$X2, indS+1, nchar(as.character(aux$X2))))
##     aux$month   <- as.Date(aux$X1, format="%m/%d/%Y")
##     aux <- aux[, c("delivPt", "month", "bucket", "value")]
    
##     # extend the matlab marks periodically into the future
##     aux <- PM:::.extendPeriodically(aux, seq(min(aux$month), maxMonth, by="1 month"))
    
##     if (!exists("MAP_DP"))
##       data(MAP_DP)
##     aux <- merge(aux, MAP_DP[,c("delivPt", "parentDelivPt")], all.x=TRUE)
##     if (any(is.na(aux$parentDelivPt)))
##       rError("Cannot find a parent for delivPt: ",
##              unique(aux$delivPt[which(is.na(aux$parentDelivPt))]))
    
    
##     MM <- addSpreadToParent(aux, asOfDate=asOfDate, matrix=FALSE)
##     names(MM)[which(names(MM)=="value")] <- "childMark"
##     names(MM)[which(names(MM)=="addedValue")] <- "value"    
##   }
  
##   return(MM)
## }




  ## if (!file.exists(fileMat) & toupper(userName)!="PROCMON"){
  ##   rLog("I'll try running marknepoolspreads('U') on your local machine.")
  ##   # run Matlab in batch.  You can only run scripts in the home dir!
  ##   writeLines(c("marknepoolspreads('U');","exit;"), con="H:/runMat.m")
  ##   system('matlab -nodesktop -nosplash -r runMat')
  ##   unlink("H:/runMat.m")
  ##   rLog("Matlab output written to S:/All/Structured Risk/NEPOOL/Marks/Marks Automation/")
  ## 



## fileName <- paste("S:/All/Structured Risk/NEPOOL/Marks/Sent Files/",
##   "NEPOOL Marks Nodal Spreads ", format(asOfDate, "%m-%d-%y"), "_AAD1.csv",
##   sep="")
## writeCSVfile(MM.hardCoded, asOfDate, fileName)

#buckLocs <- colnames(MM)
## # get the mapped nodes and their parents
## MAP <- read.xls(paste(dirMat, "Equivalent Nodes.xls", sep=""), sheet=2)
## for (r in 1:nrow(MAP))
## {
##   ind <- grep(MAP$Parent[r], buckLocs)
##   if (length(ind)!=3)
##     stop("Cannot find", MAP$Parent[r],
##          "in the columns of the Matlab output file!")

##   aux <- MM[,ind]
##   aux[2,] <- gsub(MAP$Parent[r], MAP$Child[r], aux[2,])
##   MM <- cbind(MM, aux)
## }
