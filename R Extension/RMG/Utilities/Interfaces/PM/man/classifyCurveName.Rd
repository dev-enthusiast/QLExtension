\name{classifyCurveName}
\alias{classifyCurveName}
\title{Infer curve classifications from SecDb curve name.}
\description{
Infer curve classifications from SecDb curve name.  For example, find
the bucket, or the market from the curve name.  This is not foolproof,
as there can be exceptions.  Best to use a FOIT SOX compliant database
table, but for small projects, and not too esoteric cases it works.

Please double check your results and report issues, submit a patch. 

}
\usage{
classifyCurveName(curvenames, by=c("bucket", "peak/offpeak",
  "location", "market"))
}
\arguments{
  \item{curvenames}{A vector of SecDb curve names in upper
    case. E.g. "COMMOD PWX 5X16 CT PHYSICAL".}
  \item{by}{The vector of classification factors.  See usage for
    supported values.}
}
\value{Returns a dataframe with new columns showing the
  classifications.  Book keeping is done internally to ensure that the
  ordering of this data.frame will correspond
  to the initial curve.names, so easy cbinding can be performed.

  The "location" column is not supposed to stand for a proper elec
  location!  It is more of a convenience... 
} 
%\seealso{\code{\link{splitBucket}}}
\examples{

# some curves, duplicates are allowed 
curvenames <- c("COMMOD PWX 5X16 CT PHYSICAL", "COMMOD NG EXCHANGE",
  "COMMOD PWX 5X16 CT PHYSICAL")

classifyCurveName(curvenames, by=c("bucket", "peak/offpeak",
  "location", "market"))
#                        curve pkoffpk bucket market location
#1 COMMOD PWX 5X16 CT PHYSICAL    PEAK   5X16    PWX       CT
#2          COMMOD NG EXCHANGE    FLAT   7X24     NG EXCHANGE
#3 COMMOD PWX 5X16 CT PHYSICAL    PEAK   5X16    PWX       CT

}
%\note{}