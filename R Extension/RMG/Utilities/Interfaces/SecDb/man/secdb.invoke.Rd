\name{secdb.invoke}
\alias{secdb.invoke}
\title{Invoke a SecDb Library Function}
\description{
Executes a function in the specified library with optional arguments.
}
\usage{
secdb.invoke( libraryName, functionName, argument1, argument2, ... )
}
\arguments{
  \item{libraryName}{a character string representing the SecDb library}
  \item{functionName}{a character string representing the SecDb function to execute}
  \item{argument0..n}{optional arguments required for calling the function}
}
\value{Returns the SecDb value type in a compatible R format (list, vector, etc).}
\examples{
library(SecDb)

secdb.setDatabase("!Bal Prod 1;ReadOnly{!Bal Prod 2}")

##Get the curve data using the tsdb utility functions
curveData = secdb.invoke("_Lib ELEC Matlab Dispatch Fns", "TSDB::read",
  "PRC_NG_EXCHANGE_0706", Sys.Date()-50, Sys.Date()-1)
print(head(data.frame(curveData)))


##Won't work!  It will print a SecDb error because not all arguments were passed
res1 <- secdb.invoke("_lib elec spread regression fns", "SpreadUtil::FuturesStrip",
             "Commod NG exchange", as.Date("2007-06-18"))
print( head( data.frame(res1) ) )

##Now it will run! :) (Notice the extra NULL for the VT argument)
res1 <- secdb.invoke("_lib elec spread regression fns", "SpreadUtil::FuturesStrip",
             "Commod NG exchange", as.Date("2007-06-18"), NULL)
print( head( data.frame(res1) ) )


##This creates an array with 2 structure elements
Locations <- list( list(loc="COMMOD WTI EXCHANGE", name="WTI"),
                   list(loc="COMMOD NG Exchange", name="NG") )
last.mark <- Sys.Date()
yesterday <- Sys.Date()-1
dateTill  <- as.Date("2009-12-01")
useClose  <- TRUE

x <- secdb.invoke("_lib elec dly fut changes", "DlyFutChanges::GetFutStrip", 
  Locations, last.mark, yesterday, dateTill, useClose, "CPS")

}
\details{This code is equivalent to loading the appropriate library and invoking
the specified function in SecDb.  You MUST pass all arguments specified in the 
SecDb function definition to secdb.invoke.  This means that even if a function
specifies default parameters for some of it's arguments, you MUST pass those in
your R secdb.invoke call.  See the examples section for more elaboration.

Because SecDb types can be created dynamically, it is possible that the
conversion from a SecDb value to R type (and R->SecDb as well) would be 
unsupported.  This code attempts to use the most suitable R type for the SecDb 
objects it knows about. In the event of an unsupported type, please contact 
CCG RMG Analytics and we will look into adding the functionality. If a SecDb 
object is unsupported, the C++ conversion routine will return NULL back to 
the R code.

The proper R type for both SecDb structures and arrays is the list. If your list
has any named elements, it will be converted to a SecDb structure, and if it is
completely unnamed, it will be converted to a SecDb array.  Both types support
nested elements, so it is possible to have arrays of structures, 
multi-dimensional arrays, etc.  See the example code for more information.
}
\note{This call will also return NULL if the library or function is not found.

All Errors From SecDb should now propagate back to the R console.

All curves returned will be in list form (a limitation of the R-C library). 
Simply call 'curveData = data.frame(curveData)' to make it more useful.
}