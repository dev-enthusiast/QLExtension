\name{tsdb.getSymbolInfo}
\alias{tsdb.getSymbolInfo}
\title{Get Information on a TsDb Symbol}
\description{
Returns the information in TsDb on a given symbol name.
}
\usage{
tsdb.getSymbolInfo(symbolName)
}
\arguments{
  \item{symbolName}{a character string representing the TsDb symbol to query}
}
\value{Returns a list containing all the information provided by TsDb for a 
given symbol.}
\examples{

secdb.setDatabase("!Bal Prod 1;ReadOnly{!Bal Prod 2}")

##Returns a list object of all specified symbol info.  
curveInfo =  tsdb.getSymbolInfo( "PRC_NG_Exchange_0706" ) 
print(curveInfo)

}
