#-------------------------------------------------------------------------------
# Simulate forward prices following the curve dependence hierachy
#
# RiskAnalytics
#-------------------------------------------------------------------------------

# Curve groups 
CG_FUEL_REFERENCE  = "Fuel Reference Curves"
CG_NG_REGIONAL_REF = "NG Regional Reference Curves"
CG_ALL_WTI         = "All WTI Curves"
CG_ALL_FCO         = "All FCO Curves"
CG_ALL_COL         = "All COL Curves"
CG_ALL_NG          = "All NG Curves"
CG_PWR_REF         = "Power Reference Curves"
CG_ALL_PWR         = "All Power Curves"

#-------------------------------------------------------------------------------
# function to simulate D correlated OU process for T days
# hp:           log historical prices
# nsim:         number of simulations
# forward.time: time point to be simulated, of length T+1.
#               First element should be time 0 so it has one more element
# x0:           vector of length D for starting values (current price)
# K:            number of PC used in simulation
#
# result is a T x D x nsim array
#-------------------------------------------------------------------------------
sim.corr.ou <- function(hp, nsim, forward.time, K=1) 
{
    # init result
    nhday  <- dim(hp)[1]
    D      <- dim(hp)[2]
    K      <- min(K, D)
  
    ## data holder
    lp.for <- array(NA, dim=c(length(forward.time)-1, D, nsim))
    pc.for <- array(NA, dim=c(nsim, length(forward.time)-1, K))
   
    ## number of business days between forward.time
    n.bday <- diff.bday(forward.time)
    #  matplot(hp, type="l")
    
    if( K<D )   # do PCA only if needed!  
    {  
        ## pca on hp with  na.action=na.omit (default)
        hp.pca <- princomp(~., data=data.frame(hp) )  
        y      <- hp.pca[["scores"]]
        W      <- t(hp.pca[["loadings"]][,1:K])
        center <- matrix(hp.pca[["center"]], ncol=D, nrow=length(n.bday),
                         byrow=T)
    }else 
    {           # K==D
        y <- na.omit(hp) # problem if you have missing data in the middle
    }
  
    nhday        <- dim(y)[1]
    ## loop on PCs
    ou.option    <- NULL
    ou.option$dt <- 1
    for( ipc in 1:K )  
    {
        ## estimate OU process
        ou.param      <- estimate.OU.regress(y[,ipc], ou.option, resid=F, sd=T)
        
        pc.for[,,ipc] <- sim.OU(ou.param, y[nhday,ipc], nsim, n.bday)
    }
  
    if( K<D )
    {                ## transform back to original scale
        for( isim in 1:nsim ) 
        {
            lp.for[,,isim] <- pc.for[isim,,]%*%W + center
        }
    }else 
    {                   # K==D
        lp.for <- aperm(pc.for, c(2,3,1))
    }
  
    #   windows(); matplot(t(pc.for[1:100,,1]), type="l")
      
    #   ind <- 1    # pick this curve, and show the historical pc and 5 simulations 
    #   aux <- t(apply(lp.for[,ind,], 1, quantile, c(0, 1)))
    #   aux <- rbind(tail(matrix(hp[,ind], nrow=nrow(hp), ncol=7), 50), cbind(aux, lp.for[,ind,1:5]))
    #   windows(); matplot(aux, type="l")
    
    return(lp.for)
}

#-------------------------------------------------------------------------------
# Function to simulate forward curves for several curves,
# many months. This is used to simulate parents.
# 
# Input is a list of log historical prices or the PCA result
# for log historical prices. Each element in the list is
# for a curve. 
# 
# Simulation steps are:
# 1. Assume each PC is an OU process, estimate the OU parameters;
# 2. Generate OU process for each PC correlatedly.
# 3. Transform back to original curve
#-------------------------------------------------------------------------------
sim.forward.parents <- function(pca.lp, FP.options, ret=c("pc", "lp")) 
{
    require(rstream)
    
    ret       <- match.arg(ret)
    ncurve    <- length(pca.lp)
    ## number of PC for curve used in simulation cannot be bigger than
    ## number of curves
    Kpc.curve <- min(FP.options$Kpc.curve, dim(pca.lp[[1]])[2])
    
    ## process inputs - do PCA if input is not
    if( class(pca.lp[[1]]) != "princomp" ) 
    {
        allnames <- names(pca.lp)
        for(ic in 1:ncurve)
            pca.lp[[ic]] <- princomp(t(pca.lp[[ic]]))
        names(pca.lp) <- allnames
    }
    
    ## local variables
    ## minimum number of historical days - 
    nhday <- 0
    for( i in 1:length(pca.lp) ) 
    {
        nhday <- max(nhday, dim(pca.lp[[i]][["scores"]])[1])
    }
    ## init memory for data holders results
    result <- vector("list", ncurve)
    for( ic in 1:ncurve ) 
    {
        result[[ic]] <- array( NA, dim=c(length(FP.options$sim.days),
                               FP.options$Kpc.month, FP.options$nsims.pkt) )
    }
    ou.param <- matrix(NA, nrow=3, ncol=ncurve)
    resid    <- matrix(NA, nhday-1, ncurve)
  
    #  matplot(pca.lp[[1]]["scores"]$scores[,1:5], type="l")
   
    # start simulation
    ou.option    <- NULL
    ou.option$dt <- 1
    x0           <- rep(0, ncurve)
    rLog("Simulate parent PC's ")
    # loop through PC's
    for( ipc in 1:FP.options$Kpc.month ) 
    { 
        # get historical prices for all curves
        # NOTE the length of curves are different. A newly added curve will have
        # much shorter history than others. This brings trouble to PCA.
        # If I only use the historical days with full data, it's possible that one
        # new curves causes throwing away of many data. So I'll set the
        # prices to be 0 if there's no data and hopeful it work fine.
        hp <- matrix(NA, nrow=nhday, ncol=ncurve)   
        # loop through curves
        for( ic in 1:ncurve ) 
        { 
            tmpn                         <- length(pca.lp[[ic]][["scores"]][,ipc])
            hp[(1:tmpn)+(nhday-tmpn),ic] <- pca.lp[[ic]][["scores"]][,ipc]
            x0[ic]                       <- pca.lp[[ic]][["scores"]][tmpn,ipc]
        }
        pricing.dts <- c(FP.options$asOfDate,FP.options$sim.days)
    
        # estimate and simulate OU
        tmp <- sim.corr.ou(hp, FP.options$nsims.pkt, pricing.dts, FP.options$Kpc.curve) 
     
        if( ncurve==1 ) ## only one curve
            result[[ic]][,ipc,] <- tmp
        else { ## multiple curves
            for(ic in 1:ncurve) 
                result[[ic]][,ipc,] <- tmp[,ic,]
        }
    }
    names(result) <- names(pca.lp)
    rLog("Done.")
    
    ## transform back to log price if requested
    if(ret == "lp") 
    {
        result <- pc2lp(result, pca.lp, FP.options$sim.days)
    }
        
    invisible(result)
}

#-------------------------------------------------------------------------------
# function to convert forward PC to forward log price
# I wrote this function separately for flexibility.
# But it'll take more memory by using extra copy of list
#
# Inputs are two lists for forward PC and PCA result on lp.
# Output is a list of array of dimension T x D x nsim.
# Note the input forward.pc and pca.lp could have different length.
# So I'll index all lists by curve name.
#
# sim.days is used to make dimnames 
#-------------------------------------------------------------------------------
pc2lp <- function(forward.pc, pca.lp, sim.days) 
{
    dd         <- dim(forward.pc[[1]])
    T          <- dd[1]; nsim <- dd[3]; Kpc <- dd[2]
    nc         <- length(forward.pc)
    curvenames <- names(forward.pc)
  
    result     <- NULL
    # loop through curves
    for( ic in 1:nc ) 
    { 
        cc           <- curvenames[ic]
        D            <- dim(pca.lp[[ic]][["scores"]])[2] 
        result[[cc]] <- array(NA, dim=c(T, D, nsim))
        W            <- t(pca.lp[[cc]][["loadings"]])[1:Kpc,]  
        # center     <- rep(pca.lp[[cc]][["center"]], each=T)
        center       <- matrix(pca.lp[[cc]][["center"]], nrow=T, ncol=D, byrow=T)
        for( isim in 1:nsim ) 
        {
            result[[cc]][,,isim] <- forward.pc[[cc]][,,isim]%*%W + center
        }
        # make dimnames
        dimnames(result[[cc]]) <- list(as.character(sim.days), names(pca.lp[[cc]][["center"]]),
                                       paste("sim", 1:nsim))
    }
    invisible(result)
}

#-------------------------------------------------------------------------------
# Function to simulate forward curves for children.
# The children curves to be simulated should be correlated
# conditional on their parents (otherwise they can be
# generated independently). 
#
# Inputs include a list of PCA results of lhp for children and parents,
# simulated forward PC for parents, and a data frame for pedigree.
# Note it takes lists of log price as input, then do PCA on them.
#
# Output is a list of simulated forward PCA
# (optionally forward log price). Each one is
# for a child and with dimension T x Kpc.month x nsim
# 
# Simulation steps are:
# 1. loop through PCs, do steps 2-3
# 2. For each curve, regress its lret.pc on its parents' lret.pc,
# save all coefficients and residuals. Obtain correlation matrix from
# regression residuals.
# 3. Generate lret for each curve (for this PC) based on
# regression: lret.pc.child = beta*lret.pc.forward.parent + eps.
# Here eps for all curves are MVN.
# 4. cumsum lret.pc.forward to get all simulated forward curves
# for all PC, all curve (lp.pc.forward).
# 5. Optionally transform back
#
# Modified Aug 2, 2007:
# Add new regression method ("lp"): Y_t ~ Y_{t-1} + X_t + X_{t-1}, or
# lhp.child_t ~ lhp.child_{t-1} + lhp.parent_t + lhp.parent_{t-1}
# There's a model selection procedure and non-significant parents
# will be dropped. 
# This gives good result but a little slower. For NG curves
# which are VERY similar, this doesn't gain much.
# But it works very well for electricity curves.
#
# ret = "lp" if you want to return log-prices
#     = "pc" if you want to return principal components
#-------------------------------------------------------------------------------

sim.forward.children <- function( pca.lp.children, 
                                  pca.lp.parent, 
                                  pc.forward.parent,
                                  pedigree, 
                                  FP.options, 
                                  ret=c("lp", "pc"),
                                  regress.method=c("lp")) 
{
    require(rstream)
    ret            <- match.arg(ret)
    regress.method <- match.arg(regress.method)
    
    # covert pedigree to a matrix if it's a data.frame
    if( is.data.frame(pedigree) )
        pedigree <- as.matrix(pedigree)
    
    nchildren <- length(pca.lp.children)
    # number of historical days for each child
    # note they could be different for each child
    n.hday <- rep(NA, nchildren)
    for( i in 1:nchildren )
        if( !is.null(pca.lp.children[[i]]) ) # for safety
            n.hday[i] <- dim(pca.lp.children[[i]][["scores"]])[1]

    N <- max(n.hday, na.rm=T)    # is this variable useful? -- AAD
  
    rLog("Start regression of PCA's")
    # number of PC.curve used in simulation cannot be bigger than
    # number of curves
    Kpc.curve <- min(FP.options$Kpc.curve, dim(pca.lp.children[[1]][["scores"]])[2])
    
    # init results for regression
    beta        <- vector("list",nchildren)   # there could be several coef for a curve for several parents
    beta.std    <- vector("list",nchildren)   # 
    std         <- rep(NA, nchildren)         # one residual sd per curve
    allhp       <- matrix(NA, N-1, nchildren) # I don't think we use resid anywhere: AAD 01/28/08
    validparent <- vector("list", nchildren)
                          
    # init simulation results
    result <- vector("list", nchildren)
    for(ichild in 1:nchildren)
        result[[ichild]] <- array( NA, dim=c(length(FP.options$sim.days),
                                   FP.options$Kpc.month, FP.options$nsims.pkt) )
    names(result) <- names(pca.lp.children)
    for( ipc in 1:FP.options$Kpc.month )    # loop on PCs
    { 
        rDebug(paste("Doing child regression on PC number ", ipc, sep=""))
        # loop on child curves to obtain regress parameters
        for( ichild in 1:nchildren ) 
        {
            nam <- names(pca.lp.children)[ichild]     # children name
            # only method supported 
            if( regress.method=="lp" ) 
            {               
                y <- pca.lp.children[[nam]][["scores"]][,ipc]
            }
            # get this child's parents --
            # Note input lists for parents are for all curve, but each curve
            # could have different parents. For example parent 1 for curve 1
            # could be different from parent 1 for curve 2. So lists for parents
            # need to be indexed by name, not index. Be careful!! 
            parents.name <- getParents(nam, pedigree)
            npar         <- length(parents.name)
            # curve has no parent
            # note: it shouldn't happend that in the same group of children,
            # some have parents and some don't. My code will handle that anyway
            # and issue a warning msg.
            if( npar<1 ) 
            { 
                warning("Curve ", nam, " has no parent curve!!")
                beta[[ichild]] <- 0
                tmpidx         <-(N-n.hday[ichild]+1):(N-1) 
                # resid[tmpidx,ichild] <- y  # not needed
                std[ichild]    <- sd(y)
            }else
            {
                ## make regression matrix -
                ## be careful when children and parents have different number of historical days
                ## Parents should always have equal or more days then children
                ## use log price
                if( regress.method=="lp" ) 
                { 
                    min.npar   <- min(sapply(parents.name, # find the minimum history for all parents
                                             function(x){length(pca.lp.parent[[x]][["scores"]][,ipc])}))
                    n.max.hday <- min(min.npar, n.hday[ichild])  # can't squeeze more history!
                    X          <- matrix(NA, nrow=n.max.hday-1, ncol=npar*2+1)
                    X[,1]      <- tail(y[-length(y)], n=(n.max.hday-1)) # lag 1 recent history 
                    for( ipar in 1:npar ) 
                    {
                        thispc <- pca.lp.parent[[parents.name[ipar]]][["scores"]][,ipc]
                        tmpn   <- length(thispc)
                        if( tmpn - n.hday[ichild]<0 )
                        {
                            rWarn(paste("The parent: ", parents.name[ipar], " has less price history",
                                  " than the child: ", nam, " (", tmpn, " vs. ", n.hday[ichild], ")!", sep=""))
                        }
                        thispc           <- tail(thispc, n=n.max.hday)
                        X[,(ipar-1)*2+2] <- thispc[2:n.max.hday]
                        X[,(ipar-1)*2+3] <- thispc[1:(n.max.hday-1)]
                    }
                    yc  <- tail(y, n=(n.max.hday-1))     # the most recent history for the child
                    ## regression - without intercept
                    fit <- lm(yc ~ X - 1)
          
                    ## model selection: determine which parents are important
                    pval.parent <- matrix(coef(summary(fit))[,4][-(1)], ncol=2, byrow=T)
                    ## first term y_{t-1} will always be there, from 2nd on, see which parents
                    ## with p-values < 0.05
                    validparent[[ichild]] <- which(apply(pval.parent, 1, function(x) any(x<0.05)))
                    if( length(validparent[[ichild]])==0 )
                    {
                        rWarn(paste("The PC no. ", ipc," for proposed parent: ", parents.name[ipar],
                                    " is not significant for child: ", nam, "!", sep=""))
                    }
                    ## redo regression with significant parent only
                    this.npar <- length(validparent[[ichild]])
                    X         <- matrix(NA, nrow=n.max.hday-1, ncol=this.npar*2+1)
                    X[,1]     <- tail(y[-length(y)], n=(n.max.hday-1))
                    for( ipar in seq(along=validparent[[ichild]]) ) 
                    {
                        id.parent        <- validparent[[ichild]][ipar] ## parent index
                        thispc           <- pca.lp.parent[[parents.name[id.parent]]][["scores"]][,ipc]
                        thispc           <- tail(thispc, n=n.max.hday)
                        X[,(ipar-1)*2+2] <- thispc[2:n.max.hday]
                        X[,(ipar-1)*2+3] <- thispc[1:(n.max.hday-1)]
                    }
                    ## regression again with valid parents only
                    ## note because y and X are PC's so they are centered around 0.
                    ## So intercept in regression is unnecessary, including it will
                    ## bring extra noise.
                    fit <- lm(yc ~ X - 1)
                }
        
                ## store regression result
                tmpidx                  <-(N-n.max.hday+1):(N-1) 
                tmpcoef                 <- coef(fit)
                tmpcoef[is.na(tmpcoef)] <- 0
               
                ## sometimes there's NA in beta, that's because some curves are EXACTLY the same
                ## as its parent curve (like CHICAGO GDM and CHICAGO PHYS are the same).
                ## I'm coverting NA to 0
                beta[[ichild]]     <- tmpcoef
                
                ## add standard deviation to beta values of ith child curve
                tmpss              <- summary(fit)
                beta.std[[ichild]] <- 0.5*tmpss$coefficients[,2]
                
                ##resid[tmpidx,ichild] <- resid(fit)
                std[ichild]          <- sqrt(mean(resid(fit)^2, na.rm=T))
                allhp[tmpidx,ichild] <- yc      # y cut            ## DO WE NEED allhp ?!
            }
        }     
        # covariance in the residuals:
        # use correlation in residual gives bad result.
        # That's because we regress y_t on y_{t-1} so if children
        # processes are cointegrated, regression is spurious and 
        # residual correlation didn't reflect the truth.
        # We can't use full error correction model because there're
        # many children processes. As a result I'm using correlation
        # in original price - this is kind of cheating because we force
        # the daily residuals for all curves to be very similar. 
        # Since the regression R^2 is high this effect (over-estimation
        # of correlation) is minor. Plus we lost something in rotaion
        # (PCA) so it compensates that. The result looks good.
        #    vv <- cov(resid, use="pairwise.complete.obs")
        if( nchildren==1 )
            vv <- std^2
        else 
        {
            rho             <- cor(allhp, use="pairwise.complete.obs")
            ## sometimes a curve is (piece-wise) flat and calculation of cor failed.
            ## make those cor to be 0.
            rho[is.na(rho)] <- 0        
            vv              <- diag(std) %*% rho %*% diag(std)
        }
        rLog("Done!")
        result.old <- result
        #-------------------------------------------------------------------------------
        rLog("Start simulation of children...") 
        # start simulation based on regression result
        # first generate T*nsim independent random vectors
        # should I rescale the random variables based on different time step???
        # If linear regression R^2 is high, parent lret has high predictive power
        # for children lret, then I shouldn't rescale them (because the difference
        # in log returns should be stationary).
        # However if parent lret has no predictive power, not rescaling will
        # underestimate the variance.
        # I am NOT rescaling it at this time.
        ee      <- eigen(vv)
        eps     <- mymvrnorm(length(FP.options$sim.days)*FP.options$nsims.pkt, rep(0,nchildren), ee)
        ind.pkt <- ((FP.options$packet.no-1)*FP.options$nsims.pkt + 1):
                   min(FP.options$packet.no*FP.options$nsims.pkt, FP.options$nsim)
       
        #initiate random beta matrix using mean and std of beta values
        beta.rand<-vector("list", nchildren)
        for( ichild in 1:nchildren )
        {
            beta.rand[[ichild]] <- array(0, dim=c(FP.options$nsim,length(beta[[ichild]])))
        }
        #simulation of children by regression (ECM)
        # loop on child curves
        for( ichild in 1:nchildren )  
        {
            nam          <- names(pca.lp.children)[ichild]
            rDebug(paste("Child: ", nam, sep=""))
            parents.name <- getParents(nam, pedigree)
            npar         <- length(parents.name)
            rLog("PC#",ipc,"children: ", nam, "parents:", parents.name[validparent[[ichild]]])
            rLog("beta values:", beta[[ichild]])
            rLog("beta std:", beta.std[[ichild]])
            
            ## Create random beta matrix
            #  If you want to add noise to beta:
            beta.rand[[ichild]][,1] <- rnorm(FP.options$nsim, mean=beta[[ichild]][1],sd=beta.std[[ichild]][1])#for Yt-1 of the same child curve
            #  If you don't want to add noise to beta:
            #beta.rand[[ichild]][,1]<- rnorm(FP.options$nsim, mean=beta[[ichild]][1],sd=0)
            #this child has a non-trivial parent pc
            if( length(beta[[ichild]])>1 )
            { 
                for( i in c(2:length(beta[[ichild]])) )
                {
                  #If you want to add noise to beta:
                  beta.rand[[ichild]][,i]=rnorm(FP.options$nsim, mean=beta[[ichild]][i], sd=beta.std[[ichild]][i])
                  #If you don't want to add noise to beta:
                  #beta.rand[[ichild]][,i]=rnorm(FP.options$nsim, mean=beta[[ichild]][i], sd=0)
                }
            }
            colnames(beta.rand[[ichild]])=c(1:dim(beta.rand[[ichild]])[2])
            # has no parent, this shouldn't happen
            if( npar<1 )
            { 
                result[[nam]][,ipc,] <- eps[,ichild]
            }else
            { # has parents
                if( regress.method=="lp" ) 
                {   ## use log price
                    ## reshape epsilon for this child for easier manipulation
                    tmpeps <- matrix(eps[,ichild], nrow=FP.options$nsims.pkt)
                    ## get today's price
                    tmpnn <- dim(pca.lp.children[[nam]][["scores"]])[1]
                    lhp.today <- pca.lp.children[[nam]][["scores"]][tmpnn, ipc]
                 
                    ##simulation for the first day, using beta.rand which has normal distribution by using beta.std  
                    sim.tmp <- lhp.today*beta.rand[[ichild]][,1]  
                    ## loop for valid parents
                    for( ipar in seq(along=validparent[[ichild]]) ) 
                    {
                        pname <- parents.name[validparent[[ichild]][ipar]]
                        n.max.par <- length(pca.lp.parent[[pname]][["scores"]][,ipc])
                        ## x_t
                        sim.tmp <- sim.tmp + beta.rand[[ichild]][,(ipar-1)*2+2]*
                          pc.forward.parent[[pname]][1,ipc,ind.pkt]             
                          
                        ## x_{t-1}
                        sim.tmp <- sim.tmp + beta.rand[[ichild]][, (ipar-1)*2+3]*
                          pca.lp.parent[[pname]][["scores"]][n.max.par,ipc]
                    }
                    ## epsilon
                    sim.tmp <- sim.tmp+ tmpeps[,1]
                    result[[nam]][1,ipc,] <- sim.tmp
                   
                    ## has to loop on days because of AR
                    for( iday in 2:length(FP.options$sim.days) ) 
                    {
                        # y_{t-1}
                        sim.tmp <- result[[nam]][iday-1,ipc,]*beta.rand[[ichild]][,1] ## for the child 
                        # for all parents
                        for( ipar in seq(along=validparent[[ichild]]) ) 
                        { 
                            pname <- parents.name[validparent[[ichild]][ipar]]
                            # x_t
                            sim.tmp <- sim.tmp + beta.rand[[ichild]][,(ipar-1)*2+2]*
                                                 pc.forward.parent[[pname]][iday,ipc,ind.pkt]
                            # x_{t-1}
                            sim.tmp <- sim.tmp + beta.rand[[ichild]][,(ipar-1)*2+3]*
                                                 pc.forward.parent[[pname]][iday-1,ipc,ind.pkt]
                        }
                        # epsilon
                        sim.tmp                  <- sim.tmp + tmpeps[,iday]
                        result[[nam]][iday,ipc,] <- sim.tmp
                    }
                }   # end of if log price        
            }
        }  ## finish simulating all children on this PC
    } ## end loop over the PC's
    names(result) <- names(pca.lp.children)
    rLog("Done")
    
    ## transform back to log price if requested
    if( ret == "lp" ) 
    {
        result <- pc2lp(result, pca.lp.children, FP.options$sim.days)
    }      
    invisible(result)
}

#-------------------------------------------------------------------------------
# Simulate curve groups using sampling from joint distribution. 
# Should work better for curves with jumps like NOX, SO2, ICAP, etc. 
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# a small utility function to find parents for a child curve,
# given pedigree data frame
#-------------------------------------------------------------------------------
getParents <- function(childname, pedigree) 
{
    # make sure pedigree is a matrix
    # I should subset matrix using drop=F but just in case
    if( class(pedigree) != "matrix" )
      pedigree <- matrix(pedigree, nrow=1)
    
    # get this child's parents
    parents.name <- pedigree[pedigree[,1]==childname,3:dim(pedigree)[2]]
    # remove empty string
    parents.name <- parents.name[sapply(parents.name, function(x) nchar(x)>0)]
    # remove NA
    parents.name <- parents.name[!is.na(parents.name)]
    unique(parents.name)
}

#-------------------------------------------------------------------------------
# a wrapper function to take a pedigree and simulate.
# 
# Note last column in curves is a flag for requested or not
# because simulation for some curves (usually parents)
# are not requested by user and those are only
# used as mid steps so I don't need to save them.
# Note cbind make last column string of "TRUE"/"FALSE",
# instead of boolean variable,  but it doesn't matter.
# I'M SAVING EVERYTHING NOW.
#-------------------------------------------------------------------------------
sim.forward <- function(curves, FP.options, regress.method="lp") 
{
    ## init data holder
    lhp <- lhp.pca   <- NULL
    omitted.curve    <- NULL ## for curves being omitted
    identical.curves <- curves[which(!is.na(curves[,"Identical"])),, drop=F]
    # simulate only these curves 
    curves           <- curves[which(is.na(curves[,"Identical"])),, drop=F]  
    
    ## first load in historical price data for children and parent curves
    children   <- curves[,1] # children curves
    ## parent curves - the simulation results for parents should already be in place
    nn         <- max(grep("PARENT", colnames(curves))) # to the last column labeled parents
    parents    <- unique(as.vector(curves[,3:nn]))
    parents    <- parents[!is.na(parents)]
    curvenames <- c(parents,children) ## process parents before children
  
    rLog("Load historical prices and do PCA ...")
  
    loptions            <- NULL
    loptions$asOfDate   <- FP.options$asOfDate
    loptions$calc$hdays <- seq( FP.options$asOfDate-FP.options$N.hist,
                                FP.options$asOfDate, 
                                by="day")
      
    for( icurve in 1:length(curvenames) )   # loop on curves 
    {          
        thiscurve <- as.character(curvenames[icurve])
        QQ        <- data.frame(curve.name=thiscurve, contract.dt=FP.options$contract.dts)
        
        #check if the start of historical pricing dates are specified in allcurves.xls
        #Note: for parent curves, "curves" may not have a row for them. But it doesn't matter because 
        #parent curves should have already been simulated. They won't be re-simulated
        loptions.thiscurve <- loptions
        hist.dt            <- as.Date(curves[which(curves[,1]== thiscurve), "HistStartDate"])
        if( !is.na(hist.dt) && length(hist.dt)>0 )
        {
            loptions.thiscurve$calc$hdays <- loptions$calc$hdays[loptions$calc$hdays>=hist.dt]
            rLog( "The starting hist date for curve", thiscurve,
                  "is changed to", as.character(hist.dt) )
        } 
        # get prices from the hash, hPP.env is global, set in 
        hp <- try(get.prices.from.R(QQ, loptions.thiscurve, hPP.env))  
    
        ## report on missing data 
        if( class(hp) == "try-error" )
        {
            ## shouldn't come here, but some time do
            ## there's no historical price data for this curve
            warnings(paste(thiscurve, "not found in the filehash."))
            omitted.curve <- c(omitted.curve, thiscurve)
            next
        }
        thisprice           <- hp[, -(1:2), drop=F]            ## all prices for this curve
        rownames(thisprice) <- as.character(hp[,2])
    
        # A clunky way to treat expiration in the nearby contract.
        # Will happen to the exchange products at the end of the month. 
        if (is.na(thisprice[1,ncol(thisprice)]))
        {
            thisprice[1:(FP.options$D-1),] <- thisprice[2:FP.options$D,]
        } 
    
        ## if prices for this curve are all flat, generate data and skip it
        dd               <- apply(thisprice, 1, diff)
        ## sometimes there're numeric problems, correct them
        dd[abs(dd)<1e-5] <- 0
        if( all(dd==0, na.rm=T) ) 
        {
            rWarn("Prices for ", thiscurve, " are flat, skip simulation")
            omitted.curve <- c(omitted.curve, thiscurve)  # we're done with this one!
            ## make all sumulated data to be this price
            res           <- array(rep(rep(thisprice[,ncol(thisprice)], 
                                           each=length(FP.options$sim.days)),
                                       FP.options$nsims.pkt), 
                                   dim=c(length(FP.options$sim.days), 
                                         dim(thisprice)[1],
                                         FP.options$nsims.pkt))
            dimnames(res) <- list( as.character(FP.options$sim.days), 
                                   rownames(thisprice),
                                   paste("sim", 1:FP.options$nsims.pkt))
            ## save
            fh.dir <- substr(FP.options$targetdata.dir, 1, nchar(FP.options$targetdata.dir)-1)
            db     <- dbInit(fh.dir)   #  dbInit does not like dir names ending in /
            #  .sPP <- db2env(db)   # will there be a problem if we cache it?
            res    <- try(dbInsert(db, tolower(thiscurve), res))
            if( class(res)=="try-error" )
            {
                rLog(paste("Could not insert curve:", thiscurve," into sim filehash.\n",sep=""))
                next
            }
        }
            
        ## convert 0 or negative price to NA
        tmpidx <- apply(thisprice, 1, function(x) any(x<=0, na.rm=T))
        if( sum(tmpidx)>0 )  
        {
            warning( thiscurve, " has price <= 0 for following months:",
                     paste(rownames(thisprice)[tmpidx], collapse=", "), "\n")
            thisprice[thisprice<=0] <- NA
        }
        ## take log
        lhp[[thiscurve]] <- as.matrix(log(thisprice))
    
        ## PCA for this curve --                               
        ## I could have saved PCA result but doing PCA is fast (0.01s each) so didn't bother
        ## clean data before doing PCA -
        
        ## remove columns with NA's. This is the case for newer curves
        ## What if there's an NA in middle? We should clean data before this          
        tmpidx <- apply(lhp[[thiscurve]], 2, function(x) all(!is.na(x)))
        if (!all(tmpidx)) ## if there's NA's, subset
        { 
             lhp[[thiscurve]] <- lhp[[thiscurve]][,tmpidx, drop=F]
             rWarn(paste("Some historical days for ", thiscurve, " have NA prices.", sep=""))
        }
       
        ## It be trouble if after subsetting, there're more contract months
        ## then pricing dates, then PCA will fail. 
        dd <- dim(lhp[[thiscurve]])
        if( dd[1]<dd[2] )
        { 
            lhp.pca[[thiscurve]] <- princomp(t(lhp[[thiscurve]]))
            stdev                <- lhp.pca[[thiscurve]]$sdev
            coverage             <- cumsum(stdev^2)/sum(stdev^2)
            rLog(paste(thiscurve, " needs ", min(which(coverage>0.95)), " PCs to cover more than 95%", sep=""))
        }else
        {
            ## more contract months than pricing date, this happened for new curves.
            ## Inherit its parent(if any) PCA result
            thisparent <- curves[curves[,1]==thiscurve,3]
            if (is.na(thisparent))
            {
                ## this curve has no parent, maybe should get data from buddy curve,
                ## I'll skip simulating it for now.
                rLog("Not enough data for simulating ", thiscurve, ", skip it")
                omitted.curve <- c(omitted.curve, thiscurve)
            }else
            {#use the parents PCA but keeps the original center of the child curve
                lhp.center<-apply(t(lhp[[thiscurve]]), 2, mean)
                #note, this still has problem of disconnect in simulated price vs hist. 
                #will work this problem later (Jing:08-04-02)
                lhp.pca[[thiscurve]] <- lhp.pca[[thisparent]]
                lhp.pca[[thiscurve]][["center"]]<-lhp.center
                rLog("*********The pca of:", thisparent," is used for the pca of", thiscurve, "*********")
            }
        }
    } 
    rLog("Done.")
    
    rLog("Start simulation ...")
    ## Get a list of children to be simulated
    ## Note we could have omitted something due to insufficient data
    children <- setdiff(children, omitted.curve)
    
    tmp <- NULL
    if( (length(parents)==0 | is.null(parents))&(!is.null(children)) )    # if there are no parents
    {      
        rLog("  simulate parents")                                           #if there are parents(ref curves), they are already simulated
        parents  <- children
        children <- NULL
        tmp <- sim.forward.parents(lhp.pca[parents], FP.options, ret="pc")  # log-prices for parents
        for( ipar in 1:length(parents) )     # decided to loop to avoid duplicated memory
        {      
            aux <- tmp[parents[ipar]]           # you need tmp below...
            aux <- pc2lp(aux, lhp.pca[parents[ipar]], FP.options$sim.days)
            #browser()
            #set price caps to FRT and FRC markets, according to prices on AOD (or the last valid hist pricing date)
           
            thismkt <- toupper(strsplit(parents[ipar], " ")[[1]][2])
            add.max <- c(1:48)*1.1/35+1
            add.max <- ifelse(add.max>2.1, 2.1, add.max)
            if( thismkt == "FRC" | thismkt == "FRT" )
            {
                lhp.max <- lhp[[parents[ipar]]][,dim(lhp[[parents[ipar]]])[2]]+log(add.max)
                lhp.min <- lhp[[parents[ipar]]][,dim(lhp[[parents[ipar]]])[2]]-log(add.max)
                for( i in 1:dim(aux[[parents[ipar]]])[2] )
                {
                    aux.sub <- aux[[parents[ipar]]][,i,]
                    idx.max <- which(aux.sub > lhp.max[i])
                    idx.min <- which(aux.sub < lhp.min[i])
                    if(length(idx.max)>0) aux.sub[idx.max] <- lhp.max[i]
                    if(length(idx.min)>0) aux.sub[idx.min] <- lhp.min[i]
                    aux[[parents[ipar]]][,i,] <- aux.sub
                } #this for loop takes about 2 seconds
            }
            add.max <- c(1:48)*1.1/35+1.3
            add.max <- ifelse(add.max>2.4, 2.4, add.max)
            if( thismkt == "IFO" | thismkt == "HO" )
            {
                lhp.max <- lhp[[parents[ipar]]][,dim(lhp[[parents[ipar]]])[2]]+log(add.max)
                lhp.min <- lhp[[parents[ipar]]][,dim(lhp[[parents[ipar]]])[2]]-log(add.max)
                for( i in 1:dim(aux[[parents[ipar]]])[2] )
                {
                    aux.sub <- aux[[parents[ipar]]][,i,]
                    idx.max <- which(aux.sub > lhp.max[i])
                    idx.min <- which(aux.sub < lhp.min[i])
                    if(length(idx.max)>0) aux.sub[idx.max] <- lhp.max[i]
                    if(length(idx.min)>0) aux.sub[idx.min] <- lhp.min[i]
                    aux[[parents[ipar]]][,i,] <- aux.sub
                } #this for loop takes about 2 seconds
            }
            write.sim.fh(aux, FP.options)
        }
        rLog(paste("  wrote prices for ", parents, " to results folder.", sep=""))
    }
    if( length(children)!=0 )      # if you have children 
    {            
        if( length(parents)==0 )
        {
            rError(paste(children, "have no parents!"))
        }
        lp.pc.forward.parents <- NULL
        for( iparent in 1:length(parents) ) 
        {
            nam      <- parents[iparent]
            ## load forward PC for parents then simulate children 
            filename <- paste(FP.options$swapdata.dir, nam, ".forward.PC.RData", sep="")
            load(filename)
            lp.pc.forward.parents[nam] <- forward.pc
        }
        ## simulate - remove the last column of curves (a flag) to pass in
        rLog("  simulate children")
        tmp <- sim.forward.children( lhp.pca[children], 
                                     lhp.pca[parents],
                                     lp.pc.forward.parents[parents], 
                                     curves[,1:nn],                     # returns principal 
                                     FP.options, 
                                     ret="pc", 
                                     regress.method=regress.method)     # components for kids
        for( ichild in 1:length(children) )    # decided to loop to avoid duplicated memory
        {   
            aux <- tmp[children[ichild]]        # you need tmp below...
            aux <- pc2lp(aux, lhp.pca[children[ichild]], FP.options$sim.days)
            
            #set price caps according to prices on AOD (or the last valid hist pricing date)
            thismkt <- toupper(strsplit(children[ichild], " ")[[1]][2])
            add.max <- c(1:48)*1.1/35+1
            add.max <- ifelse(add.max>2.1, 2.1, add.max)
            if( thismkt == "FRC" | thismkt == "FRT" )
            {
                #browser()
                lhp.max <- lhp[[children[ichild]]][,dim(lhp[[children[ichild]]])[2]]+log(add.max)
                lhp.min <- lhp[[children[ichild]]][,dim(lhp[[children[ichild]]])[2]]-log(add.max)
                for( i in 1:dim(aux[[children[ichild]]])[2] )
                {
                    aux.sub <- aux[[children[ichild]]][,i,]
                    idx.max <- which(aux.sub >lhp.max[i])
                    idx.min <- which(aux.sub < lhp.min[i])
                    if(length(idx.max)>0) aux.sub[idx.max] <- lhp.max[i]
                    if(length(idx.min)>0) aux.sub[idx.min] <- lhp.min[i]
                    aux[[children[ichild]]][,i,] <- aux.sub
                }
            }
            add.max <- c(1:48)*1.1/35+1.3
            add.max <- ifelse(add.max>2.4, 2.4, add.max)
            if( thismkt == "IFO" | thismkt == "HO" )
            {
                #browser()
                lhp.max <- lhp[[children[ichild]]][,dim(lhp[[children[ichild]]])[2]]+log(add.max)
                lhp.min <- lhp[[children[ichild]]][,dim(lhp[[children[ichild]]])[2]]-log(add.max)
                for( i in 1:dim(aux[[children[ichild]]])[2] )
                {
                    aux.sub <- aux[[children[ichild]]][,i,]
                    idx.max <- which(aux.sub >lhp.max[i])
                    idx.min <- which(aux.sub < lhp.min[i])
                    if(length(idx.max)>0) aux.sub[idx.max] <- lhp.max[i]
                    if(length(idx.min)>0) aux.sub[idx.min] <- lhp.min[i]
                    aux[[children[ichild]]][,i,] <- aux.sub
                }
            }
            write.sim.fh(aux, FP.options)
        }
        rLog(paste("  wrote prices for ", children, " to results folder.", sep=""))
    }
    rLog("Done.")
    for( iii in seq(length.out=length(tmp)) )
    {    
        rLog("Start saving the principal components for reference curves...")
        nam    <- names(tmp)[iii]
        tmpidx <- nam==curves[,1] ## tmpidx should be of length 1
        ## if this is some kind of reference curve, save forward PC
        if( !is.na(curves[tmpidx,2]) ) 
        { ## this is some reference
            if( FP.options$no.packets > 1 )
            { 
                filename <- paste(FP.options$swapdata.dir, nam, ".forward.PC.pkt",
                                  FP.options$packet.no, ".RData", sep="")
            }else
            {
                filename <- paste(FP.options$swapdata.dir, nam, ".forward.PC.RData", sep="")
            }
            forward.pc <- tmp[iii]
            rLog(paste("  saving", nam, ", PC."))
            save(forward.pc, file=filename) ## save as a list to keep the name
        }
        rLog("Done.") 
    }
}

#-------------------------------------------------------------------------------
# The the top level function to simulate a list of curves
#-------------------------------------------------------------------------------
sim.all <- function(curvenames, FP.options) 
{
    rLog("Connecting to the historical price filehash ... ")
    hPP.env <<- .getFileHashEnv("//nas-msw-07/rmgapp/Prices/Historical")
    rLog("Done")
    
    allcurves <- get.curve.pedigree(curvenames, FP.options)
    
    # Start simulation assuming data have been preprocessed
    # LoadCurve should be called before this function
  
    # Get markets of all curves
    allmkt <- sapply(allcurves[,1], function(x) strsplit(x, " ")[[1]][2])
     
    # step 1. Simulate fuel commodity reference curves, if requested
    simulateCurveGroup( CG_FUEL_REFERENCE, allcurves, allmkt, FP.options )
  
    # step 2: Simulate regional refernce curves for NG, and the rest of fuel curves
    # NG REGIONAL REF
    simulateCurveGroup( CG_NG_REGIONAL_REF, allcurves, allmkt, FP.options, regress.method="lp" )
    # WTI
    simulateCurveGroup( CG_ALL_WTI, allcurves, allmkt, FP.options )
    # FCO
    simulateCurveGroup( CG_ALL_FCO, allcurves, allmkt, FP.options )
    # COL
    simulateCurveGroup( CG_ALL_COL, allcurves, allmkt, FP.options )
      
    # step 3: Rest of the NG curves
    simulateCurveGroup( CG_ALL_NG, allcurves, allmkt, FP.options, regress.method="lp" )
      
    # step 4: Electricity reference curves
    simulateCurveGroup( CG_PWR_REF, allcurves, allmkt, FP.options, regress.method="lp" )
                                                                   
    # step 5: Rest of the electricity curves altogether
    simulateCurveGroup( CG_ALL_PWR, allcurves, allmkt, FP.options, regress.method="lp" )
   
    # step 6: All other markets, frieght, emission, etc.
    idx.elec  <- grep("PW", allmkt)
    elec.mkt  <- unique(allmkt[idx.elec])
    other.mkt <- setdiff(allmkt, c(elec.mkt, "NG", "COL","FCO","WTI"))
    for( iother in  seq(along=other.mkt) ) 
    {
        curveIndexes <- which( allmkt==other.mkt[iother] )
        if( length(curveIndexes)>0 )
        { 
            groupName <- other.mkt[iother]
            simulateCurves( groupName, curveIndexes, allcurves, FP.options )
        }
    }
}

#-------------------------------------------------------------------------------
# Read in the group name and extract the curve indexes for that group
simulateCurveGroup <- function( CURVE_GROUP, allcurves, allmkt, 
                                FP.options, regress.method="lp" )
{
    curveIndexes <- getCurveIndexes( CURVE_GROUP, allcurves, allmkt )
    if( length(curveIndexes)>0 )
    {
        simulateCurves( CURVE_GROUP, curveIndexes, allcurves, 
                        FP.options, regress.method )
    }
}

#-------------------------------------------------------------------------------
simulateCurves <- function( groupName, curveIndexes, allcurves, 
                            FP.options, regress.method="lp" )
{
    # Get the file names for all the requested curves
    filenames <- paste( FP.options$targetdata.dir,
                        tolower(allcurves[curveIndexes,1]), sep="" )
    # If any of the files don't exist, resimulate
    if( any(!file.exists(filenames)) )
    {
        FP.options$no.packets <- as.numeric(Sys.getenv("FP_NO_PACKETS"))
        rLog("No of packets is:", FP.options$no.packets)
        if( is.na(FP.options$no.packets) )
        { 
            FP.options$no.packets <- 1
        }
        FP.options$packet.no <- as.numeric(Sys.getenv("FP_PACKET_NO"))
        rLog("Packet number is:", FP.options$packet.no)
        if( is.na(FP.options$packet.no) )
        {
            FP.options$packet.no <- 1
        }
        # FP.options$no.packets <- 2  # make sure it splits nsim exactly!
        # FP.options$packet.no  <- 2
        FP.options$nsims.pkt  <- ceiling(FP.options$nsim/FP.options$no.packets)
        rLog("Simulating ", groupName, "... \n", sep="")
        sim.forward( allcurves[curveIndexes,,drop=F], 
                     FP.options, 
                     regress.method=regress.method )
    }
}

#-------------------------------------------------------------------------------
# Based on what group should be extracted, determine the indexes in the 
# allcurves dataframe and return that set.
getCurveIndexes <- function( CURVE_GROUP, allcurves, allmkt )
{
    if( CURVE_GROUP == CG_FUEL_REFERENCE )
    {
        tmpidx <- which( ( allcurves[,2]=="COMMOD REF" ) &
                         ( allmkt=="NG" | allmkt=="FCO" | allmkt=="WTI" | 
                           allmkt=="COL" ) )
      
    }else if( CURVE_GROUP == CG_NG_REGIONAL_REF )
    {
        tmpidx <- which((allcurves[,2]=="REGIONAL REF") & (allmkt=="NG"))
      
    }else if( CURVE_GROUP == CG_ALL_WTI )
    {
        tmpidx <- which(is.na(allcurves[,2]) & (allmkt=="WTI"))
    
    }else if( CURVE_GROUP == CG_ALL_FCO )
    {
        tmpidx <- which(is.na(allcurves[,2]) & (allmkt=="FCO"))
    
    }else if( CURVE_GROUP == CG_ALL_COL )
    {
        tmpidx <- which(is.na(allcurves[,2]) & (allmkt=="COL"))
    
    }else if( CURVE_GROUP == CG_ALL_NG )
    {
        tmpidx <- which(is.na(allcurves[,2]) & (allmkt=="NG"))
    
    }else if( CURVE_GROUP == CG_PWR_REF )
    {
        idx.elec       <- grep("PW", allmkt)
        flag           <- rep(FALSE, length(allmkt))
        flag[idx.elec] <- TRUE
        tmpidx         <- which( (allcurves[,2]=="COMMOD REF") & flag )
    
    }else if( CURVE_GROUP == CG_ALL_PWR )
    {
        idx.elec       <- grep("PW", allmkt)
        flag           <- rep(FALSE, length(allmkt))
        flag[idx.elec] <- TRUE
        tmpidx         <- which( is.na(allcurves[,2]) & flag )
    
    }else
    {
        stop("Unknown curve group: ", CURVE_GROUP)
    }
    
    return(tmpidx)
} 


#############################################################################
#
#    OLD STUFF BELOW THIS LINE
#
#############################################################################
########################################################
## function to pre-process data for all curves
## This function only need to be run once 
########################################################
## load.AllCurves <- function(curvenames, FP.options) {
##   ## get pedigree for all curves
##   allcurves <- get.curve.pedigree(curvenames, FP.options=FP.options)
##   curves <- unique(as.vector(allcurves[,c(1,3,4,5)]))
##   curves <- curves[!is.na(curves)]
  
##   ## pre-process data
##   loadCurve(curves, FP.options)
## }


###########################################################
## This is the defunct version of sim.corr.ou.
## I'll leave it for now.
###########################################################
## sim.corr.ou.defunct <- function(ou.param, rho, x0, nsim=100, forward.time, K=20) {
##   ## skip data checking

##   ## init result
##   D <- length(as.numeric(x0))
##   K <- min(K, D)
##   lp.for <- array(NA, dim=c(length(forward.time), D, nsim))
##   ## number of business days between forward.time
##   n.bday <- diff.bday(forward.time)

##   ## for different time step, pre-compute eigen value decomposition
##   evd <- NULL
##   nbday.unique <- unique(n.bday)
##   for(i in 1:length(nbday.unique)) {
##     ## build covariance matrix from sigma and correlation matrix
##     sd.x <- sqrt((1-exp(-2*ou.param[1,]*nbday.unique[i]))*ou.param[2,]^2/2/ou.param[1,])
##     if(K==1) ## only one input curve 
##       Sigma <- sd.x^2 * rho
##     else
##       Sigma <- diag(sd.x) %*% rho %*% diag(sd.x)
##     ## find new basis matrix W so that Y=XW are uncorrelated
##     ee <- eigen(Sigma)
##     W <- ee$vectors ## rotation matrix
##     ev <- ee$values;
##     ## correct for numeric problem.
##     ## Sometimes eigenvalues are very small negative numbers, make them 0.
##     ev[ev<0] <- 0
##     ev[abs(ev)<1e-10] <- 0
##     std <- sqrt(ev)[1:K]
##     invW <- t(W)
##     evd[[i]] <- list(W=W, invW=invW, std=std)
##     names(evd)[i] <- nbday.unique[i]
##   }
  
##   ## set today's price 
##   lp.for[1,,] <- rep(x0, nsim)
  
##   for(it in 2:length(forward.time)) { ## loop for future date
##     ## evd (eigen value decomposition) result for this time step
##     thisevd <- evd[[as.character(n.bday[it-1])]]
##     ## f1 and f2 depends on time step
##     ## f1 and f2 are vectors, each element is for a curve
##     f1 <- exp(-ou.param[1,]*n.bday[it-1])
##     f2 <- ou.param[3,]*(1-f1)
##     ## calculate mean of distribution for today
##     if(K==1)
##       mu <- lp.for[it-1,,]*f1+f2
##     else
##       mu <- t(apply(lp.for[it-1,,], 2, function(x) x*f1+f2))  %*% thisevd$W[,1:K,drop=F]
##     ## 
##     ## generate r.v Yt=Xt*W
##     delta <- sapply(std, function(x) rnorm(nsim, sd=x) )
##     Y.t <- mu + delta
##     ## transform back to X
##     lp.for[it,,] <- t(Y.t %*% thisevd$invW[1:K,])
##   }
##   ## disgard the first row of lp.for (today's price)
##   lp.for <- lp.for[-1,,]
##   ## return
##   invisible(lp.for)
## }

##  apply(aux[[1]][1,,], 1, summary)
##  hist.pc <- tail(lhp.pca[children[ichild]][[1]][["scores"]],1)
## (hist.pc[,1:5])
      
##  W <- t(lhp.pca[[children[ichild]]][["loadings"]])
##  hist.pc %*%  W + lhp.pca[children[ichild]][[1]][["center"]]     

##  tail(t(lhp[[children[ichild]]]), 1)      
      
##  forward.pc <- aux
##  pca.lp <- lhp.pca[children[ichild]]
##  sim.days <- FP.options$sim.days
##  result <- NULL
##  ic <- 1

##  res <- t(forward.pc[[cc]][1,,]) %*% W
##  sres <- apply(res, 2, summary)
##  matplot(t(sres), type="l")
##  abline(h=0) 