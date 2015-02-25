# Do the automatic replacement of source argument
#
#

#################################################################
# one file at a time 
replace_absolute_paths <- function(filename)
{
  rLog("Working on ", filename)
  aux <- readLines(filename)
    
  pattern <- "((?i)h:/user/)"
  ind <- grep(pattern, aux, perl=TRUE)
  for (i in ind) 
    aux[i] <- gsub(pattern, "", aux[i])
  
  pattern <- "((?i)h:\\\\\\\\user\\\\\\\\)"
  ind <- grep(pattern, aux, perl=TRUE)
  for (i in ind) 
    aux[i] <- gsub(pattern, "", aux[i])
  
  writeLines(aux, filename)
}


#################################################################
#
.main <- function()
{
  require(CEGbase)

  mapuser()  # you would not have this in a production script!

  source("R/RMG/Utilities/Environment/R.Release.Notes/R-2.12.1/lib.R.to.prod.R")

  # pick up a directory to focus on
  DIR <- "H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Capacity/"

  files <- list.files(DIR, full.names=TRUE, pattern="\\.(R|r)$")
  for (filename in files)
    replace_absolute_paths(filename)
  

  
  
}

