################################################################################
#This file should be run to generate the finished PDF version of the 
#Daily Weather Position Report
#RS
#Author: Sean McWilliams 
#
#June 2, 2006
#
################################################################################


library(utils)


################################################################################
WeatherPositionReport <- new.env(hash=TRUE)


################################################################################
# Initialize SecDb for use on Procmon
DB = "!Bal ProdSource;!BAL DEV!home!E14600;!BAL Home"
Sys.setenv(SECDB_SOURCE_DATABASE=DB)
Sys.setenv(SECDB_DATABASE="Prod")
options(stringsAsFactors=FALSE)


################################################################################
#main function that runs sweave on the R/Latex script and outputs .tex file

WeatherPositionReport$main <- function(asOfDate){
	filePath <- "S:/All/Risk/Reports/TradingControlReports/DailyWeatherPositionReports/WPR"
    outFile <- paste(filePath, as.character(asOfDate), ".tex", sep = "") 
	graphicFile <- paste(filePath, "MapGraphic.tex", sep = "")
    Sweave("H:/user/R/RMG/Projects/Weather_Position_Report/WPRLatexScript.Rnw", output = outFile)
	Sweave("H:/user/R/RMG/Projects/Weather_Position_Report/WPRMapGraphic.rnw", output = graphicFile)
    graphicOut <- paste("//ceg/cershare/Risk/Software/Latex/fptex/TeXLive/bin/win32/pdflatex", graphicFile, "-output-directory=S:/All/Risk/Reports/TradingControlReports/DailyWeatherPositionReports/")
	pdfOut <- paste("//ceg/cershare/Risk/Software/Latex/fptex/TeXLive/bin/win32/pdflatex", outFile, "-output-directory=S:/All/Risk/Reports/TradingControlReports/DailyWeatherPositionReports/")
    system(graphicOut)
	system(pdfOut)
	reportPDF <- paste(filePath, as.character(asOfDate), ".pdf", sep = "")
	graphicPDF <- paste(filePath, "MapGraphic-mapGraphic.pdf", sep = "")
	setwd("S:/Risk/Software/PDFTk/pdftk-1.41.exe")
	finalFile <- paste(filePath, "Final", as.character(asOfDate), ".pdf", sep = "")
	finalOut <- paste("pdftk", reportPDF, graphicPDF, "output", finalFile)
	system(finalOut)
}

################################################################################
#call main function

WeatherPositionReport$main(Sys.Date() - 1)

################################################################################
