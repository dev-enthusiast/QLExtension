\name{finalize}
\alias{finalize}
\title{Apply a function to a bunch of files and return the results.}
\description{
  Finalize operates a function on a bunch of files and returns the
  results back into memory.  It can be used to aggregate many smaller
  files into bigger ones, or it can be used to subset data from each
  input file. 

  Finalize is different from conquer in that it brings the results in
  memory and that it operates directly on files.  Each file is supposed
  to fit into memory, as well as the aggregated results.  It should be
  seen as the last step of a sequence of conquer commands.
}
\usage{
finalize(files, FUN, agg="rbind", ...)

}
\arguments{
  \item{files}{A vector of pathnames pointing to files on the
    disk.}
  \item{FUN}{The function that needs to be applied to each file.  It
    needs a named argument file=, that will be used as the file name
    from the list keys.}
  \item{agg}{String with the function name that should be applied to
    each function output.}
  \item{...}{Other arguments passed to FUN.}
}
\value{}
\author{Adrian Dragulescu}
\note{}
\seealso{}
\examples{

cFun <- function(file=file, id){
  load(file)  # file contains variable res
  subset(res, ID == id)
}

files = c("file_1.RData", "file_2.RData")

res <- finalize(files, FUN, id=2)


}