\name{sendEmail}
\alias{sendEmail}
\title{Send emails from R}
\description{Uses the blat utility to send emails from R.}
\usage{
sendEmail(from, to, subject, body, cc=NULL, attachments=NULL,
  links=NULL, html=FALSE)

getEmailDistro(distroName=NULL, projectName="", on.success=TRUE)
}
\arguments{
  \item{from}{A string with the sender's email address}
  \item{to}{A string with the receiver's email addresses, separated by commas.}
  \item{subject}{The email's subject.}
  \item{body}{A vector of characters. Each element will be a line.}
  \item{cc}{Same as the \code{to} field}
  \item{attachments}{A vector of strings with paths to files.}
  \item{links}{A vector of strings with the links.}
  \item{html}{A boolean, if html formatting should be used.  It wraps
    the text, so I suggest not.  However, links to files work. }
  \item{distroName}{A distribution name.  See Notes.}
  \item{projectName}{A project name.  See Notes.}
}
\value{
None.
}
\author{John Scillieri, Adrian Dragulescu}
\note{getEmailDistro is a utility to manage email distribution lists, and
  avoid hardcoding them into code.  All distribution lists are
  maintained in the file:
  \code{S:/All/Risk/Software/R/prod/Utilities/DistroToEmailMap.csv}.
  See this file for examples.


  IMPORTANT:  Make sure you have your Microsoft Outlook configured
  properly.  Go to Tools>Options.  In the Preferences tab, click on the
  E-mail Options button.  The checkbox "Remove extra lines in
  plain text messages" needs to be unchecked.  Microsoft tries to be
  helpful by removing the end-of-line characters, and wrapping the R output.   

}
\seealso{}
\examples{

from <- "OVERNIGHT_REPORTS@constellation.com"
to   <- "adrian.dragulescu@constellation.com"
subject <- "test"
body <- c("My data:\n\n", capture.output(print(iris, row.names=FALSE)))
sendEmail(from, to, subject, body)

getEmailDistro(distroName="PNL")

}
