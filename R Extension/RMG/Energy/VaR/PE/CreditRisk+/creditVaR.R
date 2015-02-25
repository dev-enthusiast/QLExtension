# Implement different algorithms from the CreditRisk+ methodology.
# See the CreditRisk+ (1997) documentation located on
# S:\Risk\Software\R\Documentation\Credit\CSFB--CreditRisk+ documentation.pdf 
#
# First version by Adrian Dragulescu on 18-Jun-2007
#
# TO DO: - there are some cparties with NA IRC.  Map them to something.
#        - find a way to split the exposure by ... 
#

require(RODBC)

##############################################################################
#source("H:/user/R/RMG/Energy/VaR/PE/CreditRisk+/lib.credit.functions.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Utilities/sendemail.R")


##############################################################################
# File Namespace
#
CVaR = new.env(hash=TRUE)

##############################################################################
#
CVaR$main <- function(asOfDate){

  rLog(paste("Calculating Credit VaR for ", asOfDate, sep=""))
  
  options <- NULL
  options$asOfDate <- as.Date(asOfDate)
  options$MC$batch.size <- 10000      # number of simulations
  options$MC$no.batches <- 10        # repeat for better estimation of qtiles
  options$recovery.rate <- 0.4       # 40% recovery rate
  options$source.dir <- "H:/user/R/RMG/Energy/VaR/PE/CreditRisk+/"
  options$output.dir <- paste("S:/All/Risk/Reports/Credit/MPE/", asOfDate,
                              "/", sep="")
  dir.create(options$output.dir)
  
  EX <- CVaR$get.current.exposure(options)
 
  DR <- CVaR$get.default.rates()

  EX <- merge(EX, DR[,c("icr","drate")], all.x=T)

  sim.ex <- CVaR$model.fixed.default.rates.Bernoulli.MC(EX, options)[[1]]
  
  # apply the recovery rate, this is what we lose 
  sim.ex <- (1-options$recovery.rate) * sim.ex

  # start report ...
  res <- CVaR$summary.exposure(EX, sim.ex, options)
  
  # email results ...
  CVaR$email.results(res, options)

  rLog("Done.")
  
  
}

##############################################################################
# Report on the exposure
#
CVaR$summary.exposure <- function(EX, sim.ex, options)
{
  rLog("Start report ...")
  res <- NULL
  res$expected.loss <- sum(EX$exposure * EX$drate)
  res$loss.distrib$sd <- sd(as.numeric(sim.ex))
  res$loss.distrib$quantiles  <- quantile(sim.ex, c(seq(0, 0.9, 0.1),
                                                    0.95, 0.99, 0.999))
  res$P0 <- length(which(sim.ex==0))
  
  # probability of no loss = prod(1-EX$drate) for independent Bernoulli
  res$loss.distrib$p0   <- length(which(sim.ex==0))/length(sim.ex)

  HH <- hist(sim.ex/1000000, 100, plot=FALSE)
  fileName <- paste(options$output.dir, "credit.VaR.", options$asOfDate,
                    ".pdf", sep="")
  pdf(fileName, 8,5)
  plot(HH, col="wheat", 
       xlab="Loss, M$", ylab="Histogram",
       main=paste("Distribution of credit losses (1 year horizon),",
         options$asOfDate), cex.main=1)
  abline(v=res$loss.distrib$quantiles["99%"]/1000000, col="gray", lwd=2)
  text(res$loss.distrib$quantiles["99%"]/1000000, 0.85*max(HH$counts),
       paste("CVaR = ", dollar(res$loss.distrib$quantiles["99%"]/1000000,0),
             " M", sep=""), pos=4)
  abline(v=res$expected.loss/1000000, col="grey", lwd=2)  
  text(res$expected.loss/1000000, 0.85*max(HH$counts),
       paste("E[L] = ", dollar(res$expected.loss/1000000,0),
             " M", sep=""), pos=4)
   
  cat(paste("Expected loss: ", dollar(res$expected.loss,0), "\n", sep=""))
  cat(paste("Prob. no loss: ", dollar(res$P0,0), "\n", sep=""))
  cat(paste("CVaR (99%):    ", dollar(res$loss.distrib$quantiles["99%"],0),
            "\n",sep=""))
  dev.off()

  fileName <- paste(options$output.dir, "credit.VaR.", options$asOfDate,
                    ".RData", sep="")
  save(res, file=fileName)
  
  rLog("Done.\n")  
  return(res)
}

#########################################################################
# Read the default rates from an xls. 
#
CVaR$get.default.rates <- function()
{
  rLog("Get default rates ... ")

  file <- "H:/user/R/RMG/Energy/VaR/PE/CreditRisk+/default.rates.xls"
  con  <- odbcConnectExcel(file)
  tables <- sqlTables(con) 
  DR   <- sqlFetch(con, "DR")
  odbcCloseAll()
  
  names(DR) <- c("icr", "credit.rating", "drate", "sd")
  rLog("Done.\n")  
  return(DR)
}

#########################################################################
# Read current exposure from RAFT (an xls for now).  
#
CVaR$get.current.exposure <- function(options)
{
  rLog("Get current exposure ... ")
  con.string <- paste("FileDSN=H:/user/R/RMG/Utilities/DSN/RMGDBP.dsn",  
       ";UID=rmgdbuser;PWD=rmgdbpwd_prod;", sep="")  
  con <- odbcDriverConnect(con.string)
  query <- paste("select CE.counterparty, CE.cparty_shortname, ",
    "CE.mtm, CE.arap, CE.accrual, CI.icr, CE.limit_category ", 
    " from credit_exposure CE, ",
    "counterparty_icr CI where CE.report_date = CI.dataasofdate ", 
    "and CE.cparty_shortname = CI.counterparty ", 
    "and CE.report_date = to_date('", options$asOfDate,
    "', 'yyyy-mm-dd')  ", sep="")
  EX <- sqlQuery(con, query)
  colnames(EX) <- tolower(colnames(EX))
  colnames(EX)[1:2] <- c("counterparty", "cparty")
  EX$icr <- as.numeric(EX$icr)
  EX$exposure <- EX$mtm + EX$arap + EX$accrual

  # aggregate over limit_category, may want to split in the future
  EX <- aggregate(EX$exposure, by=list(cparty=EX$cparty, icr=EX$icr), sum)
  colnames(EX)[3] <- "exposure"
  if (nrow(EX)==0){
    stop(paste("Could not find counterparty exposure/icr's in database ",
      "for ", options$asOfDate, ".  Exiting.", sep=""))
  } else {
    rLog(paste("Found", nrow(EX), "counterparties."))
  }
  rLog("Done.\n")

  EX <- subset(EX, exposure > 0)  # take only the positive part
  
  return(EX)
}

##############################################################################
# email results
#
CVaR$email.results <- function(res, options)
{
  subject <- paste("Credit VaR (99% CI) for ", options$asOfDate, " is ",
    dollar(res$loss.distrib$quantiles["99%"]/1000000, 0), "M", sep="")
  toList <- getEmailDistro("cvar")
  from   <- "OVERNIGHT-DO-NOT-REPLY@constellation.com"
  body   <- paste("Expected credit losses due to counterparty ",
    "defaults for one year horizon, calculated as of ", options$asOfDate,
    " is ", dollar(res$expected.loss/1000000, 0, F), "M$.  ",
    "The loss distribution is calculated at the counterparty level, ",
    "for the entire CCG portfolio (in the money positions) and ",
    "assumes a 40% recovery rate.  In the calculation the counterparty ",
    "defaults are independent.\n\nLoss distribution (M$):", sep="")

  aux <- data.frame(probability=names(res$loss.distrib$quantiles),
    quantile=dollar(res$loss.distrib$quantiles/1000000, 0))
  rownames(aux) <- NULL
  
  body <- c(body, paste(aux[,1], aux[,2], sep=",\t    \t", collapse="\n"))
  
  fileName <- paste(options$output.dir, "credit.VaR.", options$asOfDate,
                    ".pdf", sep="")

  sendEmail(from, toList, subject, body=body, links=fileName)
  
}


##############################################################################
# Idependent defaults
#
CVaR$model.fixed.default.rates.Bernoulli.MC <- function(EX, options)
{
  rLog("Simulate defaults ...")
  if (length(options$MC$batch.size)==0){options$MC$batch.size <- 10000}
  if (length(options$MC$no.batches)==0){options$MC$no.batches <- 30}  
  sim.ex <- matrix(NA, ncol=options$MC$no.batches, nrow=options$MC$batch.size)
#  CVaR   <- array(NA, dim=options$MC$no.batches)
  cat("Running ", options$MC$no.batches, " batches of ",
      options$MC$batch.size, " simulations.\n")
  for (b in 1:options$MC$no.batches){
    sims <- matrix(runif(nrow(EX)*options$MC$batch.size),
                   ncol=options$MC$batch.size)
    dp   <- matrix(EX$drate, nrow=nrow(EX), ncol=options$MC$batch.size)
    is.default <- (sims <= dp)
    defaults <- apply(is.default, 2, which)

    sim.ex[,b] <- sapply(defaults, function(x){sum(EX$exposure[x])})
#    CVaR[b]    <- quantile(sim.ex[,b], 0.99)
    cat("Done with batch ", b, ".\n", sep="")
  }
  rLog("Done.\n")
  
  return(list(sim.ex, CVaR))
}

#=========================================================================
# Fixed default rates, Poisson approximation, recursive polynomials
# see page 38, CreditRisk+ documentation
#
# Have a BUG.  The total probability does not add up to 1!
#
## model.fixed.default.rates.Poisson <- function(EX, error=10^-6)
## {
##   max.no.bands <- 5*max(EX$band)
  
##   # calculate expected number of defaults by band
##   aux <- tapply(EX$drate, list(EX$band), mean)
##   mu  <- array(0, dim=max.no.bands)
##   mu[as.numeric(names(aux))] <- aux

##   Pr <- matrix(NA, ncol=3, nrow=max.no.bands)
##   dimnames(Pr)[[2]] <- c("band", "loss", "pdf")
##   Pr[, "band"] <- seq(0, max.no.bands-1, by=1)
##   Pr[1, "pdf"] <- exp(-sum(mu))                # Prob of no default
##   for (n in 1:(max.no.bands-1)){
##     aux <- mu[1:n]*Pr[1:n,"band"]/n
##     Pr[n+1, "pdf"] <- sum(aux * rev(Pr[1:n, "pdf"]))
##     if (sum(Pr[1:(n+1), "pdf"]) > (1-error)){break}
##   }
##   Pr[,"loss"] <- options$unit.exposure*Pr[,"band"]
  
##   plot(options$unit.exposure*, Pr, type="l",
##       col="blue", xlab="Exposure, $", ylab="Pdf")
##   return(Pr)  
## }


##   cat("From ", no.batches, " batches of ", batch.size, " simulations, ",
##     "sd() of the 99 percentile of the loss distribution is ",
##     trunc(sd(CVaR)), ".\n", sep="")

  
##     cat(paste("CVaR = ", trunc(newQ), ".  ", sep=""))
##     if (abs(newQ-CVaR)/newQ < 0.001){break}
##     cat("Run another batch of", no.Sims, "sims.\n")
##     CVaR  <- newQ
 
## model.fixed.default.rates.Bernoulli.MC <- function(EX)
## {
##   sim.ex  <- NULL
##   CVaR    <- 0
##   no.Sims <- 20000
##   batch.size <- 10000
##   no.batches <- 10
##   repeat{
##     sims <- matrix(runif(nrow(EX)*no.Sims), ncol=no.Sims)
##     dp   <- matrix(EX$drate, nrow=nrow(EX), ncol=no.Sims)
##     is.default <- (sims <= dp)
##     defaults <- apply(is.default, 2, which)

##     sim.ex <- c(sim.ex, sapply(defaults, function(x){sum(EX$exposure[x])}))
##     newQ   <- quantile(sim.ex, 0.99)
##     cat(paste("CVaR = ", trunc(newQ), ".  ", sep=""))
##     if (abs(newQ-CVaR)/newQ < 0.001){break}
##     cat("Run another batch of", no.Sims, "sims.\n")
##     CVaR  <- newQ
##   }
##   cat("\n")
  
##   return(sim.ex)
## }



##   mids <- 0.5                    # first bin
##   Pr   <- exp(-sum(mu))          # Prob of no default
##   n    <- 1
##   repeat {
##     aux <- mu[1:n]*mids[1:n]/n
##     Pr  <- c(Pr, sum(aux * rev(Pr[1:n])))
##     if (sum(Pr) > (1-error)){break}   # cumulative probability
##     n    <- n+1
##     mids <- c(mids, mids[length(mids)]+1)
##   }
  
##   options$unit.exposure <- 202389   # 1 MM$
##   breaks   <- seq(0, max(EX$exposure), by=options$unit.exposure)
##   EX$band  <- findInterval(EX$exposure, breaks)  # calculate exposure band

##   max.band <- max(EX$band)

