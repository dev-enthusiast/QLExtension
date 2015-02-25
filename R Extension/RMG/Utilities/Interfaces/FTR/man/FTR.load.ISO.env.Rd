\name{FTR.load.ISO.env}
\alias{FTR.load.ISO.env}
\title{Load an ISO environment for a given NERC region.}
\description{Load an ISO environment for a given NERC region.}
\usage{
FTR.load.ISO.env(region="NEPOOL")
}
\arguments{
  \item{region}{A NERC region, e.g. NEPOOL.}
}
\value{
  An environment with a lot of stuff, functions and data.frames.}
\details{
  \enumerate{
  \item{MAP}{A data.frame with node name, ptid, zone, SecDb location.}
  \item{AWD}{FTR awards in previous auctions.}
  \item{CP}{Clearing prices of }
  \item{hP}{Historical daily congeston prices for all ISO nodes.}
  \item{HRS}{A data.frame with the hours by bucket by month.}
  }
  etc.
}
\author{Adrian Dragulescu}
\examples{
# Load an ISO.env, say NEPOOL which contains NEPOOL$hSP
#
FTR.load.ISO.env("NEPOOL")

ls(NEPOOL)
# ls(NEPOOL)
# [1] "analysis.start.dt"     "AWD"                   "CP"                   
# [4] "getBindingConstraints" "hist.start.dt"         "hP"                   
# [7] "HRS"                   "main"                  "MAP"                  
#[10] "mkt"                   "options"               "region"               
#[13] "SP"                   

}