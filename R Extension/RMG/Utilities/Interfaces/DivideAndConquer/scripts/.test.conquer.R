#
#
#

parmsDC <- parmsDivideConquer()
parmsDC$conquer$preFileName <- paste(outdir, "out1/DB_", sep="")

outdir <- "S:/All/Risk/Data/FTRs/NEPOOL/ISO/DB/DemandBids/"
files  <- list.files(outdir, full.names=TRUE, pattern="_200810")

keys <- list("200810"=files)

cUnzip <- function(fileName){
  system(paste("I:/gnu/bin/gunzip.exe ", fileName, sep=""))
}


# aggregate daily files into one montly data
cMonth <- function(fileName)
{
  rLog("Working on file", fileName)
  day <- strsplit(fileName, "_|\\.")[[1]][4]
  aux <- read.csv(fileName, skip=4)
  aux <- aux[-nrow(aux),]   # remove the last comment line
  if (ncol(aux)!=26)
    stop("File format changed!")
  names(aux)[c(3,4,6)] <- c("participantId", "nodeId", "bidType")
  aux <- aux[,-1]
  names(aux)[6:25] <- paste(c("P.", "MW."), rep(1:10, each=2), sep="")
  aux <- cbind(datetime = (as.POSIXct(day, format="%Y%m%d") +
    3600*as.numeric(aux$Hour.End)), aux)
  aux$Hour.End <- NULL
  
  aux <- subset(melt(aux, id=1:5), !is.na(value))

  return(aux)  
}


#files  <- list.files(outdir, full.names=TRUE)
#keys <- list("200811"=files)

conquer(keys, cUnzip)
conquer(keys, cMonth, parmsDC)
