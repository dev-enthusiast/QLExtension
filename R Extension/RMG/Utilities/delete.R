# Recursively delete the path passed in. Accounts for the stupid R 2.5.0
# convention that
#   //nas-msw-07/rmgapp/PotentialExposure/2007-09-04/overnight/
# is not a valid directory path whereas
#   //nas-msw-07/rmgapp/PotentialExposure/2007-09-04/overnight
# is.  What else would I want when I asked for the first one?!?
#

delete <- function(path){
   endWithSlash = which(regexpr("/$", path) != -1)
   path[endWithSlash] = substr(path[endWithSlash], 1,
    nchar(path[endWithSlash])-1)

   unlink(path, recursive=TRUE)
}
