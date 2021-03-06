
bookMappingCPSUAT <- function(options){
  #con  <- odbcConnect("CPSPROD", uid="stratdatread", pwd="stratdatread")  
  file.dsn.path = "H:/user/R/RMG/Utilities/DSN/"
  file.dsn = "CPSUAT.dsn"
  connection.string <- paste("FileDSN=", file.dsn.path, file.dsn,
     ";UID=foitsox;PWD=foitsox;", sep="")
  con <- odbcDriverConnect(connection.string) 
  
  queryCore <- paste("SELECT BOOK as BOOK_NAME, ACCT_TREATMENT as ACCOUNTING_TREATMENT, ",
    "ECONOMIC_OWNER as ECONOMIC_OWNER,   ",
    "EXECUTIVE_OWNER as EXECUTIVE_OWNER, OFFICE as OFFICE,  ORGANIZATION as ORGANIZATION ",
    "FROM FOITSOX.RMG_BOOKMAPPINGS  ", sep = "")      
  data <- NULL
  iIn <- 0
  while(iIn*options$nFieldLimit < length(options$books)) {
    books <- paste(options$books[(iIn*options$nFieldLimit+1):min(length(options$books),(iIn+1)*options$nFieldLimit)], 
                    sep="", collapse="', '")
    query <- paste(queryCore, "WHERE UPPER(BOOK) in ('", books, "') and EXCLUDE_FLAG is null ", sep="")
    data <- rbind(data, sqlQuery(con, query))
    iIn <- iIn + 1
  }    
  odbcCloseAll()
  colnames(data)[1] <- "BOOK"
  return(data)
}

