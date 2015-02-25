#Query prismp database to discover if a book is owned by Portfolio Management 
#or not
# 
# Author: Sean McWilliams
###############################################################################

library(RODBC)
source("H:/user/R/RMG/Utilities/RLogger.R")

################################################################################

BookOwnership = new.env(hash = TRUE)

################################################################################
#returns true if book is in owned by PM; otherwise, reutrns false
BookOwnership$isPM <- function(bookName){
	
	rLog("Querying prismp database...")		
	dsnPath = "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/PRISMU.dsn"
	conString = paste(dsnPath, ";UID=ATM;PWD=ATM;", sep = "")
	chan = odbcDriverConnect(conString)
	
	query = paste("select EXECUTIVE_OWNER from prism.book_vw where book_name='", bookName, "'", sep="")
	
	firstTry = sqlQuery(chan, query)
	Sys.sleep(1)
	secondTry = sqlQuery(chan, query)
	
	while( !all(dim(secondTry)==dim(firstTry)) )
	{
		rLog("PRISMP in the middle of an update, trying again...")
		firstTry = sqlQuery(chan, query)
		Sys.sleep(1)
		secondTry = sqlQuery(chan, query)
	}
	
	odbcClose(chan)
	if(secondTry$EXECUTIVE_OWNER == "Trading"){
		return(FALSE)	
	}
	else if(secondTry$EXECUTIVE_OWNER[1] == "PM"){
		return(TRUE)
	}
	
}


