\name{FTR.getBindingConstraints}
\alias{FTR.getBindingConstraints}
\title{Get the binding constraints for a NERC region.}
\description{Get the binding constraints for a NERC region.}
\usage{
FTR.getBindingConstraints(region="NEPOOL", type="DA", start.dt=NULL,
  end.dt=NULL, constraint.names=NULL)
}
\arguments{
  \item{region}{A NERC region. Only NEPOOL and NYPP are implemented now.}
  \item{type}{Either "DA" or "RT" for day-ahead or real-time
    constraints. }
  \item{start.dt}{An R Date.}
  \item{end.dt}{An R Date.}
  \item{constraint.names}{A vector of constraint names.  As it will be
    passed to the SQL query, it needs to match exactly the names as they
    appear in the table.  Better get everything if you're not sure of the
    exact spelling.}
}
\value{
  A data frame with at least c("constraint.name", "contingency.name",
  "time").  Column "time" has class POSIXct. 
  
}
\author{Adrian Dragulescu}
\note{Only for NEPOOL and NYPP now.}
%\seealso{\code{\link{FTR.countBindingConstraints}}}
\examples{

BC <- FTR.getBindingConstraints(region="NEPOOL", type="DA")

head(BC)
#          constraint.name contingency.name                time
#1 BAKER_ST_110-510-4_A_LN           Actual 2005-12-01 18:00:00
#2 BAKER_ST_110-510-4_A_LN           Actual 2006-01-17 18:00:00
#3 BAKER_ST_110-510-4_A_LN           Actual 2006-02-24 18:00:00
#4 BAKER_ST_110-510-4_A_LN           Actual 2006-03-05 18:00:00
#5 BAKER_ST_110-510-4_A_LN           Actual 2006-03-06 18:00:00
#6 BAKER_ST_110-510-4_A_LN           Actual 2006-03-07 18:00:00


}
