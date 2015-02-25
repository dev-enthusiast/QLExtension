# Download a file from the internet using the wget utility
#
# options=NULL
# options$file <- "C:/temp/junk.xls"
# options$url <- "http://www.iso-ne.com/markets/othrmkts_data/ftr/auction_results/2005/jun/FTRRSLT_JUN2005.xls"
#
# Written by Adrian Dragulescu on 12-Jul-2005

download.Web <- function(options){

   command <- paste("wget -O", options$file, options$url)
   shell(command)
}

