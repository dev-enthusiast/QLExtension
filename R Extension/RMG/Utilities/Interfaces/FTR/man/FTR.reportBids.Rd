\name{FTR.reportBids}
\alias{FTR.reportBids}
\title{Generate an Sweave report analyzing the bids we intend to
  submit.} 
\description{Generate an Sweave file from a template to produce
  a the data necessary for the presentation before the auction.
  This collection of functions saves an RData file with all the
  info that the Sweave template file needs.
}
\usage{
FTR.reportBids( filepath, ISO.env)
}
\arguments{
  \item{filepath}{Filepath to the submitted bids file.}
  \item{ISO.env}{Your regular ISO.env}
}
\value{
  An .RData file "Reports/Awards/bidsReport.RData" with all
  the data necessary for Sweave to run. 
}
\author{Adrian Dragulescu}
\seealso{\code{\link{FTR.load.ISO.env}}}
\examples{


}