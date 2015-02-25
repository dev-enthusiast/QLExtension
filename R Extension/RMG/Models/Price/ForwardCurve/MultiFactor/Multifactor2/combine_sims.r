# assume that PCSimsData$changeSims has the changes
# dT is set to the time scale of the sims
# X0 is the initial seed
#hedgeRatios =c(-.72,-.825,-.95)
hedgeRatios =c(-1,-1,-1)
X0 = marketData$seedRow
simsBase = PCSimsData$changeSims
simValsBase = PCSimsData$simData
simX0base = X0[,pickCols]
dT=21
DF = (1+5.32/100)^(-dT/252)

# now we need to combine and roll down.
# so let P48 be the initial price of the nearby+48
# in round 1 P48 = N48 * exp(chgs N48)
# in round 2 P48 = P48 * exp(chgs N36)
# in round 3 P48 = P48 * exp(chgs N24)

pickCols = c(12,24,36,48)
simsBase = simsBase[,pickCols]
simX0base = X0[,pickCols]
simeta = PCSimsData$eta2[,pickCols]
# first lets generate the timespread dist for initial hedge
no_rolls = ncol(simsBase)-1



for (i in 1:no_rolls) {
  usedCol = no_rolls+1-(i-1)
  
  targetDistribution = simsBase[,usedCol]
  hedgeDistribution = simsBase[,1]
  nextContractDistribution = simsBase[,2]

  if (i>1) {


    targetPrices = kronecker(targetPrices,exp(targetDistribution*sqrt(dT)+simeta[,usedCol]*dT))
    hedgePrices = kronecker(hedgePrices,exp(hedgeDistribution*sqrt(dT)+simeta[,1]*dT))
    nextContractPrices = kronecker(nextContractPrices,exp(nextContractDistribution*sqrt(dT)+simeta[,2]*dT))
    rollCost = nextContractPrices -  hedgePrices
    rerollMTM =  targetPrices+hedgeRatios[i]*hedgePrices + rollCost
    MTM = rerollMTM  + kronecker(MTM,exp(targetDistribution * 0))

    TSday1 = kronecker(TSday1,exp(targetDistribution * 0))
    PnL = MTM - TSday1

    newTSday1 = targetPrices+hedgeRatios[i+1]*nextContractPrices
    
    if (i<no_rolls) {
      TSday1 = TSday1 + newTSday1
    }
    
  } else {

    targetPrices = exp(targetDistribution*sqrt(dT)+simeta[,usedCol]*dT)*simX0base[usedCol]
    hedgePrices = exp(hedgeDistribution*sqrt(dT)+simeta[,1]*dT)*simX0base[1]
    nextContractPrices = exp(nextContractDistribution*sqrt(dT)+simeta[,2]*dT)*simX0base[2]
    rollCost = nextContractPrices -  hedgePrices
    rerollMTM =  targetPrices+hedgeRatios[i]*hedgePrices + rollCost
    MTM = rerollMTM

    TSday1 = kronecker(simX0base[usedCol] + hedgeRatios[i]*simX0base[1],exp(targetDistribution * 0))
    PnL = MTM - TSday1

    newTSday1 = targetPrices+hedgeRatios[i+1]*nextContractPrices

    if (i<no_rolls) {
      TSday1 = TSday1 + newTSday1
    }


  }

}




