# Download nuke generation levels from TSDB 
#
#
# Written by Adrian Dragulescu on 16-Feb-2005

nuke.generation <- function(region, options, save){

  lsave <- save; loptions <- options          # local options
  lsave$dir <- "S:/All/Risk/Data/Units/"
  filename <- paste(lsave$dir, "units.csv", sep="")
  Units <- read.csv(filename)

  ind <- which(Units$Region==options$region)
  Units <- Units[ind,]
  loptions$hdata.startDate <- as.Date("1999-01-01")
  loptions$hdata.symbols <- as.character(Units$Tsdb.name)
  loptions$hdata.filename <- paste(options$region,".nuke.generation.csv",sep="")
  loptions$tsdb.refresh <- 1
  
  source("C:/R/Work/MyLibrary/Utilities/tsdbRead.R") 
  res <- tsdbRead(loptions, lsave)

  data <- as.matrix(res[,-(1:3)]) %*% as.matrix(Units$Capacity)
  data <- 0.01*data/sum(Units$Capacity)
  data <- data.frame(res[, 1:3], availability=data)
  data$days <- as.Date(paste(res$year, res$month, res$day, sep="-"))

  data <- na.omit(data)
  plot(data$days, data$availability, pch=20, col="blue")
  
  
  source("C:/R/Work/MyLibrary/TimeFunctions/mean.by.month.R")
  lsave$Tables$filename <- paste(lsave$dir, "hgeneration.",
                                 options$region, ".month.csv", sep="")
  OutT <- mean.by.month(cbind(data,x=data$availability), loptions, lsave)
  round(OutT,2)
  

  
}
