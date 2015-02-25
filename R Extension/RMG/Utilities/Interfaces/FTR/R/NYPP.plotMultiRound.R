# pattern can be a 
#


NYPP.plotMultiRound <- function(paths=.getSuggestedTCCPaths(),
  auctions=NULL, pattern=NULL, ...)
{
  require(lattice)
  
  if (!("label" %in% names(paths)) &
      any(!(c("source.name", "sink.name") %in% names(paths))))
      stop("Please add source.name and sink.name to your paths",
           " or provide a label.")
  
  ind <- grep("auction", names(paths))
  if (length(ind)>0) paths <- paths[,-ind]
  
  .get.round <- function(x){
    sapply(strsplit(x, "-"), function(y)if (length(y)>=3)y[3] else "")}
  
  CP <- FTR.get.CP.for.paths(paths, NYPP)
  CP$round <- .get.round(CP$auction)
  CP <- merge(CP, paths)
  if (!("label" %in% names(paths)))
    CP$label <- paste(CP$sink.name, "/", CP$source.name, sep="")
    
  #CP$sink.name <- factor(CP$sink.name, levels=paths$sink.name)

  if (!is.null(auctions)){
    aux <- subset(CP, auction %in% auctions)
  }
  if (!is.null(pattern)){
    aux <- CP[grep(pattern, CP$auction), ]
  }
  dotplot(auction ~ CP | label, data=aux,
        scales=list(x="free", y="same"))

  
}


.getSuggestedTCCPaths <- function()
{
  paths <- c(61752, 61753, "B/A",
             61752, 61754, "C/A",
             61752, 61758, "G/A",
             61758, 61761, "J/G",
             61761, 61762, "J/K",
             61752, 61845, "NPX/A",
             61755, 61757, "F/D",
             61758, 61759, "H/G")
  paths <- as.data.frame(matrix(paths, ncol=3, byrow=TRUE),
                         stringsAsFactors=FALSE)
  colnames(paths) <- c("source.ptid", "sink.ptid", "label")
  paths[,1:2] <- sapply(paths[,1:2], as.numeric)

  paths <- cbind(path.no=1:nrow(paths), paths, class.type="FLAT")
  
  paths
}
