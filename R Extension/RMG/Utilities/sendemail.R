################################################################################
# Send email from R.
# Where body is as.character.
#  from  <- adrian.dragulescu@constellation.com
#  to <- "frank.hubbard@constellation.com, wang.yu@constellation.com"
#  subject <- "Curves that are missing from Frank's list"
#  body <- "test message" or c("text", "text", "text")
#  cc <- same as to: field
#  attachments <- "file.txt" or c("file1", "file2", "file3")
#  links <- "Link Location" or c("Link 1", "Link 2", "Link 3")
#
sendEmail <- function(from, to, subject, body, cc=NA, attachments=NA, links=NA)
{
#  BLAT <- "\\\\NAS-OMF-01\\cpsapps\\util\\blat.exe"
  BLAT <- "blat.exe"
  MAILSERVER <- "relay1.ceg.corp.net";
  
  command <- paste(BLAT, "-", "-to", shQuote(to), "-server", 
    MAILSERVER, "-s", shQuote(subject), "-f", shQuote(from))
  
  # if we have a cc field  
  if ( !is.na(cc) )
  {
    command = paste(command, "-cc", shQuote(cc))
  }  

  # send any attachments passed in
  if ( !is.na(attachments) )
  {
    command = .appendAttachments(command, attachments)
  }
  
  # handle any links
  if ( !is.na(links) )
  {
    command = paste( command, "-html")
    body = .handleLinks( body, links )
  }
  
  system(command, input=body)
}


################################################################################
# Add an attach flag for each file the user would like to include
#
.appendAttachments <- function(command, attachments)
{
  attachments = .correctPaths(attachments)
  for( attachment in attachments )
  {
    command = paste( command, "-attach", shQuote(attachment) )
  }
  
  return(command)
}


################################################################################
# Based on if the body was passed as a single string or array of strings, 
# take those links and add them to the bottom of the body of the email
#
.handleLinks <- function(body, links)
{
  links = .correctPaths(links)
  
  if (length(body) == 1)
  {
    body = paste(body,"\n\nIncluded Links:\n",sep="", collapse="\n")
    for (link in links)
    {
      body = paste(body, "<a href=\"",link, "\">",link,"</a> ",sep="", 
        collapse="\n")
    }
  } else
  {
    body = append(body, "\nIncluded Links:")
    for (link in links)
    {
      body = append(body,paste("<a href=\"",link, "\">",link,"</a>",sep=""))
    }
  }
  
  body = gsub("\n","<br>",body, fixed=TRUE)
  body = paste(body,"<br>")
}

################################################################################
# Function to correct any paths that R may have munged so blat can find the
# files.
#
.correctPaths <- function(paths)
{
  paths = encodeString(paths)
  if (.Platform$OS.type == "windows")
  {
    paths = gsub("/","\\", paths, fixed=TRUE)
  }
  return(paths)
}
################################################################################
getEmailDistro <- function( distroName=NULL, projectName="", on.success=TRUE )
{
    mapping = read.csv("H:/user/R/RMG/Utilities/DistroToEmailMap.csv")
    
    if( is.null(distroName) )
    {
        callingFile = sys.frames()[[1]]$ofile
        rLog("getEmailDistro called from:", callingFile)
        
        if( is.null(callingFile) )
        {
            distroName = "default"
            
        } else
        {
            distroName = basename( callingFile )
        } 
    }

    if (projectName != ""){mapping <- subset(mapping, project==projectName)}
    
    rowData = mapping[ which(mapping$name==distroName), ]
    if( nrow(rowData) != 1 )
    {
        rowData = mapping[ which(mapping$name=="default"), ]
    }

    if (on.success){
      addresses <- rowData$addresses.on.success
    } else {
      addresses <- rowData$addresses.on.fail
    }

    toList = strsplit( as.character(addresses), ";" )[[1]]
    toList = paste( toList, "@constellation.com", sep="", collapse=", ")
    
    return( toList )
}
