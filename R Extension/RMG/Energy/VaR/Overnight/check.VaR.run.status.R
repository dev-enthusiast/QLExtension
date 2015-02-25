# See how many portfolios ran last nigth, determine if you need 
# to resubmit some portfolios again, etc. 
#
# First version by Adrian Dragulescu on 18-May-2007

check.VaR.run.status <- function(){

  source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
  asOfDate <- as.Date(overnightUtils.getAsOfDate())
  options  <- overnightUtils.loadOvernightParameters(asOfDate)

  run.all <- get.portfolio.book(options)
  uRuns   <- sort(unique(run.all$RUN_NAME))

  status <- data.frame(name = uRuns,
    run.name = gsub(" ", ".", tolower(uRuns)), have.pdf=FALSE,
    VaR=NA_real_)
  for (r in 1:nrow(status)){
    filename <- paste(options$save$overnight.dir, "/", 
      status$run.name[r], "/VaR.report.pdf", sep="")
    status$have.pdf[r] <- file.exists(filename)
    filename <- paste(options$save$overnight.dir, "/", 
      status$run.name[r], "/all.VaR.RData", sep="")
    if (file.exists(filename)){
      safeLoad(filename)
      status$VaR[r] <- VaR$Total
    }
  }
  
  if ( options$email$send )
  {
    todo <- status[is.na(status$VaR), -2]
    subject <- paste("These portfolios have not finished for", asOfDate)
    body    <- capture.output(todo)
    sendEmail(from="OVERNIGHT-DO-NOT-REPLY@constellation.com",
              to=options$email$show.status.list, subject, body)
  }
  return(status)
}

################################################################################
# Ready for Condor.
res = try(check.VaR.run.status())
if (class(res)=="try-error"){
  cat("Failed VaR Status Check.\n")
}
