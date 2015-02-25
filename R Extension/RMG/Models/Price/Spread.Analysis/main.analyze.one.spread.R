# Called by demo.analyze.one.spread.R
# Run file: H:/Matlab/StructuredDeals/TOOLS/save_data_for_LMP_spreadAnalysis.m
#
# Written by Adrian Dragulescu on 7-Dec-2004


main.analyze.one.spread <- function(options,save){

  source("C:/R/Work/Energy/Models/Physical/Price/Spread.Analysis/set.parms.model.R")
  res  <- set.parms.model(save,options)
  save <- res[[1]];  options <- res[[2]]
  
  MM <- tsdbRead(options,save)  
  R   <- nrow(MM);  C <- ncol(MM)
  options$between   <- paste(colnames(MM)[6], " (", options$shortNames[2],
       ") - ",colnames(MM)[5], " (", options$shortNames[1], ")" , sep="") 
  options$main      <- paste(options$shortNames[1],"_to_",
                             options$shortNames[2],sep="")
  options$dateRange <- paste(format(options$use.data.from, "%b%y"), "to",
                           format(options$hdata.endDate, "%b%y"))
  colnames(MM)[5:6] <- options$shortNames
  options$spread <- paste(colnames(MM)[6],"-",colnames(MM)[5])
  options$period <- paste(MM[1,"year"], "to", MM[R, "year"])
  MM  <- cbind(MM, spread=MM[,6]-MM[,5])
  
  if (length(options$omit.months)!=0){
    ind    <- NULL
    months <- format(as.Date(paste(MM$year, MM$month, "1", sep="-")), "%Y-%m")
    for (m in 1:length(options$omit.months)){
      ind <- c(ind, which(months==options$omit.months[m]))}
    MM <- MM[-ind,]
  }  
  if (options$use.data.from!=options$hdata.startDate){
    days <- as.Date(paste(MM$year, MM$month, MM$day, sep="-"))
    ind  <- which(days==options$use.data.from)
    MM   <- MM[-(1:(ind[1]-1)),]
  }
    
  cat("Start analysis ...\n"); flush.console()
  OutData <- analyze.one.spread(MM, save, options)
  cat("Analysis finished. \n"); flush.console()

  cat("Start creation of report... "); flush.console()
  source(paste(options$source.dir, "make.pdf.R", sep=""))
  make.pdf(options)
  cat("Done!  \n Please open your pdf report.\n"); flush.console()
}


#   from <- "c:/R/Work/Energy/Models/Price/SpreadModel/spreadAnalysis.tex"
#   to <- paste(save$dir, "spreadAnalysis.tex", sep="")
#   if (file.exists(to)){file.remove(to)} 
#   file.copy(from,to)
#   system("C:/TeXLive/bin/win32/pdflatex spreadAnalysis")
