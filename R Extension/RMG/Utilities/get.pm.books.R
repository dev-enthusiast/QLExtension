#
#
#  source("H:/user/R/RMG/Utilities/get.pm.books.R")
#  x <- get.pm.books()
get.pm.books <- function(){
  library(RODBC)
  #con  <- odbcConnect("CPSPROD", uid="stratdatread", pwd="stratdatread")  
  file.dsn.path = "H:/user/R/RMG/Utilities/DSN/"
  file.dsn = "GRIZZLYP.dsn"
  connection.string <- paste("FileDSN=", file.dsn.path, file.dsn,
     ";UID=rmg_read;PWD=rmg_read;", sep="")
  con <- odbcDriverConnect(connection.string) 
  
  query <- paste("SELECT BOOK ",
    "FROM bookmgr.RMG_BOOKMAPPINGS  ", 
    "WHERE RISK_REPORTING_FLAG = 'Y' ",
    "  AND ORGANIZATION = 'PM' ",
    "  AND EXECUTIVE_OWNER = 'Mark Orman' ", 
    "  AND BOOK NOT IN ('FIGHTMD', 'FIGHTSEL', 'SCOUTMD', 'SCOUTSEL' ) ", sep = "")      
  data <- NULL
  data <- sqlQuery(con, query)
  
  odbcCloseAll()

  return(data)
}

get.upstream.books <- function(){
  library(RODBC)
  #con  <- odbcConnect("CPSPROD", uid="stratdatread", pwd="stratdatread")  
  file.dsn.path = "H:/user/R/RMG/Utilities/DSN/"
  file.dsn = "GRIZZLYP.dsn"
  connection.string <- paste("FileDSN=", file.dsn.path, file.dsn,
     ";UID=rmg_read;PWD=rmg_read;", sep="")
  con <- odbcDriverConnect(connection.string) 
  
  query <- paste("SELECT BOOK ",
    "FROM bookmgr.RMG_BOOKMAPPINGS  ", 
    "WHERE EXCLUDE_FLAG is null ",
    "  AND ORGANIZATION = 'PM' ", 
    "  AND ECONOMIC_OWNER = 'Jack Miglioretti' ",
    "  AND EXECUTIVE_OWNER = 'Mark Orman' ", 
    "  AND BOOK NOT IN ('FIGHTMD', 'FIGHTSEL', 'SCOUTMD', 'SCOUTSEL' ) ", sep = "")      
  data <- NULL
  data <- sqlQuery(con, query)
  
  odbcCloseAll()

  return(data)
}