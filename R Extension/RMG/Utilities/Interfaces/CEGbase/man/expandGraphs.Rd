\name{expandGraphs}
\alias{expandGraphs}
\title{Expand directed graphs given in the link description.}
\description{Given a link description of several directed graphs, expand
the relationship into a table showing the graph level.  The first two
levels returned are always the input data.frame.  The number of levels
is automatically determined by the deepest child.}
\usage{
expandGraphs(X)
}
\arguments{
  \item{X}{A data.frame with two columns c("parent", "child") that
    represents several directed graphs.}
}
\value{
Another data.frame with expanded levels. 
}
\author{Adrian Dragulescu}
\note{}
\seealso{}
\examples{

X <- data.frame(parent = c("A", "B", "B", "E", "C"),
                child  = c("B", "C", "D", "F", "G")
expandGraphs(X)

#   level3 level2 level1 level0
# 1      A      A      A      B
# 2      A      A      B      C
# 3      A      A      B      D
# 4      A      B      C      G
# 5      E      E      E      F

}