# Report positions for Adrian Dragulescu
#
#
# Written by AAD on 12-Feb-2009

#################################################################
.loadPositions <- function(date1, date2, whatToPull)
{
  rLog("Load the positions from RData files")
  dirOut <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
  fileNameHeader <- "positionsAAD_"

  if (date1 > date2)
    stop("date2 should come after date1!")

  rLog("Get prices for", as.character(date1))
  file1 <- paste(dirOut, fileNameHeader, as.character(date1), ".RData", sep="")
  rLog("Process file", file1)
  load(file1)
  QQ <- subset(QQ, portfolio %in% whatToPull)
  QQ1 <- aggregate(QQ$delta, list(QQ$trading.book, QQ$curve, QQ$contract.dt),
                  sum)
  names(QQ1) <- c("book", "curve.name", "contract.dt", "value")
  QQ1$date <- paste("QQ.", date1, sep="")
  QQ <- QQ1
  
  if (date2 != date1){
    file2 <- paste(dirOut, fileNameHeader, as.character(date2),
                   ".RData", sep="")
    rLog("Process file", file2)
    load(file2)
    QQ  <- subset(QQ, portfolio %in% whatToPull)
    QQ2 <- aggregate(QQ$delta, list(QQ$trading.book, QQ$curve, QQ$contract.dt),
                  sum)  
    names(QQ2) <- c("book", "curve.name", "contract.dt", "value")
    QQ2$date   <- paste("QQ.", date2, sep="")

    QQ <- merge(QQ, QQ2)
  }
  
  QQ <- subset(QQ, abs(value)>1) 
  QQ <- cast(QQ, ... ~ date, I, fill=0)
  rLog("Done")

  return(as.data.frame(QQ))
}

#################################################################
# 
.loadPrices <- function(date1, date2, df)
{
  rLog("Loading prices ...")
  if (date1 > date2)
    stop("Please have date2 > date1.")
  
  dirOut <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Prices/"
  file1 <- paste(dirOut, "eod_commod_prices_", as.character(date1), ".RData", sep="")
  if (!file.exists(file1))
    stop("Cannot find file ", file1)
  load(file1)
  hP1 <- merge(hP, df, by=c("curve.name", "contract.dt"))
  colnames(hP1)[3] <- paste("PP.", date1, sep="")
  
  # if you want the current prices
  if (date2 == Sys.Date()){
    rLog("Get current prices ...")
    hP2 <- secdb.getCurrentMarks(df$curve.name, df$contract.dt)
    colnames(hP2) <- c("curve.name", "contract.dt", paste("PP.", date2, sep=""))
  } else {
    file2 <- paste(dirOut, "hPrices.", as.character(date2), ".RData", sep="")
    if (!file.exists(file1))
      stop("Cannot find file ", file2)
    load(file2)
    hP2 <- merge(hP, df, by=c("curve.name", "contract.dt"))
    colnames(hP2)[3] <- paste("PP.", date2, sep="")
  }

  PP <- merge(hP1, hP2, all=TRUE)
#  PP$dP <- PP[,as.character(date2)] - PP[,as.character(date1)]
  rLog("Done!")
  
  return(PP)
}

#################################################################
#
.getTerm <- function(date=Sys.Date(), yearsOut=8)
{
  currYear <- format(date, "%y")
  boy <- as.Date(format(date, "%Y-01-01"))
  yrs <- format(seq(boy, length.out=yearsOut, by="1 year"), "%y")[-1]
  
  term <- c(paste("BAL", currYear), paste("CAL", yrs))
  
  sD <- as.Date(sapply(term, termStartDate), origin="1970-01-01")
  eD <- as.Date(sapply(term, termEndDate), origin="1970-01-01")
  term <- melt(mapply(seq, sD, eD, by="1 month", SIMPLIFY=FALSE))
  rownames(term) <- NULL
  colnames(term) <- c("contract.dt", "term")
  
  return(term)
}

#################################################################
# Prepare report
#
.report <- function(date1, date2, QQ, PP, threshold=30000)
{
  date1 <- as.character(date1)
  date2 <- as.character(date2)
  out <- paste("Change between ", date2, " and ", date1, ":\n\n", sep="") 

  PQ <- merge(QQ, PP, all=TRUE, by=c("curve.name", "contract.dt"))

  # add the peak/offpeak & location info
  PQ <- cbind(PQ, classifyCurveName(PQ$curve.name, by=c("peak/offpeak",
                                                     "location"))[,-1])
              
  PQ$PnL <- PQ[,paste("QQ.",date1, sep="")]*PQ[,paste("PP.", date2, sep="")] -
            PQ[,paste("QQ.",date1, sep="")]*PQ[,paste("PP.", date1, sep="")]
   
  PQ <- merge(PQ, .getTerm(as.Date(date2)))
  # net quantity, useful to have
  netQ <- cast(PQ, term ~ pkoffpk, function(x){round(sum(x))}, fill=0,
               value=paste("QQ.",date1, sep=""))
  
  TT <- aggregate(PQ$PnL, list(term=PQ$term), sum)
  colnames(TT)[2] <- "PnL"
  TTT     <- rbind(TT, data.frame(term="Total", PnL=sum(PQ$PnL)))
  TTT$PnL <- dollar(TTT$PnL, digits=0)
  out <- c(out, capture.output(print(TTT, row.names=FALSE)))

  # if bigger then threshold, try to explain the location/bucket
  ind <- which(abs(TT$PnL) > threshold)
  for (i in seq_along(ind)){
    pq <- subset(PQ, term == TT[ind[i],"term"])
    out <- c(out, "\n=============================================")
    aux <- cast(pq, contract.dt ~ pkoffpk, function(x){dollar(round(sum(x)),
      0, FALSE)}, fill=0, value="PnL")
    out <- c(out, capture.output(print(aux, row.names=FALSE)), "\n")
    
    aux <- cast(pq, . ~ location, function(x){dollar(round(sum(x)),
      0, FALSE)}, fill=0, value="PnL")
    out <- c(out, capture.output(print(aux, row.names=FALSE)), "\n")
  }
  
  out <- c(out, "\n\n Net quantity (MWh):\n",
           capture.output(print(netQ, row.names=FALSE)))
  
  fileOut <- paste("pqPnL_AAD_", date2, ".RData", sep="")
  save(PQ, file=fileOut)
  rLog("Saved PQ PnL results to file", fileOut)

  return(out)
}


#################################################################
#################################################################
options(width=500)  # make some room for the output
options(stringsAsFactors=FALSE)
require(methods)   # not sure why this does not load!
require(CEGbase); require(CEGterm); require(SecDb); require(PM)
require(PM); require(reshape)


# Initialize SecDb for use on Procmon
DB = "!Bal ProdSource;!BAL DEV!home!E47187;!BAL Home"
Sys.setenv(SECDB_SOURCE_DATABASE=DB)
Sys.setenv(SECDB_DATABASE="Prod")

rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

cArgs <- commandArgs()
if (length(cArgs)>6){
  asOfDate <- as.Date(cArgs[7])  # pass asOfDate ... 
} else {
  date1 <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
  date2 <- as.Date(secdb.dateRuleApply(Sys.Date(), "-0b"))
}

rLog(paste("Running report for", as.character(date2)))
rLog("\n\n")

# save the file in this directory
dirOut <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
setwd(dirOut)

# this is the PnL I get calculated from FPA
whatToPull <- c(
  "NEPOOL CONGESTION PORTFOLIO",
  "NPFTRMT1",
  "NESTAT",
  "VIRTSTAT"
)

QQ <- try(.loadPositions(date1, date1, whatToPull))  
if (class(QQ) == "try-error") {
  rLog("No positions found for ", as.character(date2))
  q(status=0)
}

df <- unique(QQ[,c("curve.name", "contract.dt")])
PP <- try(.loadPrices(date1, date2, df))
if (class(PP) == "try-error") {
  rLog("Cannot find prices for previous two days.")
  q(status=0)
}



out <- .report(date1, date2, QQ, PP) 

# load("pqPnL_AAD_2009-07-07.RData")


rLog("Email results")
sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
  "adrian.dragulescu@constellation.com",
  paste("PxQ PnL done for", as.character(date2)), out)

rLog(paste("Done at ", Sys.time()))


if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}

