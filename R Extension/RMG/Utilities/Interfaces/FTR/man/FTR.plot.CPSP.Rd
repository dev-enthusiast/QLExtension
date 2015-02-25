\name{FTR.plot.CPSP}
\alias{FTR.plot.CPSP}
\title{Plot historical clearing prices (CP) and settle prices (SP) for a
  set of paths.}
\description{Plot side by side in a lattice plot, the clearing prices
  (CP) and the settle prices (SP) for a set of paths.}
\usage{
FTR.plot.CPSP(CPSP, bid.limits=NULL, runName="", orderHistorical=TRUE, 
  maxPlotsPage=10, save=TRUE, maxMonths=NULL, layOut=NULL, ...)
}
\arguments{
  \item{CPPS}{A data.frame with at least c("path.no", "month", "CP", "SP")
    and maybe "panelName" column names.  This data.frame gets created by
    \code{FTR.get.CP.for.paths}.}
  \item{bid.limits}{An optional data.frame with columns c("path.no",
    "min.bid", "max.bid").  Two vertical lines will be drawn in each
    panel.}
  \item{runName}{A string that will be prepended to the pdf filename.}
  \item{orderHistorical}{Boolean.  If \code{TRUE} the months will be ordered
    historically (year/month), otherwise it will be ordered by
    month/year.  Historical trends are easier to spot with
    \code{orderHistorical=TRUE} while seasonality is easier to spot with
    \code{orderHistorical=FALSE}.}
  \item{maxPlotsPage}{Usually 10 is the most you can see things
    clearly.  The layout depends on this number.  For a high number,
    you'll get two rows of panels.}
  \item{save}{Boolean.  If FALSE, it will print on the screen.}
  \item{maxMonths}{How many historical months you want to keep.}
  \item{layOut}{A vector of length two to force a particular layout.}
  \item{...}{Other arguments passed to xyplot, e.g. \code{xlab},
    \code{main}, etc.}
}
\value{
None.  A multi-page pdf plot will be generated in the current directory.
}
\author{Adrian Dragulescu}
\seealso{\code{\link{FTR.get.CP.for.paths}}}
\examples{

# Load an ISO.env, say NEPOOL which contains NEPOOL$CP
#
FTR.load.ISO.env("NEPOOL")
paths <- data.frame(path.no=1:2, source.ptid=4001, sink.ptid=4000,
                    class.type=c("ONPEAK", "OFFPEAK"))

# get the CPSP data.frame
CPSP <- FTR.get.CP.for.paths(paths, NEPOOL, add.SP=TRUE, panelName=TRUE)

# do the pdf plot
FTR.plot.CPSP(CPSP)


}