`FTR.nodeToCommod` <-
function(nodeNames, ISO.env)
{
  region <- ISO.env$region
  MAP <- merge(ISO.env$MAP, data.frame(name=nodeNames), all.y=T)

  if (any(is.na(MAP$location))){
    rLog("Could not map these nodes: ")
    print(MAP[is.na(MAP$location), ])
  }
  MAP <- MAP[!is.na(MAP$location), ]

  all.locations <- sort(toupper(unlist(secdb.getValueType(paste("Commod PWR",
      region, "Physical"), "locations"))))
  commods <- vector("list", length=nrow(MAP))
  names(commods) <- MAP$location
  for (r in 1:nrow(MAP)){
    if (!(MAP$location[r] %in% all.locations)){
      location <- paste(MAP$location[r], "DA")
    } else {
      location <- MAP$location[r] 
    }
    aux <- secdb.invoke("_Lib Vcentral delivery point", "VCN::EML2CML",
           region, location , "Energy", "7x24" )
    
    if (length(aux)==0){aux <- NA}
    commods[[r]] <- toupper(unlist(aux))
  }

  commods <- melt(commods)
  rownames(commods) <- NULL
  colnames(commods) <- c("curve.name", "location")
  
##   # append the commod name
##   commods$curve.name <- ifelse(is.na(commods$curve.name), commods$curve.name,
##     toupper(paste("COMMOD", substr(commods$curve.name, 1, 3),
##                             substr(commods$curve.name, 8, 100))))
  
  commods <- merge(MAP[,c("location","name")], commods, all.y=T)

  if (any(is.na(commods$curve.name))){
    print("The following curves will get mapped to their respective zones:")
    print(commods$name[is.na(commods$curve.name)])
    aux <- commods[is.na(commods$curve.name),]
    commods <- na.omit(commods)
    aux <- merge(aux, ISO.env$MAP[, c("location", "zone")])
    bux <- merge(ISO.env$MAP[, c("name", "ptid")],
      data.frame(ptid=as.numeric(aux$zone), location=aux[,"location"]))
    # hopefully it won't infinitely recurse below
    aux <- FTR.nodeToCommod(unique(bux$name), ISO.env)
    aux <- merge(bux[c("location","name")], aux[,c("name", "curve.name")])

    commods <- rbind(commods, aux)    
  }
  
  return(commods)
}

