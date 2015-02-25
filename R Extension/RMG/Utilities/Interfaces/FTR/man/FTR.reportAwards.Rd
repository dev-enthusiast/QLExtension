\name{FTR.reportAwards}
\alias{FTR.reportAwards}
\title{Generate an Sweave report analyzing the awards.} 
\description{Generate an Sweave file from a template to produce
  a report detailing our winning bids/losses and other analysis.
  This collection of functions save an RData file with all the info
  that the Sweave template file needs.

  Make sure that your ISO.env environment incorporates the most
  recent auction results.
}
\usage{
FTR.reportAwards( filepath, ISO.env)
}
\arguments{
  \item{filepath}{Filepath to the submitted bids file.}
  \item{ISO.env}{Your regular ISO.env}
}
\value{
  An .RData file "Reports/Awards/awardsReport.RData" with all
  the data necessary for Sweave to run. 
}
\author{Adrian Dragulescu}
\seealso{\code{\link{FTR.load.ISO.env}}}
\examples{


}