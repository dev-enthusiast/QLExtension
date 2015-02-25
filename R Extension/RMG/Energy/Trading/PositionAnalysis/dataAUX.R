dataAUX <- function(data, LimCommodCurveTable) {

  data <- merge(data, LimCommodCurveTable) # add LIM Commod Curve
  data <- subset(data, SERVICE_TYPE == "ENERGY")
  
  ind_x <- which(data$AUX_PEAK == ".OFFPEAK")
  if(length(ind_x) >0) {
    data$LIM_COMMOD_CURVE <- as.character(data$LIM_COMMOD_CURVE)
    for (i in 1:length(ind_x)) {
      if( data$QUANTITY_BUCKET_TYPE[ind_x[i]] == "Offpeak" )
        data$LIM_COMMOD_CURVE[ind_x[i]] <- paste(dataDOI$LIM_COMMOD_CURVE[ind_x[i]], data$AUX_PEAK[ind_x[i]], sep = "")
      if(data$TIME_INTERVAL[ind_x[i]] == "Daily")
        data$QUANTITY_BUCKET_TYPE[ind_x[i]] <- "7x24"
    }
  }
  COMMOD_CURVE_BUCKET <- paste(data$LIM_COMMOD_CURVE, data$QUANTITY_BUCKET_TYPE, sep="_")
  data <- cbind(data,COMMOD_CURVE_BUCKET)

  for(i in 1:length(unique(data$COMMOD_CURVE_BUCKET))) {
    ind_x <- which(data$COMMOD_CURVE_BUCKET == unique(data$COMMOD_CURVE_BUCKET)[i])
    ind_y <- unique(data$COMMOD_CURVE[ind_x])
    if(length(ind_y) > 1){
      ind_z <- which(LimCommodCurveTable$LIM_COMMOD_CURVE == data$LIM_COMMOD_CURVE[ind_x][1])
      ind_1 <- which(as.character(data$MAPPED_REGION_NAME[ind_x]) ==
                     as.character(LimCommodCurveTable$MAPPED_REGION_NAME[ind_z[1]]))
      ind_2 <- which(as.character(data$MARKET_SYMBOL[ind_x]) ==
                     as.character(LimCommodCurveTable$MARKET_SYMBOL[ind_z[1]]))
      ind_3 <- which(as.character(data$MAPPED_DELIVERY_POINT[ind_x]) ==
                     as.character(LimCommodCurveTable$MAPPED_DELIVERY_POINT[ind_z[1]]))
      ind_123 <- intersect(intersect(ind_1,ind_2),ind_3)
      data$COMMOD_CURVE[ind_x] <- data$COMMOD_CURVE[ind_x[ind_123[1]]]
    }
  }
  return(data)
}