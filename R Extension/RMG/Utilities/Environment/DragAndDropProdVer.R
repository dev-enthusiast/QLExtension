source("H:/user/R/RMG/Utilities/Environment/prodver.R")

filesToPush <- commandArgs()
filesToPush <- filesToPush[-(1:5)]

filesToPush <- gsub("\\", "/", filesToPush, fixed=TRUE)
print(filesToPush)

lapply(filesToPush, prodver)
