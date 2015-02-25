\name{ftrbids_csv_file}
\alias{ftrbids_csv_file}
\alias{write_ftrbids_csv_file}
\alias{read_ftrbids_csv_file}
\title{Read/Write the bid file for submission to the ISO.}
\description{Read/Write the bid file for submission to the ISO.}
\usage{
  read_ftrbids_csv_file(filename)

  write_ftrbids_csv_file(bids, filename, do.checks=TRUE)
}
\arguments{
  \item{bids}{A data.frame with all the necessary info for bid submission.}
  \item{filename}{Filename for reading/writing.}
  \item{do.checks}{A boolean, should some checks be done on the bids.}  
}
\value{
  A data.frame of bids for \code{read_ftrbids_csv_file}.  A csv file for
  \code{write_ftrbids_csv_file}.
}
\author{Adrian Dragulescu}
%\seealso{\code{\link{FTR.get.CP.for.paths}}}
\examples{



}