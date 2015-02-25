# Find trades for SecDB regtests as requested by Coleman.
#
#
#
# Written by Adrian Dragulescu on 3-Nov-2006

require(RODBC); require(reshape)

asOfDate <- "2006-11-02"
cutoffDate <- as.Date("2007-01-01")
query <- paste("select th.externaldealid, td.LOCATION_NM, ",
    "td.transaction_type, max(td.VALUATION_MONTH) max_date ",
    "from vcentral.trade_detail ",
    "td, vcentral.trade_header th where td.TRADE_HEADER_ID=th.TRADE_HEADER_ID",
    " and td.valuation_date = th.valuation_date", 
    " and td.SOURCE_FEED = 'G' and td.valuation_date",
    "=to_date('", asOfDate,"','yyyy-mm-dd') ",
    "and td.location_nm is not null ",            
    "and td.TRADE_HEADER_ID in ( select trade_header_id ",
    "from vcentral.trade_header where valuation_date = to_date('",
    asOfDate, "','yyyy-mm-dd') and source_name='SECDB') ",
    "group by th.externaldealid, td.LOCATION_NM, td.transaction_type", sep="")
con  <- odbcConnect("VCTRLP", uid="vcentral_read", pwd="vcentral_read")
MM <- sqlQuery(con, query)
#odbcCloseAll()
dim(MM)
head(MM)
colnames(MM) <- c("eti", "name", "type", "max.date")
MM$max.date <- as.Date(MM$max.date )
MM$type <- as.character(MM$type)
table(MM$type)

uTypes <- unique(MM$type)
#type <- uTypes[1]
trades <- NULL
for (i in 1:length(uTypes)){
  cat(paste(uTypes[i], "\n")); flush.console()
  aux <- subset(MM, type == uTypes[i])
  bux <- subset(aux, max.date>=cutoffDate)
  if (nrow(bux)>=10){   # if you have enough trades above the cutoff take them
    ind <- sample(1:nrow(bux), 10)   
    trades <- rbind(trades, bux[ind,])
  } else {
    ind <- 1:nrow(aux)
    trades <- rbind(trades, aux[ind,])
  }
}
write.csv(trades, file="c:/temp/etis.for.regtest.csv", row.names=F)





aux <- MM
head


extract.max <- function(x){
 browser()
 max(x)
}

bux <- aggregate(aux$EXTERNALDEALID, list(location=aux$LOCATION_NM,
        type=aux$TRANSACTION_TYPE), extract.max)

table(aux$EXTERNALTRADEID)

bux <- reshape(aux, EXTERNALTRADEID + LOCATION_NM)

