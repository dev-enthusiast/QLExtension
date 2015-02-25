# Scrape the Saint John port authority for canaport data
#
#


#################################################################
# 
#
.archive_and_email <- function( x )
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/LNG/Raw/"

  archiveName <- paste(DIR, "all_ships.RData", sep="")
  if ( !file.exists( archiveName ) ) {
    AA <- x
    save(AA, file=archiveName)
  }
  load(archiveName) # load AA

  x <- x[, colnames(AA)]
  
  x0 <- subset(AA, tstamp == max(AA$tstamp))  # latest archived info
  res <- .compare_data_frames(x0[,-1], x[,-1])

  if (nrow(res$yNew)) {
    subject <- "New LNG ship at Canaport!"
    out <- c("NEW:\n", capture.output(res$yNew))
    if (nrow(res$xRemoved)>0)
      out <- c(out, "\n\nREMOVED/MODIFIED:\n", capture.output(res$xRemoved))
    
    out <- c(out,
      paste("\n\n\nPlease contact Adrian Dragulescu for questions regarding",
       "this procmon job."),
      paste("Procmon job:",
       "RMG/R/Reports/Energy/Trading/Congestion/download_lng_canaport"))

    to <- paste(c("robert.cirincione@constellation.com",
      "steven.webster@constellation.com", 
      "mitchell.walk@constellation.com", "al.gallo@constellation.com",
      "paul.nelson@constellation.com", "david.hochberg@constellation.com",
      "robert.gennarini@constellation.com", 
      "necash@constellation.com"), sep="", collapse=",")
    #to <- "adrian.dragulescu@constellation.com"
    from <- "OVERNIGHT_PM_REPORTS@constellation.com"
    sendEmail(from, to, subject, out)

    # save the new archive
    AA <- merge(AA, x, all=TRUE) 
    save(AA, file=archiveName)
    rLog("Archived new entries.")
  }
  
}



#################################################################
# Assume 6 columns for the table.
# @param tb is the table, a org.jsoup.select.Elements
# @return res a character matrix with the table
#
## <tr> 
##  <td class="blueCell"><span class="whiteText">Vessel Name</span></td> 
##  <td class="blueCell"><span class="whiteText">Ship Line</span></td> 
##  <td class="blueCell"><span class="whiteText">Berth</span></td> 
##  <td class="blueCell"><span class="whiteText">Agent</span></td> 
##  <td class="blueCell"><span class="whiteText">Date of Arrival</span></td> 
##  <td class="blueCell"><span class="whiteText">Cargo Activity</span></td> 
## </tr>
## <tr>
##  <td class="row2">Asphalt Eagle</td>
##  <td class="row2">Charter</td>
##  <td class="row2">CB#1</td>
##  <td class="row2">Kent Agency</td>
##  <td class="row2">2013-05-12</td>
##  <td class="row2">Petroleum for foreign ports</td>
## </tr>

.extract_table <- function(tb)
{
  noRows <- tb$size()
  noCols <- tb$get(0L)$select("td")$toArray()$length
  
  res <- matrix("", ncol=noCols, nrow=noRows)
  for (r in 1:noRows) {
    row <- tb$get(as.integer(r-1))
    rb <- row$select("td")          # cols are embedded in a td element
    #.jclass(rb)
    
    for (c in 1:noCols) {      
      aux <- rb$get(as.integer(c-1))
      res[r,c] <- aux$text()
    }
    
  }

  colnames(res) <- gsub(" ", ".", res[1,])
  res <- data.frame(res[-1,])
  res <- cbind(tstamp=format(Sys.time()), res)
  
  res
}


#################################################################
# 
#
.scrape_data <- function()
{
  #URL <- "http://www.sjport.com/english/port_services/daily_vessel_report_print.asp"
  URL <- "http://www.sjport.com/business-resources/cargo-movement/daily-vessel-activity-report/"
  suppressWarnings(str <- readLines(URL))
  str <- paste(str, sep="", collapse="")
  
  doc  <- J("org.jsoup.Jsoup")$parse(str)
  body <- doc$body()
  ch1  <- body$child(0L)
  main <- body$getElementsByClass("maincontent")

  tbs <- main$select("table")  # all the tables
  #t1 <- tbs$get(as.integer(0))   # first table: vessels in port  
  #cat(t1$toString())
  t2 <- tbs$get(as.integer(1))   # second table: expected vessels in port 
  tb <- t2$select("tbody")$select("tr")   # the tbody
  cat(tb$toString())

  
  res <- .extract_table(tb)
  
  ind <- grepl("LIQUID NATURAL GAS", toupper(res$Cargo.Activity)) |
         grepl("LNG", toupper(res$Cargo.Activity)) |
         grepl("NATURAL GAS", toupper(res$Cargo.Activity)) |
         grepl("BW GDF SUEZ BOSTON", toupper(res$Vessel.Name)) # this one has wrong description
  
  res <- res[ind, ]

  colnames(res)[which(colnames(res)=="E.T.A")] <- "E.T.A."
  
  res
}



#################################################################
#################################################################
options(width=500)  # make some room for the output
require(CEGbase)
require(reshape2)
require(rJava)
.jinit()
.jaddClassPath("S:/All/Structured Risk/NEPOOL/Software/Utilities/jsoup-1.7.2.jar")

rLog(paste("Start proces at", Sys.time()))
returnCode <- 99    # succeed = 0

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.compare.df.R")

x <- try(.scrape_data())

if (class(x) != "try-error" && nrow(x) != 0) {
  .archive_and_email(x)
} else {
  rLog("Nothing to archive.  Exiting.")
}

hour <- as.numeric(format(Sys.time(), "%H"))
if (hour >= 18)
  returnCode <- 0           # succeed after 6pm

q( status = returnCode )












## .extract_row <- function(x)
## {
##   indtr <- grep("<tr>", x)

##   res <- NULL
##   for (i in indtr) {
##     aux <- c(takeWhile(x[i:length(x)], function(z){z != "</tr>"}), "</tr>")
##     aux <- paste(aux, sep="", collapse="")
##     aux <- gsub("<tr>(.*)?</tr>", "\\1", aux)
##     aux <- gsub("^ ", "", aux)
    
##     bux <- .extract_td( aux )
##     browser()
##     if (length(bux) != 5)
##       next
##     cux <- as.data.frame(lapply(bux, .extract_contents))
##     colnames(cux) <- 1:ncol(cux)
##     #print(cux)
##     res <- rbind(res, cux)    
##   }
##   colnames(res) <- res[1,]
##   res <- res[-1,]

##   res
## }



## #################################################################
## # 
## #
## .extract_contents <- function(x)
## {
##   gsub(".*>(.*)</td>", "\\1", x)
## }

## #################################################################
## # one row at a time.
## # "<td class=\"row2\">Alpine Stealth</td><td class=\"row2\">Charter</td><td class=\"row2\">Anchor</td><td class=\"row2\">Kent Agency</td><td class=\"row2\">2013-05-10</td><td class=\"row2\">Petroleum for foreign ports</td>"
## # to "Alpine Stealth", "Charter", "Anchor", "Kent Agency", "2013-05-10", "Petroleum for foreign ports"

## #
## .extract_td <- function( x )
## {
##   y <- strsplit(x,"</td>")[[1]]
##   gsub("<td.*>(.*)", "\\1", y)
## }

