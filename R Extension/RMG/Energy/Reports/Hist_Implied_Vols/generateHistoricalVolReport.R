################################################################################
# This file generates a report showing the ratio of implied volatility to
# the historical volatility over the last X days.  It will generate a PDF
# displaying two tables, one of the largest ratios and one of the smallest.
# 
generateHistoricalVolReport.main <- function()
{
  # Load the required overnight parameters
  rm(list=ls())
  source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
  asOfDate   <- overnightUtils.getAsOfDate()
  options = overnightUtils.loadOvernightParameters(asOfDate, FALSE)
  
  print("Getting Table for Largest Ratios...")
  largestRatioData = paste(options$save$reports.dir, asOfDate,
    .Platform$file.sep, "LargestImpVsHistTable.csv", sep="")
  largestRatioTable = read.csv( largestRatioData )
  largestRatioLatex = generateLatexTable(largestRatioTable, 
    "Largest 20 Ratios of Implied Volatility to Historical Volatility")
  
  print("Getting Table for Smallest Ratios...")
  smallestRatioData = paste(options$save$reports.dir, asOfDate,
    .Platform$file.sep, "SmallestImpVsHistTable.csv", sep="")
  smallestRatioTable = read.csv( smallestRatioData )
  smallestRatioLatex = generateLatexTable(smallestRatioTable, 
    "Smallest 20 Ratios of Implied Volatility to Historical Volatility")
    
  print("Getting Table for Largest Vol Gainers...")
  largestGainersData = paste(options$save$reports.dir, asOfDate,
    .Platform$file.sep, "VolatilityLargestGainersTable.csv", sep="")
  largestGainersTable = read.csv( largestGainersData )
  largestGainersLatex = generateLatexTable(largestGainersTable, 
    "Largest Percent Gain In Implied Volatility")
    
  print("Getting Table for Largest Vol Losers...")
  largestLosersData = paste(options$save$reports.dir, asOfDate,
    .Platform$file.sep, "VolatilityLargestLosersTable.csv", sep="")
  largestLosersTable = read.csv( largestLosersData )
  largestLosersLatex = generateLatexTable(largestLosersTable, 
    "Largest Percent Loss In Implied Volatility")
  
  # Take the two latex output tables and write them to a PDF file
  reportPath = generateVolPDF(options, largestRatioLatex, smallestRatioLatex,
    largestGainersLatex, largestLosersLatex, asOfDate)
  
  print("Done!")
  
  overnightUtils.sendCompletionEmail("Volatility Report", reportPath)
}


################################################################################
# Generate a latex table from the data frame passed in
# 
generateLatexTable <- function(tableData, caption)
{
  print("Converting to a data frame...")
  if (!is.data.frame(tableData)){tableData <- data.frame(tableData)}

  size <- "scriptsize"
  if (ncol(tableData) >  6){size <- "scriptsize"} 
  if (ncol(tableData) >= 18){size <- "tiny"}
  
  print("Generating xtable...")
  xtableData = print.xtable( xtable(tableData, caption=caption, digits=2) , 
    include.rownames=FALSE, size=size, tabular.environment="longtable",
    floating=FALSE )
 
  return(xtableData)
}


################################################################################
# Read in the vol report latex template, update it with our data
#  
generateVolPDF <- function(options, largestRatioLatex, smallestRatioLatex, 
  largestGainerLatex, largestLoserLatex, asOfDate)
{
  template <- paste( "H:/user/R/RMG/Energy/Reports/Hist_Implied_Vols/",
    "template.Vol.report.tex", sep="")
  
  templateText <- readLines(template)
  
  # Generate the header
  templateText <- gsub("_asofdate", as.character(asOfDate), templateText)
  templateText <- gsub("_rundatetime", as.character(Sys.time()), templateText)
  
  # Add in the latex tables passed in for volitility
  ind <- grep("\\input{LargestVolatilityTable}",extended=FALSE, templateText)
  templateText  <- append(templateText, largestRatioLatex, after=(ind-2))
  
  ind <- grep("\\input{SmallestVolatilityTable}",extended=FALSE, templateText)
  templateText  <- append(templateText, smallestRatioLatex, after=(ind-2))
  
  ind <- grep("\\input{LargestGainerTable}",extended=FALSE, templateText)
  templateText  <- append(templateText, largestGainerLatex, after=(ind-2))
  
  ind <- grep("\\input{LargestLoserTable}",extended=FALSE, templateText)
  templateText  <- append(templateText, largestLoserLatex, after=(ind-2))
  
  # Write out the file to the reports directory
  tex.filename = paste(options$save$reports.dir, asOfDate, .Platform$file.sep, 
    "Vol.report", sep="")
  writeLines(templateText, paste(tex.filename,".tex",sep=""))

  # run pdflatex to convert the output to a pdf file
  outputDir = paste(options$save$reports.dir, asOfDate, 
    .Platform$file.sep, sep="")
  convertToPDF( paste(tex.filename,".tex",sep=""), outputDir )
  
  print( paste("Done! \nYour report was saved in folder:", outputDir) )
  
  return(paste(tex.filename,".pdf",sep=""))
}


################################################################################
# Convert the latex file passed in to a pdf and place it in outputDir 
#
convertToPDF <- function( filename, outputDir )
{
  print(paste("Changing Directory To:", outputDir, "for pdflatex..."))
  setwd(outputDir)
  
  command = 
    "//ceg/cershare/Risk/Software/Latex/fptex/TeXLive/bin/win32/pdflatex"
  
  cmd <- paste(command, filename)
  system(cmd, invisible=FALSE)
  
  # remove any log files
  basePath = substr(filename,1,nchar(filename)-4)
  file.remove(paste(basePath, ".aux", sep=""))
  file.remove(paste(basePath, ".log", sep=""))
  file.remove(paste(basePath, ".toc", sep=""))
}


################################################################################
# Start the program 
# 
res = try(generateHistoricalVolReport.main())
if (class(res)=="try-error"){
  cat("Failed Overnight Historical Volatility Report Job.\n")
}
