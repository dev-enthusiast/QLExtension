################################################################################
# Run VaR for one (synthetic) portfolio, and asOfDate. 
# Ready for Condor.
#
# asOfDate <- "2007-04-05"
# run.name <- "RMG NORLING PORTFOLIO"
#
# R --vanilla --args "2007-04-05" "RMG NORLING PORTFOLIO" < run.one.VaR.R > out.txt  

run.one.VaR.main <- function()
{
  rm(list=ls())
  
  source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
    
  asOfDate   <- overnightUtils.getAsOfDate()
  options = overnightUtils.loadOvernightParameters(asOfDate, FALSE)
  
  #Get the portfolio name
  portfolioName = as.character(Sys.getenv("portfolio"))
  
  computerName = as.character(Sys.getenv("COMPUTERNAME"))
  
  print(paste("Running VaR For Portfolio: ",portfolioName,
    ", As Of Date: ",asOfDate," on Computer: ",computerName,".",sep=""))
  
  if ( portfolioName != "" )
  {
    run.name <- gsub(" ", ".", tolower(portfolioName))
    filename <- paste(options$save$overnight.dir, "/", run.name, "/VaR.report.pdf", sep="")
    have.pdf <- file.exists(filename)
    filename <- paste(options$save$overnight.dir, "/", run.name, "/all.VaR.RData", sep="")
    have.var <- file.exists(filename)
    if (have.pdf & have.var & !options$run$force.recalc){
      print("A Recalculation was not forced and we have the pdf and VaR data. Exiting.\n")
      return
    } else {
      run.one.VaR(options, portfolioName)
    }
  } else {
    stop("Couldn't get portfolio name from environment")
  }

}


################################################################################
run.one.VaR <- function(options, run.name){
    
    run <- NULL
    
    print("Getting the portfolio book...")
    run.all <-  get.portfolio.book(options)
    run$run.name <- toupper(run.name)
    run$specification <- subset(run.all, RUN_NAME == run$run.name)
    
    print("Running the VaR Calculation...")
    res <- try(main.run.VaR(run, options))
    if (class(res)!="try-error"){
        cat(paste("Succeeded individual run = ", run.name, ".\n", sep=""))
    } else {
        cat(paste("Failed individual run = ", run.name, ".\n", sep=""))
        sendErrorEmail()
    }
    
}


################################################################################
sendErrorEmail <- function()
{
    portfolioName = as.character(Sys.getenv("portfolio"))
    computerName = as.character(Sys.getenv("COMPUTERNAME"))
    
    toList = getEmailDistro()
    
    subject = paste("VaR Failure on", computerName)
    
    body = paste("The VaR job for portfolio", portfolioName, "failed on", computerName, "\n")
            
    sendEmail(from="OVERNIGHT-DO-NOT-REPLY@constellation.com",
            to=toList, subject, body)
}


################################################################################
# Run VaR for one (synthetic) portfolio, and asOfDate. 
# Ready for Condor.
res = try(run.one.VaR.main())
if (class(res)=="try-error"){
  cat("Failed VaR run outside of main.run.VaR scope.\n")
  sendErrorEmail()
}


