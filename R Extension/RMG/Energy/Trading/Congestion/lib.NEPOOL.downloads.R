# Various utilities to use when downloading data from NEPOOL
#
# .download_and_archive
# .getLinksInPage 
# .isLink
#
#

################################################################
# Download a set of files from urls and archive them if file is
# not already in archive
# If file names are not unique you can add a prefix.
# @param prefix if you want to add something to the filename 
# @param filenames if you need to overwrite the default 
# Return the filenames you downloaded
#
.download_and_archive <- function(links, DIR_OUT, prefix="",
   overwrite=FALSE, filenames=NULL)
{
  if ( is.null(links) )
    return(NULL)
  
  out <- NULL
  archivedFiles <- list.files(DIR_OUT)
  for (link in links) {
    if (is.null(filenames)) {
      filename <- basename(link)   # should work 99% of time
    } else {
      filename <- filenames[which(links %in% link)]  # for my case!
    }
    fileOut  <- paste(DIR_OUT, prefix, filename, sep="")
    if ( !file.exists(fileOut) | overwrite ) {
      #browser()
      res <- download.file(link, fileOut, mode="wb")
      if (res == 0) {
        out <- c(out, filename)
      }
    }  
  }
  
  out
}



################################################################################
# Nepool is really cool about changing the filenames based on "their" report
# time!  So I need to find them in the html page source ... 
#
# Returns NULL when it does't find it
#
.getLinksInPage <- function(mainLink, pattern="WW_DAHBENERGY_ISO_")
{
  con <- url(mainLink)
  suppressWarnings(LL <- tryCatch(readLines(con),
     error=function(e) NULL, finally=close(con)))

  if (!is.null(LL)) {
    ind <- grep(pattern, LL)
    if (length(ind)==0) {
      cat(paste("Could not find any files with the pattern", pattern, "\n"))
      NULL
    } else {
      LL <- LL[ind]
      aux <- sapply(strsplit(LL, 'href=\"'), "[", 2)
      aux <- unique(sapply(strsplit(aux, '\"'), "[", 1))

      paste("http://www.iso-ne.com", aux, sep="")
    }
  } else {
    NULL     # url does not exist
  }
}


################################################################################
# Check that a given link is valid.
# @param urlName is a string
#
.isLink <- function( urlName )
{
  aux <- try(open(url(urlName)))
  if (class(aux) == "try-error") {
    FALSE
  } else {
    close(url(urlName))
    TRUE
  }
}


################################################################################
#
.test <- function()
{
  require(CEGbase)
  require(reshape)
  

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.downloads.R")

  BASE_URL <- "http://www.iso-ne.com/markets/othrmkts_data/fcm/reports/per/monthlyper/"
  DIR_OUT  <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FCM/PER/Monthly/Raw/"

  year <- 2009
  url <- paste(BASE_URL,  year, "/index.html", sep="")
  links <- .getLinksInPage(url, pattern="\\.gz")    
  .download_and_archive(links, DIR_OUT=DIR_OUT)

  
  

}
