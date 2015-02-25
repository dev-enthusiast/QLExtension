# Pull Deep Panuke activity
#
#
#
#

#################################################################
#################################################################
options(width=500)  # make some room for the output
require(CEGbase)

source("H:/user/R/RMG/Utilities/read.pdf.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.deeppanuke.R")

rLog(paste("Start proces at", Sys.time()))
returnCode <- 99    # succeed = 0

year <- as.numeric(format(Sys.Date(), "%Y"))

links <- .get_links_year(year=year)
  
downloaded_files <- .download_pdfs( links )

if (length(downloaded_files) > 0) {
  for (filename in downloaded_files) 
    .email_file( filename )
}

hour <- as.numeric(format(Sys.time(), "%H"))
if (hour >= 17)
  returnCode <- 0           # succeed after 6pm

q( status = returnCode )


