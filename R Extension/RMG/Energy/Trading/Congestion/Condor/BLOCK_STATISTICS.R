# Calculate path statistics with CONDOR
#
# Written by AAD on 21-Apr-2008

require(FTR)
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.analyze.paths.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
files <- list.files("H:/user/R/RMG/Utilities/Interfaces/FTR/R/",
  full.names=T, pattern="R$")
for (f in files) source(f)

PS.main <- function()
{
  ISO <- Sys.getenv("ISO_REGION")
  block.size <- as.numeric(Sys.getenv("BLOCK_SIZE"))
  block.no   <- as.numeric(Sys.getenv("BLOCK_NO"))
  if (is.na(block.no)){block.no <- 1}
  rLog(paste("Running path statistics for block", block.no, "region", ISO))

  filename.out <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/", ISO,
    "_STATS_BLK_", block.no, ".RData", sep="")
  if (file.exists(filename.out)){
    rLog("File is already there.  Exiting.")
    return()
  }
  
  FTR.load.ISO.env( ISO )
  
  filename <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/all.paths.",
                    ISO, ".RData", sep="")
  load(filename)

  if (trunc(nrow(all.paths)/block.size)+ 1 < block.no){
    rLog("Nothing to do here.  Exiting")
    return()
  }
  
  ind <- ((block.no-1)*block.size+1):min(block.no*block.size, nrow(all.paths))
  paths <- all.paths[ind, ]
  if (ISO=="NEPOOL") ISO.env <- NEPOOL

  # get the prices
  hSP <- FTR.get.hSP.for.paths(paths, ISO.env, melt=F)

  # calc stats
  res <- RPT$basic.stats(paths, hSP, ISO.env)

  # write results to file
  rLog("Saving results to file ...")
  save(res, file=filename.out)
  
  rLog("Done.")
}


##################################################################
PS.main()
