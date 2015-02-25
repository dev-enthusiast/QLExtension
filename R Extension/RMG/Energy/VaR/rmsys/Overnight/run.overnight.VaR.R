# Kickoff the nightly VaR calculation.
#
#
#

options <- NULL
#options$asOfDate   <- as.Date("2007-03-12")
options$asOfDate   <- Sys.Date()
options$keep.log   <- TRUE
options$source.dir <- "H:/user/R/RMG/Energy/VaR/rmsys/"
source(paste(options$source.dir,"Overnight/set.parms.overnight.R",sep=""))
options <- set.parms.overnight(options)

prepare.overnight.VaR(options)

file <- paste(options$source.dir,"Overnight/schedule.overnight.jobs_A.xls", sep="")
con <- odbcConnectExcel(file)
run.all <- sqlFetch(con, "overnight")
odbcCloseAll()
colnames(run.all) <- gsub("#", ".", colnames(run.all))
ind.class <- grep("class", colnames(run.all))
  
run.all <- subset(run.all, toupper(run)=="YES")
ind <- sapply(run.all, class) %in% c("factor", "logical")
run.all[,ind]   <- sapply(run.all[,ind], as.character)
run.all[,ind]   <- sapply(run.all[,ind], toupper)
individual.runs <- which(is.na(run.all$run.name))
aggregated.runs <- as.character(unique(na.omit(run.all$run.name)))

for (r in 1:length(individual.runs)){
  cat(paste("Starting individual run = ", r, "\n"), sep="")
  aux <- run.all[individual.runs[r], ]
  run <- res <- NULL
  run$run.name <- NA
  run$PBT <- aux[,-ind.class]
  run$classification <- tolower(as.character(na.omit(sapply(aux[,ind.class],
                                                            as.character))))
  source(paste(options$source.dir,"main.run.VaR.R",sep=""))
  res <- try(main.run.VaR(run, options))
  if (class(res)!="try-error"){
    cat(paste("Succeeded individual run = ", r, ".\n", sep=""))
  } else {
    cat(paste("Failed individual run = ", r, ".\n", sep=""))
  }
}
if (options$keep.log){sink()}  # come back to the console



## source("H:/user/R/RMG/Energy/VaR/rmsys/Overnight/get.historical.prices.R")
## for (d in 1:length(options$calc$hdays)){
##   day <- options$calc$hdays[d]
##   get.one.day.vols(day, options)
## }
## load("S:/All/Risk/Data/VaR/hPrices.2006-10-02.RData")

## run$PBT$portfolio <- "INTERCOMPANY"
## #run$PBT$book[1] <- NA
## run$PBT <- rbind(run$PBT, run$PBT)
## #run$PBT$book[2] <- "ACC-HEDGE"
## run$PBT$portfolio[2] <- NA
## run$PBT$book[2] <- "EAST"

################################################################################
################################################################################
## aux <- rbind(DD, VV)
## write.xls(aux, "c:/Temp/east.positions.xls")
## rm(list=ls())
## run.all <- list(run            = "YES",
##                 run.name       = NA,
## #                      portfolio  = "GAS TRADING PORTFOLIO",
## #                      portfolio  = "HOUSE GAS PORTFOLIO",
##                       portfolio      = "LG PORTFOLIO",
##                       book           = NA,
##                       trade          = NA, 
##                       start.dt       = NA,
##                       end.dt         = NA,
##                       weight         = 1,
##                       report         = "YES",
##                      classification  = c("gasseason", "in.20072008"))

## run <- run.all

## options <- NULL
## options$source.dir <- "H:/user/R/RMG/Energy/VaR/rmsys/"
## source(paste(options$source.dir,"Overnight/set.parms.overnight.R",sep=""))
## options <- set.parms.overnight(options)

## #prepare.overnight.VaR(options)

## source(paste(options$source.dir,"Overnight/main.overnight.VaR.R",sep=""))
## main.overnight.VaR(run, options)

