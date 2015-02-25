\name{DivideAndConquer-package}
\alias{DivideAndConquer-package}
\docType{package}
\title{
A package for operating on chunks of data too big to fit in memory
}
\description{
Tools to manipulate data too big to fit in memory using the file
system.  The package has three core functions: divide, conquer and finalize.

Divide is used to split a blob of data into separate files on disk.
Data may come from a database, internet, etc.

With data split, one can operate on each files or groups of files at one
time.  Conquer applies a function to each group of files and writes the
result to another file.  This output file can then be the input to
another conquer command and so on.

When the output from the last conquer command is small enough to be
loaded in memory, one can use finalize to operate on this last set of
reduced files.  The result of finalize will be returned in memory.

Different backends can be implemented.  For example it would be useful
to send the processing to a grid using Condor, hadoop, etc.  For now,
everyting is done serially using the local machine, so the
implementation is really simple. 

}
\details{
\tabular{ll}{
Package: \tab DivideAndConquer\cr
Type: \tab Package\cr
Version: \tab 0.0.x\cr
Date: \tab 2009-04-01\cr
License: \tab GPLv3\cr
}

}
\author{
Adrian Dragulescu
}
\references{
}
\keyword{ package }
\examples{
}
