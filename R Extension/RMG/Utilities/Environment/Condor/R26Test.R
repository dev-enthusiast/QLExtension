source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/RLogger.R")

rLog("Starting machine test...")
start = Sys.time()

require(RODBC)
library(SecDb)

computer = Sys.getenv("COMPUTERNAME")

file.dsn <- "VCTRLP.dsn"
file.dsn.path <- "//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/"
connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
        ";UID=vcentral_read;PWD=vcentral_read;", sep="")  

rLog("Connecting to VCentral...")
con <- odbcDriverConnect(connection.string)  

query <-  paste(
        "select td.location_nm, td.valuation_month, ",
        "td.vol_frequency, td.vega ",
        "from vcentral.trade_detail td ",
        "where rownum<10",
        sep = "")

rLog("Querying Database...")
vegas = sqlQuery(con, query) 
rLog("Query Complete.")
odbcClose(con)

rLog("Testing SecDb...")
secDbDate <- as.Date( secdb.dateRuleApply(Sys.Date(), "-1b") )
print(secDbDate)


rLog("Done.\n\n")

REPORT_DIR = "//ceg/cershare/All/Risk/Reports/VaR/test/PoolHealthResults/"

sink( paste(REPORT_DIR, computer, ".txt", sep="") )
rLog( "Computer:", computer)
rLog( "Vegas:" )
print(head(vegas, n=5))
rLog( "SecDb Date:", as.character(secDbDate) )
print(Sys.time()-start)
sink()

