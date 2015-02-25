################################################################################
# This is the file responsible for orchestrating the potential exposure
# calculation.
#

################################################################################
# External Libraries
#
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Energy/VaR/PE/PEUtils.R")
source("H:/user/R/RMG/Energy/VaR/PE/Condor/PECondorUtils.R")
source("H:/user/R/RMG/Energy/VaR/PE/Condor/CleanPEReportData.R")
source("H:/user/R/RMG/Energy/VaR/PE/Condor/CleanSimulationData.R")
source("H:/user/R/RMG/Energy/VaR/PE/Condor/PriceSimulationDAG.R")
source("H:/user/R/RMG/Utilities/Environment/Condor/DAGCreator.R")
source("H:/user/R/RMG/Utilities/Environment/Condor/CondorUtilities.R")


################################################################################
# File Namespace
#
PECondorMaster = new.env(hash=TRUE)


################################################################################
# The overall PE DAG has 5 nodes; a LoadCurves node, a Price
# simulation DAG, the potential exposure by counterparty node, a results 
# aggregation job, and a credit portfolio calculation. The PE by counterparty
# node is actually 3 sets of two jobs, the calculation and a status check.
# Each of these requires a condor file that will be put into the overall PE.DAG
# file. These condor files are generated below.
#
PECondorMaster$main <- function()
{
  # CLEAN UP & SETUP
    # Remove old condor, sub, rdata, and dag files
    try(PECondorUtils$cleanCondorDirectory())
    try(CleanPEReportData$main())
    try(CleanSimulationData$main())

    # Recreate the counterparty position file by determining the last valid
    # set of positions 
    PE$loadPEOptions()    
    datastoreDir = paste(PE$options$save$datastore.dir, "Positions", sep="")
    
    #Determine the date of our last valid set of positions
    validPositionFiles = list.files( datastoreDir, 
      pattern="^vcv.positions.+[.RData]$" ) 
    validDates = as.Date( gsub( "vcv.positions.", "", 
      gsub(".RData", "", validPositionFiles) ) )
      
    PEUtils$createCounterpartyPositionFile(asOfDate = max(validDates, na.rm=TRUE))


  # LOAD CURVES NODE
    # Generate Condor File to Load Curve Info
  #  curveLoadSubmitFile = PECondorUtils$createDataLoadJobFile()
  
  
  # PRICE SIMULATION NODE
    # Generate Curve Group RData Files and
    # Generate Condor Files to Simulate Prices By Curve Group
    PECondorUtils$createCurveJobFiles()
  
    # Generate DAG File to Organize Price Simulations
    priceDAGFile = PriceSimulationDAG$create()
    
    # Create .condor.sub File From Price Simulation DAG
    command = paste( "C:/condor/bin/condor_submit_dag",
      "-dagman C:/condor/bin/condor_dagman.exe -f -no_submit",
      priceDAGFile )
    system(command)
    priceSimSubmitFile = paste(priceDAGFile, ".condor.sub", sep="")

  # GET PE RUN.NAMES (COUNTERPARTY) LIST
    filename <- paste(PE$SRC_DIR, "schedule.overnight.PE.xls", sep="")
    con  <- odbcConnectExcel(filename)
    data <- sqlFetch(con, "PE_RUN_OVERNIGHT")
    odbcCloseAll()
    runNames <- toupper(sort(as.character(unique(data$RUN_NAME))))

  # PE BY COUNTERPARTY NODE
    # Generate PE By Counterparty Condor File
    peSubmitFile = PECondorUtils$createCounterpartyJobFile(runNames)

    # Regenerate the preSubmitFile after each run & email status
    updateStatusFile = PECondorUtils$createCounterpartyStatusJob()

  # GENERATE & SUBMIT THE OVERALL PE DAG FILE
    # Generate the PE DAG File
#    DAGCreator$addJob("LoadCurves", curveLoadSubmitFile)
    DAGCreator$addJob("PriceSimulation", priceSimSubmitFile)
    DAGCreator$addJob("PEByCounterparty1", peSubmitFile)
    DAGCreator$addJob("UpdateCPJob1", updateStatusFile)
    DAGCreator$addJob("PEByCounterparty2", peSubmitFile)
    DAGCreator$addJob("UpdateCPJob2", updateStatusFile)
    DAGCreator$addJob("PEByCounterparty3", peSubmitFile)
    DAGCreator$addJob("UpdateCPJob3", updateStatusFile)
    resultsAggregationFile = paste( PE$SRC_DIR, "Condor/", 
      "AGGREGATE_RESULTS.condor", sep="" )
    DAGCreator$addJob("AggregateResults", resultsAggregationFile)
    creditPortfoliosFile = paste( PE$SRC_DIR, "Condor/", 
      "CREDIT_PORTFOLIOS.condor", sep="" ) 
    DAGCreator$addJob("CalculateCreditPortfolios", creditPortfoliosFile)
    
    simcurveCheckFile = paste( PE$SRC_DIR, "Condor/", 
      "Sim_Curve_Check.condor", sep="" )
    DAGCreator$addJob("CheckSimulatedCurves", simcurveCheckFile) 
    
 #   DAGCreator$addDependency("LoadCurves", "PriceSimulation")
    DAGCreator$addDependency("PriceSimulation", "PEByCounterparty1")
    DAGCreator$addDependency("PEByCounterparty1", "UpdateCPJob1")
    DAGCreator$addDependency("UpdateCPJob1", "PEByCounterparty2")
    DAGCreator$addDependency("PEByCounterparty2", "UpdateCPJob2")
    DAGCreator$addDependency("UpdateCPJob2", "PEByCounterparty3")
    DAGCreator$addDependency("PEByCounterparty3", "UpdateCPJob3")
    DAGCreator$addDependency( "UpdateCPJob3", 
      c("AggregateResults", "CalculateCreditPortfolios","CheckSimulatedCurves") )
  
    
    mainDAGFile = DAGCreator$writeDAG( paste(PE$CONDOR_DIR, "PE_MAIN.DAG",
      sep="") )
    
  rLog("Done.")

  # Create .condor.sub File for the PE_MAIN.DAG (cannot condor_submit_dag it!)
  rLog("Create the PE_MAIN.condor.sub file ...")  
  command = paste( "C:/condor/bin/condor_submit_dag",
    "-dagman C:/condor/bin/condor_dagman.exe -f -no_submit",
    mainDAGFile )
  system(command)
  mainDAGSubmitFile = paste(mainDAGFile, ".condor.sub", sep="")
  rLog("Done.")  
    
  rLog("Submit the PE_MAIN.DAG.condor.sub job ...")
  aux <- Sys.getenv("COMPUTERNAME")
  system(paste("C:/condor/bin/condor_submit.exe -name", aux, mainDAGSubmitFile))
  rLog("Done.")  

  invisible(mainDAGFile)
}




################################ MAIN EXECUTION ################################
.start = Sys.time()
dagFile = PECondorMaster$main()
Sys.time() - .start

