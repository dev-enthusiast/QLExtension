################################################################################
# Throws an error if a given function is not found in the specified package's
# namespace. Useful for testing if the interface to a package has changed.
#
testPackageForFunctions <- function(packageName, functions)
{
  library(packageName, character.only=TRUE)
  for( funCall in functions )
  {
    fullName = paste(packageName, "::", funCall, sep="")
    eval(parse(text=fullName))
  }

  detach(pos = match(paste("package", packageName, sep=":"), search()))
}