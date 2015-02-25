\name{conquer}
\alias{conquer}
\title{Apply a function to a bunch of files and save the results.}
\description{Conquer operates a function on a bunch of files and saves
  the results to another file.  It can be used to aggregate many smaller
  files into bigger ones, or it can be used to subset data from each
  input file and save the results to an output file for further
  processing.

  Conquer uses rbind to aggregate the results of the different input
  files by key, and therefore it is strongly encouraged that the
  function FUN returns only ONE object that supports rbind (be it a
  data.frame, matrix, zoo, etc.)
    
}
\usage{
conquer(keys, FUN, preFileName="", overwrite=TRUE, ...)

}
\arguments{
  \item{keys}{A list with the files that need to be processed.  Each
    list element contains a vector of pathnames pointing to files on the
    disk.  The list names are the keys to be processed, and used in the
    output file names.  For each key, the data should fit into memory,
    otherwise the process fails.  }
  \item{FUN}{The function that needs to be applied to each file.  It
    needs a named argument file=, that will be used as the file name
    from the list keys.}
  \item{preFileName}{String to add to the output file name.  You should
    pass the full path + the file prefix.  Given the right permissions,
    directories will be created if they don't exist.}
  \item{overwrite}{Boolean specifying if the output files will be
    overwritten if they already exists.  When overwrite==FALSE, it
    allows conquer to accumulate the output from different keys.}
  \item{...}{Other arguments passed to FUN.}
}
\value{}
\author{Adrian Dragulescu}
\note{}
\seealso{}
\examples{

cFun <- function(file=file, id){
  load(file)  # file contains variable QQ 
  subset(QQ, ID == id)
}

keys = list(
  "A"=c("file_1.RData", "file_2.RData"),
  "B"=c("file_3.RData", "file_4.RData"))

preFileName <- "C:/Temp/out_"

conquer(keys, FUN, preFileName=preFileName, id=2)

# two files will be output: C:/Temp/out_A.RData and C:/Temp/out_B.RData
# that contain the subset of 

}