#######################################################
##
## Utility functions for the network price simulation
##
## Written by Hao Wu, July 2007
##
#######################################################


#################################################
## my function to genearte normal RV using rstream
## to ensure independent simulation on clusters.
## I'll skip checking for package to save
## a tiny little bit of time.
#################################################
myrnorm <- function(n, mu, sd) {
  ss <- new("rstream.mrg32k3a")
  ## simulation
  xx <- rstream.sample(ss, n)
  ## invert cdf to get standard normal
  nx <- qnorm(xx)
  ## scale and shift then return
  return( nx*sd+mu )
}

#################################################
## Function to generate MVN random vectors
## This function will take eigen result so
## it doesn't have to compute it over and over.
## Notations are:
## X ~ MVN(u, Sigma2),
## Y=XW, Y ~ MVN(uy, Tau), here
## uy = u %*% W
## Tau=W %*% Sigma2 %*% t(W) is diagonal
## Input is eigen result object
#################################################
mymvrnorm <- function(n, u.y, eig) {
  require(rstream)
  ss <- new("rstream.mrg32k3a")

   ## skip data checking here
  Tau <- sqrt(pmax(eig[[1]],0))
  W <- eig[[2]]
  ## generate uncorelated Y
  Y <- apply(cbind(u.y, Tau), 1, function(x) myrnorm(n, x[1], x[2]))
  ## transform back
  ## note rotation matrix W satisfied solve(W) = t(W)
  X <- Y %*% t(W)
}


##########################################################
## function to simulate OU process
## Allow parameters to be noisy, normally distributed
##########################################################
sim.OU <- function(params, x0, nsim, dt) {
  gamma <- params$coeff[1]
  sigma <- params$coeff[2]
  theta <- params$coeff[3]

  ## init result
  nt <- length(dt) ## number of forward time point
  x <- array(0,c(nsim, nt+1))
  x[,1] <- x0
  # generate random deviates
  dW <- matrix(myrnorm(nt*nsim, 0, sigma), ncol=nt) 
  
  # simulate parameters
  if (!("sd" %in% names(params))){
    browser()
  }
  
  if(abs(params$sd[1])<=3*abs(gamma)){
    gamma <- rnorm(nsim, mean=gamma, sd=params$sd[1])
  }else{
     rLog("*******From sim.OU: the std dev of rev rate is too big, reset to 3abs(gamma)*********")
     rLog("gamma= ", gamma, " std= ", params$sd[1]) 
     gamma <- rnorm(nsim, mean=gamma, sd=3*abs(gamma))

  }
  #gamma <- rnorm(nsim, mean=gamma, sd=0)
  ind.0 <- which(gamma < 0)
  if (length(ind.0)>0){gamma[ind.0] <- 0}   # don't allow for explosive simulations!
  
  if(abs(params$sd[3])<=3*abs(theta)){
    theta <- rnorm(nsim, mean=theta, sd=params$sd[3])
  }else{
    rLog("From sim.OU: the std dev of this PC is too big, reset to 3abs(theta)!********")
    rLog("theta= ", theta, "std= ", params$sd[3])
    theta <- rnorm(nsim, mean=theta, sd=3*abs(theta))
  }
  #theta <- rnorm(nsim, mean=theta, sd=0)
  
  ## calculate f1 and f2 - note because length of time points are different
  ## we have several values
  dt.unique <- sort(unique(dt))
  f1 <- f2 <- array(NA, c(nsim, length(dt.unique)))
  for(i in 1:length(dt.unique)) {
    f1[,i] <- exp(-gamma*dt.unique[i])
    f2[,i] <- sqrt((1-f1[,i]^2)/(2*gamma))
  }
  colnames(f1) <- colnames(f2) <- dt.unique
  
  # fix gamma degeneracy
  if (length(ind.0)>0){f2[ind.0, ] <- rep(sqrt(dt.unique), each=length(ind.0))} 
  
  ## simulate
  for (i in 1:nt){
    step <- as.character(dt[i])
    x[,i+1] <- theta + f1[,step]*(x[,i]-theta) + f2[,step]*dW[,i]
  }
  return(x[,-1])   # the simulated paths, with first column (today's price) removed
}


############################################################
## Function to write simulation results to a price filehash.
## Prices need to be exponentiated before saving!
############################################################
write.sim.fh <- function(simresult, FP.options) {

  fh.dir <- substr(FP.options$targetdata.dir, 1, nchar(FP.options$targetdata.dir)-1)
  db  <- dbInit(fh.dir)   #  dbInit does not like dir names ending in /
  .sPP <- db2env(db)
  uCurves.fh <- ls(.sPP)

  ## make file name
  root <- FP.options$targetdata.dir ## root directory for data file, must be ended with "/"
  nam <- names(simresult)
  ## make index for lower triangle of the simulated price
  ## those will be set to NA
  ## Note that dimension of all simulation should be the same
  pricing.dt <- as.Date(dimnames(simresult[[1]])[[1]])
  # we're changing all the pricing dates to the first of the month
  # so that when it falls on a day other than the first, we don't substitute
  # it with NA below.
  pricing.dt = as.Date(format(pricing.dt, "%Y-%m-01"))
  npd <- length(pricing.dt)
  contract.dt <- as.Date(dimnames(simresult[[1]])[[2]])
  ncd <- length(contract.dt)
  
  ## index for entries where pricing date is after contract date
  idx.after <- matrix(rep(pricing.dt,ncd), ncol=ncd) >
    matrix(rep(contract.dt,npd), ncol=ncd, byrow=T)
  nsim <- dim(simresult[[1]])[3]
  ## indexing is trouble so I have to make a 3d array index.
  allidx <- array(rep(idx.after, nsim), dim=c(npd, ncd, nsim))

  for(i in 1:length(nam)) {
    ## set the simulated price to NA when pricing date is over contract month
    res <- simresult[[nam[i]]]
    res[allidx] <- NA
    ## exponentiate and save to a file
    res <- exp(res)

    if (FP.options$no.packets > 1){ 
       cname <- paste(nam[i], ".pkt", FP.options$packet.no, sep="")
    } else {
       cname <- nam[i]
    }
    res <- try(dbInsert(db, tolower(cname), res))
    if (class(res)=="try-error"){
      rLog(paste("Could not insert curve:",cname," into sim filehash.\n",sep=""))
      next
    }
   }
}


############################################################
## Aggregate packets  
## 
############################################################
aggregate.packets <- function(cnames, FP.options)
{
  # aggregate the price sims for parents (if split)
  fh.dir <- substr(FP.options$targetdata.dir, 1, nchar(FP.options$targetdata.dir)-1)
  db  <- dbInit(fh.dir)   #  dbInit does not like dir names ending in /
#  if (!exists(".sPP") || !is.environment(.sPP)){  # don't cache here, maybe in PE
    .sPP <<- db2env(db)
  curves.fh <- ls(.sPP)
  
  ind <- grep("\\.pkt", curves.fh)
  if (length(ind)==0){return()}        #  NOTHING TO DO!  You should not get here       
  curves.fh <- curves.fh[ind]          # these curves need to be aggregated
  uCurves <- unique(sapply(curves.fh, function(x){strsplit(x, "\\.pkt")[[1]][1]}))
  uCurves <- na.omit(uCurves[which(uCurves %in% tolower(cnames) )])
  for (ind.curve in seq_along(uCurves)){
    curve <- uCurves[ind.curve]
    rLog(paste("Aggregate prices for", curve))
    res <- array(NA, dim=c(length(FP.options$sim.days),
                   length(FP.options$contract.dts), FP.options$nsim))
    dimnames(res) <- list(as.character(FP.options$sim.days),
      as.character(FP.options$contract.dts), paste("sim", 1:FP.options$nsim))
    ind.pkt <- grep(paste(curve, "\\.pkt", sep=""), curves.fh)  # in case some 
    no.packets <- length(ind.pkt)                               # packet job fails ...
    if (no.packets == 0){next}          # nothing to do
    nsims.pkt  <- ceiling(FP.options$nsim/no.packets)   
    for (p in 1:no.packets){            # loop over the available packets 
      pkt <- as.numeric(strsplit(curves.fh[ind.pkt[p]], "\\.pkt")[[1]][2])
      ind <- ((pkt-1)*nsims.pkt + 1):(pkt*nsims.pkt)
      res[,,ind] <- .sPP[[curves.fh[ind.pkt[p]]]]
      dbDelete(db, curves.fh[ind.pkt[p]])
    }
    dbInsert(db, curve, res)    # insert the complete curve
    rLog("Done.")
  }
  
  # aggregate forward pc's
  files <- list.files(FP.options$swapdata.dir)
  ind <- grep("\\.pkt", files)
  uCurves <- unique(sapply(files[ind], function(x){strsplit(x, "\\.pkt")[[1]][1]}))
  for (curve in uCurves){
    rLog(paste("Aggregate PC's for", curve))
    ind.pkt <- grep(curve, curves.fh)
    res <- array(NA, dim=c(length(FP.options$sim.days),
                 FP.options$Kpc.month, FP.options$nsim))
    for (p in 1:length(ind.pkt)){    
      filename <- paste(FP.options$swapdata.dir, curve, ".pkt", p, ".RData", sep="")
      load(filename)
      ind <- ((p-1)*FP.options$nsims.pkt + 1):(p*FP.options$nsims.pkt)      
      res[,,ind] <- forward.pc[[1]]
      unlink(filename)
    }
    forward.pc <- list(res)
    names(forward.pc) <- curve
    filename <- paste(FP.options$swapdata.dir, curve, ".RData", sep="")    
    save(forward.pc, file=filename) ## save as a list to keep the name
    rLog("Done")
  }
  rLog("Done aggregation of packets.")
}

############################################################
## copy identical curves
## 
############################################################
copy.Identicals <- function(allcurves, FP.options)
{  
  # check if there are any Identical curves that need to get copied
  uCurves <- sort(unique(allcurves$Identical))
  for (ind.curve in seq_along(uCurves)){
    PP <- subset(allcurves, Identical==uCurves[ind.curve])
    for (ind in 1:nrow(PP)){
      rLog(paste("Copying ", PP[ind,"Identical"], "to", PP[ind,"SecDbCurve"]))
      file.to   <- paste(FP.options$targetdata.dir,
                         tolower(PP[ind,"SecDbCurve"]), sep="")
      file.from <- paste(FP.options$targetdata.dir,
                         tolower(PP[ind,"Identical"]), sep="")
      if (!file.copy(file.from, file.to, overwrite=TRUE)){
         rError(paste("Failed to copy ", PP[ind,"SecDbCurve"], "!"))
      }
    }
  } 
} 

############################################################
## Function to calculate simulation step size given 
## a vector of dates. I need to figure out how many
## business days are in between.
##
## All input days must be business days.
## I'll skip checking for that
############################################################
diff.bday <- function(days) {
  source("H:/user/R/RMG/Utilities/Packages/load.packages.R")
  load.packages( "SecDb" )
  calendar <- "CPS-BAL" ## use the same calendar? UK has different holiday schedules??
  skipWeekends <- FALSE
  
  nn <- length(days) ## I assume nn>=2
  result <- rep(NA, nn-1)
  for(i in 2:nn) {
    days.inbetween <- seq(days[i-1], days[i], by="day")
    ldays <- lapply(as.list(as.character(days.inbetween)), as.Date)   # can we avoid this step??
    bday.flag <- unlist(secdb.invoke("AAD: IsHoliday", "IsHoliday", ldays, calendar, skipWeekends))
    result[i-1] <- sum(bday.flag==0)-1
  }
  return(result)
}


  
##################################################
## function to get pedigree data frame for some 
## given curves. If curves are not given, use all
## curves in portfolio
##################################################
get.curve.pedigree <- function(curvenames, FP.options) {
  require(RODBC)
  require(SecDb)
  ## take all curves if not given
  if(missing(curvenames)) {
    ## get positions
    MarketData <- options <- NULL
    MarketData$asOfDate <- FP.options$asOfDate
    source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
    options <- overnightUtils.loadOvernightParameters(MarketData$asOfDate, 
                                                      connectToDB=FALSE)
    file <- paste(options$save$datastore.dir, "Positions/forc.positions.",
                  MarketData$asOfDate, ".RData", sep="")
    if (file.exists(file))
    {
      load(file)
    } else {
      #Try the previous business day, fail otherwise.
      MarketData$asOfDate = secdb.dateRuleApply(MarketData$asOfDate, "-1b")
      file <- paste(options$save$datastore.dir, "Positions/forc.positions.",
        MarketData$asOfDate, ".RData", sep="")
      load(file)
    }
    QQ <- unique(QQ.all[, c("CURVE_NAME", "CONTRACT_MONTH")])
    names(QQ) <- c("curve.name", "contract.dt")
    QQ$contract.dt <- as.Date(format(as.Date(QQ$contract.dt), "%Y-%m-01"))
    QQ$curve.name  <- as.character(QQ$curve.name)
    QQ <- unique(QQ)
    curvenames <- toupper(unique(QQ[,1])) ## these are all the unique curves
  }

  ## read in pedigree file
  ## note that I'm NOT assuming the xls file is complete,
  ## there could be curves in position but not in pedigree file
  con    <- odbcConnectExcel("H:/user/R/RMG/Models/Price/ForwardCurve/Network/allcurves.xls")
  tables <- sqlTables(con)
  pedigree <- toupper(as.matrix(sqlFetch(con, "allcurves")))
  odbcCloseAll()
##   ind <- grep("PARENT", colnames(pedigree))
##   pedigree <- pedigree[,1:max(ind)]   # columns after parent are dropped
  
  ## traverse the pedigree files and get all curves need to be simulated
  allcurves <- NULL
  depth <- 4 ## depth of pedigree is 4
  tmpcurves <- curvenames
  for(iped in 1:depth) {
    iii <- sapply(tmpcurves,
                  function(x) {
                    tmpidx <- which(pedigree[,1]==x)
                    if(length(tmpidx)==0) 
                      warning("Curve ", x, " is not found in peidgree file.")
                    tmpidx
                  })
    ## if there're missing curves iii will be a list, unlist it
    ## iii should be unique but I'll unique it again for safety
    if(class(iii) == "list")
      iii <- unlist(iii)
    iii <- unique(iii)

    ## put them into allcurves
    allcurves <- rbind(allcurves, pedigree[iii,])
    ## get their parents
    parents <- unique(as.vector(pedigree[iii,3:5]))
    ## remove NA
    parents <- parents[!is.na(parents)]
    if(length(parents)==0)
      break
    tmpcurves <- parents
  }
  allcurves <- unique(allcurves) ## allcurves contains the curves need to be simulated
  
  ## add a column for saving result or not
  ## because in allcurves matrix, some curves are parents and need to be simulated
  ## but their results are not requested so don't need to be saved
  ## !!!! This is dangerous. When calling sim.all separately and
  ## the parents curves are not saved, each call will generate independent
  ## simulation. I'll write out the result anyway for now.
  tmpidx <- unlist(sapply(allcurves[,1], function(x) x %in% curvenames))
  flag <- rep(TRUE, dim(allcurves)[1])
#  flag[tmpidx] <- TRUE
  allcurves <- cbind(allcurves, flag)

  return(allcurves)
}







##################################################
## function to construct forward pricing date.
## genearal rule is to do daily until the end of
## next month, then do monthly for nmon months (default is 46). 
## This is a modified version of Adrian's function.
##################################################
makePricingDates <- function(asOfDate, nmon=46) {
  calendar <- "CPS-BAL" ## use the same calendar? UK has different holiday schedules??
  skipWeekends <- FALSE
  
  bom <- as.Date(format(asOfDate, "%Y-%m-01"))  # beginning of the month
  enm <- seq(bom, by="month", length.out=3)[3] - 1 ## end of next month
  pricing.dts <- seq(asOfDate+1, enm, by="day")
  ## keep business days only
  ldays <- lapply(as.list(as.character(pricing.dts)), as.Date)
  flag <- unlist(secdb.invoke("AAD: IsHoliday", "IsHoliday", ldays, calendar, skipWeekends))
  pricing.dts <- pricing.dts[flag==0]
    
  ## get the first business day of each month
  ## I'll take first four days of each month then determine
  ## what is the first business day. There'll be at least a business
  ## day in the first 4 days of any month.
  monthly.dts <- rep(Sys.Date(), nmon)
  allmon <- seq(enm+1, by="month", length.out=nmon)
  for(i in 1:length(allmon)) {
    tmpday <- seq(allmon[i], by="day", length.out=4)
    ldays <- lapply(as.list(as.character(tmpday)), as.Date)
    flag <- unlist(secdb.invoke("AAD: IsHoliday", "IsHoliday", ldays, calendar, skipWeekends))
    monthly.dts[i] <- tmpday[flag==0][1]
  }

  ## return
  c(pricing.dts, as.Date(monthly.dts))
}

##########################################################################
mkdir <- function(dir){
  shell(paste("mkdir", dir), translate=TRUE, invisible=TRUE)
}

##########################################################################
#  Map the NG curves
##########################################################################
fix.relationship.curves <- function(){

  up3rd.space <- function(x){
    ind <- grep(" ", strsplit(x, "")[[1]])  
    return(substr(x, 1, (ind[3]-1)))
  }
  root <- apply(matrix(data$curve.name), 1, up3rd.space)
  

}




#####################################################################################
#   OLD STUFF
#####################################################################################
## ##################################################
## ## Moving window apply. This works for a vector.
## ## This is useful if we want to do, say, moving average.
## ## window is chosen before the current point
## ################################################
## mwapply <- function(x, size=50, step=1, FUN) {
##   FUN <- match.fun(FUN)
##   n <- length(x)
##   result <- rep(NA, n)
##   if(n >size) {
##     for(i in seq(size+1,n, by=step)) {
##       tmp <- FUN(x[(i-size):i])
##       result[i:(i+step-1)] <- tmp
##     }
##   }
##   else { ## if n smaller than window size, stop
##     stop("series is too short\n")
##   }
##   result
## }
## ############################################################
## ## Function to clean a curve
## ## if the volatility in two consecutive days are all over k*sd,
## ## AND they are on different direction, mark it as bad data
## ## interpolate the value
## ############################################################
## clean.curve <- function(curve, k=4) {
##   allvol <- diff(curve)
##   sd.vol <- sd(allvol, na.rm=T)
##   iii <- which(abs(allvol)>k*sd.vol)
##   ## pick the consecutive ones
##   dif <- diff(iii)
##   idx <- which(dif==1)
##   idx.bad <- iii[idx]+1 ## days with bad price
##   for(j in seq(along=idx.bad)) 
##     curve[idx.bad[j]] <- mean(c(curve[idx.bad[j]-1],curve[idx.bad[j]+1]), na.rm=T )
##   curve
## }

############################################################
## get historical price for one curve for past days,
## all contract date
##########################################################
## get.hp.curve <- function(curve, dayfrom, dayto, past=150, contract.dt) {
##   require(RODBC)
##   con.string <- paste("FileDSN=H:/user/R/RMG/Utilities/DSN/CPSPROD_Condor.dsn",  
##                       ";UID=stratdatread;PWD=stratdatread;", sep="")  
##   CPSPROD <- odbcDriverConnect(con.string)
##   cat(paste("Get all prices for", curve, "... \n"))
  
##   ## date range
##   if(missing(dayto))  ## to today by default
##     dayto <- Sys.Date()
##   if(missing(dayfrom)) 
##     dayfrom <- dayto - past
  
##   ## make query string
##   if(missing(contract.dt)) ## get all contract date
##     query <- paste("SELECT pricing_date, start_date, end_date, price ",
##                    "from foitsox.sdb_prices WHERE commod_curve='", curve,
##                    "' and PRICING_DATE<=TO_DATE('", as.character(dayto),
##                    "','YYYY-MM-DD') and PRICING_DATE>=TO_DATE('",
##                    as.character(dayfrom),
##                    "' ,'YYYY-MM-DD') order by start_date, pricing_date",
##                    sep="")
##   else
##     query <- paste("SELECT pricing_date, start_date, end_date, price ",
##                    "from foitsox.sdb_prices WHERE commod_curve='", curve,
##                    "' and PRICING_DATE<=TO_DATE('", as.character(dayto),
##                    "','YYYY-MM-DD') and PRICING_DATE>=TO_DATE('",
##                    as.character(dayfrom), "' ,'YYYY-MM-DD') and START_DATE=TO_DATE('",
##                    as.character(contract.dt), 
##                    "' ,'YYYY-MM-DD') order by start_date, pricing_date",
##                    sep="")

  
##   hP <- sqlQuery(CPSPROD, query)  # all the prices 
##   names(hP) <- c("pricing.dt", "start.dt", "end.dt", "price")
##   hP$start.dt <- as.Date(hP$start.dt);
##   hP$end.dt <- as.Date(hP$end.dt);
##   hP$pricing.dt <- as.Date(hP$pricing.dt)
##   hP$contract.dt <- as.Date(format(hP$start.dt, "%Y-%m-01")) 
##   eom <- as.Date(format(hP$contract.dt + 31, "%Y-%m-01"))-1  # end of the month 
##   hP  <- hP[(hP$end.dt == eom),]  # keep only the balance of the month prices
##   hP$start.dt <- hP$end.dt <- NULL
##   hP <- hP[!duplicated(hP),]                   # there are some weird duplicates

##   ## close handle
##   odbcClose(CPSPROD)
##   ## return
##   hP
## }
############################################################
## Function to write simulation results, (ideally along with
## historical price) to a RData file. Inputs should be lists.
## note inputs are log prices, so need to exponentiate.
############################################################
## write.sim <- function(simresult, FP.options) {
##   ## make file name
##   root <- FP.options$targetdata.dir ## root directory for data file, must be ended with "/"
##   nam <- names(simresult)
##   ## make index for lower triangle of the simulated price
##   ## those will be set to NA
##   ## Note that dimension of all simulation should be the same
##   pricing.dt <- as.Date(dimnames(simresult[[1]])[[1]])
##   # we're changing all the pricing dates to the first of the month
##   # so that when it falls on a day other than the first, we don't substitute
##   # it with NA below.
##   pricing.dt = as.Date(format(pricing.dt, "%Y-%m-01"))
##   npd <- length(pricing.dt)
##   contract.dt <- as.Date(dimnames(simresult[[1]])[[2]])
##   ncd <- length(contract.dt)
  
##   ## index for entries where pricing date is after contract date
##   idx.after <- matrix(rep(pricing.dt,ncd), ncol=ncd) >
##     matrix(rep(contract.dt,npd), ncol=ncd, byrow=T)
##   nsim <- dim(simresult[[1]])[3]
##   ## indexing is trouble so I have to make a 3d array index.
##   allidx <- array(rep(idx.after, nsim), dim=c(npd, ncd, nsim))

##   for(i in 1:length(nam)) {
##     filename <- paste(root, nam[i],  ".sim.RData", sep="")
##     ## set the simulated price to NA when pricing date is over contract month
##     res <- simresult[[nam[i]]]
##     res[allidx] <- NA
##     ## exponentiate and save to a file
##     res <- exp(res)
##     if (file.exists(filename)){
##       res.new <- res              # this batch
##       load(filename)
##       res.old <- res
##       dims <- dim(res); dims[3] <- dim(res)[3]+dim(res.new)[3]
##       res  <- array(res.old, dim=dims, dimnames=list(dimnames(res.old)[[1]],
##         dimnames(res.old)[[2]], paste("sim", 1:dims[3])))
##       res[,,(dim(res.old)[3]+1):dims[3]] <- res.new
##     }
##     save(res, file=filename)
##   }
## }

## #===========================================================================
## get.no.packets <- function(C, FP.options){
##   memory.max  <- memory.limit()    # how much this computer can chug
##   if (length(grep("2\\.6\\.0", R.version$version.string))>0){
##      memory.max <- memory.max*1000000}
##   size.1.sim  <- length(FP.options$sim.days) * FP.options$Kpc.curve * C * 8

##   FP.options$max.no.sims <- trunc(memory.max/(5*size.1.sim))
##   FP.options$no.packets  <- max(1, ceiling(FP.options$nsim/FP.options$max.no.sims))
##   FP.options$nsim.original <- FP.options$nsim
##   if (FP.options$no.packets>1){
##     FP.options$nsim <- FP.options$max.no.sims
##     rLog("===========================================================================")
##     rLog("Loop over simulations.  You need ", FP.options$no.packets, " packets",
##         ".  One packet has ", FP.options$max.no.sims, " simulations!", sep="")
##     rLog("===========================================================================")
##   }
##   return(FP.options)
## }


##########################################################
## a small function to load in raw historical price data
## keep data for recent N days for next D contract month
## Ideally this will take a matrix of positions and
## load data based on that
##########################################################
## load.hP <- function(rdafile, asofDate, N=300, D=48) {
##   ## little function to load in data and process
##   load(rdafile)
##   if(missing(asofDate))
##     asofDate <- Sys.Date() ## use today

##   ## remove data for contract date before the given date
##   ## - should we use all the data??
##   tmp <- hP.mkt[hP.mkt[,2]>asofDate,]
##   ## only keep data for D months
##   dd <- sort(unique(tmp[,2]))
##   if(length(dd)>D) 
##     tmp <- tmp[tmp[,2]<=dd[D],]
  
##   ## keep data for pricing date before asofDate
##   n <- dim(tmp)[2]
##   pricing.date <- as.Date(colnames(tmp)[3:n])
##   tmp <- tmp[,c(TRUE, TRUE, pricing.date<=asofDate)]
  
##   ## only keep N days
##   n <- dim(tmp)[2]
##   if(n>N)
##     tmp <- tmp[,-(1:(n-N))]

##   ## remove curves if the latest price is NA
##   idx <- !is.na(tmp[,dim(tmp)[2]])

##   ## return
##   tmp[idx,]
## }

## ########################################################
## ## function to pre-process the data, given a matrix of
## ## curves
## ########################################################
## loadCurve <- function(curvenames, FP.options) {
##   source("H:/user/R/RMG/Utilities/Database/R/get.prices.from.R.R")
##   ## market of all the curves
##   allmkt <- sapply(curvenames, function(x) strsplit(x, " ")[[1]][2])
##   allmkt.unique <- unique(allmkt) ## use this to avoid curves like "xxx FCO COL xxx"
  
##   ## make options
##   options <- NULL
##   options$asOfDate <- FP.options$asOfDate
##   options$calc$hdays <- seq(options$asOfDate, by="-1 day", length.out=FP.options$N.hist)
  
##   ## contract months
##   ct.mon <- as.Date(format(FP.options$asOfDate, "%Y-%m-01"))
##   allcontract <- seq(ct.mon, by="month", length.out=FP.options$D)
##   allcontract <- allcontract[-1]
##   ## market by market, load in data, subset clean then save
##   for(imkt in 1:length(allmkt.unique)) {
##     hp <- load.hP(paste(FP.options$srcdata.dir, "all.", allmkt.unique[imkt], ".Prices.RData", sep=""),
##                               asofDate=FP.options$asOfDate, D=FP.options$D)
##     ## curves in this market
## #    curve.mkt <- curvenames[allmkt==allmkt.unique[imkt],1]
## #    QQ <- expand.grid(curve.name=curve.mkt, contract.dt=allcontract)
## #    hp <- get.prices.from.R(QQ, options)

##     ## clean the data - I will assume FP.options$N.hist is not too big
##     ## so we have data from N.hist days ago.
##     ## some contracts might have been expired, remove those curves
##     nn <- dim(hp)[2]
##     idx <- is.na(hp[,nn])
##     hp <- hp[!idx,]

##     ## call my clean.curve function to filter out bad points?

##     ## save
##     filename <- paste(FP.options$hpdata.dir, "all.",
##                       allmkt.unique[imkt], ".hp.RData", sep="")
##     save(hp, file=filename)
##   }
## }

## ############################################################
## ## Function to generate some diagnostic plots given
## ## simulation results as a list
## ## This function is defunct. 
## ############################################################
## plot.forward <- function(simresult, lhp, draw=c("screen", "png", "jpeg", "ps", "pdf"),
##                          filestem) {
##   draw <- match.arg(draw) ## draw on screen by default

##   ## open device for drawing 
##   if(draw!="screen") {
##     ## size of the picture
##     pixheight <- 800;     pixwidth <- 800
##     inchheight <- 4; inchwidth <- 4
##     if(draw=="png") {
##       filename <- paste(filestem, "%03d.png", sep="")
##       png(filename, width=pixwidth, height=pixheight)
##     }
##     else if(draw=="jpeg") {
##       filename <- paste(filestem,"%03d.jpg", sep="")
##       jpeg(filename, width=pixwidth, height=pixheight)
##     }
##     else if(draw=="ps") {
##       filename <- paste(filestem,".ps", sep="")
##       postscript(filename, width=inchwidth, height=inchheight, horizontal=F, pointsize=5)
##     }
##     else if(draw=="pdf") {
##       filename <- paste(filestem,".pdf", sep="")
##       pdf(filename, width=inchwidth, height=inchheight, pointsize=5)
##     }
##   }

##   ###### start plotting - this is to be changed if the forward pricing date
##   ## is not equally spaced.
##   ## pick a curve randomly if missing
##   iii <- sample(length(simresult), 1)
##   curvename <- names(simresult)[iii]
##   sim <- simresult[[curvename]]
##   T <- dim(sim)[1]; D <- dim(sim)[2]; nsim <- dim(sim)[3]
  
##   ## 1. one curve, all month, one simulation
##   if(draw=="screen")
##     get(getOption("device"))()
##   layout(1:2); par(mar=c(3.5,3.5,1.6,1.1),mgp=c(1.5,.5,0))
##   matplot(exp(sim[,,sample(nsim,1)]), type="l", main=curvename, xlab="Forward pricing date",
##           ylab="price")
##   ## from another angle
##   matplot(t(exp(sim[,,sample(nsim,1)])), type="l", xlab="Contract Month",
##           ylab="price")

##   ## 2. one curve, one month, all simulation
##   if(draw=="screen")
##     get(getOption("device"))()
##   layout(1:2); par(mar=c(3.5,3.5,1.6,1.1),mgp=c(1.5,.5,0))
##   idx <- sample(D,1)
##   n.hday <- dim(lhp[[curvename]])[2]
##   xlim <- c(0, n.hday+T)
##   ylim <- range(exp(lhp[[curvename]][idx,]), exp(sim[,idx,]))
##   plot(exp(lhp[[curvename]][idx,]), type="l", xlim=xlim, ylim=ylim,
##        main=paste(curvename, "Month", idx), lwd=2,
##        xlab="Pricing date",  ylab="price")

##   apply(exp(sim[,idx,]), 2, function(x) lines(n.hday+(1:T), x, col="grey"))

##   ## 3. one month, one simulation, all curves
##   tmp <- NULL
##   ncurve <- length(simresult)
##   imon <- sample(D,1); isim <- sample(nsim, 1)
##   for(i in 1:ncurve)
##     tmp <- cbind(tmp, simresult[[i]][,imon,isim])
##   matplot(exp(tmp), type="l", main=paste("Month", imon, "Sim", isim),
##           xlab="Forward pricing date", ylab="price")

##   if(draw!="screen")
##     ## close device
##     graphics.off()
## }

## ##########################################################
## ## function to simulate OU process
## ## I took Adrian's function and made some modification
## ##########################################################
## sim.OU <- function(params, x0, nsim, dt) {
##   gamma <- params[1]
##   sigma <- params[2]
##   theta <- params[3]

##   ## init result
##   nt <- length(dt) ## number of forward time point
##   x <- array(0,c(nsim, nt+1))
##   x[,1] <- x0
##   dW <- matrix(myrnorm(nt*nsim,0,sqrt(sigma^2/(2*gamma))), ncol=nt) # generate random deviates
  
##   ## calculate f1 and f2 - note because length of time points are different
##   ## we have several values
##   dt.unique <- sort(unique(dt))
##   f1 <- f2 <- rep(NA, length(dt.unique))
##   for(i in 1:length(dt.unique)) {
##     f1[i] <- exp(-gamma*dt.unique[i])
##     f2[i] <- sqrt(1-f1[i]^2)
##   }
##   names(f1) <- names(f2) <- dt.unique

##   ## simulate
##   for (i in 1:nt){
##     step <- as.character(dt[i])
##     x[,i+1] <- theta + f1[step]*(x[,i]-theta) + f2[step]*dW[,i]
##   }
##   return(x[,-1])   # the simulated paths, with first column (today's price) removed
## }
