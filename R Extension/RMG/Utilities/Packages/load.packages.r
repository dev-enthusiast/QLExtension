################################################################################
# Function load.packages will load or install->load the libraries specified.
# 
# It operates as follows:
#   * Update any packages found in the local repository that are newer than the 
#     user's installed version.
#   * Try to load the package. If not found...
#   * Try to install & load from the local repository. If not found...
#   * Try to install & load from the Internet. Error if not found.
#
# To generate the package list for the local repository:
#   library(tools)
#   # where RVERSION = 2.4, 2.5, etc for your version of R
#   reposPath = "H:/user/R/RMG/utilities/packages/bin/windows/contrib/RVERSION"
#   write_PACKAGES(reposPath) 
#
# To Use:
#   options <- NULL
#   options$source.dir <-  "H:/user/R/RMG/"
#   # if it is true, it will install from archive regardless.
#   options$force.load <- FALSE     
#                                
#   package.names <- c("reshape", "RODBC")
#
#   source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
#   load.packages(package.names, options)
#
load.packages <- function(package.names, options) 
{
    for(lib in package.names)
    {
        library(lib, character.only=TRUE)
    }
    
##   LOCAL_REPOSITORY = "file:////nas-omf-01/cpsshare/All/Risk/Software/R/prod/utilities/packages"
##   INTERNET_REPOSITORY = "http://rh-mirror.linux.iastate.edu/CRAN/"
  
##   #update anything that's new in the local repository
##   update.packages(ask=FALSE, repos=LOCAL_REPOSITORY)
  
## #  if (nargs()==1){
##     options=NULL
## #  }
##   options$force.load <- FALSE
##   if (length(options$force.load)==0){options$force.load <- FALSE}

##   # Load package one by one. If it does not exist or force.load is true, 
##   # install from local repos. If it's not there, install it from the Internet.
##   for (package in package.names)  
##   {
##     loadExpression = parse( text=paste("suppressWarnings(!library(", package, 
##       ", logical.return=TRUE))", sep="") )

##     # if it exists in our library, just load it and we're done.
##     req <- eval(loadExpression)
      
##     if( req || options$force.load ) 
##     {
##       suppressWarnings(install.packages(package, repos=LOCAL_REPOSITORY))
      
##       # did the install from the local repos work? Try to load it and see.
##       if( eval(loadExpression) )
##       {
##         warningMsg <- paste("Could not install package ", package, 
##           " from local repository.  Loading from the Internet...\n", sep="")
##         cat(warningMsg)
        
##         #last try... FROM THE INTERNET!!!
##         suppressWarnings(install.packages(package, repos=INTERNET_REPOSITORY))
        
##         if( eval(loadExpression) )
##         {
##           errorMessage <- paste("WARNING: Could not install package ", package, 
##             " from the Internet. \nIt is most likely not available... Continuing.", 
##             sep="")
##         }
        
##       }
##     } 
##   }
}

