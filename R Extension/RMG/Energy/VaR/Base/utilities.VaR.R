#======================================================================
mkdir <- function(dir){
  shell(paste("mkdir", dir), translate=TRUE, invisible=TRUE)
}

#======================================================================
mem <- function(){               # show the size of elements in memory
  res <- sort(sapply(ls(envir=parent.frame()),
    function(x){object.size(get(x))}), decreasing=TRUE)
  res <- res[res>1000000]  # show only objects larger than 1MB
  cat("All objects larger than 1MB (size in MB):\n")
  return(round(res/1000000))
}

#======================================================================
gas.season <- function(contract.dt){  # contract.dt is a vector of R Dates
  aux <- data.frame(mm  = as.numeric(format(contract.dt, "%m")),
                    yy  = format(contract.dt, "%y"),
                    pyy = format(contract.dt-365, "%y"), # previous
                    yyp = format(contract.dt+365, "%y")) # post
  aux$cut <- as.numeric(cut(aux$mm, breaks=c(0, 3, 10, 12)))
  aux$season <- NA
  ind <- aux$cut == 1
  aux$season[ind]  <- paste("W.",aux$pyy[ind],"-",aux$yy[ind], sep="")
  ind <- aux$cut == 2
  aux$season[ind]  <- paste("S.",aux$yy[ind], sep="")
  ind <- aux$cut == 3
  aux$season[ind]  <- paste("W.",aux$yy[ind],"-",aux$yyp[ind], sep="")
  return(aux$season)
}

#========================================================================
plot.PnL.density <- function(run.name, options){
  run.name <- "rmg.baltimore"
  file <- paste("S:/All/Risk/Reports/VaR/prod/2007-09-06/", run.name,
                "/all.PnL.RData", sep="")
  load(file)
  res <- colSums(all.PnL[["market"]][,-1])/1000000
  hist(res, col="wheat", breaks=10, xlab="PnL, M$", freq=F, 
       main="Baltimore trading portfolio")
  rug(res, side=1)
  lines(density(res), col="blue")
  quantile(res, probs=0.05)
  sd(res)
}


## #======================================================================
## clean.hPrice.files <- function(){
##   options$save$datastore.dir <- "S:/All/Risk/Data/VaR/"
##   files <- list.files(options$save$datastore.dir, pattern="hPrices*")
##   dates <- strsplit(files, "\\.")
##   dates <- sapply(dates, function(x){x[[2]]})
##   for (d in dates){
##     file <- paste(options$save$datastore.dir,"hPrices.", d,
##       ".RData", sep="")
##     if (!file.exists(file)){get.one.day.prices(options$calc$hdays[d], options)}
##     load(file)

##     if (class(hP$curve.name)=="factor"){ 
##       hP$curve.name <- as.character(hP$curve.name)}
##     hP <- hP[!duplicated(hP),]
##     save(hP, file=file)
##     cat(as.character(d),"\n")
##   }
## }

## #======================================================================
## make.run.name <- function(options){   # not used now 
##   if (is.na(run$run.name) | length(run$run.name)==0){
##     rname <- NULL     
##     aux <- paste(run$PBT$portfolio[which(!is.na(run$PBT$portfolio))], collapse=".")
##     if (length(aux)>0 & aux!=""){rname <- paste("p.", aux, ".", sep="")}
##     aux <- paste(run$PBT$book[which(!is.na(run$PBT$book))], collapse=".")
##     if (length(aux)>0 & aux!=""){rname <- paste(rname, "b.", aux, ".", sep="")}
##     aux <- paste(run$PBT$strategy[which(!is.na(run$PBT$strategy))], collapse=".")
##     if (length(aux)>0 & aux!=""){rname <- paste(rname, "s.", aux, ".", sep="")}
##     aux <- paste(run$PBT$trade[which(!is.na(run$PBT$trade))], collapse=".")
##     if (length(aux)>0 & aux!=""){rname <- paste(rname, "t.", aux, ".", sep="")}
##     rname <- gsub("\\.*$", "", rname)  # remove trailing .
##     run$run.name <- gsub(" ", "", tolower(rname))
##   }  
##   return(options)
## }
