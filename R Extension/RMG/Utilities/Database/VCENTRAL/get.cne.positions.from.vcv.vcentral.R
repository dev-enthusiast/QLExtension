# Read Positions from FORC schema in VCTRLP, where
# 
# options <- NULL
# options$asOfDate <- Sys.Date()-1
# leave both options$books and options$portfolios empty if you want to have all
# options$books <- c("PRBCOAL", "MVSO2", "MVCRUDE")  
# options$portfolios <- c("GERMANY PORTFOLIO", "CONTINENTAL CO2 PORTFOLIO") 
# options$save.flag <- 1 save postions to the archive  
# options$data.directory <- "S:/All/Risk/Data/VaR/prod/Positions/"
# options$save.file.name <- "test.RData"
# options$exclude <- c("RMLGMTM","RMMBMTM","RMPSACC","RMPSMTM","RMSWMTM","RMWWMTM","RMJKMTM",
#                               "RMBAMTM","RMBAACC","RMHOUMTM","RMROMMTM","RMTVRMTM")
# VCentral File DSN  
# options$file.dsn.path <-  "H:/user/R/RMG/Utilities/DSN/"  
# options$file.dsn <- "VCTRLP.dsn"   
#
# source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.cne.positions.from.vcv.vcentral.R")
# res <- get.cne.positions.from.vcv.vcentral(options)

get.cne.positions.from.vcv.vcentral <- function(options) {
    if (length(options$asOfDate)==0){options$asOfDate <- Sys.Date()-1}
    if (length(options$file.dsn)==0){options$file.dsn <- "VCTRLP.dsn"}
    if (length(options$file.dsn.path)==0){options$file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"}
    if (length(options$data.directory)==0) {options$data.directory <- "S:/All/Risk/Data/VaR/prod/Positions/"}
    if (length(options$save.file.name) == 0) {
        filename <- paste(options$data.directory, "vcv.positions.", as.character(options$asOfDate),
                          ".RData", sep="")
        options$save.file.name <- filename
    }
    
    if (length(options$exclude) == 0) {
        options$exclude <-  c("RMLGMTM","RMMBMTM","RMPSACC","RMPSMTM","RMSWMTM",
                              "RMWWMTM","RMJKMTM","RMBAMTM","RMBAACC","RMHOUMTM",
                              "RMROMMTM","RMTVRMTM")
    }
    
    require(RODBC)
    
    query <-  paste("select SOURCE_EID, VALUATION_DATE, LOCATION_NM CURVE_NAME,  ",
                      "PROFIT_CENTER_1 BOOK, CONTRACT_MONTH, REGION, PRICE_TYPE,  ",
                      "QUANTITY_BUCKET_TYPE, QTY_UOM, DELTA, UNDISC_DELTA, ",
                      "PRICEBYQTY_USD, MTM_VALUE_USD, UNDISC_MTM_VALUE_USD, QUANTITY, ",
                      "TOTAL_ORIGINAL_QUANTITY, null CREATED, PROFIT_CENTER_2, VALUATION_MONTH, ",
                      "null MAPPED_SDB_CURVE, SOR_INDICATOR, 0 FILTER_MASK, QUARTER,  ",
                      "REPORTING_PORTFOLIO, VEGA, MARKET_SYMBOL, MARKET_FAMILY,   ",
                      "MAPPED_REGION_NAME, MAPPED_DELIVERY_POINT, SERVICE_TYPE,   ",
                      "CURVE_TYPE, ACCOUNTING_TREATMENT, STRATEGY, BOOK_USAGE,   ",
                      "CCG_ORGANIZATION, CROSS_HEDGE_TO_PORT, TRADER, EXECUTIVE_OWNER, ",
                      "ECONOMIC_OWNER, ACCOUNTING_TYPE, SEAT_LABEL, ACCOUNTING_REGION, ",
                      "ACCOUNTING_SUBREGION, COAL_ACCT_TYPE, COAL_BASIN, COAL_DEAL_TYPE, ",
                      "COAL_USA_INTERNATIONAL, null MARKET_FAMILY_DETAIL, COUNTERPARTY,  ",
                      "null RMSYS_REGION, XDELARCH_REGION_NAME DELARCH_REGION  ",
                      "from vcv.app_positions_aggregate_cne where ", 
                      "valuation_date = to_date('", format(options$asOfDate, "%m/%d/%Y"), 
                      "','MM/DD/YYYY')  ", sep = "")
                      
    connection.string <- paste("FileDSN=", options$file.dsn.path, options$file.dsn, 
     ";UID=TRR;PWD=TRR;", sep="")  
    con <- odbcDriverConnect(connection.string)  

    if (length(options$books)!=0){
      books <- paste(toupper(options$books), sep="", collapse="', '")
      books <- paste("profit_center_1 in ('", books, "')", sep = "")
    } 
    
    if (length(options$exclude)!=0){
      exclude <- paste(toupper(options$exclude), sep="", collapse="', '")
      exclude <- paste("profit_center_1 not in ('", exclude, "')", sep = "")
    } 
    
    if (length(options$portfolios)!=0){
      portfolios <- paste(toupper(options$portfolios), sep="", collapse="', '")
      portfolios <- paste("reporting_portfolio in ('", portfolios, "')", sep = "")
    } 
    
    if (length(options$exclude)!=0) {
       query <- paste(query, " and  ", exclude, sep = "")
    }
    
    if (length(options$books)!=0 & length(options$portfolios)!=0) {
       query <- paste(query, " and ( ", books, " or ", portfolios, " )", sep = "")
    } else if (length(options$books)!=0 & length(options$portfolios)==0) {
       query <- paste(query, " and  ", books, sep = "")
    } else  if (length(options$books)==0 & length(options$portfolios)!=0) {
       query <- paste(query, " and  ", portfolios, sep = "")
    }                        

    QQ.cne <- NULL
    QQ.cne <- sqlQuery(con, query) 
    
    odbcCloseAll()

    # fix this stupid problem here
    ind <- grep("COMMOD PWR NEPOOL", data$CURVE_NAME)
    if (length(ind)>0){data$CURVE_NAME[ind] <- "COMMOD PWX 5X16 BOS PHYSICAL"}
    
    save(QQ.cne, file=options$save.file.name)
    
    return(QQ.cne)
}
