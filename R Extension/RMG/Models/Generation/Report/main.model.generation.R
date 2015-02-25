# Trunk of the entire process.
# 
#
# Written by Adrian Dragulescu on 21-Apr-2005

main.model.generation <- function(save, options){

source("H:/user/R/RMG/Models/Generation/Report/set.parms.model.R")
aux <- set.parms.model(save, options)
save <- aux[[1]]; options <- aux[[2]]

query <- paste("SELECT ID, YEAR, MONTH, ITERATION, BUCKET, PRICES, ",
  "GENERATION, COST, REVENUE, MONTHLYSTARTS, NUMHOURS, FUELCONSUMED, ",
  "FUELPRICE, VOM, FUELCOST, STARTCOSTS FROM TLP.RESULTS_GEN_T WHERE ",
  "RUN_ID='", options$run, "'", sep="")
con <- odbcConnect("load_wiz", uid="tlpops", pwd="tlpops")
cat("Grabbing data from load wiz.  This can take a while...")
flush.console()
MM <- sqlQuery(con, query)           # this can take a while ...
cat("Done.\n"); flush.console()
odbcClose(con)
MM[1:10,]


source(paste(options$path.G, "analyze.generation.R", sep=""))
analyze.generation(MM, save, options)

#------------------------------------Make pdf report----
source(paste(options$path.G, "make.pdf.R", sep=""))
make.pdf(save, options)


}


# filename <- paste(save$dir$main, "Data/results.by.year.",options$run,".csv", sep="")
# MM <- read.csv(filename); MM <- MM[,-ncol(MM)]
