# Check which NEPOOL nodes have not been mapped by Ops for inc/decs.
# This job is necessary only because Ops map nodes one by one, so they
# need to be notified if you put a position at an unmapped node.
# Yes, they could map all nodes but did not want to do it. 
#
#
# Written by Adrian Dragulescu on 20-Jan-2010


#################################################################
# search names by space insensitive!
#
.get_unmapped_nodes <- function(nodes, filemap)
{
  nodesNotMapped <- NULL
  
  rLog("Reading ", filemap)
  nodesMapped <- read.xlsx(filemap, sheetIndex=1)
  nodesMapped$nameWoSpaces <- gsub(" ", "", nodesMapped$MappedNodes)

  nodesDF <- data.frame(nodes=nodes, nameWoSpaces=gsub(" ", "", nodes))
  allNodes <- merge(nodesDF, nodesMapped, all=TRUE)
  
  ind <- which(is.na(allNodes$MappedNodes))
  if (length(ind)>0){
    nodesNotMapped <- allNodes$nodes[ind]
    rLog("Adding\n", paste(nodesNotMapped, sep="", collapse="\n"),
       "\nto ", filemap, sep="")
    allNodes$MappedNodes[ind] <- allNodes$nodes[ind]

    write.xlsx(allNodes[,c("MappedNodes", "Info")], file=filemap,
      sheetName="Sheet1", row.names=FALSE)
    rLog("Wrote", filemap)
  }
  
  return(nodesNotMapped)
}

#################################################################
#
.read_nodes_filename <- function(filename)
{
  con    <- odbcConnectExcel(filename)
  data   <- sqlFetch(con, "Inputs")
  odbcCloseAll()

  data <- data[,1:9]
  data <- data[-which(is.na(data[,1])),]

  nodes <- sort(unique(data[,1]))

  return(nodes)
}


#################################################################
options(width=200)  # make some room for the output
options(stringsAsFactors=FALSE) 
require(methods)   
require(CEGbase); require(xlsx); require(RODBC)
Sys.setenv(tz="")

source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/lib.inc-dec.R")

rLog(paste("Start proces at ", Sys.time()))
returnCode <- 0    # succeed = 0

asOfDate <- Sys.Date()+1
rLog(paste("Running report for", as.character(asOfDate)))
rLog("\n")

err <- out <- NULL
filemap <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/Nepool Nodes Status.xlsx"
emailFrom <- "PM_AUTOMATIC_CHECK@constellation.com"
emailTo <- paste(c(
  "craig.hopmann@constellation.com",
  "david.riker@constellation.com", "nicholas.lombardi@constellation.com",   
  "necash@constellation.com", "robert.douglass@constellation.com"),
  sep="", collapse=",")
#emailTo <- "adrian.dragulescu@constellation.com"

filename <- .get_last_incdec_filename(asOfDate)    # from lib.inc-dec
rLog("File for tomorrow:", filename)

if (is.null(filename)){
  err <- "Cannot find the latest incdec file!"
  sendEmail(emailFrom, "adrian.dragulescu@constellation.com", err, err)
} else {
  rLog("Reading incdec file", filename)
  nodes <- .read_nodes_filename(filename)

  nodesNotMapped <- .get_unmapped_nodes(nodes, filemap)
  if (length(nodesNotMapped)>0){
    out <- c("Following nodes need to be mapped:", nodesNotMapped)
    sendEmail(emailFrom, emailTo, "Nepool inc/dec nodes to map", out)
  } else {
    rLog("No nodes need to be mapped!")
  }
}


rLog(paste("Done at ", Sys.time()))

returnCode <- 0        # make it succeed.  
if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}


