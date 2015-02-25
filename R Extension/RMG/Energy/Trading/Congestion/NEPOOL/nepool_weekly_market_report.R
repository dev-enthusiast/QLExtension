# Used by Condor/archive_nepool_weekly_market_report.R
#
# Written by AAD on 07-Jan-2010

#################################################################
# out is a text, token is an keyword
# fails if last line if indented ...
.extractIndentedToken <- function(out, token)
{
  rLog("Working on token", token)
  ind <- grep(token, out)
  if (length(ind) != 1){
    rLog("   Cannot find", token)
    return(NULL)
  }
  
  keep <- NULL
  while (grepl("^ +", out[ind+1])){
    ind  <- ind + 1
    keep <- c(keep, ind)
   }

  if (length(keep)==0){
    rLog("Cannot find indented entries!")
    return(NULL)
  } else {
    res <- gsub("^ *", "", out[keep])
    return(res)
  }
}

#################################################################
# get the date for the end of the week from the report name
#
.getEndDate <- function(filename)
{
  # the end of the week
  date <- sub(".*([[:digit:]]{4}_[[:digit:]]{2}_[[:digit:]]{2}).*",
              "\\1", filename)
  dateEnd <- as.Date(date, "%Y_%m_%d")

  if (is.na(dateEnd))
    rLog("Cannot extract to end date!")
  
  return(dateEnd)
}


#################################################################
# 
.parse.PDF <- function(filename, page=c(2,3))
{
  rLog("Reading file", filename)
  out <- read.pdf(filename, first=page[1], last=page[2])
  out <- out[-which(out=="")]  # you have some empty lines in the new report
  
  res <- NULL
  token <- "Day-Ahead Price Separation"
  try(res[[token]] <- .extractIndentedToken(out, token))

  token <- "Real-Time Price Separation"
  try(res[[token]] <- .extractIndentedToken(out, token))
  
  token <- "Real-Time Price Corrections"
  try(res[[token]] <- .extractIndentedToken(out, token))

  res <- lapply(res, paste, collapse=" ")
  if (length(res)==0)
    return(NULL)
  
  res <- melt(res)
  names(res)  <- c("comment", "event")
  res$endDate <- .getEndDate(filename)
  res$startDate <- res$endDate - 6
  res <- res[, c("startDate", "endDate", "event", "comment")]
  
  return(res)
}



  


 

