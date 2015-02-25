# Calculate the correct path to the user directory
#
# Written By: John Scillieri
# Last Updated By: John Scillieri - 04/11/07

# In development this will return the user's directory
# and in production it should be replaced with the path to S
#
# Return - the path to the base directory to find files.
getBaseDir <- function()
{
  hDrive = "//NAS-MSW-07/"
  userName = Sys.info()['login']
  
  #Returns the full path
  path = paste(hDrive,userName,"/",sep="")
}
