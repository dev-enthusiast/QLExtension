# Rewrite it in R...
#
# check_new_notices
# .db_insert
# .db_read
# .db_trim
# get_urls
# read_notice
#
#
# ADD PNGTS!
#   http://www.gasnom.com/ip/pngts/notices/notices_1_42.html
#   http://www.gasnom.com/ip/pngts/notices/notices_3_42.html
#

CONFIG <- list(
  ALGCG = list(
    url=c("https://infopost.spectraenergy.com/infopost/noticesList.asp?pipe=AG&type=CRI",
          "https://infopost.spectraenergy.com/infopost/NoticesList.asp?pipe=AG&type=OUT"),
    email="necash@constellation.com,steven.webster@constellation.com"),
    #paste(c("NECash@constellation.com","steven.webster@constellation.com",
    #  "robert.gennarini@constellation.com", "robert.cirincione@constellation.com"), sep=",")),
               
  MNCA = list(
    url=c("https://infopost.spectraenergy.com/infopost/noticesList.asp?pipe=MNCA&type=CRI",
          "https://infopost.spectraenergy.com/infopost/noticesList.asp?pipe=MNCA&type=OUT"),
    email="steven.webster@constellation.com,necash@constellation.com"),
    ## email=paste(c("NECash@constellation.com","steven.webster@constellation.com",
    ##    "robert.gennarini@constellation.com", "robert.cirincione@constellation.com"), sep=",")),

  MNUS = list(
    url=c("https://infopost.spectraenergy.com/infopost/noticesList.asp?pipe=MNUS&type=CRI",
          "https://infopost.spectraenergy.com/infopost/noticesList.asp?pipe=MNUS&type=OUT"),
    email="steven.webster@constellation.com,necash@constellation.com")
    ## email=paste(c("NECash@constellation.com","steven.webster@constellation.com",
    ##    "robert.gennarini@constellation.com", "robert.cirincione@constellation.com"), sep=","))
)


#################################################################
# Insert new entries in the db file
#
.db_insert <- function(newUrls)
{
  if (length(newUrls)==0) {
    rLog("Found no new notices to archive")
    return(invisible())
  }
  
  fname <- "S:/All/Structured Risk/NEPOOL/Software/Apps/Pipewatcher/resources/pipewatcher_archive.csv"
  db <- read.csv(fname)

  rLog("Adding", length(newUrls), "new entries to the db.")
  db <- rbind(db,
    data.frame(url = newUrls,
               timestamp = paste("Inserted on:", Sys.Date(), sep="")))
  
  write.csv(db, file=fname, row.names=FALSE)
  rLog(paste("Updated ", fname))
}


#################################################################
# Read the archive
# @return a notice as a string
#
.db_read <- function()
{
  fname <- "S:/All/Structured Risk/NEPOOL/Software/Apps/Pipewatcher/resources/pipewatcher_archive.csv"

  if (file.exists(fname)) {
    db <- read.csv(fname)
  } else {
    db <- data.frame(url="", timestamp="")
  }

  db
}


#################################################################
# Evict notices older than 400 days 
#
.db_trim <- function()
{
  fname <- "S:/All/Structured Risk/NEPOOL/Software/Apps/Pipewatcher/resources/pipewatcher_archive.csv"
  db <- read.csv(fname)

  days <- as.Date(gsub("Inserted on:", "", db$timestamp))
  ind <- which(as.numeric(Sys.Date() - days) > 400)
  if (length(ind) > 0) {
    rLog("Trimming the archive")
    db <- db[-ind,]
    
    write.csv(db, file=fname, row.names=FALSE)
    rLog(paste("Updated ", fname))
  } 
}



#################################################################
# Look for new notices.  Loop over all the pipelines.
# 
#
check_new_notices <- function()
{
  db <- .db_read()

  urlsToArchive <- NULL
  
  for (pipeId in names(CONFIG)) {
    #browser()
    rLog("Working on pipeId", pipeId)
    
    # each pipeline can have several urls to check for notices
    # so aggregate them
    urls <- NULL
    for (url_master in CONFIG[[pipeId]]$url) {
      try(urls_page <- get_urls( url_master ) )
      #browser()

      if (class(urls_page) == "try-error") {
        next
      } else {
        urls <- c(urls, urls_page)
      } 
    }
    

    # check if there are new notices
    newUrls <- setdiff(urls, db$url)
    urlsToArchive <- c(urlsToArchive, newUrls)  # accumulate the new notices
    
    if (length(newUrls)>0) {
      
      # loop over the new urls
      for (url in newUrls) {
        try(notice <- read_notice(url))
        
        if (class(notice)=="try-error")
          notice <- "Could not read the notice.  Click on the link above."
        
        subject <- paste("Pipewatch Alert!  New critical notice for", pipeId)
        out <- c(paste("See url:", url), "\n\n",  notice,
                 "\n\n", "Contact: Adrian Dragulescu")

        to <- CONFIG[[pipeId]]$email
        #to <- c("adrian.dragulescu@constellation.com")
        from <- "OVERNIGHT_PM_REPORTS@constellation.com"

        if (is_important(notice)) {
          sendEmail(from, paste(to, collapse=","), subject, out)
        }
      } 
    }
  }
  
  .db_insert(urlsToArchive)         # archive them 
}


#################################################################
# Get the urls of the individual notices from the master page
# @param URL, a url where notices are posted 
# @return a vector of string urls, relative urls
#
get_urls <- function( URL )
{
  # prior to 2/13/2015 the protocol was http so simple readLines worked
  # suppressWarnings(str <- readLines( url(master_url) ))
  URL <- "https://infopost.spectraenergy.com/infopost/noticesList.asp?pipe=AG&type=CRI"
  r <- GET( URL )
  str <- content(r, "text")  
  
  doc  <- J("org.jsoup.Jsoup")$parse(str)
  body <- doc$body()
  main <- doc$getElementById("main")
  table <- main$getElementsByTag("table")

  tr <- table$select("tr")  # each link is inside of this 
  #cat(tr$toString())

  links <- table$select("a[href]")
  N <- links$size()

  # sometimes there are no notices
  if (N == 0)
    return(NULL)
  
  res <- vector(mode="character", length=N)
  for (i in 1:N) {
    aux <- links$get(as.integer(i-1))
    res[i] <- aux$attr("href")
  }


  paste(dirname(URL), res, sep="/")  
}


#################################################################
# 
#
is_important <- function(notice)
{
  grepl("outage", tolower(notice))
}





#################################################################
# Read one particular notice. 
# @param url
# @return a notice as a string
#
read_notice <- function(fullUrl)
{
  con  <- J("org.jsoup.Jsoup")$connect(fullUrl)
  doc <- con$get()

  contents <- doc$getElementById("bulletin")
  contents$text()
}



#################################################################
#
.test <- function()
{
  require(CEGbase)
  require(httr)
  require(rJava)
  .jinit()
  .jaddClassPath("S:/All/Structured Risk/NEPOOL/Software/Utilities/jsoup-1.7.2.jar")
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pipewatcher.R")

  check_new_notices()

  
  .db_trim()
  
  
  
}
