# Take the tex template, add the current date and portfolio,
# and run pdflatex on it to produce the pdf report.
#
# Written by Adrian Dragulescu on 7-Jan-2005


make.pdf <- function(run, options){

  template <- paste(options$source.dir,"template.VaR.report.tex",sep="")
  TT <- readLines(template)
  TT <- gsub("_asOfDate", as.character(options$asOfDate), TT)
  TT <- gsub("_rundatetime", as.character(Sys.time()), TT)
  TT <- gsub("_runname", run$run.name, TT)
  if (all(is.na(run$PBT$portfolio))){TT <- gsub("_portfolios", "", TT)
  } else {
    aux <- paste(run$PBT$portfolio[which(!is.na(run$PBT$portfolio))],sep="",collapse=".")
    TT <- gsub("_portfolios", aux, TT)}
  if (all(is.na(run$PBT$book))){TT <- gsub("_books", "", TT)
  } else {
    aux <- paste(run$PBT$book[which(!is.na(run$PBT$book))],sep="",collapse=".")
    TT <- gsub("_books", aux, TT)}
  if (all(is.na(run$PBT$strategy))){TT <- gsub("_strategies", "", TT)
  } else {
    aux <- paste(run$PBT$strategy[which(!is.na(run$PBT$strategy))],sep="",collapse=".")
    TT <- gsub("_strategies", aux, TT)}
  if (all(is.na(run$PBT$trade))){TT <- gsub("_trades", "", TT)
  } else {
    aux <- paste(run$PBT$trade[which(!is.na(run$PBT$trade))],sep="",collapse=".")
    TT <- gsub("_trades", aux, TT)}
  TT <- gsub("_startdate", format(options$calc$hdays[1], "%d%b%y"), TT)
  TT <- gsub("_enddate", format(options$calc$hdays[length(options$calc$hdays)],
                                 "%d%b%y"), TT)
  TT <- gsub("_LivePositions", ifelse(options$run$useLivePositions,"Yes","No"), TT) 
  TT <- gsub("_savereportsdir", options$save$reports.dir.run, TT)
  
  if ("hub.basis" %in% run$classification){
    ind <- grep("insert.basis_var_by_region_season.tex.here", TT)
    TT[ind] <- paste("\\input{basis_var_by_region_season}", sep="")    
  }
  if ("gasseason" %in% run$classification){
    ind <- grep("insert.commodityname.type.gasseason.tex.here", TT)
    TT[ind] <- paste("\\input{var_by_commodityname_type_gasseason}", sep="")    
  }
  if ("in.20072008" %in% run$classification){
    ind <- grep("insert.20072008.type.tex.here", TT)
    TT[ind] <- paste("\\input{var_by_20072008_type}", sep="")    
  }

  tex.filename <- "VaR.report"
  writeLines(TT, paste(tex.filename, ".tex", sep=""))

  # run latex several times to get the formatting right...
  for (n in 1:3){
    cmd <- paste("S:/Risk/Software/Latex/fptex/TeXLive/bin/win32/pdflatex", tex.filename)
    system(cmd, invisible=TRUE)
  }
  file.remove(paste(tex.filename, ".aux", sep=""))
  file.remove(paste(tex.filename, ".log", sep=""))
  file.remove(paste(tex.filename, ".toc", sep=""))
  if (!options$run$is.overnight){
    cmd <- paste("S:/Risk/Software/Latex/fptex/TeXLive/bin/win32/pdfopen",
                 "--file VaR.report.pdf")
    system(cmd, invisible=TRUE)
    msg <- paste("Done! \nYour report was saved in folder: ",
              options$save$reports.dir, "/", run$run.name, sep="")
    tkmessageBox(message=msg)  
  }
}


##   ind <- grep("cstart", aux)
##   if (is.na(options$startDate)){
##     aux[ind] <- gsub("cstart", "today", aux[ind])
##   } else {
##     aux[ind] <- gsub("cstart", as.character(options$startDate), aux[ind])
##   }
##   if (is.na(options$endDate)){
##     aux[ind] <- gsub("cend", "end", aux[ind])
##   } else {
##     aux[ind] <- gsub("cend", as.character(options$endDate), aux[ind])
##   }
  
##   if (options$have.vegas){
##     ind <- grep("insert.vegas.table.here", aux)
##     aux[ind] <- paste("\\input{Rtables/top30_vegas_",options$AsOfDate,"}", sep="")    
##   }
