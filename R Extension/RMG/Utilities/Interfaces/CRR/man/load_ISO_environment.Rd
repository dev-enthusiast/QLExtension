\name{load_ISO_environment}
\alias{load_ISO_environment}
\title{Load a data.frame with basic info about nodes in the pool.}
\description{Load a data.frame with basic info about nodes in the pool.}
\usage{
  load_ISO_environment(region="NEPOOL")
}
\arguments{
  \item{region}{A String with the region name "NEPOOL", "NYPP", "MISO",
    "PJM", "SPP".}
}
\value{
  The function loads an environment with basic info about the pool into
  the global namespace.  It should contain at least a \code{data.frame}
  called MAP that has information about nodes in the pool.
}
%\details{}
\author{Adrian Dragulescu}
\note{}
%\seealso{\code{\link{CRR_auction}}}
\examples{

load_ISO_environment(region="NYPP")
ls(NYPP)
# [1] "DIR"    "MAP"    "region"

head(NYPP$MAP)
#  ptid     location           name zone      type     deliv.pt          tsdb.symbol mccLocations subzone
#1  359 BEAR SWAMP 1 BEAR_SWAMP___1  NPX GENERATOR BEAR SWAMP 1 NYPP_DAM_GEN_MCC_359         <NA>  NPX-AC
#2  360 BEAR SWAMP 2 BEAR_SWAMP___2  NPX GENERATOR BEAR SWAMP 2 NYPP_DAM_GEN_MCC_360         <NA>  NPX-AC
#3  413  FIFEBROOK 1  FIFEBROOK___1  NPX GENERATOR  FIFEBROOK 1 NYPP_DAM_GEN_MCC_413         <NA>  NPX-AC
#4  424  PENOBSCOT 1  PENOBSCOT___1  NPX GENERATOR  PENOBSCOT 1 NYPP_DAM_GEN_MCC_424         <NA>  NPX-AC
#5  501 DEEP CREEK 1 DEEP_CREEK___1  PJM GENERATOR DEEP CREEK 1 NYPP_DAM_GEN_MCC_501         <NA>  PJM-AC
#6  502 DEEP CREEK 2 DEEP_CREEK___2  PJM GENERATOR DEEP CREEK 2 NYPP_DAM_GEN_MCC_502         <NA>  PJM-AC


}
