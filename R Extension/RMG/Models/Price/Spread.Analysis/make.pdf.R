# Take the tex template, add the current date and portfolio,
# and run pdflatex on it to produce the pdf report.
#
# Written by Adrian Dragulescu on 7-Jan-2005


make.pdf <- function(options){
  
  if (options$polynomial==1){options$degree="linear "}
  if (options$polynomial==2){options$degree="quadratic "}
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
  
  template <- paste(options$source.dir,"template.spreadAnalysis.tex", sep="")
  aux <- readLines(template)
  bux <- gsub("SSSSS", title, aux)
  cux <- gsub("MMMMM", as.character(options$degree),bux)
  dux <- gsub("RRRRR", ifelse(options$regression=="rlm", "robust ", " "),cux)
  eux <- gsub("OOOOO", months.out, dux)
  if (length(options$deal.start)==0){
     ind <- grep("bootstrap_value", eux)
     eux <- eux[-ind]       # if you don't have a deal, remove the table
  }
  #-------------------------------------------Daily regression------
  if (length(options$focus.months)!=0){
    rng     <- grep("DDDREG", eux); ind.Sec <- rng[1];
    eux[ind.Sec] <-  gsub("%DDDREG", "", eux[ind.Sec])
    rng <- rng[-1]
    block <- eux[grep("DDDREG", eux)]; block.Size <- length(block)
    for (m in options$focus.months){
      block.Sub <- gsub("MMM", month.abb[m], block)
      block.Sub <- gsub("%DDDREG", "", block.Sub)
      ind <- (ind.Sec+1):length(eux)
      fux <- eux;  fux[ind + block.Size] <- eux[ind]
      fux[(ind.Sec+1):(ind.Sec + block.Size)] <- block.Sub
      eux <- fux; ind.Sec <- ind.Sec + block.Size
    }
  }
  ind <- grep("DDDREG", eux)
  eux <- eux[-ind]
  
  #------------------------------Daily regression other variables------
  if (options$other.variables==1){
    if (length(options$focus.months)!=0){
      rng     <- grep("DDDOOO", eux); ind.Sec <- rng[1];
      eux[ind.Sec] <-  gsub("%DDDOOO", "", eux[ind.Sec])
      rng <- rng[-1]
      block <- eux[grep("DDDOOO", eux)]; block.Size <- length(block)
      for (m in options$focus.months){
        block.Sub <- gsub("MMM", month.abb[m], block)
        block.Sub <- gsub("%DDDOOO", "", block.Sub)
        ind <- (ind.Sec+1):length(eux)
        fux <- eux;  fux[ind + block.Size] <- eux[ind]
        fux[(ind.Sec+1):(ind.Sec + block.Size)] <- block.Sub
        eux <- fux; ind.Sec <- ind.Sec + block.Size
      }
    }
  }
  ind <- grep("DDDOOO", eux)
  eux <- eux[-ind]  
  
  tex.filename <- paste("spreadAnalysis_", Sys.Date(), sep="")
  writeLines(eux, paste(tex.filename, ".tex", sep=""))

  system(paste("c:/TexLive/bin/win32/pdflatex", tex.filename))
  file.remove(paste(tex.filename, ".aux", sep=""))
  file.remove(paste(tex.filename, ".log", sep=""))
  file.remove(paste(tex.filename, ".toc", sep=""))
  
}
