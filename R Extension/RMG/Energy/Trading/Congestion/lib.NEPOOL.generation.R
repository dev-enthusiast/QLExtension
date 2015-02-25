# Functions to monitor NEPOOL units, etc. 
# 
# Monitor both the Genscape and the Interface flows!
#

# RT Genscape symbols
GENRT_TSDB_SYMBOLS <<- c(
  "GenRT_BearSwamp", 
  "GenRT_BraytonPoint", 
  "GenRT_Canal", 
  "GenRT_LakeRoad", 
  "GenRT_MaineIndependence", 
  "GenRT_Millstone", 
  "GenRT_NEABellingham", 
  "GenRT_Newington_Schiller", 
  "GenRT_NorthfieldMountain", 
  "GenRT_Pilgrim", 
  "GenRT_Seabrook", 
  "GenRT_VermontYankee", 
  "GenRT_Wyman")


############################################################################
# out of merit generation from folder:
# Geographic\NEPOOL\NEPOOL Data\Nepool Transmission Congestion
# can be DA or RT, 
OOMRT_TSDB_SYMBOLS <<- c(
  "NEPL_TC_RT_CONN_RMRMW",
  "NEPL_TC_RT_MAINE_RMRMW",
  "NEPL_TC_RT_NEMA_RMRMW",
  "NEPL_TC_RT_NH_RMRMW",
  "NEPL_TC_RT_RI_RMRMW",
  "NEPL_TC_RT_SEMA_RMRMW",
  "NEPL_TC_RT_VT_RMRMW",
  "NEPL_TC_RT_WCMA_RMRMW")

############################################################################
# out of merit generation from folder:
# Geographic\NEPOOL\NEPOOL Data\Nepool Transmission Congestion
# can be DA or RT, 
VARUPRT_TSDB_SYMBOLS <<- c(
  "NEPL_TC_RT_CONN_VARUPMW",
  "NEPL_TC_RT_MAINE_VARUPMW",
  "NEPL_TC_RT_NEMA_VARUPMW",
  "NEPL_TC_RT_NH_VARUPMW",
  "NEPL_TC_RT_RI_VARUPMW",
  "NEPL_TC_RT_SEMA_VARUPMW",
  "NEPL_TC_RT_VT_VARUPMW",
  "NEPL_TC_RT_WCMA_VARUPMW")



## ALL_UNITS <- list()
## ALL_UNITS[["555"]] <- 

unit.monitor.default <- function()
{


}

unit.monitor.NORTHFIELDMOUNTAIN <- function()
{


}


unit.monitor.SEABROOK <- function()
{
  symb <- "GenRT_Seabrook"

}
