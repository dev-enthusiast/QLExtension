# Split the NEPOOL Gen data by masked asset id
# and write it to CSV files
#
# Written by Adrian Dragulescu on 17-Jun-2008


require(RODBC)
source("H:/user/R/RMG/Utilities/RLogger.R")

.get.unique.asset.ids <- function()
{
  con.string <- paste("FileDSN=S:/All/Risk/Software/R/prod/Utilities/",
    "DSN/ISOP.dsn;UID=geminiops;PWD=gemup05;", sep="")  
  con <- odbcDriverConnect(con.string)

  query <- "select distinct masked_asset_id from gemini.iso_gen_offers"

  uID <- sqlQuery(con, query)
  odbcCloseAll()
  uID <- na.omit(uID)
  write.csv(uID, file="S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Generation/all.uID.csv")
  
  return(uID)
}

.make.condor.file <- function()
{
  .get.unique.asset.ids()
  
  uID <- read.csv("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Generation/all.uID.csv")

  filename <- paste("H:/user/R/RMG/Energy/Trading/Congestion/Condor/",
                    "template.BLOCK_STATISTICS.condor", sep="")
  aux <- readLines(filename)

  block.size <- 5
  no.blocks  <- trunc(nrow(uID)/block.size) + 1
  
  ind <- grep("environment = ", aux)
  aux[ind] <- gsub("ISO_REGION='NEPOOL'", "", aux[ind])
  aux[ind] <- gsub("XXX", block.size, aux[ind])
  
  ind <- grep("STATISTICS", aux)
  aux[ind] <- gsub("STATISTICS", "GEN_DATA", aux[ind])
  
  for (b in 1:no.blocks){
    aux <- c(aux, paste("block = '", b, "'", sep=""), "Queue")
  }
    
  dir.out  <- "S:/All/Risk/Software/R/prod/Energy/Trading/Congestion/Condor/"
  filename <- paste(dir.out, "BLOCK_GEN_DATA.condor", sep="")
  writeLines(aux, con=filename)

}

gen.data.main <- function()
{
  dir <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Generation/"
  block.size <- as.numeric(Sys.getenv("BLOCK_SIZE"))
  block.no   <- as.numeric(Sys.getenv("BLOCK_NO"))
  if (is.na(block.no)){block.no <- 1}
  rLog(paste("Getting NEPOOL gen data for block", block.no))
  
  ind <- ((block.no-1)*block.size+1):(block.size*block.no)
  
  uID <- read.csv("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Generation/all.uID.csv")
  uID <- uID[ind,2]

  fnames <- paste(dir, uID, ".csv", sep="")
  if (all(file.exists(fnames))){
    rLog("All files already exist.  Exiting.")
    return()
  }

  con.string <- paste("FileDSN=S:/All/Risk/Software/R/prod/Utilities/",
    "DSN/ISOP.dsn;UID=geminiops;PWD=gemup05;", sep="")  
  con <- odbcDriverConnect(con.string)

  query <- paste("select masked_lead_participant_id, masked_asset_id, ",
    "must_take_energy, max_daily_energy, economic_max, economic_min, ",
    "cold_startup_price, intermediate_startup_price, hot_startup_price, ",
    "noload_price, ", paste(c("offer_block_price", "offer_block_energy"),
    rep(1:10, each=2), sep="", collapse=", "), ", report_date, ",
    "trading_interval ", 
    "from gemini.iso_gen_offers where masked_asset_id in ('",
    paste(uID, sep="", collapse="', '"), "')", sep="")

  MM <- sqlQuery(con, query)
  odbcCloseAll()

  
  MM <- split(MM, MM$MASKED_ASSET_ID)
  for (i in seq_along(MM)){
    aux <- MM[[i]]
    aux <- aux[order(aux$REPORT_DATE, aux$TRADING_INTERVAL), ]
    fileout <- paste(dir, names(MM)[[i]], ".csv", sep="")
    write.csv(aux, file=fileout, row.names=F)
    fileout <- paste(dir, names(MM)[[i]], ".RData", sep="")
    save(aux, file=fileout)
  }
  rLog("Done.")
}


#######################################################################
gen.data.main()
