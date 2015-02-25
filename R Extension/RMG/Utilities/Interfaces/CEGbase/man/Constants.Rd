\name{Constants}
\alias{Constants}
\alias{maparea}
\alias{mapuser}
\alias{mapdev}
\alias{mapprod}
\alias{getroot}
\title{Define various utilities for CEG R environment.}
\description{Define various utilities for CEG R environment.}
\usage{
  maparea(area="prod", verbose=TRUE)

  mapuser(verbose=TRUE)

  mapdev(verbose=TRUE)

  mapprod(verbose=TRUE)

  getroot(verbose=TRUE)
}
\arguments{
  \item{area}{a character string pointing to a defined area.  See the
    global environment .ENV_CEG defined in etc/Rprofile.site. ("user",
    "dev", "prod", etc.)}
  \item{verbose}{a boolean}
}
\value{

  Function \code{getroot} returns the root directory of the R scripts
  depending on the environment variable \code{R_ENV_CEG}. 
[}
\details{
  The functions to change the environment \code{mapuser},
  \code{mapdev}, \code{mapprod} modify the path to R libraries and set
  the \code{R_ENV_CEG} environment variable to the coresponding area.
  By default, a user starting R will be mapped to "prod". 

  For example, the function \code{mapuser} sets the R library path to
  local packages and the scripts will point to your local CVS area.
  Also, when executing scripts at CEG it is recommended to have the
  package \code{CEGbase} loaded. 
}
\author{Adrian Dragulescu}
\note{}
\seealso{}
\examples{

  mapuser()
  getroot()  # returns "H:/user/"

  mapprod()
  getroot()  # returns "I:/ja/prod/"

}