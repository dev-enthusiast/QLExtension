\name{get_binding_constraints}
\alias{get_binding_constraints}
\title{Get the binding constraints for a NERC region.}
\description{Get the binding constraints for a NERC region.}
\usage{
get_binding_constraints(region="NEPOOL", type="DA", startDt=NULL,
  endDt=NULL, constraintNames=NULL)
}
\arguments{
  \item{region}{A NERC region. Only NEPOOL and NYPP are implemented now.}
  \item{type}{Either "DA" or "RT" for day-ahead or real-time
    constraints. }
  \item{startDt}{An R Date.}
  \item{endDt}{An R Date.}
  \item{constraintNames}{A vector of constraint names.  As it will be
    passed to the SQL query, it needs to match exactly the names as they
    appear in the table.  Better get everything if you're not sure of the
    exact spelling.}
}
\value{
  A data frame with at least c("constraint.name", "contingency.name",
  "time").  Column "time" has class POSIXct and is in Hour Ending convention. 
}
\author{Adrian Dragulescu}
\note{Only for NEPOOL and NYPP now.}
%\seealso{\code{\link{FTR.countBindingConstraints}}}
\examples{

BC <- get_binding_constraints(region="NEPOOL", type="DA")

head(BC)
#          constraint.name contingency.name                time
#1 BAKER_ST_110-510-4_A_LN           Actual 2005-12-01 18:00:00
#2 BAKER_ST_110-510-4_A_LN           Actual 2006-01-17 18:00:00
#3 BAKER_ST_110-510-4_A_LN           Actual 2006-02-24 18:00:00
#4 BAKER_ST_110-510-4_A_LN           Actual 2006-03-05 18:00:00
#5 BAKER_ST_110-510-4_A_LN           Actual 2006-03-06 18:00:00
#6 BAKER_ST_110-510-4_A_LN           Actual 2006-03-07 18:00:00

BC <- FTR.getBindingConstraints(region="NYPP", type="DA", startDt=Sys.Date())
print(head(BC), row.names=FALSE) 
#             constraint.name      contingency.name                time shadowprice  ptid
# FARRAGUT 138 HUDS_AVE 138 1             BASE CASE 2014-06-24 02:00:00       -0.02 25291
# FRESHKLS 138 WILLWBRK 138 1             BASE CASE 2014-06-24 09:00:00        0.03 25319
# FRESHKLS 138 WILLWBRK 138 1             BASE CASE 2014-06-24 10:00:00        0.02 25319
# FRESHKLS 138 WILLWBRK 138 1             BASE CASE 2014-06-24 11:00:00        0.03 25319
# NIAGARA  230 PACKARD  230 1 TWR:PACKARD 62 & BP76 2014-06-24 11:00:00        1.53 25186
# FRESHKLS 138 WILLWBRK 138 1             BASE CASE 2014-06-24 12:00:00        0.02 25319


}
