##################################################################
# Write the output of the pmMark to ExternalData file format.
#   X - is a list of pmMark as returned by constructNodeMark
#
#
writePMViewToExternalData <- function(X, asOfDate=Sys.Date(),
  maxMonth=as.Date("2019-12-01"), save=TRUE, filePostfix="_AAD")
{
  rLog("DEPRECATED:  Start using NEPOOL/Marks/lib.mark.nodes.SecDb.R")
  
  MM <- vector("list", length=length(X))
  names(MM) <- names(X)
  for (dp in names(X)){
    rLog("Working on", dp)
    #aux <- constructNodeMark(X[[dp]], asOfDate=asOfDate,
    #  startDate, endDate=maxMonth)
    aux <- cbind(delivPt=dp, subset(X[[dp]], component=="nodeMark"))
    aux$component=NULL
    aux <- spreadToParent(aux, asOfDate=asOfDate, maxMonth=maxMonth)
    
    MM[[dp]] <- aux
  }
  MM <- do.call(rbind, MM)

  
  # reshape to a matrix
  MM <- cast(MM, month ~ bucket + delivPt, I, fill=NA)
  if ((ncol(MM)-1)%%3!=0)
    stop("You're missing a bucket somewhere!")
  # order them by delivPt not by bucket
  MM <- MM[,c(1, as.numeric(matrix(2:ncol(MM), byrow=TRUE, nrow=3)))]
  rnames <- format(MM[,1], "%m/%d/%Y")  # the format for ExternalData!
  MM  <- MM[,-1]
  rownames(MM) <- rnames
  cnames <- names(MM)
  MM <- as.matrix(data.frame(MM))
  colnames(MM) <- cnames
  colnames(MM) <- sub("_{1}", " ", colnames(MM)) # space after buckets


  rLog("Final table has", ncol(MM), "columns.")
  MM <- rbind(matrix("", ncol=ncol(MM), nrow=5), MM)
  MM <- cbind(matrix("", ncol=2, nrow=nrow(MM)), MM)
  MM[1, ] <- c("NEPOOL Marks Spreads", rep("", ncol(MM)-1))
  MM[2,1] <- format(asOfDate, "%m/%d/%Y")
  MM[3,]  <- c("Commods", colnames(MM)[-1])
  MM[6:nrow(MM),1] <- rownames(MM)[6:nrow(MM)]

  # write the output to a csv for submission ...
  if (save){
    fileName <- paste("S:/All/Structured Risk/NEPOOL/Marks/Sent Files/",
      "NEPOOL Marks Nodal Spreads ", format(asOfDate, "%m-%d-%y"), "_",
      filePostfix, ".csv", sep="")
  
    write.table(MM, file=fileName, sep=",", col.names=FALSE, row.names=FALSE)
    rLog("Wrote file", fileName)
  }
  
}


##################################################################
# X a data frame with marks relative to zone.  
#
writeExternalDataFile <- function(X, marksForDate, 
  maxMonth=as.Date("2017-12-01"), save=TRUE, filePostfix="_AAD", 
  email=FALSE)
{
  if (any(!(c("month", "bucket", "deliv.pt", "value") %in% names(X))))
    stop("Missing one of 'month', 'bucket', 'deliv.pt', 'value' columns in X.")

  # cast the data, and prepare for write.csv
  X <- cast(X, month ~ bucket + deliv.pt, I, fill=NA)
  if (ncol(X)%%3 != 1)
    stop("Something is wrong.  You don't have all buckets for all locations.")
  names(X) <- sub("_{1}", " ", names(X)) # put a space after the buckets

  # abye use PM:::.extendPeriodically
  if (max(X$month) <= maxMonth){
    toMonths <- seq(min(X$month), maxMonth, by="month")
    X <- PM:::.extendPeriodically(X, toMonths)
  }
  X <- X[order(X$month), ]  # make sure you're ordered
  
  # make the csv file
  MM <- matrix("", nrow=(nrow(X)+5), ncol=(ncol(X)+1))
  MM[6:nrow(MM), 3:ncol(MM)] <- as.matrix(X[,-1])
  MM[6:nrow(MM), 1] <- format(X[,1], "%m/%d/%Y")
  MM[1,1] <- "NEPOOL Marks Spreads"
  MM[2,1] <- format(marksForDate, "%m/%d/%Y")
  MM[3,]  <- c("Commods", "", names(X)[-1])

  # write the output to a csv for submission ...
  rootdir <- "S:/All/Structured Risk/NEPOOL/Marks/Sent Files/"
  files <- list.files(path=rootdir, pattern=format(marksForDate, "%m-%d-%y"))
  ind <- grep("AAD", files)  # how many automatic files are there?
  fileOut <- paste("S:/All/Structured Risk/NEPOOL/Marks/Sent Files/",
    "NEPOOL Marks Nodal Spreads ", format(marksForDate, "%m-%d-%y"),
    "_AAD", length(ind), ".csv", sep="")
  write.table(MM, file=fileOut, sep=",", col.names=FALSE, row.names=FALSE)
  rLog("Wrote file to: ", fileOut)
  
  if (email){
    rLog("Sending email...")
    subject <- paste("Emailing: NEPOOL Marks Nodal Spreads ",
    format(asOfDate, "%m-%d-%y"), ".csv", sep="")
    body    <- paste(userName, "is sending", fileOut, "to ExternalData.")
    sendEmail(from=paste("adrian.dragulescu@constellation.com", sep=""),
      to="External.Data2@constellation.com", subject, body,
      attachments=fileOut)
    rLog("Email sent.")
  }

  return(invisible(MM))
}
