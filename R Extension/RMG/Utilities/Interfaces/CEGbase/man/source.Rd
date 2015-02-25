\name{source}
\alias{source}
\title{Read R code from a File or a Connection}
\description{Enhances the functionality of the \code{base:::source} by
  allowing tuncated paths from the CEG codebase.}
\usage{
  source(file, verbose=getOption("verbose"), ...)
}
\arguments{
  \item{file}{a character string giving the pathname of the
          file or URL to read from.}
  \item{verbose}{same as in \code{\link[base]{source}}.}
  \item{\ldots}{other arguments to pass to \code{\link[base]{source}}.}
}
\details{  
  When developing your own scripts, if you want to use the CEG
  infrastructure and promote your code to production, you will need to
  use relative paths from the root of the R CVS tree.
}
\author{Adrian Dragulescu}
\note{}
\seealso{}
\examples{

  mapuser()   # maps you to the user area
  source("R/Utilities/read.pdf.R")  # sources H:/user/R/RMG/Utilities/read.pdf.R

}