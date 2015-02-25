# Take the tex template, add the current date and portfolio,
# and run pdflatex on it to produce the pdf report.
#
# Written by Adrian Dragulescu on 7-Jan-2005


make.pdf <- function(save, options){

  template <- paste(options$path.G, "template.analysisGen.tex", sep="")
  aux <- readLines(template)
  bux <- gsub("UUUUU",options$unit.name,aux)
  bux <- gsub("AAAAA",options$asOfDate, bux)
  
  tex.filename <- paste(save$dir$main, "analysisGen", sep="")
  writeLines(bux, paste(tex.filename, ".tex", sep=""))

  setwd(save$dir$main)
  system(paste("c:/TexLive/bin/win32/pdflatex", tex.filename))
  file.remove(paste(tex.filename, ".aux", sep=""))
  file.remove(paste(tex.filename, ".log", sep=""))
  file.remove(paste(tex.filename, ".toc", sep=""))
  
}
