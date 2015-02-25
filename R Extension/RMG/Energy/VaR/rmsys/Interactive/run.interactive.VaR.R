# Run VaR interactively.  This function is called from the GUI
# when the Run! button is pressed. 
#
# Written by Adrian Dragulescu on 1-Jan-2007


run.interactive.VaR <- function(RR, options){

  options$asOfDate   <- as.Date(options$asOfDate)
  options$source.dir <- "H:/user/R/RMG/Energy/VaR/rmsys/"
  if (toupper(options$run$useLivePositions) == "NO"){
    options$run$useLivePositions <- FALSE
  } else {options$run$useLivePositions <- TRUE}  
  source(paste(options$source.dir,"Interactive/set.parms.interactive.R",sep=""))
  options <- set.parms.interactive(options)
 #  options$save$reports.dir <- gsub("\\\\", "/", tempdir())
  options$save$reports.dir <- "S:/All/Temporary/VaR/"

  ind <- which(RR == "", arr.ind=TRUE); RR[ind] <- NA
  RR  <- data.frame(RR)
  ind <- apply(RR, 1, function(x){all(is.na(x))})
  RR  <- RR[!ind,]
  ind <- sapply(RR, class) %in% c("factor", "logical")
  RR[,ind] <- sapply(RR[,ind], as.character)
  ind <- which(colnames(RR) %in% c("portfolio", "book", "strategy", "trade"))
  RR[,ind] <- sapply(RR[,ind], toupper)
  RR$weight <- ifelse(is.na(RR$weight), 1, ifelse(RR$weight=="-",-1,RR$weight))
  
  run <- NULL
  run$run.name <- NA
  run$PBT <- RR             # portfolio, book, trade
  if (length(options$run$run.name)>0){
    run$run.name <- options$run$run.name
    options$run$run.name <- NULL
  }
  run$classification <- options$run$extra.classFactors
  options$extra.classFactors <- NULL
  
  res <- NULL
  source(paste(options$source.dir,"main.run.VaR.R",sep=""))
  res <- try(main.run.VaR(run, options))
  if (class(res)!="try-error"){
    cat(paste("Succeeded.", sep=""))
  } else {
    cat(paste("Failed.", sep=""))
  }
  return(options)
}
