source("H:/user/R/RMG/Utilities/Environment/Condor/CondorUtilities.R")
source("H:/user/R/RMG/Energy/VaR/PE/Condor/PECondorUtils.R")

LOG_DIRECTORY =
  "//ceg/cershare/All/Risk/Reports/VaR/prod/CondorNightlyLogs"

CondorUtilities$removeEmptyLogs(LOG_DIRECTORY)
  
errorFiles = list.files( LOG_DIRECTORY,
  pattern="SimulateCounterparties.+error$", all.files=FALSE, full.names=TRUE)

print(errorFiles)

correspondingOutputFiles = sub("error$", "out", errorFiles)

failInformation = data.frame(file=correspondingOutputFiles, cparty=NA)

extractCounterpartyName <- function(fileName)
{
  splitValues = strsplit(fileName, "'", fixed=FALSE)

  return(splitValues[[1]][2])
}

failInformation$cparty = lapply( as.character(failInformation$file),
  extractCounterpartyName )

counterpartyList = sort(as.character(failInformation$cparty))

PECondorUtils$createCounterpartyJobFile(counterpartyList)
