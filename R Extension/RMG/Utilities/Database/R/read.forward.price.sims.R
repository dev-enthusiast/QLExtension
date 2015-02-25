################################################################################
# Read forward price sims from a given filehash
#
# John Scillieri, 2007-11-16.


################################################################################
library(filehash)


################################################################################
.demo.read.forward.price.sims <- function(){
    
    curve.names = c("COMMOD NG EXCHANGE", "COMMOD PWJ 5X16 WEST PHYSICAL")
    contract.dts = NULL
    pricing.dts = NULL
    fhPath = "R:/SimPrices/Network/2007-12-21/overnight/result"

    sPP.env <<- .getFileHashEnv(fhPath)

    
    source("H:/user/R/RMG/Utilities/Database/R/read.forward.price.sims.R")
    res <- read.forward.price.sims( curve.names, contract.dts=NULL,
            pricing.dts=NULL, path.to.filehash=fhPath, env=sPP.env )
    
    return(res)
}


################################################################################
# Returns a 4D array of price simulations. 
#
# Dimensionality:
#	curve name X pricing date X contract date X simulation number
# 
read.forward.price.sims <- function(curve.names, contract.dts=NULL, 
        pricing.dts=NULL, path.to.filehash=NULL, env=NULL)
{
    if( is.null(path.to.filehash) )
    {
        stop("You must specify the path.to.filehash argument.")
    }
    
    endWithSlash = which(regexpr("/$", path.to.filehash) != -1)
    path.to.filehash[endWithSlash] = substr(path.to.filehash[endWithSlash], 1,
            nchar(path.to.filehash[endWithSlash])-1)
    
    # load up the hash to extract out the curve data
    if (!is.environment(env)){    # allow caching, hopefully no name conflict!
      env <- .getFileHashEnv(path.to.filehash)
    }
    if (length(ls(env))==0){
      rWarn("The environment", path.to.filehash, "is empty.  Please check!")
    }
    # Use this data as a representative of the filehash
    sampleData = env[[ ls(env)[1] ]]
    
    # find out how many simulations are in the hash
    numSims = dim(sampleData)[3]
    
    if( is.null(pricing.dts) ){
        pricing.dts = rownames(sampleData)  
    } else {
        pricing.dts <- as.character(pricing.dts)
    }
    
    if( is.null(contract.dts) ){
        contract.dts = colnames(sampleData)
    } else {
        contract.dts <- as.character(contract.dts)
    }
    
    #set up the 4D array to hold the results
    results = array( dim=c( length(curve.names), length(pricing.dts), 
                    length(contract.dts), numSims ) )   
 
    for( curveIndex in seq_along(curve.names) )
    {
        #retrieve the curve data requested
        curve = curve.names[curveIndex]
        curveData = env[[ tolower(curve) ]]
        if( !is.null(curveData) )
        {
            results[curveIndex,,,] = curveData[ pricing.dts, contract.dts, ]
        }
    }
    dimnames(results) = c( list(curve.names), list(pricing.dts),
       list(contract.dts), list(dimnames(curveData)[[length(dimnames(curveData))]]) ) 
    
    return(results)  
}


################################################################################
# Load up the file hash and return it as an env
#
.getFileHashEnv <- function( path )
{
    filehashOption(defaultType = "RDS")      # need this, dbInit will fail
    endWithSlash = which(regexpr("/$", path) != -1)
    path[endWithSlash] = substr(path[endWithSlash], 1, nchar(path[endWithSlash])-1)
    db  <- dbInit(path)
    env <- db2env(db)
    if (!is.environment(env))
    {
        rError("Could not load the price filehash environment.\n")
        stop("Exiting.\n")
    }
    
    return(env)
}

