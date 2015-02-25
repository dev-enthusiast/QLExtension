# Report positions for Adrian Dragulescu
# Monthly zonal position report
#  (2) get positions and archive them
#  (3) check for non zero delta trades in my books
#  (4) check for incs/decs mismatch
#  (5) show retail positions to move
#
# .check_nonzero_delta_trades
# .check_generation_changes
# .getPositionsAggregate  - zonal positions for all the portfolio minus attrition!   
# .getPositionsPrism
# .impliedLossPosition - calculate loss position due to loss factors
# .saveXLS - formats the data in the right tabular format
# .writeXLS - does the actual saving to xlsx and formats cells
# .reconcileIncsDecs - not an issue anymore 
# .retailPositionsMove - show the daily change in retail positions 
# 
#
# Written by AAD on 12-Feb-2009

LOSS_FACTORS <- c(".Ros",    0.0,    # dropped to -3.0! in Jul-2010!
                  "CT",      2.5,
                  "Hub",     0.0,
                  "MAINE",  -4.5, 
                  "NEMA",   -1.3,
                  "NH",     -1.5,
                  "RI",     -1.1,
                  "SC",     -1.0,
                  "SEMA",   -0.7,
                  "VT",      1.0,  
                  "WMA",     0.7)
LOSS_FACTORS <- as.data.frame(matrix(LOSS_FACTORS, ncol=2, byrow=TRUE))
colnames(LOSS_FACTORS) <- c("zone.name", "loss.factor")
LOSS_FACTORS$loss.factor <- as.numeric(LOSS_FACTORS$loss.factor)

#################################################################
# send an email if trades in the past 5 days have non-zero delta
# in my books
#
.check_generation_changes <- function(asOfDate)
{
  day1 <- as.Date(secdb.dateRuleApply(asOfDate, "-1b"))
  day2 <- asOfDate

  books <- PM:::secdb.getBooksFromPortfolio(portfolio="Nepool Owned Gen Portfolio")
  aux <- .change_in_positions(books, day1, day2, aggregate=TRUE)

  res <- cast(aux, curve ~ year, sum, value="change", fill=0)
  ind <- order(-abs(apply(res[,-1], 1, sum)))
  res <- res[ind,]
  
  
  sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
     "adrian.dragulescu@constellation.com,steven.webster@constellation.com",
#     "adrian.dragulescu@constellation.com",
      "Change in positions: Nepool Owned Gen Portfolio",
      c(paste("Between day", format(day1), "and", format(day2)),
        capture.output(print(res, row.names=FALSE)),
        "\nProcmon job: daily_positions_AAD\nContact: Adrian Dragulescu"))

}



#################################################################
# send an email if trades in the past 5 days have non-zero delta
# in my books
#
.check_nonzero_delta_trades <- function(QQ)
{
  maxDate <- seq(as.Date(format(Sys.Date(), "%Y-12-01")), length.out=2, by="5 years")[2]
  qq <- subset(QQ, contract.dt <= maxDate &
                   contract.dt >= Sys.Date() &
                   portfolio %in% c("NEPOOL BASIS PORTFOLIO") &
                   #trading.book %in% c("NPFTRMT1", "CONGIBT", "NEPPWRBS") &
                   mkt == "PWX"
              )

  res <- cast(qq, trade.date + trading.book ~ ., sum, value="delta")
  res <- subset(res, trade.date >= Sys.Date()-7)
  names(res)[3] <- "net.delta"

  ind <- which(abs(res$net.delta) > 100)
  if (length(ind) > 0) {
    aux <- res[ind,]
    rownames(aux) <- NULL
    sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
      "adrian.dragulescu@constellation.com",
      "WARNING! Non flat trades in my books", capture.output(aux))
  }
  
  invisible()
}



#################################################################
#
.getPositionsPrism <- function(asOfDate, whatToPull=NULL)
{
  rLog("Get the positions from prism for", as.character(asOfDate))

  if (is.null(whatToPull)) {
    whatToPull <- if (asOfDate < as.Date("2013-07-25")) {
      c("Nepool Accrual Portfolio",              
      "PM East Retail Portfolio",
      "Nepool MTM Hedges of Accrual",              
      "Nepool Congestion Portfolio",
      "NEPOOL FP Portfolio", 
      "NPFTRMT1",
      "NEPPWRBS",              
      "NESTAT"              
      )
    } else {
    c("CNE NewEngland Portfolio",
      "Nepool Hedge Portfolio",
      "Nepool Load Portfolio",
      "Nepool Gen Portfolio",
      "Nepool Basis Portfolio",
      "NE Congestion")
    }
  }

  QQ <- list()
  for (p in seq_along(whatToPull)) {
    pName <- toupper(whatToPull)[p]
    rLog("Getting ", pName)
    books <- .getPortfolioBooksFromSecDb( pName )
    aux   <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
                                          fix=TRUE)
    if (nrow(aux)>0){
      aux   <- data.frame(portfolio=pName, aux)
      QQ[[pName]] <- subset(aux, !(curve %in% c("USD", "CAD")))
    }
  }
  QQ <- do.call(rbind, QQ)
  rownames(QQ) <- NULL

  fileOut <- paste(fileNameHeader, asOfDate, ".RData", sep="")
  save(QQ, file=fileOut)
  rLog("Saved", fileOut)

  return(QQ)
}


#################################################################
# from SecDb  - the commod locations!
#
.getPositionsSecDb <- function(asOfDate, mw=TRUE, peakOffpeak=TRUE)
{
  lastMonth <- as.Date(paste(as.numeric(format(asOfDate, "%Y"))+6,
                  "-12-01", sep=""))
  months <- seq(nextMonth(), lastMonth, by="1 month")

  FTR.load.ISO.env("NEPOOL")
  NEPOOL$hP <- NULL
  NEPOOL$SP <- NULL
  
  whatToPull <- c("PM NorthEast Portfolio")

  res <- vector("list", length=length(whatToPull))
  names(res) <- toupper(whatToPull)
  for (w in whatToPull){
    what <- toupper(w)

    x <-  secdb.readDeltas(asOfDate, what)
    res[[what]] <- cbind(pfolio=what, .format_secdb_deltas(x))
  }  
  res <- do.call(rbind, res)
  rownames(res) <- NULL

  # aggregate over the portfolios
  res <-  cast(res, market + month + location + bucket ~ .,
    sum, fill=0, value="value")
  names(res)[ncol(res)] <- "value"

  if (peakOffpeak) {
    # combine 2X16H, 7X8 into OFFPEAK
    res$contract.dt <- res$month; res$month <- NULL
    res <- combineBuckets(res, weight="none", buckets=c("2X16H", "7X8"),
                        name="OFFPEAK")
    res$month <- res$contract.dt;  res$contract.dt <- NULL
  }

  if (mw) {
    HRS <- NEPOOL$HRS
    names(HRS)[1:2] <- c("month", "bucket")
    res <- merge(res, HRS, by=c("month", "bucket"))
    res$value <- res$value/res$hours
    res$hours <- NULL
  }

  res <- cast(res, month ~ location + bucket, round, fill=0, value="value")

  rLog("Done pulling SecDb positions!")
  
  res
}


#################################################################
# calculate total synthetic hub position due to loss factors
#
.impliedLossPosition <- function(QQ.zone)
{
  rLog("Calculate the implied loss position")
  aux <- merge(QQ.zone, LOSS_FACTORS)
  aux$impliedLossPosition <- aux$loss.factor * aux$value *0.01
  aux$value <- NULL
  aux <- cast(aux, contract.dt ~ bucket, sum, fill=NA,
    value="impliedLossPosition")
  aux <- data.frame(aux)
  colnames(aux)[2:3] <- paste("Loss_", colnames(aux)[2:3], sep="")
  colnames(aux) <- gsub("OFFPEAK", "OFF", colnames(aux))
  colnames(aux) <- gsub("PEAK", "ON", colnames(aux))
  aux[,2:ncol(aux)] <- round(aux[,2:ncol(aux)], 0)

  return(aux)
  rLog("Done.")
}



#################################################################
# my position 
#
.saveXLS_sheet1 <- function(wb, QQ)
{
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")
  
  QQ$value <- QQ$delta
  QQ <- subset(QQ, service=="ENERGY" &
                   mkt=="PWX" &
                   portfolio %in% c("NEPOOL BASIS PORTFOLIO",
                                    "CNE NEWENGLAND PORTFOLIO")
               )
  QQ.zone <- getPositionsZone(QQ, MAP, subset=TRUE, mw=TRUE)
  ILP <- .impliedLossPosition(QQ.zone)
  
  res <- cast(QQ.zone, contract.dt ~ zone.name + bucket, I, fill=0)
  res[,2:ncol(res)] <- round(res[,2:ncol(res)], 0)
  res <- as.data.frame(res)
  colnames(res) <- gsub("OFFPEAK", "OFF", colnames(res))
  colnames(res) <- gsub("PEAK", "ON", colnames(res))
  colnames(res) <- gsub("MAINE", "ME", colnames(res))
  # move to end ... "Hub", ".Ros", "SC"
  ind <- names(res) %like% "Hub|.Ros|SC"
  res <- res[,c((1:ncol(res))[-ind], ind)]
  # add the implied loss position
  res <- merge(res, ILP)

  #--------------------------------------------------------------
  sheet <- createSheet(wb, "MyPositions")
  rows  <- createRow(sheet, rowIndex=1:(nrow(res)+1))
  cells <- createCell(rows, colIndex=1:ncol(res))
  
  createFreezePane(sheet, 2, 1)
  
  # add colnames
  invisible(mapply(setCellValue, cells[1,], colnames(res)))

  iOffset <- 1
  for (ic in 1:ncol(res))
    mapply(setCellValue, cells[(1+iOffset):nrow(cells),ic], res[,ic])
  
  # create header cell style, and apply it
  cs.header <- CellStyle(wb) + Fill(backgroundColor="wheat",
    foregroundColor="wheat", pattern="SOLID_FOREGROUND")  
  invisible(lapply(cells[1,], setCellStyle, cs.header))
  
  # Date and POSIXct classes need to be formatted
  indDT <- which(sapply(res, class) == "Date")
  dateFormat <- CellStyle(wb) + DataFormat("m/d/yyyy")
  invisible(lapply(cells[2:nrow(res),1], setCellStyle, dateFormat))

  # color odd years differently
  cs.fill <- CellStyle(wb) + Fill(backgroundColor="skyblue",
    foregroundColor="skyblue", pattern="SOLID_FOREGROUND")
  year <- as.numeric(format(res[,1], "%Y"))
  ind <- which(year %% 2 == 1)                 # odd years
  invisible(lapply(cells[ind+1,2:ncol(res)], setCellStyle, cs.fill))

  cs.fill.dt <- CellStyle(wb) + Fill(backgroundColor="skyblue",
    foregroundColor="skyblue", pattern="SOLID_FOREGROUND") + 
    DataFormat("m/d/yyyy")
  invisible(lapply(cells[ind+1,1], setCellStyle, cs.fill.dt))
  
  autoSizeColumn(sheet, 1)

  invisible()
}


#################################################################
# 
#
.saveXLS_sheet2 <- function(wb, QQ)
{
  ## colnames(QQ)[which(colnames(QQ)=="month")] <- "contract.dt"
  ## res <- cast(QQ, contract.dt ~ location + bucket, sum, fill=0, 
  ##   value="mw")
  ## res[,-1] <- round(res[,-1])
 
  sheet <- createSheet(wb, "TotalPosition")
  addDataFrame(QQ, sheet, row.names=FALSE, startRow=1)
  
  createFreezePane(sheet, 2, 2)
  autoSizeColumn(sheet, 1)

  invisible()
}




#################################################################
# Simple recon
.reconcileIncsDecs <- function(QQ, asOfDate)
{
  out <- NULL
  fileIncsDecs <- paste("S:/All/Structured Risk/NEPOOL/FTRs/Analysis/",
    "AAD_incdec_bids.RData", sep="")
  load(fileIncsDecs)   # load variable incdec
  if (!(as.character(asOfDate) %in% names(incdec)))
    return()
  incdec <- incdec[[as.character(asOfDate)]]
  incdec <- subset(incdec, book == "VIRTSTAT")  # I care about mine only
  
  qq <- subset(QQ, (portfolio == "VIRTSTAT" &
                    as.Date(trade.start.date) == asOfDate))
  qq$deliv.pt <- PM:::.cleanDeliveryPt(qq$delivery.point)

  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")
  qq <- merge(qq, MAP[,c("ptid", "deliv.pt")], all.x=TRUE)     
  qq <- merge(incdec, qq[, c("ptid", "eti", "deliv.pt")], by="ptid", all=TRUE)

  aux <- qq[is.na(qq$inc.dec), ]  # locations I did not bid on!
  if (nrow(aux)>0){
     out <- c(out, "I did not bid on these trades?!")
     out <- c(out, capture.output(print(aux[,c("eti", "deliv.pt")],
                                        row.names=FALSE)))
  }
  aux <- qq[is.na(qq$eti), ]  # locations that did not get booked
  if (nrow(aux)>0){
     out <- c(out, "These locations appear not to have been booked/cleared")
     out <- c(out, capture.output(print(aux, row.names=FALSE)))
  }
  
  return(out)
}

#################################################################
# Should some Retail zonal positions move to cong?
#
.retailPositionsMove <- function(QQ, asOfDate, threshold=25)
{
  RR <- subset(QQ, portfolio=="CNE NEWENGLAND PORTFOLIO")
  #RR <- rbind(RR, subset(QQ, trading.book=="NEBKNES")) # from MTM Other Hedges, CNE block trades!
  RR <- subset(RR, mkt == "PWX")  # only NEPOOL 
  RR <- RR[,c("curve", "contract.dt", "delta", "bucket", "quantity.uom")]

  out <- ""
  if (any(RR$quantity.uom != "MWH"))
    out <- c(out, "STOP!  Some NEPOOL power positions have quantity not MWH.")

  aux <- classifyCurveName(RR$curve, by=c("location", "peak/offpeak"))
  RR <- cbind(RR, aux[,-1])
  aux <- cast(RR, contract.dt + location + bucket ~ ., sum, value="delta")
  names(aux)[ncol(aux)] <- "delta"

  # add the hours
  HRS <- ntplot.bucketHours(c("2X16H", "7X8", "5X16"), region="NEPOOL",
      startDate=min(RR$contract.dt), endDate=max(RR$contract.dt))
  names(HRS)[2:3] <- c("contract.dt", "hours")
  aux <- merge(aux, HRS, by=c("bucket", "contract.dt"), all.x=TRUE)
  aux$mw <- aux$delta/aux$hours

  # pick only the positions above the 25 MW threshold
  aux <- subset(aux, abs(mw) >= threshold & location != "HUB")
  
  if (nrow(aux)>0){
    aux <- cast(aux, contract.dt ~ location + bucket,
      function(x){dollar(I(x), 0, FALSE)}, fill=0, value="mw")

    out <- c(out,
      "Need to move these positions from PM East Retail Portfolio:\n")
    out <- c(out, capture.output(print(aux, row.names=FALSE)))
  }
      
  return(out) 
}



#################################################################
#################################################################
options(width=500)  # make some room for the output
options(stringsAsFactors=FALSE)

require(methods)   # not sure why this does not load!
require(CEGbase)
require(xlsx)

require(SecDb)
require(FTR)
require(PM)
require(RODBC)
require(reshape)
require(lattice)
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")


rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

cArgs <- commandArgs()
if (length(cArgs)>6){
  asOfDate <- as.Date(cArgs[7])  # pass asOfDate ... 
} else {
  asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
}
# asOfDate <- as.Date("2013-07-03")
rLog(paste("Running report for", as.character(asOfDate)))
rLog("\n\n")


# save the file in this directory
dirOut <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
fileNameHeader <- "positionsAAD_"
setwd(dirOut)

whatToPull <- c("CNE NewEngland Portfolio", "Nepool Basis Portfolio", "NE Congestion")
QQ1 <- try(.getPositionsPrism(asOfDate, whatToPull))   # includes the FIXES
#load("positionsAAD_2013-04-12.RData"); QQ1 <- QQ; QQ <- NULL
QQ2 <- try(.getPositionsSecDb(asOfDate))

if ("try-error" %in% class(QQ1)| "try-error" %in% class(QQ2)) {
  rLog("Cannot find some positions, exiting")
  sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
  "adrian.dragulescu@constellation.com",
  paste("PRISM positions for", as.character(asOfDate), "not found"), "")
  q(status=0)
}

wb <- createWorkbook()
.saveXLS_sheet1(wb, QQ1)
.saveXLS_sheet2(wb, QQ2)
saveWorkbook(wb, file=paste(dirOut, "/positionsAAD_", as.character(asOfDate),
    ".xlsx", sep=""))
rLog("Wrote xls file to S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions.")  


.check_nonzero_delta_trades(QQ1)

try(.check_generation_changes(asOfDate))

rLog("Email results")
sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
  "adrian.dragulescu@constellation.com",
  paste("PRISM positions done for", as.character(asOfDate)), "")

rLog(paste("Done at ", Sys.time()))

# reconcile inc/decs ...
try(out <- .reconcileIncsDecs(QQ1, asOfDate))
if ((class(out)!="try-error") & (class(out)!="NULL")){
  sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
    "adrian.dragulescu@constellation.com",
    paste("INCDEC position mismatch for", as.character(asOfDate)), out)
}

# check if you need to move some positions from retail
try(out <- .retailPositionsMove(QQ1, asOfDate, threshold=1))
if ((class(out)!="try-error") & (length(out)> 2)){
  sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
    "adrian.dragulescu@constellation.com",
    paste("Move these Retail positions as of", as.character(asOfDate)), out)
}


if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}



## # get day on day changes ... 
## files <- sort(list.files(dirOut, pattern=paste(fileNameHeader,".*RData", sep="")))
## file1 <- paste(dirOut, files[length(files)-1], sep="")
## file2 <- paste(dirOut, files[length(files)], sep="")

## setwd(dirOut)
## try(out <- PM:::.compareTwoPositionsFiles(file1, file2, sink=TRUE,
##        portfolio.book=toupper(whatToPull)[c(2,4,7)], service="ENERGY"))
## if (class(out)=="try-error")
##   returnCode <- 1                 # fail
