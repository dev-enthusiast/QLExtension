# Take the tex template, add the current date and portfolio,
# and run pdflatex on it to produce the pdf report.
#
# Written by Adrian Dragulescu on 7-Jan-2005


make.pdf <- function(options){

  months.out=""
  if (length(options$omit.months)!=0){
     months.out <- options$omit.months[1]
     if (length(options$omit.months)==1){
       months.out <- paste("The month", months.out, "has been")
     }
     if (length(options$omit.months)>1){
       for (m in 2:length(options$omit.months)){
         months.out <- paste(months.out, ", ", options$omit.months[m], sep="")
       }
       months.out <- paste("The months", months.out, "have been") 
     }
     months.out <- paste(months.out, "left out of the data analysis.")
  } 
  
  title <- paste(options$shortNames[2],"$-$", options$shortNames[1])
  
  template <- "H:/user/R/RMG/Models/Gas/Transport/template.transportAnalysis.tex"
  aux <- readLines(template)
  bux <- gsub("SSSSS", title, aux)
  eux <- gsub("OOOOO", months.out, bux)
  if (length(options$deal.start)==0){
     ind <- grep("bootstrap_value", eux)
     eux <- eux[-ind]       # if you don't have a deal, remove the table
  }
  
  tex.filename <- paste("transportAnalysis_", Sys.Date(), sep="")
  writeLines(eux, paste(tex.filename, ".tex", sep=""))

  system(paste("c:/TexLive/bin/win32/pdflatex", tex.filename))
  file.remove(paste(tex.filename, ".aux", sep=""))
  file.remove(paste(tex.filename, ".log", sep=""))
  file.remove(paste(tex.filename, ".toc", sep=""))
  
}
