# first load the data for the example
# PLEASE MAKE SURE THE FOLLOWING FUNCTIONS ARE SAVED IN THE APPROPRIATE DIRECTORY OR MAKE THE NECESSARY CHANGES!!

# No simulations
rm(list=ls())


source("H:/user/R/RMG/Models/Price/ForwardCurve/MultiFactor/Multifactor2/FC.PCA.Functions.r")
# read in data
# defines modifierr for data files
############################
####  MAKE SURE NUMBER FORMAT IN EXCEL IS DEVOID OF % AND $ FORMATS - USE ONLY GENERAL FORMAT IN EXCEL
###########################
#dataFileName = "Swap Rates"
dataFileName = "coal data 2"
#dataFileName = "coal data"


X = read.csv(paste(dataFileName,".csv",sep=""), row.names=1)
  
dT=90
NSims = 100000
pctPlot = 99
getRow=1
generatingDistribution=0
modelType=2

simDataStruct = PCSimGenerate(dataFileName, dT, NSims, getRow,pctPlot,FALSE,generatingDistribution,modelType, FALSE )
 
pct_expl = cumsum(simDataStruct[[4]])

hDataSet=simDataStruct[[2]]
simDataSet = simDataStruct[[3]]

PCObject = simDataSet[[1]]
simData = simDataSet[[2]]   # simulations
X0 = simDataSet[[8]]
PC = simDataSet[[4]]
SC = PCObject$x

#windows()
#plot(ecdf(simData[,1]))
#windows()
#plot(ecdf(SC[,1]))

hData = hDataSet[[1]]

labels(hData)                                
#acf(hData,lag.max=5)

avgpctdiffannual = (X0/colMeans(simData)-1)/(dT/252)*100
eta2 = simDataSet[[7]]
etadiffannual =  eta2[1,]/(1/252)*100


