###############################################################################
# Procmon.R
# 
# R Utilities To Query Procmon
#
# Author: e14600


################################################################################
# External Libraries
#
library(RCurl)
source("H:/user/R/RMG/Utilities/RLogger.R")


################################################################################
# File Namespace
#
Procmon = new.env(hash=TRUE)


################################################################################
Procmon$getStatus <- function( procName, asOfDate, exact=FALSE )
{
    library(RCurl)
    date = format( as.Date(asOfDate), "%Y%m%d" )
    
    host = "procmonster"
    
    urlName = gsub("/", "%2F", procName)
    url = paste("http://", host, "/cgi-bin/procmon/sf.cgi?date=", date, 
            "&pattern=", urlName, sep="")
    
    pageData = getURL(url)
    
    statusData = NULL
    groups = strsplit(pageData, "<HR><h3>", fixed=TRUE)[[1]][-1]
    for( section in groups )
    {
        status = strsplit(section, "<A NAME=\"", fixed=TRUE)[[1]][2]
        status = strsplit(status, "\">", fixed=TRUE)[[1]][1]
        
        jobSection = strsplit(section, "<PRE>\n")[[1]][2]
        jobSection = strsplit(jobSection, "\n")[[1]]
        jobSection = strsplit(jobSection, "proc=")
        jobSection = unlist( lapply(jobSection, function(x){ return(x[2]) } ) )
        jobSection = strsplit(jobSection, "\">", fixed=TRUE)
        jobSection = unlist( lapply(jobSection, function(x){ return(x[1]) } ) )
        
        sectionInfo = data.frame(job_name = na.omit(jobSection), status = status)
        
        statusData = rbind(statusData, sectionInfo)
    }
    
    if( exact )
    {
        matchingRows = which( tolower(statusData$job_name) == tolower(procName) )
        statusData = statusData[matchingRows,"status"]
    }
    
    return(statusData)
}
