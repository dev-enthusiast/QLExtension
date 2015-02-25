# tested with R 2.7.1 from S:/All/Risk/Software

source("H:/user/R/RMG/Energy/Stress.Tests/StressTestEnv.R")
STenv$asOfDate <- as.Date("2008-10-31")

# create STenv$uCparty
STenv$.loadExternalCounterparties()
head(STenv$uCparty)

# create STenv$specification
STenv$.loadSTdefinitions()
head(STenv$specifications)

# get curve classifications needed
STenv$.getCurveClassifications()

STenv$QQ <- STenv$.loadPositions.VCV()
STenv$addPrices()  # add the prices to STenv$QQ
head(STenv$QQ)

# calculate the change in MTM for a stress test
dMTM <- STenv$calcOneStressTest(1)
head(dMTM)

# join by eti with RAFT data
