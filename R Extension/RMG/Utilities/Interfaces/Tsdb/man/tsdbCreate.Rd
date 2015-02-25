\name{tsdbCreate}
\alias{tsdbCreate}
\title{Create a new tsdb symbol.}
\description{
  Create a new tsdb symbol.
}
\usage{
  tsdbCreate(symbol, path, description, dataSource, realTimeFlag=TRUE)
}
\arguments{
  \item{symbol}{A name for your symbol.  No longer than 31 characters,
    no special characters )(.+-*/}
  \item{path}{A string with the full path to the binary file (see the
	example).}
  \item{description}{A string with information about the data (units,
	etc.).}
  \item{dataSource}{A string with information about data source.}
  \item{realTimeFlag}{A boolean, \code{TRUE} for hourly data,
	\code{FALSE} for daily data.}
}
\details{
  You need to have permissions for this.  Don't hate the playa', call
  IT.

  Symbols created by PM group go into the folder "T:/Data/pm" ONLY.
  
}
\value{Returns \code{TRUE} for a successful creation, returns
  \code{FALSE} if the symbol already exists.  This is the convention in
  SecDb, so don't try to create the same symbol several times as this
  will return FALSE.} 
\seealso{\code{\link{tsdbUpdate}}}
\examples{

symbol <- "pm_nepool_trashme"
path <- "T:/Data/pm/nepool/pm_nepool_trashme.bin"
description  <- "For experiments"
dataSource   <- "Imagination Inc." 
realTimeFlag <- TRUE   

tsdbCreate(symbol, path, description, dataSource, realTimeFlag=realTimeFlag)

}
%\note{}