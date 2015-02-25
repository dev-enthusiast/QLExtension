# Take the tex template, add the current date and portfolio,
# and run pdflatex on it to produce the pdf report.
#
# Written by Adrian Dragulescu on 7-Jan-2005


make.pdf <- function(save, options,MarketLoc,Bucket){
  
  setwd(save$dir$plots)
  template <- paste(save$dir$report,"template.Regression.Analysis.tex", sep="")
 
  aux <- readLines(template)
   
  bux <- gsub("PWXCT",MarketLoc,aux)
  cux <- gsub("5x16", Bucket,bux)

  tex.filename <- paste(save$dir$report, MarketLoc,"_RegressionAnalysis", sep="")
  writeLines(cux, paste(tex.filename, ".tex", sep=""))

  gsub(" ", "\\ ", tex.filename)
  system(paste("//ceg/cershare/Risk/Software/Latex/fptex/TeXLive/bin/win32/pdflatex -output-directory=",save$dir$report," ",tex.filename,sep=""))
  file.remove(paste(tex.filename, ".aux", sep=""))
  file.remove(paste(tex.filename, ".log", sep=""))
  file.remove(paste(tex.filename, ".toc", sep=""))

}
