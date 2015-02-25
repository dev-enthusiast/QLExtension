# Get Trade Patterns from vCentral schema in VCTRLP
# 
# options <- NULL
# options$asOfDate <- Sys.Date()-1
#
# VCentral File DSN  
# options$file.dsn.path <-  "H:/user/R/RMG/Utilities/DSN/"  
# options$file.dsn <- "VCTRLP.dsn"   
#
# source("H:/user/R/RMG/Energy/Trading/Controls/Pattern.of.Trades/get.tradepattern.from.vcentral.R")
# res <- get.tradepattern.from.vcentral(options)

get.tradepattern.from.vcentral <- function(options) {
    require(RODBC)
    require(SecDb)
    require(reshape)
    require(xtable)
    
    source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.holiday.calendar.R")
    options <- NULL
    if (length(options$asOfDate)==0){options$asOfDate <- Sys.Date()-1}
    if (length(options$number.of.days)==0) {options$number.of.days <- 14 }
    if (length(options$calendar.type)==0) {options$calendar.type <- "CPS-BAL"}
    if (length(options$report.dir)==0) {
        options$report.dir <- "S:/All/Risk/Reports/TradingControlReports/TradePattern/"
        options$report.dir <- paste(options$report.dir, options$asOfDate, "/", sep="")
    }
    
    dir.create(options$report.dir,  showWarnings = FALSE)
    # Get 14 business days
    cal <- get.holiday.calendar(options)
    start.date <- as.character(cal$start.date)
    end.date <- as.character(cal$end.date)
    
    if (length(options$file.dsn)==0){options$file.dsn <- "VCTRLP.dsn"}
    if (length(options$file.dsn.path)==0){options$file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"}
   
    query <-  paste(" select th.trade_entry_date, th.trade_start_date,",
               " th.trade_end_date, th.last_modification_date,",  
               " th.trader, th.profit_center_1 as book, th.externaldealid", 
               " from vcentral.trade_header th", 
               " where th.valuation_date between to_date('", start.date, "', 'yyyy-mm-dd') ",
               " and to_date('", end.date, "', 'yyyy-mm-dd') ",
               " and th.valuation_date = th.trade_entry_date ",
               " and th.sor_indicator = th.source_eid and th.source_feed = 'R'",
               sep = "")
                      
    connection.string <- paste("FileDSN=", options$file.dsn.path, options$file.dsn, 
     ";UID=vcentral_read;PWD=vcentral_read;", sep="")  
    con <- odbcDriverConnect(connection.string)  

    data <- sqlQuery(con, query)     
    odbcCloseAll()
    names(data) <- tolower(names(data))
    data$trade_entry_date = as.Date(data$trade_entry_date)
    data$trade_start_date = as.Date(data$trade_start_date)
    data$trade_end_date = as.Date(data$trade_end_date)
    data$last_modification_date = as.Date(data$last_modification_date)
    data$trader = as.character(data$trader)
    data$book = as.character(data$book)
    data$externaldealid = as.character(data$externaldealid)
    
    trader.id <- unique(data$trader)
    traderwk <- NULL
    trader <- NULL
    for (i in 1:length(trader.id)) {
      traderwk$trader <- trader.id[i]
      traderwk$name <- secdb.getValueType(trader.id[i], "Full Name")
      trader <- rbind(trader, traderwk)
    }
    trader <- data.frame(trader)
    trader$trader <- as.character(trader$trader)
    trader$name <- as.character(trader$name)
    data <- merge(data, trader)
    
    short.term.trades <- subset(data, trade_end_date - trade_start_date <= 5)
    short.term.trades <- short.term.trades[, c("name","trade_entry_date","book","externaldealid","last_modification_date")]
    long.term.trades <- subset(data, trade_end_date - trade_start_date > 5)
    long.term.trades <- long.term.trades[, c("name","trade_entry_date","book","externaldealid","last_modification_date")]

    res <- NULL
    res$all<- data.frame(tapply(rep(1, nrow(data)), list(data$name, 
                        data$trade_entry_date), sum))    
    res$short <- data.frame(tapply(rep(1,nrow(short.term.trades)), list(short.term.trades$name,
                        short.term.trades$trade_entry_date), sum))                        
    res$long <- data.frame(tapply(rep(1,nrow(long.term.trades)), list(long.term.trades$name, 
                        long.term.trades$trade_entry_date), sum)) 
    #indx <- which(is.na(res.long))
    #res.long[indx] <- 0
    
    res$large.number.trades <- NULL
    for (i in 1:nrow(res$long)) {
      if (length(which(res$long[i,] >= 50)) >0) {
         res$large.number.trades <- rbind(res$large.number.trades, res$long[i,]) 
      }
    }
    
    cname <- gsub("X", "", colnames(res$large.number.trades))
    for (i in 1:nrow(res$large.number.trades)) { 
       file <- paste(options$report.dir, gsub(" ", "", gsub("\\.", "",rownames(res$large.number.trades)[i])), "pic.pdf", sep="")
       # Open pdf for output
       pdf(file, width=11, height=8)      
       plot(as.Date(cname, "%Y.%m.%d"), res$large.number.trades[i,], type="o", col="blue", 
                    ylab="No of trades", main=rownames(res$large.number.trades)[i])
       dev.off()
    } 
    
    colnames(res$all) <- gsub("X", "", colnames(res$all))
    captionAll <- paste("{\\large All Trades From ", start.date, " To ", end.date, ".}", sep="")                 
    fileAll <- paste(options$report.dir, "AllTrades.tex", sep = "")
    print(xtable(res$all, caption = captionAll, digits=0), file = fileAll,   
          size="scriptsize", tabular.environment="longtable", floating=FALSE, include.rownames=TRUE) 
    
    colnames(res$short) <- gsub("X", "", colnames(res$short))
    captionShort <- paste("\\large {Short Term Trades From ", start.date, " To ", end.date, 
      ". Short Term are defined as (TradeEndDate - TradeStartDate) less than or equal 5.", "}", sep="")                 
    fileShort <- paste(options$report.dir, "ShortTermTrades.tex", sep = "")
    print(xtable(res$short, caption = captionShort, digits=0), file = fileShort,  
          size="scriptsize", tabular.environment="longtable", floating=FALSE, include.rownames=TRUE) 
    
    colnames(res$long) <- gsub("X", "", colnames(res$long))
    captionLong <- paste("\\large {Long Term Trades From ", start.date, " To ", end.date, 
      ". Long Term are defined as (TradeEndDate - TradeStartDate) bigger than 5.", "}", sep="")                 
    fileLong <- paste(options$report.dir, "LongTermTrades.tex", sep = "")
    print(xtable(res$long, caption = captionLong, digits=0), file = fileLong,  
          size="scriptsize", tabular.environment="longtable", floating=FALSE, include.rownames=TRUE)                    
   # identify(1:nrow(res), NULL, labels=colnames(res), pos=T, plot=T, tolerance=2)
    
    report.name <- "TradePattern"
    template <- "H:/user/R/RMG/Energy/Trading/Controls/Pattern.of.Trades/templateTradePattern.tex"
    aux <- readLines(template)
    bux <- gsub("asOfDate",as.character(options$asOfDate),aux) 
    
    pic.files <- list.files(options$report.dir, pattern="\\.pdf")
    pic.files <- pic.files[grep("pic", pic.files)]
    line <- NULL
    line[1] = "\\clearpage"
    line[2] = "\\begin{figure}"
    line[3] = "\\vspace*{-1\\baselineskip}"
    line[4] = "\\centerline{\\includegraphics[width=0.90\\linewidth]"
    line[6] = "\\end{figure}"
    for (i in 1:length(pic.files)){   
      line[5] = paste("{S:/All/Risk/Reports/TradingControlReports/TradePattern/", 
                      as.character(options$asOfDate), "/", pic.files[i], "}}", sep="")
      for (j in 1:6) {
        ind <- grep("%insert.plot.above", bux)
        bux  <- append(bux, line[j], after=(ind-1))
      }
    }
    texFilename <- paste(options$report.dir, report.name, sep = "") 
    writeLines(bux, paste(texFilename, ".tex", sep = ""))
    setwd(options$report.dir)

    system(paste("S:/Risk/Software/Latex/fptex/TeXLive/bin/win32/pdflatex", texFilename))  
    file.remove(paste(texFilename, ".aux", sep=""))
    file.remove(paste(texFilename, ".log", sep=""))
    return(data)
}

get.large.number.trade <- function(trades) {
  
  trades$value <- rep(1, nrow(trades))
  trade.number <- cast(trades, trader + trade_entry_date ~.)
  colnames(trade.number)[3] <- "value"
  large.number.trade <- subset(trade.number, value >= 50)
  large.number.trade <- data.frame(cast(large.number.trade, trader ~ trade_entry_date) )
  
  return(large.number.trade)
  
}