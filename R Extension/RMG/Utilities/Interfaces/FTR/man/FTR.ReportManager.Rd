\name{FTR.ReportManager}
\alias{FTR.ReportManager}
\title{Load the SecDb ReportManager for a NERC region.}
\description{
  Load the SecDb ReportManager for a NERC region.  This will contain the
  SecDb location name, the ptid, node name, zone name, etc. 
}
\usage{
  FTR.ReportManager(region="NEPOOL")
}
\arguments{
  \item{region} the name of the region.  
}
\value{
  A data frame usually referred as MAP in the ISO.env. See also. 
}
\author{Adrian Dragulescu}
\note{Only NEPOOL, NYPP, and PJM implemented for now.}
\seealso{\code{\link{FTR.load.ISO.env}}}
\examples{

MAP <- FTR.ReportManager(region="NEPOOL")

head(MAP)
#          location ptid                name zone                tsdb.symbol
#1513 FRNKLNSQ.10CC  321 UN.FRNKLNSQ11.510CC 4005 NEPOOL_SMD_DA_321_CongComp
#1514 FRNKLNSQ.11CC  322 UN.FRNKLNSQ11.511CC 4005 NEPOOL_SMD_DA_322_CongComp
#1515  FRNKLNSQ.9CC  323  UN.FRNKLNSQ11.59CC 4005 NEPOOL_SMD_DA_323_CongComp
#1516     CDEC.AETN  324 UN.CDEC    115 AETN 4004 NEPOOL_SMD_DA_324_CongComp
#1518 ALTRESCO.ALTR  326 UN.ALTRESCO115 ALTR 4007 NEPOOL_SMD_DA_326_CongComp
#1519     EDDY.AMOS  327 UN.EDDY    34.5AMOS 4002 NEPOOL_SMD_DA_327_CongComp


}

