# Show intramonth positions for Cash NEPOOL
#
# Written by AAD on 26-May-2011


#################################################################
#
.prepare_output <- function(qq, tt)
{
  tt$trade.date <- format(tt$trade.date, "%m/%d")

  total <- cast(tt, fwd.date + mkt + variable ~ ., sum, fill=0)
  names(total)[4] <- "value"
  total$trade.date <- "Total"
  
  aux <- rbind(tt[,c("fwd.date", "mkt", "variable", "trade.date",
                     "value")], total)
  
  out <- cast(aux, fwd.date ~ mkt + trade.date + variable, sum, fill=0)

  out
}


#################################################################
#
.write_xlsx <- function(asOfDate, res, filename)
{
  wb <- createWorkbook()
  sheet <- createSheet(wb, "Sheet1")
  rows  <- createRow(sheet, rowIndex=1:(nrow(res)+3))
  cells <- createCell(rows, colIndex=1:ncol(res))
  
  createFreezePane(sheet, 4, 1)

  aux <- strsplit(colnames(res), "_")
  cnames  <- sapply(aux, function(x){x[length(x)]})
  days <- sapply(aux, function(x){x[2]})
  days[1] <- "trd.date"
  mkt <- sapply(aux, function(x){x[1]})
  mkt[1] <- "market"
  
  # add colnames
  mapply(setCellValue, cells[1,], mkt)  
  mapply(setCellValue, cells[2,], days)
  mapply(setCellValue, cells[3,], cnames)

  iOffset <- 3
  for (ic in 1:ncol(res))
    mapply(setCellValue, cells[(1+iOffset):nrow(cells),ic], res[,ic])
  
  # create header cell style, and apply it
  cs.header <- createCellStyle(wb, fillBackgroundColor="wheat",
    fillForegroundColor="wheat", fillPattern="SOLID_FOREGROUND")
  lapply(cells[1:3,], setCellStyle, cs.header)
  
  # Date and POSIXct classes need to be formatted
  dateFormat <- createCellStyle(wb, dataFormat="m/d/yyyy")
  lapply(cells[4:nrow(cells),1], setCellStyle, dateFormat)

  # color dates 
  cs.fill.dt <- createCellStyle(wb, fillBackgroundColor="skyblue",
    fillForegroundColor="skyblue", fillPattern="SOLID_FOREGROUND",
    dataFormat="m/d/yyyy")
  lapply(cells[4:nrow(cells),1], setCellStyle, cs.fill.dt)
  
  autoSizeColumn(sheet, 1)
  # create the column groupings by market
  ig <- split(2:length(days), mkt[-1])
  for (g in ig) {
    g <- g[which(days[g] != "Total")]
    sheet$groupColumn(as.integer(g[1]-1), as.integer(g[length(g)]-1))
    #sheet$setColumnGroupCollapsed(as.integer(g[1]), TRUE)
  }
  
  saveWorkbook(wb, file=filename)
  rLog("Wrote file", filename)
}


#################################################################
#################################################################
options(width=500)  # make some room for the output
options(stringsAsFactors=FALSE)

require(methods)   # not sure why this does not load!
require(CEGbase)
require(xlsx)
require(SecDb)
require(PM)
require(reshape)

source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Positions/get_cash_positions.R")

rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

asOfDate   <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
currentMonth <- as.Date(format(Sys.Date(), "%Y-%m-01"))
lastBday <- as.Date(secdb.dateRuleApply(seq(currentMonth, by="1 month",
  length.out=2)[2], "-1b"))     # last business day of the month
if (lastBday <= asOfDate){        # move ahead one month
  focusMonth <- as.Date(format(as.Date(secdb.dateRuleApply(lastBday, "+1b")),
                               "%Y-%m-01"))
} else {
  focusMonth <- currentMonth
}

                      
books <- "CPDAYHD1"

rLog("Pulling cash positions from Prism ...")
aux <- .get_intramonth_trades(asOfDate, focusMonth, books=books)  
qq  <- aux[[1]]
tt  <- aux[[2]]
rLog("Done")

if (is.null(qq)) {
  rLog("No positions in prism for ", as.character(asOfDate), ".", sep="")
  rLog("Exiting.")
  q(0)
}

out <- .prepare_output(qq, tt)

DIR <- "S:/All/Structured Risk/NEPOOL/Daily Load Bids and Flows/Assorted Cash/Trading/"
filename <- paste(DIR, "cash_intramonth_positions.xlsx", sep="")
.write_xlsx(asOfDate, out, filename)

rLog(paste("Done at ", Sys.time()))
q(status = returnCode)




## # Initialize SecDb for use on Procmon
## DB = "!Bal ProdSource;!BAL DEV!home!E47187;!BAL Home"
## Sys.setenv(SECDB_SOURCE_DATABASE=DB)
## Sys.setenv(SECDB_DATABASE="Prod")
