
makePDF.PriceAnalysisReport <- function(options){
  source("H:/user/R/RMG/Utilities/sendemail.R")
  
  if (options$report.type == "Unaudited") {
      report.name <- paste("priceAnalysisReport_unaudited_", as.character(format(Sys.time(), "%Y-%m-%d")), sep="") 
      rptdir <- paste(as.character(format(Sys.time(), "%Y-%m-%d")), "Unaudited", sep="")
  } else {
      report.name <- paste("priceAnalysisReport_", as.character(options$pricing.dt.end), sep="")
      rptdir <- as.character(options$pricing.dt.end)
  }
  template <- "H:/user/R/RMG/Energy/Trading/PriceAnalysis/templatePriceAnalysisReport.tex"
  aux <- readLines(template)
  aux <- gsub("_ReportType",options$report.type,aux) 
  bux <- gsub("asOfDate",rptdir,aux) 
  texFilename <- paste(options$dirOut, report.name, sep = "") 
  writeLines(bux, paste(texFilename, ".tex", sep = ""))
  setwd(options$dirOut)

  system(paste("S:/Risk/Software/Latex/fptex/TeXLive/bin/win32/pdflatex", texFilename))  
  file.remove(paste(texFilename, ".aux", sep=""))
  file.remove(paste(texFilename, ".log", sep=""))
  
  fullfilename <- paste("\\\\ceg\\cershare\\All\\Risk\\Reports\\PriceChangeReport\\DailyReports\\", report.name, ".pdf", sep = "")
  file.copy(paste(texFilename, ".pdf", sep = ""),
            paste("S:/All/Risk/Reports/PriceChangeReport/DailyReports/", report.name, 
                  ".pdf", sep = ""))
  
  from <- "michael.zhang@constellation.com"  
  to <- "CPSRISK@constellation.com" 
  cc <- "michael.zhang@constellation.com"        
  #to <- "sailesh.buddhavarapu@constellation.com, kiran.d.dhaliwal@constellation.com"
  #cc <- "michael.zhang@constellation.com, adrian.dragulescu@constellation.com"
  subject <- report.name
  body <- fullfilename
  sendEmail(from, to, subject, body, cc, attachments=NA, links=NA)
#  file.remove(paste(tex.filename, ".toc", sep=""))
  
}
