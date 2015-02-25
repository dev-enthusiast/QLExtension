\name{rLog}
\alias{rLog}
\title{Log output to screen or file. }
\description{Implements logging similar to packages such as Log4j.  It
  is very  rudimentary in that all you can do is set the log level and 
  output based on that level.}
\usage{

setLogLevel(level)

rLog( ... )
rError( ... )
rWarn( ... )
rDebug( ... )
rTrace( ... )

redirectLogOutput(connection="")
}
\arguments{
  \item{level}{A numeric vector specifying the log level.  It can be 0 (no
    logging), 1 (errors), 2 (warnings), 3 (message - this is the
    default),  4 (debug), 5 (trace).}
  \item{...}{Strings to display.}
  \item{connection}{A valid connection.}
}
\value{
A string to display or write to a connection.  Only the commands with
a log level higher than the global logLevel will be displayed. 
}
\author{John Scillieri}
\note{}
\seealso{}
\examples{

setLogLevel(2)  # print only errors and warnings 
rLog("This doesn't get printed.")

rDebug("This is a debug check.")  # silent also

rError("Errors get reported.")


}