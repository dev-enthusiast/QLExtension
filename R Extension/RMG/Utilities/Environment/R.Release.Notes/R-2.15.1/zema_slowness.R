
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)

   dfSppMap <- data.frame(
      nameSpp=c('ftr', 'wh', 'han3', 'han4', 'han5', 'lp', 'mc6', 'mc7', 'mc8', 'cbec', 'qrec', 'mv'),
      spp=c('ZM_ERCOT_rt_spp_sp_ftr_cc1','ZM_ERCOT_rt_spp_sp_whccs_cc1','ZM_ERCOT_rt_spp_sp_hlses_unit3',
        'ZM_ERCOT_rt_spp_sp_hlses_unit4','ZM_ERCOT_rt_spp_sp_hlses_unit5','ZM_ERCOT_rt_spp_sp_az_all',
        'ZM_ERCOT_rt_spp_sp_mcses_unit6','ZM_ERCOT_rt_spp_sp_mcses_unit7','ZM_ERCOT_rt_spp_sp_mcses_unit8',
        'ZM_ERCOT_rt_spp_sp_cbec_all','ZM_ERCOT_rt_spp_sp_qalsw_all','ZM_ERCOT_rt_spp_sp_redfish_all')
                      )

   data <- FTR:::FTR.load.tsdb.symbols(dfSppMap[,2], Sys.Date()-1, Sys.Date())
