# Main function for getting data from LIM.
#
# query             - string, query to be sent to MIM server
# options$colnames  - array of strings for data.frame results 
# options$server    - name or IP address of the MIM server
# options$port      - integer, port number to access the MIM server
# options$num.units - integer, frequency of returned data
# options$units	    - character, can be one of the following: 
#    ("seconds","minute","hour","day","week","month","quarter","year")
#
#
# Adapted from the SPLUS version by Adrian Dragulescu, 2005-05-01.


read.LIM <- function(query, options){

  if (nargs()==1){
    options=NULL
#    options$server    <- "limprod"
#    options$server    <- "v440-msw-17"  # ATZ under MacAnespie, Alan request
    options$port      <- as.integer(0)
    options$num.units <- as.integer(1)
    options$units     <- "day"
    options$colnames  <- NULL            # get LIM colnames
  }
  if (length(options$server)==0){options$server <- "v440-msw-17"}
  if (length(options$port)==0){options$port <- as.integer(0)}
  if (length(options$num.units)==0){options$num.units <- as.integer(1)}
  if (length(options$units)==0){options$units <- "day"}
  if (length(options$colnames)==0){options$colnames <- NULL}
  if (!is.loaded("xmim_query_execute")){
    dyn.load("H:/user/R/RMG/Utilities/Database/LIM/Source/RMGxmimR.dll")
  }
  
  all.units <- c("seconds","minute","hour","day","week","month","quarter","year")
  options$units <- as.integer(grep(tolower(options$units), all.units))
 
  #------------------------------ determine amount of data MIM will return
  retval <- nrows <- ncols <- integer(1)
  r <- .C("xmim_query_execute", query, options$server, options$port, retval,
          nrows, ncols, options$num.units, options$units)
  maxbuf <- as.integer(r[[5]][1] * r[[6]][1])
  #------------------------------ validate results
  if (r[[4]][1] < 0 || maxbuf <= 0){
    warning(paste("The specified query statement could not be executed. ",
      "Perhaps the statement is invalid: \"", query, "\".\nAnother possibility",
      " is that the specified MIM server could not be accessed.\n", sep = ""))
    .C("xmim_query_finish")
    return(data.frame())
  }
  #------------------------------------get data from MIM
  offset <- as.integer(0)
  n <-  nrows <- ncols <- integer(1)
  y <- m <- d <- h <- mm <- s <- integer(maxbuf)
  v <- double(maxbuf)
  one.label <- paste(rep(" ", length=256), sep="", collapse="")
  labels    <- rep(one.label, r[[6]][1]) # create labels [ncols, 256]
  r <- .C("xmim_query_results", as.integer(maxbuf), as.integer(offset), n,
	  y, m, d, h, mm, s, v, nrows, ncols, labels)
  .C("xmim_query_finish")
  #------------------------------------ create return results
  ind <- 1:r[[11]][1]
  dates <- strptime(paste(r[[4]][ind], r[[5]][ind], r[[6]][ind], 
    r[[7]][ind], r[[8]][ind], r[[9]][ind]), format="%Y %m %d %H %M %S")     
  MM <- matrix(data=r[[10]], nrow=r[[11]][1], ncol=r[[12]][1], byrow = T)
  res <- data.frame(Date = dates, MM)     # create data.frame to return
  if (length(options$colnames)==0){       # if no colnames specified
    options$colnames <- make.names(r[[13]], unique=T)
  }
  colnames(res) <- c("Date", options$colnames)

  return(res)
}

