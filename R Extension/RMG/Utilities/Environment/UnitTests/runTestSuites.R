# Designed to work with packages.  Each package has a subfolder with 
# the name of the package to test.  Inside each package subfolder there
# are scripts with names that start with "^test". 

require(RUnit)
dirUT <- "H:/user/R/RMG/Utilities/Environment/UnitTests/"

#testSuitesToRun <- c("reshape", "SecDb")    
testSuitesToRun <- c("SecDb")    


for (ts in testSuitesToRun)
{
  eval(parse(text=paste("require(", ts, ")", sep="")))  # load the package
  tsObj <- defineTestSuite(ts,
    dirs=paste(dirUT, ts, sep=""),
    testFileRegexp="\\.R$",
    testFuncRegexp="^test.+",
    rngKind = "Marsaglia-Multicarry",
    rngNormalKind = "Kinderman-Ramage"                                     
  )
  
  results <- runTestSuite(tsObj)
  printTextProtocol(results)
  
  if (results[[1]]$nFail != 0) {
    cat("Do something.  Maybe email.")
  }
#  cat("\n\n\n")
}


# maybe special treatment to the ones that have fails!
