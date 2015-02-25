# Several utilities for the VaR project.  
#
#
# Written by Adrian Dragulescu on 12-Dec-2006


#======================================================================
dollar <- function(x, digits=0){
  prettyNum(round(x, digits=digits), big.mark=",", nsmall=digits)}

#======================================================================
get.books <- function(pHier, portfolio){
  child <- NULL
  get.one.level.down <- function(pHier, parent){
    child  <<- c(child, setdiff(parent, pHier$parent))
    parent <- pHier$child[which(pHier$parent %in% parent)]
    if (length(parent)>0){
      get.one.level.down(pHier, parent)
    } else {
      return(child)
    }
  }
  res <- get.one.level.down(pHier, portfolio)
  return(res)
}

#======================================================================
gas.season <- function(contract.dt){  # contract.dt is a vector of R Dates
  aux <- data.frame(mm  = as.numeric(format(contract.dt, "%m")),
                    yy  = format(contract.dt, "%y"),
                    pyy = format(contract.dt-365, "%y"), # previous
                    yyp = format(contract.dt+365, "%y")) # post
  aux$cut <- as.numeric(cut(aux$mm, breaks=c(0, 3, 10, 12)))
  aux$season <- NA
  ind <- aux$cut == 1
  aux$season[ind]  <- paste("W.",aux$pyy[ind],"-",aux$yy[ind], sep="")
  ind <- aux$cut == 2
  aux$season[ind]  <- paste("S.",aux$yy[ind], sep="")
  ind <- aux$cut == 3
  aux$season[ind]  <- paste("W.",aux$yy[ind],"-",aux$yyp[ind], sep="")
  return(aux$season)
}

## make.curve.hierarchy <- function(curve.info){
##   #------------------------------------for now, do gas only
##   ind <- grep("COMMOD NG ", curve.info$curve.name)
##   curve.info <- curve.info[order(curve.info$region), ]
##   curve.info$hub.name  <- "Nymex"
##   curve.info$hub.curve <- "COMMOD NG EXCHANGE"
##   ind <- grep("canada", tolower(curve.info$region))
##   curve.info$hub.name <- curve.info$hub.curve <- NA
##   curve.info$hub.name[ind]  <- "AECO"
##   curve.info$hub.curve[ind] <- "COMMOD NG AECALB CGPR USD"
## }
