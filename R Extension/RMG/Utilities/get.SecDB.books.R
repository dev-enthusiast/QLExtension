## Get books given a portfolio from SecDB
##
## source("H:/user/R/RMG/Utilities/get.SecDB.books.R")
## matlab <- Matlab(host="localhost")
## open(matlab)
## portfolio <- "Gas Trading Portfolio"
## books <- get.SecDB.books(portfolio, matlab)
##
## Written by Adrian Dragulescu on 6-Feb-2006

get.SecDB.books <- function(portfolio, matlab){

  source("H:/user/R/RMG/Utilities/lib.matlab.to.SecDB.R")
  books <- sdbGetValue(portfolio, "Nodes", matlab)
  res <- as.character(as.matrix(as.data.frame(books[[1]]$v[[1]]$n)))

  return(res)
}

