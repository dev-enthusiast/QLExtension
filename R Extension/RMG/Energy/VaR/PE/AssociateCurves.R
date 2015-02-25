referenceCurveList = c(
  "COMMOD NG NORRM PHYSICAL",
  "COMMOD NG NOVNIT PHYSICAL",
  "COMMOD NG NORCAN PHYSICAL",
  "COMMOD NG PEPL PHYSICAL",
  "COMMOD NG TRAZN6 NY PHYSICAL",
  "COMMOD NG TENZN6 PHYSICAL",
  "COMMOD NG TRAZN5 PHYSICAL",
  "COMMOD NG WAHA PHYSICAL",
  "COMMOD NG SOBCAL PHYSICAL",
  "COMMOD NG TETSTX PHYSICAL",
  "COMMOD NG TETETX PHYSICAL",
  "COMMOD NG CHICAGO PHYSICAL",
  "COMMOD NG HSCBEA PHYSICAL",
  "COMMOD NG DOMSP PHYSICAL",
  "COMMOD NG TRAS85 PHYSICAL",
  "COMMOD NG DAWN PHYSICAL",
  "COMMOD NG SABHUB PHYSICAL")

unassociatedCurveList = c(
"COMMOD NG CEGT HOTSPRI PHYSICAL",
"COMMOD NG NEWDOMINION PHYSICAL",
"COMMOD NG NNGMID15 PHYSICAL",
"COMMOD NG NNGMID17 PHYSICAL",
"COMMOD NG PMI POOL PHYSICAL",
"COMMOD NG SSTAR FERC",
"COMMOD NG SSTAR GAS-DLY MEAN",
"COMMOD NG SSTAR PHYSICAL")
  
referenceCurves = data.frame(curve.name = referenceCurveList)
unassociatedCurves = data.frame(curve.name = unassociatedCurveList)

library(RODBC)
channel = odbcConnect("CPSPROD", "stratdatread", "stratdatread")

query = paste("SELECT t.COMMOD_CURVE, v.REGION as Policy_Region",
  "FROM FOITSOX.SDB_COMMOD_MARKET_LOCATION T,",
  "FOITSOX.RMG_CPSCURVES s, FOITSOX.RMG_COMMODITIES r,",
  "FOITSOX.RMG_SUBREGIONS u, FOITSOX.RMG_REGIONS v",
  "where pricing_date='27aug2007' and t.COMMOD_CURVE=s.CURVENAME",
  "and s.COMMID=r.COMM_ID and s.SRID=u.SR_ID and u.REGION_ID=v.REGION_ID")
  
regionData = sqlQuery(channel, query)
names(regionData) = c("curve.name", "region")

referenceCurvesAndRegion = merge(referenceCurves, regionData)
names(referenceCurvesAndRegion) = c("parent.curve", "region")
othersWithRegion = merge(unassociatedCurves, regionData)

curvesWithParents = merge(othersWithRegion, referenceCurvesAndRegion,
  all.x=TRUE)

