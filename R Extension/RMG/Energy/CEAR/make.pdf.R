# Take the tex template, add the current date and portfolio,
# and run pdflatex on it to produce the pdf report.
#
# Written by Adrian Dragulescu on 7-Jan-2005


make.pdf <- function(save, options){

  setwd(save$dir)

  template <- "H:/user/R/RMG/Energy/CEAR/template.CEAR.tex"
  aux <- readLines(template)
  bux <- gsub("RRRRR",options$run.name, aux)
  tex.filename <- paste("cear", Sys.info()["nodename"],
                        gsub(pattern=" ",replacement="",options$run.name),
                        as.character(ceiling(rnorm(1)*100)),sep="-")
  tex.filename <- paste(save$dir,tex.filename,sep="")
  writeLines(bux, paste(tex.filename, ".tex", sep=""))

  gsub(" ", "\\ ", tex.filename)
  system(paste("c:/TexLive/bin/win32/pdflatex", tex.filename))
  file.remove(paste(tex.filename, ".aux", sep=""))
  file.remove(paste(tex.filename, ".log", sep=""))
  file.remove(paste(tex.filename, ".toc", sep=""))
  file.remove(paste(tex.filename, ".tex", sep=""))
  return(paste(tex.filename,".pdf",sep=""))

}
