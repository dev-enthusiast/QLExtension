# Check if a set of paths violate the most recent set of prohibited 
# groups of nodes. 
#

NYPP.prohibitGroups <- function( paths )
{
  fname <- paste("S:/All/Risk/Data/FTRs/NYPP/ISO/DB/RData/",
                  "nodeGroupConstraints.RData", sep="")
  load(fname)  # load variable grp

  cnames <- c("path.no", "source.name", "sink.name")
  if (any(!(cnames %in% colnames(paths))))
    stop("Missing path.no, source.name or sink.name from your paths.")
  aux <- paths[,cnames]

  aux <- merge(aux, grp, by.x="source.name", by.y="name", all.x=TRUE)
  names(aux)[which(names(aux)=="group")] <- "source.group"

  aux <- merge(aux, grp, by.x="sink.name", by.y="name", all.x=TRUE)
  names(aux)[which(names(aux)=="group")] <- "sink.group"

  ind <- is.na(aux$source.group + aux$sink.group)
  pNo <- aux$path.no[ind]   # path numbers that have NA's
  aux <- subset(aux, !pNo)

  res <- subset(paths, path.no %in% c(pNo, aux$path.no))

  if (nrow(res)!=nrow(paths))
    cat(paste("Dropped ", nrow(paths)-nrow(res), " paths.\n", sep=""))
  
  return( res )
}

##############################################################
# Read the pdf file with the prohibited node groups and save
# it to the DB folder.
#
## filename <- paste("S:/All/Risk/Data/FTRs/NYPP/ISO/DB/PDF/",
##  "att_f_prohibit_grps_poi_pow_init_reconfig_tcc_auctions_AUT2008(CLEAN).pdf",
##    sep="")
# working      
# gsub("[[:graph:]]*[[:alpha:]]+[[:digit:]]{1,4}*[[:graph:]]*", "", x)
.read.NYPP.prohibited.pdf <- function( filename )
{
  source("S:/All/Risk/Software/R/prod/Utilities/read.pdf.R")
  grp <- read.pdf(filename)

  ind.start <- grep("PTID Name   ", grp)     # start of table
  if (length(ind.start)==0)
    stop("Check pdf format.  Things have changed.")
  ind.end <- c(ind.start[-1]-1, length(grp)) # end of table

  aux <- NULL
  for (g in seq_along(ind.start)){
    aux[[g]] <- grp[ind.start[g]:ind.end[g]]
  }
  # clean the list
  aux <- lapply(aux, function(x){ind <- which(x=="")[1];
                                 x <- x[2:(ind-1)]})

  aux <- sapply(aux, function(x){
      namezone <- unlist(strsplit(x, " *[0-9]{5,} *", perl=TRUE))
      return(matrix(namezone, ncol=2, byrow=TRUE))
  })
  ind <- unlist(mapply(rep, 1:length(aux), sapply(aux, nrow)))
  grp <- cbind(data.frame(do.call(rbind, aux)), group=ind)
  names(grp)[1:2] <- c("name", "zone")

  fout <- paste("S:/All/Risk/Data/FTRs/NYPP/ISO/DB/RData/",
                "nodeGroupConstraints.RData", sep="")
  save(grp, file=fout)
  
  return( grp )
}

