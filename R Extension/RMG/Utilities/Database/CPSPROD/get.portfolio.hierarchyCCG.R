# Get the portfolio hierarchy given a portfolio.  
#
# parent <- "CPS Overall Portfolio"  # it's pretty slow for this one!
# parent <- "Alberta Accrual Portfolio"
# source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.portfolio.hierarchyCCG.R")
# get.portfolio.hierarchy(parent)
#
# Written by Anatoley Zheleznyak 23-Feb-2007

get.portfolio.hierarchyCCG <- function(parent, pricing.dt){
#  browser()
  if (nargs()==1){pricing.dt <- Sys.Date()-1}
  require(RODBC)
  pricing.dt = as.Date( pricing.dt )
  dsnPath = "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/CPSPROD.dsn"
  conString <- paste( dsnPath, ";UID=stratdatread;PWD=stratdatread;", sep="")
  con = odbcDriverConnect(conString)
  query <- paste("SELECT h.pricing_date, rp.name as Portfolio, rc.name as BOOK " ,  
                 "FROM foitsox.ccg_portfolio_all h, foitsox.ccg_portfolio_id_ref rp, foitsox.ccg_portfolio_id_ref rc ", 
                 "WHERE h.pricing_date = TO_DATE('", format(pricing.dt, "%m/%d/%Y"), "','MM/DD/YYYY') ",
                 "AND   upper(rp.name) = '", toupper(parent), "' ", 
                 "AND   h.parent_id = rp.id ", 
		             "AND   h.child_id  = rc.id ",
		             "AND   upper(trim(rc.type)) like 'BOOK'", sep = "") 
  MM    <- sqlQuery(con, query)
  odbcCloseAll()  
  books <- sort( as.character( MM$BOOK) )
  return(books)
}


