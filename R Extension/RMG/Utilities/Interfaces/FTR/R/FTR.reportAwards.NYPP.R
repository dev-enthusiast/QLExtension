FTR.reportAwards.NYPP <- function(filepath, ISO.env)
{
  .reportAwards.main(filepath, ISO.env)

  aux <- awds[, c("auction","source.name", "sink.name", "class.type",
                  "award.ftr.mw")]
  
}



.reportAwards.main <- function(filepath, ISO.env)
{
  this.auction <- "N09"
  
  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.reportAwards.R")
  library(FTR); library(gplots); library(xtable)
  FTR.load.ISO.env("NYPP")
  options <- FTR.makeDefaultOptions(this.auction, NYPP)

  root <- "S:/All/Structured Risk/New York/TCC/NESTAT/2009-07/"
  setwd(root)

  
  fileName <- "upload.bids_Adrian.xls"
  (.getAwardedPaths.NYPP(this.auction, fileName, sheet=1))
  
  fileName <- "upload.bids_Adams.xls"
  (.getAwardedPaths.NYPP(this.auction, fileName, sheet=1))


  # where did the zones clear ...
  aux <- subset(NYPP$MAP, type=="ZONE")
  paths <- data.frame(path.no=1:(nrow(aux)-1), class.type="FLAT",
    source.ptid=aux$ptid[1], sink.ptid=aux$ptid[2:nrow(aux)],
    auction=options$auction)
  paths <- paths[1:10,]
  
  paths <- merge(paths, FTR.get.CP.for.paths(paths, NYPP), all.x=TRUE) 
  paths <- merge(paths,
    FTR.PathNamesToPTID(paths, NYPP$MAP, to="name"))

  
  
 }

