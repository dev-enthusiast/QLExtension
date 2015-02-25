# Get the portfolio hierarchy given a portfolio.  
#
# parent <- "CPS Overall Portfolio"  # it's pretty slow for this one!
# parent <- "Alberta Accrual Portfolio"
# source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.portfolio.hierarchy.R")
# get.portfolio.hierarchy(parent)
#
# Written by Adrian Dragulescu 23-Feb-2006


get.book.attributes <- function(books=NULL)
{
  require(RODBC)
  dsnPath = "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/GRIZZLYP.dsn"
  conString <- paste( dsnPath, ";UID=RMG_READ;PWD=RMG_READ;", sep="")
  con = odbcDriverConnect(conString)
  query <- paste("SELECT * from bookmgr.rmg_bookmappings", sep="")
  MM    <- sqlQuery(con, query)
  odbcClose(con)

  if (!is.null(books)){
    MM <- subset(MM, BOOK %in% books)
  }
  
  return(MM)
}
