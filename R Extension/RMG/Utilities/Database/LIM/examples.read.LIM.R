# Examples how to call LIM from R.
#
#
#

dyn.load("H:/user/R/RMG/Utilities/Database/LIM/Source/RMGxmimR.dll")
source("H:/user/R/RMG/Utilities/Database/LIM/read.LIM.R")

#-----------------------------Get daily data-----------
query <- "SHOW 1: Close of NG WHEN date is after 2005"
res <- read.LIM(query)

#------------------------------Get hourly data----------
query <- "SHOW 1: LmpVal of front WESTERNHUB.HUB.PJMDA
               2: LmpVal of front A418.138KV.TR7212.PJMDA
               3: LmpVal of front A418.138KV.TR7312.PJMDA
               4: LmpVal of front VERONA.22KV.TX1.PJMDA
               5: LmpVal of front WAGNER.13KV.GEN04.PJMDA
               6: LmpVal of front WHEMPFIE.69KV.BUSS.PJMDA
               7: LmpVal of front WESCOSVI.69KV.691.PJMDA
               8: LmpVal of front WESTDUQ.138KV.LOAD.PJMDA
          WHEN Date is from 01/01/2003 to 05/01/2006"
options <- NULL
options$units <- "hour"
source("H:/user/R/RMG/Utilities/Database/LIM/read.LIM.R")
res <- read.LIM(query, options)

#-----------------------------Get temperature data-----------
dyn.load("H:/user/R/RMG/Utilities/Database/LIM/Source/RMGxmimR.dll")
source("H:/user/R/RMG/Utilities/Database/LIM/read.LIM.R")
query <- "SHOW  1: HighTemp of front BOSTON.LOGAN.MA
                2: LowTemp  of front BOSTON.LOGAN.MA
                3: HighTemp of front BALTIMORE.WASHINGTON.MD
          WHEN date is after 1969"
source("H:/user/R/RMG/Utilities/Database/LIM/read.LIM.R")
options=NULL
options$colnames <- c("Tmax.BOS", "Tmin.BOS", "Tmax.BWI")
res <- read.LIM(query, options)
plot(res, type="l", col="blue")

#-----------------------------Get daily data-----------
query <- "SHOW 1: Daily Index of front IGBBH21 WHEN date is after 2003"
res <- read.LIM(query)
plot(res, type="l", col="blue")




dyn.unload("H:/user/R/RMG/Utilities/Database/LIM/Source/RMGxmimR.dll")




