# Take the tex template, add the current date and portfolio,
# and run pdflatex on it to produce the pdf report.
#
# Written by Adrian Dragulescu on 7-Jan-2005


make.pdf <- function(save, options){

  setwd(save$dir.main)
  template <- paste(save$source.dir, "template.BasisAnalysis.tex", sep="")
  aux <- readLines(template)
  bux <- gsub("RRRRR",options$run.name, aux)
  if (length(options$book)==0){options$book <- "all"}
  cux <- gsub("BBBBB", paste(options$book,collapse=", "), bux)
  dux <- gsub("DDDDD", as.character(options$asOfDate), cux)
  eux <- gsub("OOOOO", paste(options$fdata$months, collapse=", "), dux)

  tex.filename <- paste(save$dir.main, "BasisAnalysis", sep="")
  writeLines(eux, paste(tex.filename, ".tex", sep=""))

  gsub(" ", "\\ ", tex.filename)
  system(paste("c:/TexLive/bin/win32/pdflatex", tex.filename))
  file.remove(paste(tex.filename, ".aux", sep=""))
  file.remove(paste(tex.filename, ".log", sep=""))
  file.remove(paste(tex.filename, ".toc", sep=""))

}
