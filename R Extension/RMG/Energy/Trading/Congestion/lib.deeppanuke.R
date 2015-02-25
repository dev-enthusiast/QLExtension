# Deal with Deep Panuke, Sable Island gas production 
#
# .download_pdfs
# .email_file
# .get_links_year
#


#################################################################
# Download the files 
#
.download_pdfs <- function(filenames)
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/DeepPanuke/Raw/"

  downloaded <- NULL
  for (file in filenames) {
    destfile <- paste(DIR, basename(file), sep="")
    if (file.exists(destfile))
      next
        
    rLog("Working on", basename(file))
    res <- download.file(file, destfile, mode="wb")
    if (res == 0) {
      downloaded <- c(downloaded, destfile)
    }
  }

  downloaded
}


#################################################################
#  
#
.email_file <- function(filename)
{
  subject <- "Deep Panuke & Sable Island production update"
  
  out <- .get_content(filename)
  
  to <- paste(c("steven.webster@constellation.com", 
      "mitchell.walk@constellation.com",
      "al.gallo@constellation.com",
      "david.hochberg@constellation.com",
      "robert.gennarini@constellation.com", 
      "necash@constellation.com"), sep="", collapse=",")
  #to <- "adrian.dragulescu@constellation.com"
  from <- "OVERNIGHT_PM_REPORTS@constellation.com"
  
  sendEmail(from, to, subject, out, attachments=filename)
}


#################################################################
#  
#
.get_content <- function(filename)
{
  out <- read.pdf(filename, last=2)

  out <- c(out, "\n\nContact: Adrian Dragulescu",
           "Process: RMG/R/Reports/Energy/Trading/Congestion/download_deeppanuke_activity")
  
  out
}


#################################################################
# Get all the URLs for a given year
#
.get_links_year <- function(year=2013)
{
  URL <- paste("http://www.cnsopb.ns.ca/offshore-activity/weekly-activity-reports/archive/",
               year, sep="")

  aux <- readLines(URL)

  ind <- grep("/default/files/pdfs/", aux)

  res <- gsub('(.*)<a href=\\"(.*)pdf.*', "\\2", aux[ind])
  res <- paste("http://www.cnsopb.ns.ca/", res, "pdf", sep="")
  
  res
}




#################################################################
#
.test <- function()
{
  library(CEGbase)

  source("H:/user/R/RMG/Utilities/read.pdf.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.deeppanuke.R")

  links <- .get_links_year(year=2014)
  
  downloaded_files <- .download_pdfs( links )

  filename <- downloaded_files[1]
  
  .email_file( filename )

  
  
}
