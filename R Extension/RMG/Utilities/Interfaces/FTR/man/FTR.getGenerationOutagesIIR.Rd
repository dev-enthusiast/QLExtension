\name{FTR.getGenerationOutagesIIR}
\alias{FTR.getGenerationOutagesIIR}
\title{Get the generation outages for a control area from IIR database.}
\description{Get the generation outages for a control area from IIR database.}
\usage{
FTR.getGenerationOutagesIIR(ctrlarea="ISNE", start.dt=NULL,
  end.dt=NULL, fuel=NULL)
}
\arguments{
  \item{ctrlarea}{A control area from IIR database. For NEPOOL use
    "ISNE", for NYPP use "NYIS", for PJM use "PJM", for Maritimes
    use "MAR", for Ontario use "IMO".  New Brunswick units are under
	"MAR".  Leave NULL if you want all regions.}
  \item{start.dt}{An R Date.}
  \item{end.dt}{An R Date.}
  \item{fuel}{Fuel type.  Leave NULL if you want all outages.}
}
\value{
  A data frame.
}
\author{Adrian Dragulescu}
%\note{Only for NEPOOL and NYPP now.}
%\seealso{\code{\link{FTR.countBindingConstraints}}}
\examples{

# get all the generation outages in NEPOOL since 1997 
GO <- FTR.getGenerationOutagesIIR(ctrlarea="ISNE")

}
