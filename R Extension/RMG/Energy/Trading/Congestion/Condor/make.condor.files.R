#
#
#
#
require(FTR)

.make.all.paths <- function(ISO="NEPOOL")
{
  FTR.load.ISO.env( ISO )

  MAP <- eval(parse(text=paste(ISO, "$MAP", sep="")))
  
  all.nodes <- subset(MAP, in.pnode.table)$ptid
  all.paths <- t(combn(all.nodes, 2))
  colnames(all.paths) <- c("source.ptid", "sink.ptid")

  all.paths <- rbind(data.frame(class.type="ONPEAK",  all.paths),
                     data.frame(class.type="OFFPEAK", all.paths))
  all.paths <- cbind(path.no=1:nrow(all.paths), all.paths)
  
  filename <- paste("S:/All/Risk/Data/FTRs/", ISO,
    "/ISO/DB/RData/all.paths.", ISO, ".RData", sep="")
  save(all.paths, file=filename) 
}

.get.all.paths <- function(ISO="NEPOOL")
{
  filename <- paste("S:/All/Risk/Data/FTRs/", ISO,
    "/ISO/DB/RData/all.paths.", ISO, ".RData", sep="")
  load(filename)
  
  return(nrow(all.paths))
}

make.BLOCK_STATISTICS.condor <- function(ISO="NEPOOL")
{
  npaths <- .get.all.paths( ISO )

  filename <- paste("H:/user/R/RMG/Energy/Trading/Congestion/Condor/",
                    "template.BLOCK_STATISTICS.condor", sep="")
  aux <- readLines(filename)

  batch.size <- 10000
  no.blocks  <- trunc(npaths/batch.size) + 1
  
  ind <- grep("environment = ", aux)
  aux[ind] <- gsub("NEPOOL", ISO, aux[ind])
  aux[ind] <- gsub("XXX", batch.size, aux[ind])
  
  for (b in 1:no.blocks){
    aux <- c(aux, paste("block = '", b, "'", sep=""), "Queue")
  }
    
  dir.out  <- "S:/All/Risk/Software/R/prod/Energy/Trading/Congestion/Condor/"
  filename <- paste(dir.out, "BLOCK_STATISTICS.condor", sep="")
  writeLines(aux, con=filename)

}

make.BLOCK_CP_and_SP.condor <- function(ISO="NEPOOL")
{
  npaths <- .get.all.paths( ISO )

  filename <- paste("H:/user/R/RMG/Energy/Trading/Congestion/Condor/",
                    "template.BLOCK_STATISTICS.condor", sep="")
  aux <- readLines(filename)

  batch.size <- 10000
  no.blocks  <- trunc(npaths/batch.size) + 1
  
  ind <- grep("environment = ", aux)
  aux[ind] <- gsub("NEPOOL", ISO, aux[ind])
  aux[ind] <- gsub("XXX", batch.size, aux[ind])
  
  ind <- grep("STATISTICS", aux)
  aux[ind] <- gsub("STATISTICS", "CP_and_SP", aux[ind])
  
  for (b in 1:no.blocks){
    aux <- c(aux, paste("block = '", b, "'", sep=""), "Queue")
  }
    
  dir.out  <- "S:/All/Risk/Software/R/prod/Energy/Trading/Congestion/Condor/"
  filename <- paste(dir.out, "BLOCK_CP_and_SP.condor", sep="")
  writeLines(aux, con=filename)

}
