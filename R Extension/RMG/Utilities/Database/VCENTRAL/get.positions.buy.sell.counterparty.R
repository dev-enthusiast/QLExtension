###############################################################################
# get.positions.buy.sell.counterparty.R
# 
# Read Positions from VCV schema in VCTRLP, where
# 
# options <- NULL
# options$asOfDate <- Sys.Date()-1
# leave both options$books and options$portfolios empty if you want to have all
# options$books <- c("PRBCOAL", "MVSO2", "MVCRUDE")  
# options$portfolios <- c("GERMANY PORTFOLIO", "CONTINENTAL CO2 PORTFOLIO") 
#
# VCentral File DSN  
# options$file.dsn.path <-  "H:/user/R/RMG/Utilities/DSN/"  
# options$file.dsn <- "VCTRLP.dsn"   
#
# source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.forc.vcentral.r")
# res <- get.positions.from.forc.vcentral(options)
source("H:/user/R/RMG/Utilities/RLogger.R")
try(memory.limit(3*1024), silent=TRUE)


###############################################################################
get.positions.buy.sell.counterparty <- function(options) 
{
    if (length(options$asOfDate)==0){options$asOfDate <- Sys.Date()-1}
    if (length(options$file.dsn)==0){options$file.dsn <- "VCTRLP.dsn"}
    if (length(options$file.dsn.path)==0){options$file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"}
    
    require(RODBC)
    
    query <-  paste("select buy_sell_type, counterparty, ", 
      "location_nm, contract_month, sum(delta) ",
      "from vcv.app_positions_detail ",
      "where buy_sell_type is not null ", 
      "and counterparty is not null ", 
      "and valuation_date = to_date('", options$asOfDate,
      "','YYYY-MM-DD') ", sep="")
    
    connection.string <- paste("FileDSN=", options$file.dsn.path, options$file.dsn, 
            ";UID=vcentral_read;PWD=vcentral_read;", sep="")  
    con <- odbcDriverConnect(connection.string)  
    
    if (length(options$books)!=0){
        books <- paste(toupper(options$books), sep="", collapse="', '")
        books <- paste("profit_center_1 in ('", books, "')", sep = "")
    } 
    
##     if (length(options$portfolios)!=0){
##         portfolios <- paste(toupper(options$portfolios), sep="", collapse="', '")
##         portfolios <- paste("reporting_portfolio in ('", portfolios, "')", sep = "")
##     } 
    
    if (length(options$books)!=0 & length(options$portfolios)!=0) {
        query <- paste(query, " and ( ", books, " or ", portfolios, " )", sep = "")
    } else if (length(options$books)!=0 & length(options$portfolios)==0) {
        query <- paste(query, " and  ", books, sep = "")
    } else  if (length(options$books)==0 & length(options$portfolios)!=0) {
        query <- paste(query, " and  ", portfolios, sep = "")
    }                        
    query <- paste(query, "group by buy_sell_type, counterparty, ",
      "location_nm, contract_month", sep="")
    
    data <- NULL
    rLog("Querying VCV.APP_POSITIONS_DETAIL...")
    data <- sqlQuery(con, query)
    names(data)[c(3,4,5)] <- c("CURVE_NAME", "CONTRACT_DT", "DELTA")
    ind <- which(sapply(data, is.factor))
    data[,ind] <- sapply(data[,ind], as.character)
    data$CONTRACT_DT <- as.Date(data$CONTRACT_DT)
     odbcCloseAll()

    data <- subset(data, DELTA != 0)
   
    # remove positions w/o curves
    ind <- which(data$CURVE_NAME == "NO CURVE NAME")
    if (length(ind)>0)
    {
        data = data[-ind,]
    }
    data$CURVE_NAME = toupper(data$CURVE_NAME)
    
    # fix this stupid problem here
    ind <- grep("COMMOD PWR NEPOOL", data$CURVE_NAME)
    if (length(ind)>0){data$CURVE_NAME[ind] <- "COMMOD PWX 5X16 BOS PHYSICAL"}
    names(data) <- gsub("_", ".", tolower(names(data)))
   
    rLog("Done querying db.")
    return(data)
}



