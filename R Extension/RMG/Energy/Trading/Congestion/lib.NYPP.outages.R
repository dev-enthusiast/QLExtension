#
# 
#
#

#################################################################
#
download_transfer_limitations <- function()
{
  url <- "http://mis.nyiso.com/public/pdf/ttcf/ttcf.csv"
  
  fname <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/",
                 "NYPP/Outages/TransferLimitations/Raw/ttcf_",
                 format(Sys.Date()), ".csv", sep="")
    
  download.file(url, fname)  
}


#################################################################
#################################################################
#
.test <- function()
{
  
}
