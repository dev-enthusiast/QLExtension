`FTR.plot.map` <-
function( paths, options )
{
  if (length(paths$mw)==0){paths$mw <- 1}

  if (nrow(paths) < 50){
    options$map$zone <- "all_zones"
    FTR.plot.map.batch(paths, options)
  }
  sPaths <- split(paths, paths$sink.zone)
  for (zone in names(sPaths)){
    options$map$zone <- zone
    .FTR.plot.map.batch(sPaths[[zone]], options)
  }
}

