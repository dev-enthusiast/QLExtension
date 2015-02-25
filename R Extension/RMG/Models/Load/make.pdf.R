# Take the tex template, add the current date and portfolio,
# and run pdflatex on it to produce the pdf report.
#
# Written by Adrian Dragulescu on 7-Jan-2005


make.pdf <- function(save, options){

  template <- paste(options$path.L, "template.analysisDeal.tex", sep="")
  aux <- readLines(template)
  bux <- gsub("DDDDD",options$loadName,aux)
  cux <- gsub("AAAAA",options$airport.name,bux)
  if (options$vlr==0){
    ind <- grep("VLRVLR", cux)
    cux <- cux[-ind]
  }
  
  tex.filename <- paste(save$dir$main, "analysisDeal", sep="")
  writeLines(cux, paste(tex.filename, ".tex", sep=""))

  gsub(" ", "\\ ", tex.filename)
  system(paste("c:/TexLive/bin/win32/pdflatex", tex.filename))
  file.remove(paste(tex.filename, ".aux", sep=""))
  file.remove(paste(tex.filename, ".log", sep=""))
  file.remove(paste(tex.filename, ".toc", sep=""))
  
}
