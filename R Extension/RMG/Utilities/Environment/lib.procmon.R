# deal with R procmon issues.  Utilities to log and update procmon files.
#
# .get_ownership
# .get_R_version
# .get_R_procmon_files
# .read_procmon_files
# .replace_R_version
#

##############################################################
# Implement a heuristic to assign procmon jobs to somebody
# Where filename is the full path to the file
# 
.get_ownership <- function(filename)
{
  as.character(sapply(filename, function(fname){
    aux <- tolower(dirname(fname))
    if (grepl("ercot", aux)) {
      "Lori"
    } else if (grepl("trading\\/congestion", aux)) {
      "Adrian"
    } else {
      "RISK"
    }
  })) 
}

##############################################################
# parse the procmon command to 
# figure out the version of R used for this job
#
.get_R_version <- function(command)
{
  as.character(sapply(command, function(cmd){
    cmd <- gsub("\\\\", "/", cmd)
    cmd <- gsub("//", "/", cmd)
    cmd <- tolower(cmd)
    aux <- gsub(".*/r/r-(.*)/bin.*", "\\1", cmd)
    if (length(aux)==0) {
      aux <- NA
    }
    aux
  })) 
}


##############################################################
# Walk the directory and pick up the R jobs
#
.get_R_procmon_files <- function()
{
  DIR <- "H:/gsp_procmon/procs/RMG"
  files <- list.files(DIR, pattern="", full.names=TRUE,
    recursive=TRUE)

  # rm CVS folders
  files <- files[-which(grepl("/CVS/", files))]
  
  # rm TEMPLATE files
  files <- files[-which(tolower(basename(files))=="template")]

  # get the process Status
  res <- ddply(data.frame(filename=files), .(filename), function(x) {
    rLog("Reading ", x$filename)
    #if (x$filename == "H:/gsp_procmon/procs/RMG/R/Reports/Energy/Trading/Congestion/AAD_positions") browser()

    aux <- readLines(x$filename, warn=FALSE)

    # skip matlab jobs
    if (any(grepl("matlabrunexe", aux))) {
      NULL
    } else {
      status <- grep("Status", aux, value=TRUE)
      if (length(status)==0) status <- NA
      data.frame(filename=x$filename, status=status)
    }
  })
  res$status <- gsub(" +|\t", "", res$status)

  res
  
}



##############################################################
# Read a procmon file and make a pairlist out of the file
# directives.
#
.read_procmon_file <- function(filename)
{
  aux <- readLines(filename, warn=FALSE)
  aux <- aux[!grepl("^#", aux)]   # remove comments
  aux <- aux[!aux==""]            # remove empty lines
  aux <- gsub("\t", "  ", aux)
  aux <- strsplit(aux, ": ")

  ind <- which(sapply(aux, length) != 2)
  if (length(ind) > 0) {
    rLog("Cannot split the properties correctly!")
    browser()
  }
  
  res <- lapply(aux, function(x){.trim_whitespace(x[[2]])})
  names(res) <- lapply(aux, function(x){.trim_whitespace(x[[1]])})

 cbind(filename=basename(filename), melt(res))
}

##############################################################
# Read a procmon file and change the R version, resave the file
# 
#
.replace_R_version <- function(filename, newVersion="2.15.1")
{
  rLog("Working on ", basename(filename))
  aux <- readLines(filename, warn=FALSE)
  
  ind <- grep("Command", aux)
  if (length(ind)==0) {
    rLog("Could not find Command line.  Exiting!")
  }

  currentVersion <- .get_R_version(aux[ind])
  aux[ind] <- gsub(currentVersion, newVersion, aux[ind])

  writeLines(aux, filename)
}



##############################################################
# rm whitespace, tabs from front/end of a string
# x is a vector
.trim_whitespace <- function(x)
{
  x <- gsub("^ +", "", x)
  x <- gsub(" +$", "", x)
  x
}




##############################################################
##############################################################
#
#
.main <- function()
{
  require(CEGbase)
  require(reshape)

  #mapuser()
  source("H:/user/R/RMG/Utilities/Environment/lib.procmon.R")

  jobs <- .get_R_procmon_files()
  owner <- .get_ownership(jobs$filename)
  owner <- data.frame(owner=owner, filename=jobs[,"filename"])
  owner <- owner[order(owner$owner), ]

  subset(owner, owner=="Adrian")

  
  CC <- ldply(unique(jobs$filename), .read_procmon_file)
  names(CC)[3] <- "key"

  # unique keys
  sort(unique(CC$key))

  subset(CC, key=="Command")

  

  
  # the owners
  owner <- subset(CC, key=="Owner")
  sort(table(owner$value))
    
  # for xls
  res <- cast(CC, filename ~ key, I, fill=NA)
  res <- res[order(res$Owner), ]
  write.csv(res, "C:/Temp/R_procmon_jobs.csv", row.names=FALSE)
  
  
  subset(CC, key=="Command")
    
  filename <- jobs$filename[3]

  ###########################################################
  # which jobs need to migrate to 2.15.1
  cmds <- subset(CC, key=="Command")
  cmds <- data.frame(owner=.get_ownership(subset(CC, key=="Name")$value), cmds)
  cmds <- data.frame(cmds, version=.get_R_version(cmds$value))

  aux <- subset(cmds[,c("owner", "filename", "version")], owner=="Adrian" & version != "2.15.1")
  names(aux)[2] <- "fname"
  print(aux)

  jobs$fname <- basename(jobs$filename)
  bux <- merge(aux, jobs[,c("fname", "filename")])
  for (filename in bux$filename) {
    .replace_R_version(filename, newVersion="2.15.1")
  }

  ###########################################################
  # which jobs use vanilla
  cmds <- subset(CC, key=="Command")
  cmds <- data.frame(owner=.get_ownership(subset(CC, key=="Name")$value), cmds)
  cmds <- data.frame(cmds, version=.get_R_version(cmds$value))

  aux <- subset(cmds, owner=="Adrian")
  aux[grep("vanilla", aux$value), "filename"]
    
  

  
}





##############################################################
#
## .add_ownership <- function(jobs)
## {
##   jobs$owner <- "RMG"

##   jobs$owner[which(grepl("Energy/Trading/Congestion/",
##                          jobs$filename))] <- "dragulescu"
##   jobs$owner[which(grepl("Environment/build_R_packages",
##                          jobs$filename))] <- "dragulescu"

##   jobs$owner[which(grepl("/PM/ERCOT/", jobs$filename))] <- "simpson"

##   jobs
## }
