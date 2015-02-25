#Ad hoc data aggregation for Adi#
#
#Sean McWilliams

#reads in the first line of a range of excel files and reorganizes the data

#library(SecDb)
library(reshape)

source("H:/user/R/RMG/Utilities/load.R")



getPriceData <- function()
{
	dateRange = as.character(format(seq( as.Date("2007-12-28"), as.Date("2008-07-16"), by="1 day" ), "%y%m%d%"))
	
	allData = NULL
	for( date in dateRange )
	{
		inputFile = paste( "S:/Risk/Reports/UK VaR SAS/JADE/R20", 
				date, "_RISKUKMNEALEPORTFOLIO.csv", sep="" )
		#rLog( "Loading file:", inputFile )
		
		
		if( !file.exists(inputFile) ) next
		
		data <- read.csv(inputFile)
		
		if(is.null(data$X_2007))
		{
			data <- data[1,c("X_2008","X_2009","X_2010","X_2011","X_2012","X_2013","Total")]
			data$pricing.date <- paste("20",date,sep="")
			data$portfolio <- "RISKUKMNEALEPORTFOLIO"
			data$X_2007 <- 0
			data <- data[c("pricing.date","portfolio","X_2007", "X_2008","X_2009","X_2010","X_2011","X_2012","X_2013","Total")]
		}else 
		{
			data <- data[1,c("X_2007","X_2008","X_2009","X_2010","X_2011","X_2012","X_2013","Total")]
			data$pricing.date <- paste("20",date,sep="")
			data$portfolio <- "RISKUKMNEALEPORTFOLIO"
			data <- data[c("pricing.date","portfolio","X_2007", "X_2008","X_2009","X_2010","X_2011","X_2012","X_2013","Total")]
		}
		
		allData = rbind( allData, data )
		
		
	}
	names(allData) <- c("pricing.date", "portfolio", "Y2007", "Y2008", "Y2009","Y2010","Y2011","Y2012","Y2013","Total")
	write.csv(allData, "C:/Neale Overall Portfolio Historical VaR Jan - Jul 2008.csv")
	
    
}
