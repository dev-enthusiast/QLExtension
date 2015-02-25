
booksToVarMapping <- function(books, dateX) {
  con   <- odbcConnect("CPSPROD", uid="stratdatread", pwd="stratdatread")
  query <- paste("SELECT VAR_MAPPING, BOOK from FOITSOX.SDB_VAR_MAPPING ",
                 "where PRICING_DATE = to_date('", dateX, "','yyyy-mm-dd') ", sep = "")
  if (length(books)>0){
    books <- paste(books, sep="", collapse="', '")
    query <- paste(query, "and BOOK in ('", books, "')", sep="")
  }
  MM <- sqlQuery(con, query)
  close(con)
  indNA <- which(is.na(MM$VAR_MAPPING) == TRUE)
  if(length(indNA) > 0) { MM <- MM[-indNA,] }
  return(MM)
}