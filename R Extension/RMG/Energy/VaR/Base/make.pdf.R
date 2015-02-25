# Take the tex template, add the current date and portfolio,
# and run pdflatex on it to produce the pdf report.
#
# Written by Adrian Dragulescu on 7-Jan-2005


make.pdf <- function(run, options){

  template <- paste(options$source.dir,"Base/template.VaR.report.tex",sep="")
  TT <- readLines(template)
  #-------------------------------------- Do header -----------------------------
  TT <- gsub("_asOfDate", as.character(options$asOfDate), TT)
  TT <- gsub("_rundatetime", as.character(Sys.time()), TT)
  TT <- gsub("_runname", run$run.name, TT)
  if (all(is.na(run$PBT$portfolio))){TT <- gsub("_portfolios", "", TT)
  } else {
    aux <- paste(run$PBT$portfolio[which(!is.na(run$PBT$portfolio))],sep="",collapse=", ")
    TT <- gsub("_portfolios", aux, TT)}
  if (all(is.na(run$PBT$book))){TT <- gsub("_books", "", TT)
  } else {
      bookList = run$PBT$book[which(!is.na(run$PBT$book))]
      if( length(bookList) > 10 )
      { 
        bookList = bookList[1:10]
        bookList[11] = "Others not shown..."
      }
    aux <- paste(bookList,sep="",collapse=", ")
    TT <- gsub("_books", aux, TT)}
  if (all(is.na(run$PBT$strategy))){TT <- gsub("_strategies", "", TT)
  } else {
    aux <- paste(run$PBT$strategy[which(!is.na(run$PBT$strategy))],sep="",collapse=", ")
    TT <- gsub("_strategies", aux, TT)}
  if (all(is.na(run$PBT$trade))){TT <- gsub("_trades", "", TT)
  } else {
    aux <- paste(run$PBT$trade[which(!is.na(run$PBT$trade))],sep="",collapse=", ")
    TT <- gsub("_trades", aux, TT)}
  TT <- gsub("_startdate", format(options$calc$hdays[1], "%d%b%y"), TT)
  TT <- gsub("_enddate", format(options$calc$hdays[length(options$calc$hdays)],
                                 "%d%b%y"), TT)
  TT <- gsub("_LivePositions", ifelse(options$run$useLivePositions,"Yes","No"), TT) 
  TT <- gsub("_savereportsdir", options$save$reports.dir.run, TT)
  
  #------------------------------------ Add VaR tables ----------------------------
  tex.files <- list.files(options$save$reports.dir.run, pattern="\\.tex")
  var.files <- tex.files[grep("var_by", tex.files)]
  ind.cf <- grep("\\|", run$classification)
  for (i in 1:length(ind.cf)){
    cf <- gsub("\\|", "_", run$classification[ind.cf[i]])
    filename <- paste("\\input{var_by_", cf, "}", sep="")
    ind <- grep("%insert.var.tables.above", TT)
    TT  <- append(TT, filename, after=(ind-1))
  }
  #------------------------------------ Add CVaR tables ----------------------------
  cvar.files <- tex.files[grep("cvar_by", tex.files)]
  if (length(cvar.files)>0){
    txt <- "\\section{Component VaR tables}"
    ind <- grep("%insert.cvar.tables.above", TT)
    TT  <- append(TT, txt, after=(ind-1))
    for (i in 1:length(cvar.files)){
      aux <- gsub("\\.tex","", cvar.files[i])
      filename <- paste("\\input{", aux, "}", sep="")
      ind <- grep("%insert.cvar.tables.above", TT)
      TT  <- append(TT, filename, after=(ind-1))
    }
  }
  #------------------------------------ Add Incremental VaR tables ----------------
  ivar.files <- tex.files[grep("ivar_by", tex.files)]
  if (length(ivar.files)>0){
    txt <- "\\section{Incremental VaR tables}"
    ind <- grep("%insert.ivar.tables.above", TT)
    TT  <- append(TT, txt, after=(ind-1))
    for (i in 1:length(ivar.files)){
      aux <- gsub("\\.tex","", ivar.files[i])
      filename <- paste("\\input{", aux, "}", sep="")
      ind <- grep("%insert.ivar.tables.above", TT)
      TT  <- append(TT, filename, after=(ind-1))
    }
  }
  #------------------------------------ Add corr tables ----------------------------
  corr.files <- tex.files[grep("corr_", tex.files)]
  if (length(corr.files)>0){
    txt <- "\\section{VaR sensitivity to correlation}"
    ind <- grep("%insert.corr.sensitivity.tables.above", TT)
    TT  <- append(TT, txt, after=(ind-1))
    for (i in 1:length(corr.files)){
      aux <- gsub("\\.tex","", corr.files[i])
      filename <- paste("\\input{", aux, "}", sep="")
      ind <- grep("%insert.corr.sensitivity.tables.above", TT)
      TT  <- append(TT, filename, after=(ind-1))
    }
  }

  tex.filename <- "VaR.report"
  writeLines(TT, paste(tex.filename, ".tex", sep=""))

  # run latex several times to get the formatting right...
  for (n in 1:3){
    cmd <- paste("//ceg/cershare/Risk/Software/Latex/fptex/TeXLive/bin/win32/pdflatex",
                 tex.filename)
    system(cmd, invisible=FALSE)
  }
  file.remove(paste(tex.filename, ".aux", sep=""))
  file.remove(paste(tex.filename, ".log", sep=""))
  file.remove(paste(tex.filename, ".toc", sep=""))
##   if (!options$run$is.overnight){
##     cmd <- paste("//NAS-OMF-01/cpsshare/Risk/Software/Latex/fptex/TeXLive/bin/win32/pdfopen",
##                  "--file VaR.report.pdf")
##     system(cmd, invisible=TRUE)
##     msg <- paste("Done! \nYour report was saved in folder: ",
##               options$save$reports.dir, "/", run$run.name, sep="")
##     tkmessageBox(message=msg)  
##   }
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
