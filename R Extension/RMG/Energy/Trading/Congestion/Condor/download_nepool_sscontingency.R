# Download the nepool single source contingency data
# and archive it
# It gets written to tsdb with job nepool_tsdb_sscontingency
#
#
# Written by Adrian Dragulescu on 2011-01-06




##################################################################
##################################################################

options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
require(plyr)
require(reshape)
require(SecDb)
require(Tsdb)
library(httr)
library(XML)
library(jsonlite)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.webservices.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/lib.NEPOOL.sscontingency.R")

returnCode <- 1   # 0 means success

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/download_nepool_sscontingency.R")
rLog("Start at ", as.character(Sys.time()))
DIR <<- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/SingleSourceContingency/"

day <- Sys.Date()-2  # data should be there

ssc <- .ws_get_singlesrccontingencylimits( as.Date(day) )
.write_ssc_RData( ssc )

X <- data.frame(Local.Time = ssc$BeginDate,
                Lowest.Limit = ssc$LowestLimitMw,
                Phase.II.RT.Flow = ssc$RtFlowMw)
returnCode <- .write_ssc_tsdb( X )


rLog("Done at ", as.character(Sys.time()))


if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}


