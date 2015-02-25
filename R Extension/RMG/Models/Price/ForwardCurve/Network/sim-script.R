###############################################
## script to generate simulation data for all
###############################################

rm(list=ls())
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/util.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/sim.forward.R")
source("H:/user/R/RMG/Statistics/estimate.OU.R")
#source("H:/user/R/RMG/Finance/Simulations/simulateOU.R")


#### set up simulation options ####
options <- NULL
## number of time points in simulation
## note we'll input a vector of date to be simulated
## so these two options will be replaced
options$T <- 300
options$step <- 1 ## time step in simulation

options$D <- 48 ## number of future contracts simulated 
options$nsim <- 100 ## number of simulations
options$Kpc.month <- 10 ## number of principal components used for each curve,all month
options$Kpc.curve <- 10 ## number of PCs used in simulating curves
options$asofDate <- Sys.Date() ## simulate for today

#### read in structure file
struct <- read.csv("H:/user/R/RMG/Models/Price/ForwardCurve/Network/allcurves.csv")

## note that the RData files for historical price can be
## constructed from COMMOD column in struct
rootdir <- "S:/All/Risk/Data/VaR/prod/Prices/Market/"

## should I first do PCA on all curves? 
## I will store PCA results for all curves - will this cause memory problem?

## lhp.pca is PCA results for log historical price,
## lp.pc.forward is forward curve for PC's of log price
## This two variables will be stored incrementally and kept all the time.
## The elements for leaves don't need to be stored. They can be
## dumped to a file then delete from memory
lhp.pca <- lp.pc.forward <- NULL 

## I'm assuming all data are cleaned. We need some procedure to clean the data.

#######################################################
## first generate all fuel curves
#######################################################
## load in all fuel data - only contracts after today will be returned
allfuel <- c("NG", "COL", "FCO", "WTI")
nfuel <- length(allfuel)
hP.fuel <- vector("list", nfuel) ## historical price for fuels
for(ifuel in 1:nfuel) {
  hP.fuel[[ifuel]] <- load.hP(paste(rootdir, "all.", allfuel[ifuel], ".Prices.RData", sep=""),
                              asofDate=options$asofDate)
}
names(hP.fuel) <- allfuel

###### step 1: Generate commod reference curves (one for each type of fuel)
## This will be done in one computer.
## Note that they don't have parents
idx.commod.ref <- (struct[,2]=="NG" | struct[,2]=="COL" |
                   struct[,2]=="FCO" | struct[,2]=="WTI") & (struct[,3]=="COMMOD REF")
curve.commod.ref <- struct[idx.commod.ref,1:2]

## get log historical price for all months for these curves
## and store PCA results
ncurve <- dim(curve.commod.ref)[1]
lhp <- NULL
for(icurve in 1:ncurve) { ## loop thru curves
  curve.name <- as.character(curve.commod.ref[icurve,1]) ## this curve name
  commod <- as.character(curve.commod.ref[icurve,2]) ## this commodity
  allnames <- hP.fuel[[commod]][,1] ## all curve names for this commodity
  ## keep maximum options$D contract months
  nn <- dim(hP.fuel[[commod]])[2]
  idxtmp <- which(allnames==curve.name)
  idxtmp <- idxtmp[1:min(options$D, length(idxtmp))]
  lhp[[curve.name]] <- as.matrix(log(hP.fuel[[commod]][idxtmp, 3:nn]))
  ## do PCA for this curve - PCA result is stored incrementally
  ## It's very fast to do PCA, only 0.01 second so I can generate them on the fly
  lhp.pca[[curve.name]] <- princomp(t(lhp[[curve.name]]))
  ## 
}

## simulate curves - note I will return forward PC only because I will use them later
## a separate procedure will be used to covert them back to original prices
## note lhp.pca will be a long list containing PCA results for all curves
## --- I can write them to disk
tmp <- sim.forward.parents(lhp.pca[as.character(curve.commod.ref[,1])], options)
lp.pc.forward[names(tmp)] <- tmp
## get simulated price. 
lp.forward <- pc2lp(lp.pc.forward, lhp.pca)
## write each simulated curve to a file separately

##### visualize simulation result for parent curves
## - one curve, all month, one simulation
## curve 3 has problem because the data is not OU process
## and estimated gamma is negative.
## I was cheating in sim.forward.parents to make gamma a small number
## if it's negative
matplot(lp.forward[[3]][,,sample(options$nsim,1)], type="l")
## from another angle, fisrt element (NG) shows seasonality
matplot(t(lp.forward[[3]][,,sample(options$nsim,1)]), type="l")

## one curve, one month, all simulation
matplot(lp.forward[[1]][,sample(options$D,1),], type="l")

## one month, one simulation, all curves
tmp <- NULL
imon <- sample(options$D,1); isim <- sample(options$nsim, 1)
for(i in 1:ncurve)
  tmp <- cbind(tmp, lp.forward[[i]][,imon,isim])
matplot(tmp, type="l")

## Might need to clean memory between steps
## Note pc.forward and lhp.pca are important and must be kept 

###### step 2: Generate all Natural Gas regional reference curves 
## This could be condor'ed but doesn't worth it (takes a minute)
## Note that they have common parent 
idx.commod.ref <- (struct[,2]=="NG") & (struct[,3]=="REG REF")
curve.ng.reg.ref <- struct[idx.commod.ref,]

## get log historical price for all months for these curves
## and store PCA results
ncurve <- dim(curve.ng.reg.ref)[1]
lhp <- NULL
for(icurve in 1:ncurve) { ## loop thru curves
  curve.name <- as.character(curve.ng.reg.ref[icurve,1]) ## this curve name
  commod <- as.character(curve.ng.reg.ref[icurve,2]) ## this commodity
  allnames <- hP.fuel[[commod]][,1] ## all curve names for this commodity
  ## keep maximum options$D contract months
  nn <- dim(hP.fuel[[commod]])[2]
  idxtmp <- which(allnames==curve.name)
  idxtmp <- idxtmp[1:min(options$D, length(idxtmp))]
  lhp[[curve.name]] <- as.matrix(log(hP.fuel[[commod]][idxtmp, 3:nn]))
  ## do PCA for this curve 
  lhp.pca[[curve.name]] <- princomp(t(lhp[[curve.name]]))
}

## simulation
## all children
children <- as.character(curve.ng.reg.ref[,1])
## all parents - this needs to be careful in processing data frame
parents <- unique(as.vector(as.matrix(curve.ng.reg.ref[,4:dim(struct)[2]])))
## remove empty string
parents <- parents[sapply(parents, function(x) nchar(x)>0)]

## simulation
tmp <- sim.forward.children(lhp.pca[children], lhp.pca[parents], lp.pc.forward[parents],
                            curve.ng.reg.ref, options, ret="pc")

lp.pc.forward[names(tmp)] <- tmp
## get simulated price. 
lp.forward <- pc2lp(tmp, lhp.pca[children])
## write to files

##### visualize simulation result for NG regional reference curves
## - one curve, all month, one simulation
layout(1:4); par(mar=rep(2,4))
icurve <- sample(ncurve,1)
isim <- sample(options$nsim,1)
matplot(lp.forward[[icurve]][,,isim], type="l",
        main=paste("Simulation", names(lp.forward)[icurve]))
## from another angle, fisrt element (NG) shows seasonality
matplot(t(lp.forward[[icurve]][,,isim]), type="l",
        main=paste("Simulation", names(lp.forward)[icurve]))
## see the observed data
matplot(t(lhp[[icurve]]), type="l",
        main=paste("Historical", names(lp.forward)[icurve]))
matplot(lhp[[icurve]], type="l",
        main=paste("Historical", names(lp.forward)[icurve]))


## one curve, one month, all simulation
layout(1)
idx=sample(options$D,1)
matplot(lp.forward[[icurve]][,idx,], type="l")

## put together with observed 
N <- dim(lhp[[icurve]])[2]
idx=sample(options$D,1)
ylim=range(lhp[[icurve]][idx,], lp.forward[[icurve]][,idx,1:10])
plot(lhp[[icurve]][idx,], type="l", xlim=c(0,N+options$T), ylim=ylim)
apply(lp.forward[[icurve]][,idx,1:10], 2, function(x) lines(N+(1:options$T), x, col="grey"))

## one month, one simulation, all curves
tmp <- NULL
imon <- sample(options$D,1); isim <- sample(options$nsim, 1)
for(i in 1:ncurve)
  tmp <- cbind(tmp, lp.forward[[i]][,imon,isim])
matplot(tmp, type="l")

#### -- result looks good

###### step 3: Generate other COL, FCO and WTI curves respectively - skip


#######################################################
## Now simulate for electricity curves
## I will first generate the reference curves
## (one for each market). Then generate other curves
## based on their own reference curve
#######################################################

#### step 1: generate market reference curves
idx.elec.ref <- struct[,2]!="NG" & struct[,2]!="COL" &  struct[,2]!="FCO" &
  struct[,2]!="WTI" & struct[,3]=="COMMOD REF"
curve.elec.ref <- struct[idx.elec.ref,]
## prices for serveral curves are constant, remove them
curve.elec.ref <- curve.elec.ref[c(3,5,7),]

## load historical prices for these curves
## I have to load in all curves in those market, very inefficient.
## maybe it's faster to pull from the database?
allelec <- unique(as.character(curve.elec.ref[,2]))
nelec <- length(allelec)
hP.elec <- vector("list", nelec) ## historical price for electricity
for(ielec in 1:nelec) {
  hP.elec[[ielec]] <- load.hP(paste(rootdir, "all.", allelec[ielec], ".Prices.RData", sep=""),
                              asofDate=options$asofDate)
}
names(hP.elec) <- allelec

## get log historical price for all months for these curves
## and store PCA results
## NOTE: PWJ OFFPEAK contains all 0, remove it for now
#curve.elec.ref <- curve.elec.ref[-4,]
ncurve <- dim(curve.elec.ref)[1]
lhp <- NULL
for(icurve in 1:ncurve) { ## loop thru curves
  curve.name <- as.character(curve.elec.ref[icurve,1]) ## this curve name
  commod <- as.character(curve.elec.ref[icurve,2]) ## this commodity
  allnames <- hP.elec[[commod]][,1] ## all curve names for this commodity
  ## keep maximum options$D contract months
  nn <- dim(hP.elec[[commod]])[2]
  idxtmp <- which(allnames==curve.name)
  idxtmp <- idxtmp[1:min(options$D, length(idxtmp))]
  lhp[[curve.name]] <- as.matrix(log(hP.elec[[commod]][idxtmp, 3:nn]))
  ## clean the curves
  lhp[[curve.name]] <- t(apply(lhp[[curve.name]], 1, clean.curve))
  ## do PCA for this curve 
  lhp.pca[[curve.name]] <- princomp(t(lhp[[curve.name]]))
}

## simulation - this is basically working but results don't look right.
## Need to take a closer look ...
## all children
children <- as.character(curve.elec.ref[,1])
## all parents - this needs to be careful in processing data frame
parents <- unique(as.vector(as.matrix(curve.elec.ref[,4:dim(struct)[2]])))
## remove empty string
parents <- parents[sapply(parents, function(x) nchar(x)>0)]

## simulation
tmp <- sim.forward.children(lhp.pca[children], lhp.pca[parents], lp.pc.forward[parents],
                            curve.elec.ref, options, ret="pc")
## forward PC should be saved in files
lp.pc.forward[children] <- tmp # c(lp.pc.forward, tmp)
## get simulated price. This can be written to a file and cleared from memory
lp.forward <- pc2lp(lp.pc.forward[children], lhp.pca[children])

##### visualize simulation result for NG regional reference curves
## - one curve, all month, one simulation
layout(1:4); par(mar=rep(2,4))
icurve <- sample(ncurve,1)
isim <- sample(options$nsim,1)
matplot(lp.forward[[icurve]][,,isim], type="l",
        main=paste("Simulation", names(lp.forward)[icurve]))
## from another angle, fisrt element (NG) shows seasonality
matplot(t(lp.forward[[icurve]][,,isim]), type="l",
        main=paste("Simulation", names(lp.forward)[icurve]))
## see the observed data
matplot(t(lhp[[icurve]]), type="l",
        main=paste("Historical", names(lp.forward)[icurve]))
matplot(lhp[[icurve]], type="l",
        main=paste("Historical", names(lp.forward)[icurve]))


## one curve, one month, all simulation
layout(1)
icurve <- 1
imon <- sample(options$D,1)
ylim <- range(lhp[[icurve]][imon,], lp.forward[[icurve]][,imon,1:30])
N <- dim(lhp[[icurve]])[2]
plot(1:N, lhp[[icurve]][imon,], type="l", lwd=3, xlim=c(1,500), ylim=ylim)
apply(lp.forward[[icurve]][,imon,1:30], 2,
      function(x) lines(N+(1:(options$T)), x, type="l", col="grey"))

## above looks good.

## one month, one simulation, all curves
tmp0 <- tmp <- NULL
imon <- 1 #sample(options$D,1);
isim <- sample(options$nsim, 1)
layout(1:2)

for(i in 1:ncurve) {
  tmp0 <- cbind(tmp0, lhp[[i]][imon,])
  tmp <- cbind(tmp, lp.forward[[i]][,imon,isim])
}
matplot(tmp0, type="l", main="Historical")
matplot(tmp, type="l", main="Simulated")

## there're some problems. Correlation in simulated forward curves
## are obviously lower than that from historical data
cor(tmp0)
cor(tmp)

## Maybe we should model the differnce among curves are OU process ...


###### Next step is to generate all electricity curves market by market,
## conditional on the market refernce curve.
