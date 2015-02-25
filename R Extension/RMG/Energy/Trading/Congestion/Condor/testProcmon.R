

require(CEGbase)
require(SecDb)

rLog("Starting at", format(Sys.time()))

symbs <- c("zm_ercot_nodal_fc_load_load_val_zone_systemtotal",  
           "ZM_ERCOT_NODAL_SCED_SP_BTC_LIMIT_CONSTR_NAME_W_TO_N")

startDt <- as.POSIXct(Sys.time() - 24*3600*5)
endDt   <- Sys.time()

res1 <- tsdb.readCurve(symbs[1], startDt, endDt)
rLog("Pulled first at:", format(Sys.time()))

res2 <- tsdb.readCurve(symbs[2], startDt, endDt)
rLog("Pulled second at:", format(Sys.time()))

rLog("Done")
