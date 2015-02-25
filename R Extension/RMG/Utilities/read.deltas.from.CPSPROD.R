# Read the house positions from CPSPROD database, where
# options$asOfDate    <- as.Date("2005-12-14")
# options$books       <- c("NYTCC1", "PJMVNY")     # optional
# options$regions     <- "NYPP"                    # optional  
#


read.deltas.from.CPSPROD <- function(options){

  #---------------------------------------READ DATA IN---------------------
  require(RODBC)
#  browser()
  con  <- odbcConnect("CPSPROD", uid="stratdatread", pwd="stratdatread")  
  queryCore <- paste("SELECT PORTFOLIO, BOOK, REPORTING_PORTFOLIO, COMMOD_CURVE, MARKET_FAMILY, MARKET_SYMBOL, ",
    "MAPPED_REGION_NAME, REGION_NAME, SERVICE_TYPE, MAPPED_DELIVERY_POINT, QUANTITY_BUCKET_TYPE, ",
    "DELIVERY_MONTH, DELIVERY_YEAR, CONTRACT_DATE, DELTA_VALUE, BOOK_OWNER, FLOW_TYPE, BOOK_ACCT_TREATMENT, ",
    "DELTA_ADJUSTED_VALUE, BOOK_ACCT_TYPE, QUANTITY_UNIT, ACCOUNTING_PORTFOLIO, ",
    "ACCOUNTING_REGION, ACCOUNTING_SUBREGION, SEAT_LABEL ",
    "FROM STRATDAT.DELTA_ARCHIVE ",
    "WHERE REPORT_DATE=TO_DATE('", format(options$asOfDate, "%m/%d/%Y"),"','MM/DD/YYYY') ",
    "AND REPORT_TYPE='ARCHIVE' ",  sep="")
  if( length(options$regions) > 0 ) {
    if( is.na(options$regions[1]) == FALSE ) {
      regions <- paste(toupper(options$regions), sep="", collapse="', '")
      queryCore   <- paste(queryCore, "AND UPPER(MAPPED_REGION_NAME) in ('", regions, "')", sep="")
    }  
  }      
  if (length(options$books)>0){
    data <- NULL
    iIn <- 0
    while(iIn*options$nFieldLimit < length(options$books)) {
      books <- paste(options$books[(iIn*options$nFieldLimit+1):min(length(options$books),(iIn+1)*options$nFieldLimit)], 
                    sep="", collapse="', '")
      query <- paste(queryCore, "AND UPPER(BOOK) in ('", books, "')", sep="")
      data <- rbind(data, sqlQuery(con, query))
      iIn <- iIn + 1
    }    
  } else { data <- sqlQuery(con, queryCore) }
  odbcCloseAll()
  data[, "CONTRACT_DATE"] <- as.Date(data[, "CONTRACT_DATE"])
  return(data)
}

