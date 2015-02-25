\name{xlsWriteSheet}
\alias{xlsWriteSheet}
\title{Write a data.frame or a list to an xls sheet.}
\description{Write a data.frame or a list to and xls sheet.  A
  data.frame is written column by column to preserve column types.  A
  list is written cell by cell, therefore it is much slower.  However, 
  with a list you can write ragged arrays.

  Several calls to this function makes possible the addition of several
  tables on the same sheet, etc.

  On exit, special care is taken to kill the excel process that was open for
  file writing. 
}
\usage{
xlsWriteSheet(x, file, sheetname="Sheet1", top.cell="A1", 
  visible=FALSE, col.names=TRUE, clearContents=TRUE, propertyList=NULL)
}
\arguments{
  \item{x}{ A data.frame or a list.}
  \item{file}{ A string with the filename to output.}
  \item{sheetname}{ A string with the sheet name you want to write to.}
  \item{top.cell}{ Where to start writing in case \code{x} is a
    data.frame.  Ignored if \code{x} is a list.}
  \item{visible}{ Set to TRUE if you want to see the xls getting
    populated.  Rarely useful for debugging purposes.}
  \item{col.names}{ If TRUE, write the column names of the input
    data.frame \code{x}.  Ignored if \code{x} is a list.}
  \item{clearContents}{ If TRUE, the entire sheet will be cleared before
    writing begins.}
  \item{propertyList}{ A list for formatting the sheet.  Only several
    options are available, until we figure how to implement them in a
    generic way.  See the examples.}
}
\value{
None.
}
\author{Adrian Dragulescu, Jing Bai}
\note{}
\seealso{}
\examples{



}