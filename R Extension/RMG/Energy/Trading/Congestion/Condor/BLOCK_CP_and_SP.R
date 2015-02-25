# Calculate path statistics with CONDOR
#
# Written by AAD on 21-Apr-2008

require(CEGbase); require(reshape); require(zoo)
source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.get.CP.for.paths.R")
source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.get.hSP.for.paths.R")
source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.AuctionTerm.R")

# I should get this from SecDb:::format.dateMYY but no NAMESPACE yet. 
format.dateMYY <- function (dateIn, direction = 1) 
{
    monthGS <- 1:12
    names(monthGS) <- c("F", "G", "H", "J", "K", "M", "N", "Q", 
        "U", "V", "X", "Z")
    if (direction == 1) {
        letter <- substr(dateIn, 1, 1)
        ind <- as.numeric(monthGS[letter])
        yr <- substr(dateIn, 2, 3)
        dateOut <- as.Date(paste(yr, ind, "01", sep = "-"), format = "%y-%m-%d")
    }
    else {
        if (direction == -1) {
            aux <- format(dateIn, format = "%m%Y")
            mth <- names(monthGS)[as.numeric(substr(aux, 1, 2))]
            yr <- substr(aux, 5, 6)
            dateOut <- paste(mth, yr, sep = "")
        }
        else {
            print("Unknown direction, check inputs.")
        }
    }
    return(dateOut)
}


#######################################################################
PS.main <- function()
{
  ISO <- Sys.getenv("ISO_REGION")
  block.size <- as.numeric(Sys.getenv("BLOCK_SIZE"))
  block.no   <- as.numeric(Sys.getenv("BLOCK_NO"))
  if (is.na(block.no)){block.no <- 1}
  rLog(paste("Running CP vs. SP for block", block.no, "region", ISO))

  filename.out <- paste("S:/All/Risk/Data/FTRs/", ISO, "/ISO/DB/RData/",
    ISO, "_CPSP_BLK_", block.no, ".RData", sep="")
  if (file.exists(filename.out)){
    rLog("File is already there.  Exiting.")
    return()
  }
  
  # avoid loading the env directly 
  ISO.env <- new.env(hash=TRUE)
  load(paste("S:/All/Risk/Data/FTRs/", ISO, "/ISO/DB/RData/map.RData",
             sep=""))
  ISO.env$MAP <- MAP
  load(paste("S:/All/Risk/Data/FTRs/", ISO, "/ISO/DB/RData/ftrcp.RData",
             sep=""))
  ISO.env$CP <- CP
  load(paste("S:/All/Risk/Data/FTRs/", ISO, "/ISO/DB/RData/hPrices.RData",
             sep=""))
  ISO.env$hP <- hP

  # load all paths
  filename <- paste("S:/All/Risk/Data/FTRs/", ISO, "/ISO/DB/RData/all.paths.",
                    ISO, ".RData", sep="")
  load(filename)

  if (trunc(nrow(all.paths)/block.size)+ 1 < block.no){
    rLog("Nothing to do here.  Exiting")
    return()
  }
  
  ind <- ((block.no-1)*block.size+1):min(block.no*block.size, nrow(all.paths))
  rLog(paste("Pulling paths with index", ind[1], "to", ind[length(ind)]))  
  paths <- all.paths[ind,]

  res <- FTR.get.CP.for.paths( paths, ISO.env, add.SP=TRUE)
  
  rLog("Saving results to file ...")
  save(res, file=filename.out)
  
  rLog("Done.")
}


##################################################################
PS.main()








