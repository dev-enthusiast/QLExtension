# Experimental list of Condor utilities from R. 
# 
#
# 
#


utilities.Condor <- new.env(hash=TRUE)
#==================================================================
# Show condor queue
#
#
utilities.Condor$showCondorQueue <- function(){

  res <- system("condor_q -dag -global", intern=TRUE,
                show.output.on.console=FALSE)
  res[1] <- paste("As of:", Sys.time())

  source("H:/user/R/RMG/Utilities/sendemail.R")
  from   <- "adrian.dragulescu@constellation.com"
  toList <- c("john.scillieri", "adrian.dragulescu", "michael.zhang" )
  toList <- paste(paste(toList, "@constellation.com",
            sep=""), collapse=", ")
  subject <- "Condor queue status"
  sendEmail(from, toList, subject, res)
  return()
}


#==================================================================
# Get computer names
#
#
utilities.Condor$getComputerNames <- function(){

  res <- system("condor_status", intern=TRUE, show.output.on.console=FALSE)
  ind <- grep(" WINNT51", res)  # OK until we go on UNIX
  res <- res[ind]

  computers <- sapply(strsplit(res, " "), function(x){x[1]})
  computers <- sapply(strsplit(computers, "\\.C"), function(x){x[1]})
  computers <- strsplit(computers, "[[:alnum:]]+@")
  computers <- sapply(computers, function(x){paste(x, sep="", collapse="")})

  computers <- sort(unique(computers))
  
  return(computers)
}

utilities.Condor$showCondorQueue()
