\name{FTR.switchSourceSink}
\alias{FTR.switchSourceSink}
\title{Reverse the path direction for a set of paths.}
\description{
  Reverse the path direction for a set of paths.  The function reverses
  the \code{"buy.sell"} and \code{"is.counterflow"} variables.  It
  switches all the variables with names \code{source} to variables with
  names \code{sink}. 
}
\usage{
FTR.switchSourceSink( paths, path.no )
}
\arguments{
  \item{paths}{Usual paths data.frame.  You need at least the columns
    \code{c("path.no", "source.ptid", "sink.ptid", "class.type")}}.
  \item{path.no}{A vector of path.no for which the direction will be
    reversed. }
}
\value{
  The paths data.frame back. 
}
\author{Adrian Dragulescu}
%\seealso{\code{\link{FTR.get.hSP.for.paths}}}
\examples{

aux <- subset(NYPP$MAP, type=="ZONE")
paths <- data.frame(path.no=1:(nrow(aux)-1), class.type="FLAT",
  source.ptid=aux$ptid[1], sink.ptid=aux$ptid[2:nrow(aux)])
paths <- paths[1:10,]

# reverse the first 5 paths 
FTR.switchSourceSink(paths, 1:5)
%     path.no class.type source.ptid sink.ptid source.name sink.name
% 61        1       FLAT       61753     61752      GENESE      WEST
% 71        2       FLAT       61754     61752      CENTRL      WEST
% 81        3       FLAT       61755     61752       NORTH      WEST
% 91        4       FLAT       61756     61752      MHK VL      WEST
% 101       5       FLAT       61757     61752      CAPITL      WEST
% 6         6       FLAT       61752     61758        WEST    HUD VL
% 7         7       FLAT       61752     61759        WEST    MILLWD
% 8         8       FLAT       61752     61760        WEST    DUNWOD
% 9         9       FLAT       61752     61761        WEST    N.Y.C.
% 10       10       FLAT       61752     61762        WEST    LONGIL



}