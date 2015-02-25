# Get the portfolio hierarchy given a portfolio.  
#
# parent <- "CPS Overall Portfolio"  # it's pretty slow for this one!
# parent <- "Alberta Accrual Portfolio"
# source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.portfolio.hierarchy.R")
# get.portfolio.hierarchy(parent)
#
# Written by Adrian Dragulescu 23-Feb-2006


get.portfolio.hierarchy <- function(parent, pricing.dt){
  if (nargs()==1){pricing.dt <- Sys.Date()-1}
  pricing.dt <- format(pricing.dt, "%m/%d/%Y")
  require(RODBC)
  dsnPath = "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/CPSPROD.dsn"
  conString <- paste( dsnPath, ";UID=stratdatread;PWD=stratdatread;", sep="")
  con = odbcDriverConnect(conString)
  query <- paste("SELECT * from foitsox.sdb_portfolio_hierarchy ",
    "WHERE PRICING_DATE=TO_DATE('", pricing.dt, "','MM/DD/YYYY')", sep="")
  MM    <- sqlQuery(con, query)
  # check if did not find any books, try another DB
  if( length(which(MM$PARENT == toupper(parent))) == 0 ) {
    query <- paste("SELECT * from foitsox.sdb_portfolio_all ",
      "WHERE PRICING_DATE=TO_DATE('", pricing.dt, "','MM/DD/YYYY')", sep="")
    MM    <- sqlQuery(con, query)
  }  
  odbcClose(con)
  MM <- MM[,c("PARENT", "CHILD")]
  MM$PARENT <- toupper(as.character(MM$PARENT))
  MM$CHILD  <- toupper(as.character(MM$CHILD))
  
  child <- NULL
  get.children <- function(parent){
    child  <<- c(child, setdiff(parent, MM[,1]))
    parent <- MM[which(MM[,1] %in% parent),2]
    if (length(parent)>0){
      get.children(parent)
    } else {
      return(child)
    }
  }
  res <- get.children(toupper(parent))
  ind <- which(nchar(res)>8)      # remove strings longer than 8 char
  if (length(ind)>0){res <- res[-ind]}
  
  return(sort(res))
}


##   parent    <- toupper(parent)
##   MM$PARENT <- toupper(as.character(MM$PARENT))  # take care of name case!
##   MM$CHILD  <- toupper(as.character(MM$CHILD))
##   get.children <- function(MM, parent){
##     ind <- as.character(which(MM$PARENT %in% parent))
##     return(as.character(MM[ind,"CHILD"]))
##   }
##   res <- data.frame(top=parent)
##   while (length(get.children(MM, res[,1]))>0){
##     lres   <- NULL
##     for (p in 1:nrow(res)){
##       children <- get.children(MM, as.character(res[p,1]))
##       if (length(children)==0){
##         lres <- rbind(lres, cbind(children=res[p,1], res[p,]))
##       } else if (length(children)==1){
##         lres <- rbind(lres, cbind(children, res[p,]))
##       } else {
##         aux <- apply(as.matrix(res[p,]), 2, rep, length(children))
##         lres <- rbind(lres, cbind(children, as.data.frame(aux)))
##       }
##     }
##     res <- lres
##   }
##   colnames(res) <- paste("level",ncol(res):1,sep=".")
