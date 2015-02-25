# Get the delta archive view from VCentral for a given set of books.
#
# options$asOfDate <- Sys.Date()-1
# options$books <- c("NYTRADE", "JBOPT")  # leave empty if you want all
# source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.vcentral.deltas.R")
# res <- get.vcentral.deltas(options)


get.vcentral.deltas <- function(options){

  if (length(options$asOfDate)==0){options$asOfDate <- Sys.Date()-1}
    
  require(RODBC)
  query <- paste("SELECT TH.VALUATION_DATE as REPORT_DATE, ", 
    "upper(trim(TH.Profit_Center_1)) as BOOK, upper(trim(TD.location_nm)) ",
    "as COMMOD_CURVE, valuation_month AS contract_date, sum(TD.delta) ",
    "AS DELTA_VALUE FROM vcentral.trade_header th join vcentral.trade_detail ",
    "td on th.valuation_date = td.valuation_date and th.trade_header_id = ",
    "td.trade_header_id WHERE td.valuation_date=to_date('", options$asOfDate,
    "', 'yyyy-mm-dd') AND upper(trim(td.location_nm)) like '%COMMOD%' ",   
    "AND td.source_feed = 'R' AND th.source_eid=th.sor_indicator ",
    "add.book.subset ",              
    "GROUP BY TH.VALUATION_DATE, upper(trim(TH.Profit_Center_1)), ", 
    "upper(trim(TD.location_nm)), valuation_month ", sep="")
#        HAVING Abs(sum(TD.delta))>0.06

  if (length(options$books)!=0){
    books <- paste(toupper(options$books), sep="", collapse="', '")
    books <- paste("and th.profit_center_1 in ('", books, "')")
    query <- gsub("add.book.subset", books, query)
  } else {
    query <- gsub("add.book.subset ", "", query)
  }
  
  con  <- odbcConnect("VCTRLP", uid="vcentral_read", pwd="vcentral_read")
  data <- sqlQuery(con, query)
  odbcCloseAll()

  return(data)
}
