###############################################################################
# get.positions.from.vcv.vcentral.R
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
source("H:/user/R/RMG/Utilities/load.R")
try(memory.limit(3*1024), silent=TRUE)


###############################################################################
get.positions.from.vcv.vcentral <- function(options) 
{
    if (length(options$asOfDate)==0){options$asOfDate <- Sys.Date()-1}
    if (length(options$file.dsn)==0){options$file.dsn <- "VCTRLP.dsn"}
    if (length(options$file.dsn.path)==0){
      options$file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"}
    
    require(RODBC)
    
    query <-  paste(
            "SELECT ",
            "PROFIT_CENTER_1 BOOK, LOCATION_NM CURVE_NAME, CONTRACT_MONTH, ", 
            "DELTA, DENOMINATED, ",
            "ACCOUNTING_TREATMENT, CCG_ORGANIZATION, CONV_FCTR_MMBTU, ",
            "DELIVERY_LOCATION, DELIVERY_POINT, ECONOMIC_OWNER, ",
            "EXECUTIVE_OWNER, FAS_133_ACCT, FAS_133_REGION, LINKED_LOCATION, ",  
            "MAPPED_DELIVERY_POINT, MAPPED_REGION_NAME, COUNTERPARTY, ", 
            "PROFIT_CENTER_2, SOR_INDICATOR, MTM_VALUE_USD ",
            "FROM VCV.APP_POSITIONS_AGGREGATE ", 
            "WHERE VALUATION_DATE = to_date('", 
            format(options$asOfDate, "%m/%d/%Y"), "','MM/DD/YYYY') ", sep = "")
    
    connection.string <- paste("FileDSN=", options$file.dsn.path, options$file.dsn, 
            ";UID=vcentral_read;PWD=vcentral_read;", sep="")  
    con <- odbcDriverConnect(connection.string)  
    
    if (length(options$books)!=0){
        books <- paste(toupper(options$books), sep="", collapse="', '")
        books <- paste("profit_center_1 in ('", books, "')", sep = "")
    } 
    
    if (length(options$portfolios)!=0){
        portfolios <- paste(toupper(options$portfolios), sep="", collapse="', '")
        portfolios <- paste("reporting_portfolio in ('", portfolios, "')", sep = "")
    } 
    
    if (length(options$books)!=0 & length(options$portfolios)!=0) {
        query <- paste(query, " and ( ", books, " or ", portfolios, " )", sep = "")
    } else if (length(options$books)!=0 & length(options$portfolios)==0) {
        query <- paste(query, " and  ", books, sep = "")
    } else  if (length(options$books)==0 & length(options$portfolios)!=0) {
        query <- paste(query, " and  ", portfolios, sep = "")
    }                        
    
    data <- NULL
    rLog("Querying VCV.APP_POSITIONS_AGGREGATE...")
    data <- sqlQuery(con, query) 
    
    odbcCloseAll()
    
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
    
    rLog("Done querying db.")
    return(data)
}



