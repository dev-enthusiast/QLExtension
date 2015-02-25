#
#
runUnitTestsCEGterm <- function()
{
  require(RUnit)
  require(SecDb)

  ts <- defineTestSuite("testsCEGterm",
    dirs           = paste(R.home(), "/library/CEGterm/tests", sep=""),
#    dirs           = "H:/user/R/RMG/Utilities/Interfaces/CEGterm/inst/tests",
    testFileRegexp = "",
    testFuncRegexp = "^test",
    rngKind        = "Marsaglia-Multicarry",
    rngNormalKind  = "Kinderman-Ramage"                                     
  )
  results <- runTestSuite(ts)
  printTextProtocol(results)

}








