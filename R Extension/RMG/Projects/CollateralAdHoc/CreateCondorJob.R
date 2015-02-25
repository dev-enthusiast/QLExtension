###############################################################################
source("H:/user/R/RMG/Utilities/sendEmail.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/Environment/Condor/GenerateCondorFile.R")
source("H:/user/R/RMG/Projects/CollateralAdHoc/StressFramework.R")


BASE_PATH = "S:/All/Risk/Collateral Scenarios"

asOfDate = .getAsOfDate()

reportDir = file.path( BASE_PATH, "Reports", as.character( asOfDate ) )
dir.create( reportDir, show = FALSE )

scenarios = read.csv( file.path( BASE_PATH, "ScenarioList.csv") )

jobName = "CollateralStressJob"
inputFilePath = "S:/All/Risk/Software/R/prod/Projects/CollateralAdHoc/CondorRunner.R"
outputFilePath = "S:/All/Risk/Software/R/prod/Projects/CollateralAdHoc/CollateralAdHoc.condor"


individualJobPortion = ""
individualJobPortion = append(individualJobPortion, 
        "# Begin individual job section")

individualJobPortion = append(individualJobPortion,
        paste('environment = "powerFactor=$(powerFactor) ',
                'gasFactor=$(gasFactor) coalFactor=$(coalFactor) ',
                'filter=$(filter) outputFile=$(outputFile) $(EnvSettings)"',
                sep=""))
individualJobPortion = append(individualJobPortion, " ")

for ( rowIndex in seq_len( nrow( scenarios ) ) )
{
    individualJobPortion = append(individualJobPortion,
            paste("powerFactor = '", scenarios$Power[rowIndex], "'", sep=""))
    individualJobPortion = append(individualJobPortion,
            paste("gasFactor = '", scenarios$Gas[rowIndex], "'", sep=""))
    individualJobPortion = append(individualJobPortion,
            paste("coalFactor = '", scenarios$Coal[rowIndex], "'", sep=""))
    individualJobPortion = append(individualJobPortion,
            paste("filter = ''", sep=""))
    
    outputFile = file.path( reportDir, paste( scenarios$Case[rowIndex], ".csv", sep="") )
    individualJobPortion = append(individualJobPortion,
            paste("outputFile = '", outputFile, "'", sep=""))
    
    individualJobPortion = append(individualJobPortion, "Queue")
    individualJobPortion = append(individualJobPortion, " ")
}


GenerateCondorFile$writeCondorFile( jobName, outputFilePath, inputFilePath, individualJobPortion )