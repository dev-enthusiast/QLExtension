# Set default parameters for a divide/conquer operation for different 
# backends.
#

parmsDivideConquer <- function(backend="local")
{
  parmsDC <- list()
  parmsDC$logLevel <- "LOG"
  
  if (backend=="local"){
    parmsDC$conquer$preFileName   <- "" # in current dir, append keynames only
    parmsDC$conquer$outFileFormat <- "RData" 
  }
  
  return(parmsDC)
}
