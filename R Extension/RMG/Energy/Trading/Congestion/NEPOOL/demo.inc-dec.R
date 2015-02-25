# You may want to check the Nodal Load Weigths report in Nepool
# http://www.iso-ne.com/markets/hstdata/hourly/nodal_load_wghts/ for
# how many MW you may want to DEC at one node.
#
#
# For how to price a given inc/dec see below
#
# commit



require(CEGbase)
require(FTR)
source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/lib.inc-dec.R")
#load NEPOOL MAP
load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")
#nfind <- function(x) NYPP$MAP[grep(toupper(x), NYPP$MAP$name),]
nfind <- function(x) MAP[grep(toupper(x), MAP$name),]
subset(MAP, ptid==332)


PM:::searchEvents(" Orrin")

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.transmission.R")
res <- search_zipped_st_outages(startMth="2011-03", endMth="2011-04",
  equipment="320-", tempDir="C:/Temp")


conNames <- c("SOTHNGTN_12X_12X_XF", "SOTHNGTN1610-1        A LN",
  "SOTHNGTN1910-IND      A LN",
  "SOTHNGTN1910          A LN", "SOTHNGTN2X              XFMR")
da <- FTR.getBindingConstraints(type="DA", start.dt=as.Date("2007-01-01"),
  end.dt=as.Date("2011-03-01"), constraint.names=NULL)
sort(unique(da$constraint.name))
aux <- da[grepl("^DEPOT", da$constraint.name), ]


da1 <- subset(da, constraint.name %in% conNames)
da1 <- cbind(mkt="DA", da1)

rt <- FTR.getBindingConstraints(type="DA", start.dt=as.Date("2011-01-01"),
  end.dt=as.Date("2011-03-01"), constraint.names=NULL)
rt1 <- rt[grepl("SOTHN", rt$constraint.name),]
rt1 <- cbind(mkt="RT", rt1)

dart <- rbind(da1, rt1)
write.csv(dart, file="C:/Temp/southington.csv", row.names=FALSE)


sort(unique(rt$constraint.name))




# Has Westbrook been off?
(NEPOOL_SMD_RT_14177_MargLoss - NEPOOL_SMD_RT_4001_MargLoss)/NEPOOL_SMD_RT_4000_LMP
(NEPOOL_SMD_RT_1216_MargLoss - NEPOOL_SMD_RT_4001_MargLoss)/NEPOOL_SMD_RT_4000_LMP




find.price.arbitrage(MAP, hDays=3)  # should run the ideas sooner ...

find.price.arbitrage(MAP, hDays=10)  # should run the ideas sooner ...

paths <- data.frame(path.no=1:2, source.ptid=484, sink.ptid=4551,
  class.type=c("ONPEAK", "OFFPEAK"))
CP <- FTR.get.CP.for.paths(paths, NEPOOL)
CP <- merge(paths, CP)
print(CP[,-1], row.names=FALSE)


source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/INC.plot.R")
INC.plot(61845, 4011)  # NPX to Roseton

INC.plot(4002, 618)


#-------------------------- MAINE PATHS
## INC.plot(642, 432)  # Maine, UN.HARRIS 115 --> YARMOUTH.YAR4

## INC.plot(629, 15464)   # DEBLOIS over STETSON even stronger?  # very strong surge on 1Apr09
##INC.plot(4140, 4169)  # LD.BUCKSPRT115 --> LD.WOODSTCK34.5, play on RT issues

# works well when INC system is good, prices are high!
#INC.plot(15464, 536)  # STETSON 34 (BHE) --> UN.CHEMICAL PERC  

INC.plot(432, 440)  # UN.HARRIS  115 HAR1 --> UN.W_BUXTON34.5HIRM

#INC.plot(642, 11424)     # UN.YARMOUTH22  YAR4 to UN.BOIS_CAS11.5RUMH, FTR?!  FOREVER!
#INC.plot(15464, 4001)    # UN.STETSON 34.5STET to MAINE   # crush stetson with 15 mw!
#INC.plot(642, 4141)     # UN.YARMOUTH22  YAR4 to LD.BELFAST 115
#INC.plot(642, 4138)     # UN.YARMOUTH22  YAR4 to LD.RUMFD_IP115
#INC.plot(629, 424)      # UN.DEBLOIS 115 WORC --> UN.POWERSVL115 GNRT , check out!

#INC.plot(1255, 4168)   # UN.RUMFD_IP18.0RUMF --> LD.RILEY   115
#INC.plot(642, 432) # UN.YARMOUTH22  YAR4 --> UN.HARRIS  115 HAR1

#INC.plot(642, 636) # UN.YARMOUTH22  YAR4 --> UN.WYMAN_HY115 WYM1 
INC.plot(13703, 642) #  UN.JAY     13.8VCG1 --> UN.YARMOUTH22  YAR4 crush it with 20 MW!

INC.plot(4001, 1216) # Maine -> MIS


INC.plot(331, 4161)  # Riley to Livermore Falls

INC.plot(367, 14177)

INC.plot(367, 14177) # Cape -> Westbrook

#INC.plot(642, 4163)  # UN.YARMOUTH22  YAR4 --> LD.BROWN_ST115

INC.plot(4001, 4002) # NH to ME 
INC.plot(4001, 4003) # ME to VT  when RT spikes (so you should dec the system),
#INC.plot(4003, 4001) # ME to VT, when you should inc the system
INC.plot(4156, 4001) # scot paper

#INC.plot(332, 15464) # UN.ELLSWRTH34.5BARH --> STETSON

#INC.plot(367, 4183) # UN.CAPE    115 CAP4 --> LD.HNKLY_PD115

#INC.plot(760, 13627)  # UN.LOUDEN  34.5NGOR --> LD.ME_YK   115


#INC.plot(445, 536) # UN.ENFLD_ME115 IND5 --> UN.CHEMICAL115 PERC; got burned 

#INC.plot(629, 15386) # UN.DEBLOIS 115 WORC --> LD.BOGGY_BK46  ; if Rebel_Hill blows up

#INC.plot(460, 4135) # UN.WINSLOW 115 LCKW --> LD.MAXCYS  115   # a rare 1 day spike
#INC.plot(432, 636)   # UN.HARRIS  115 HAR1 --> UN.WYMAN_HY115 WYM1
#INC.plot(4083, 15464)  # LD.HARRINGT115 --> UN.STETSON 34.5STET   OFFPEAK?
#INC.plot(15464, 4083)  # UN.STETSON 34.5STET  --> LD.HARRINGT115, Harr is only 5MW load
INC.plot(616, 4078)  # UN.CHSTERME46  WENF --> LD.GRAHAM  46, 130 MW of load at Graham


#-------------------------- BOSTON paths
INC.plot(10349, 10347)  # UN.KENDALL 13.8KND3 to UN.KENDALL 13.8KND1, FTR?!  FOREVER!!
#INC.plot(10349, 4085)
#INC.plot(4878, 1616) # LD.MAYNARD 115 110A LD --> UN.MYSTIC  18.1MYS9  ! was negative forever
#INC.plot(1616, 4878) # UN.MYSTIC  18.1MYS9 --> LD.MAYNARD 115 110A LD blows up in RT
INC.plot(4817, 1616) #LD.CHATHAM 115  about 50MW load  --> UN.MYSTIC  18.1MYS9 

INC.plot(4008, 1616)   # Mystic9
INC.plot(503, 1478)    # MysticJet --> Mystic8
INC.plot(1478, 1616)   # Mystic8 --> Mystic9  when DA MY9 tanks and doesn't shows up RT

INC.plot(4085, 4108)  # Lexington --> Trapelo



#INC.plot(4118, 4085)  # LD.W_FRMHAM115 --> LD.LEXINGTN115
INC.plot(547, 555)   # UN.WARD_HIL23  RESN --> UN.SEABROOK24.5SBRK  Boston Interface congested
#INC.plot(1616, 4261) # this one works
#INC.plot(1616, 4335)



INC.plot(1616, 4251)  # MYSTIC --> SANDY POND

INC.plot(4008, 4000)

#-------------------------- CENTRAL MASS paths
#INC.plot(4324, 4408)

#-------------------------- NEW HAMPSHIRE PATHS
INC.plot(1625, 342)   # UN.GR_RIDGE16  GRAN --> JACKMAN.BIO  # worked well for so LONG!
#INC.plot(570, 4412)        # LD.CHESTER 115  --> UN.SMITH_HY6.9 SMTH # FOREVER!
#INC.plot(768, 489) # UN.GARVINS 34.5GARV --> UN.MERIMACK13.8MK1
#INC.plot(4368, 440) # LD.DEERFLD569 --> UN.W_BUXTON34.5HIRM across the NHME interface
#INC.plot(4266, 4389)  # LD.SLAYTNHL13.8 --> LD.SACO_VLY115  # start this again!
#INC.plot(4389, 4266)  #  LD.SACO_VLY115 --> LD.SLAYTNHL13.8 if RT blows up, this prints ... 

INC.plot(768, 490)  # Merrimack -> Garvins

#INC.plot(4447, 4484)  # LD.IRASBURG48 --> LD.LITTLTN 115   good EW play 
#INC.plot(4409, 4357)  # major spike in RT
#INC.plot(4412, 4002) # LD.CHESTER 115 --> NH
#INC.plot(4397, 4399)  # LD.GREGGS  115 --> LD.JACKMAN 34.5  
#INC.plot(4376, 4377)   # AMHERST --> SCOBIE  good EW play  super BAD play!  Blew the node up!

#INC.plot(440, 508)  # UN.W_BUXTON34.5HIRM --> UN.NEWINGTN24.0NEWI, offpeak NHME
INC.plot(555, 4251)  # Seabrook --> Sandy Pond, if RT NH spikes

INC.plot(4002, 14595)  # does this still work? NH -> Paris

#INC.plot(330, 570)  # UN.PEMIGWAS34.5AYER --> UN.SMITH_HY6.9 SMTH  # crushed it with 12MW!
#INC.plot(4003, 4002) # East West ... 

INC.plot(4000, 4002)

INC.plot(337, 4002)







#-------------------------- North Shore
#INC.plot(10344, 4351)   # GOLDN_RK --> E_METHUE


#-------------------------- CONNECTICUT paths
#INC.plot(4551, 482)     # HADDAM --> MIDDLETN, some RT issues on LN1620 in WestCT
#INC.plot(4617, 482)     # LD.SALISBRY69 --> UN.MIDDLETN22  MID4  # onpeak Western CT, offpeak reverses!
#INC.plot(478, 4530)    # UN.MIDDLETN115 MI10 --> LD.SCITICO 115 # Western CT and RT problems ! GOOD
#INC.plot(324, 4491)   # UN.CDEC    115 AETN to LD.MANCHSTR115, good for FTRs
#INC.plot(4617, 1032)   # LD.SALISBRY69 to UN.BPT_ENER16  BHCC  # NOT ALLOWED!
INC.plot(412, 4004)     # UN.FALSVL  69  FALS to  CT; Do the opposite if RT prices are high!
#INC.plot(4438, 4598)     # LD.OLD_TOWN13.8 --> LD.PEACABLE115, RT problems in NOR
INC.plot(4598, 4438)     # LD.PEACABLE115 --> LD.OLD_TOWN13.8RT
#INC.plot(4600, 396)  # NORWALK --> UN.DEVON   115 DV10 line work on 1710 until 5/28
#INC.plot(4547, 4542)     # LD.DOOLEY  13.2 --> LD.FRMNGTON23 # Western CT
INC.plot(4544, 572)     # LD.BERLIN  115 --> UN.S_MEADOW115 SO11  # prints when WestCT binds
#INC.plot(482, 493)     #  UN.MIDDLETN22 MID4 --> UN.MONTVLLE13.8MON5 # prints when WestCT binds
#INC.plot(482, 4491)    # pure WestCT play, in RT losses money.  Good for an FTR.
INC.plot(4491, 482)    # inverse WestCT play LD.MANCHSTR115 --> UN.MIDDLETN22  MID4
# INC.plot(1032, 4596)   # UN.BPT_ENER16  BHCC --> LD.TRIANGLE115 # very violent in RT NOR
#INC.plot(492, 4276)    # UN.MONTVLLE69  MO10 --> LD.WOOD_RV 115 CT import, EW interface
INC.plot(485, 1342)    # UN.MILSTONE24.0MIL2 --> UN.LAKE_RD 21  LRD1, CT import
#INC.plot(1342, 484)    # UN.LAKE_RD 21  LRD1 --> UN.MILSTONE24.0MIL2, reversed CT import
INC.plot(484, 4600)  # Norwalk --> Devon, prints when NRST binds

#INC.plot(412, 1032) # UN.FALSVL  69  FALS --> UN.BPT_ENER16  BHCC for 1x8

INC.plot(4004, 484) # UN.MILSTONE24.0MIL2 --> UN.MILSTONE24.0MIL3

INC.plot(484, 1342)    # Mill2 --> UN.LAKE_RD 21  LRD1 when millstone3 is out!
#INC.plot(4570, 4551)  # LD.CHIPINHL115 --> LD.HADDAM  23, problems on 1810-3
#INC.plot(4566, 4490)   # LD.CANTON  23 --> LD.N_BLOOM 115, work until 10/23 - 10/25

INC.plot(14614, 4004)

INC.plot(4004, 4000)   


#-------------------------- Rhode Island paths
INC.plot(4005, 1630)   # UN.RISE    18.0RISE
#INC.plot(1342, 322)  #UN.LAKE_RD 21  LRD1 --> UN.FRNKLNSQ11.511CC
#INC.plot(4912, 4910)  #LD.WARREN  115 5X LD --> LD.WARREN  115 1X LD, spiked in RT
#INC.plot(4276, 4275)   # LD.WOOD_RV 115 --> LD.TIVERTON115
INC.plot(1226, 4005)   # UN.Tiverton binds
#INC.plot(4217, 350)     # LD.PAWTUCKT115 --> UN.BRAYTNPT18.0BRA1  # super dangerous!!
INC.plot(4217, 4218)     # LD.PAWTUCKT115 --> LD.VALLEY  115
#INC.plot(4217, 577)     # LD.PAWTUCKT115 --> UN.SOMERSET115 SO6
#INC.plot(4216, 4220) # LD.WSHNGTN 115 --> LD.RVERSIDE115 when PAWTUCKET acts up, this opens! good for FTR!
#INC.plot(4280, 1630) # LD.DRUMROCK115 --> UN.RISE    18.0RISE, when issues with 172S
#INC.plot(4005, 321)  # RI --> UN.FRNKLNSQ11.510CC  for 1x8 works well
#INC.plot(321, 4005)  # UN.FRNKLNSQ11.510CC --> RI  for 1x16 works well

INC.plot(4255, 4005)  # KentCounty --> RI

INC.plot(1226, 4005)  # UN.TIVERTON18.0TIVR --> RI
#INC.plot(322, 1226)   # UN.FRNKLNSQ11.511CC --> UN.TIVERTON18.0TIVR
INC.plot(4005, 1226)  #for an FTR

INC.plot(4004, 4005)

INC.plot(1005, 1226)  # DIGHTON --> UN.TIVERTON18.0TIVR # in DA?!

INC.plot(4297, 350)



#-------------------------- Seacoast paths
#INC.plot(10038, 508)  # LD.GREATBAY115 --> UN.NEWINGTN24.0NEWI
#INC.plot(508, 10038)  # UN.NEWINGTN24.0NEWI --> LD.GREATBAY115

#-------------------------- SEMA paths
INC.plot(366, 365)   # between CANAL1 and CANAL2   FOREVER!!!
#INC.plot(507, 1005)  # UN.BELLNGHM13.2NEA --> UN.DIGHTON  FOREVER!!!
#INC.plot(4291, 353) 
#INC.plot(4235, 4202)  # LD.KINGSTON115 --> LD.PLYMOUTH115 lower SEMA play
## INC.plot(1226, 577)   # TIVERTON & SOMMERSET
# INC.plot(1030, 4226)  # OTIS
#INC.plot(4225, 4824) # LD.FALMTH_T115 --> LD.CROSS_RD115 111X LD
#INC.plot(4202, 4837)  # LD.PLYMOUTH115 --> LD.E_HOLBRK115 1X LD
INC.plot(507, 540)     # UN.BELLNGHM13.2NEA --> UN.POTTER  115 POT2
INC.plot(4004, 1691)
#INC.plot(540, 507)   # UN.POTTER  115 POT2 did not blow up in RT !! twice in Oct
#INC.plot(4006, 16255) # SEMA --> LD.CANDLE  13.2
#INC.plot(4839, 537) # LD.E_WEYMTH115 1X LD --> PILGRIM  # nice outage ... 
INC.plot(4198, 4837) # STOUGHTN --> HOLBROOK

INC.plot(537, 16255) # PILGRIM --> LD.CANDLE  13.2 # interesting node to play!
INC.plot(537, 4241)  # PILGRIM --> PINE ST (+40 MW load)
#INC.plot(4214, 352)   # LD.BATES_ST115 --> UN.BRAYTNPT20.0BRA3
INC.plot(4000, 4006)


#-------------------------- VERMONTH paths
#INC.plot(4460, 4467)    # LD.ASCUTNEY44 --> LD.VERNONRD115
#INC.plot(329, 611)      # UN.ASCUTNEY44  ASCU --> UN.VT_YK   20.9VTYA
#INC.plot(598, 611)      # UN.NEW_HAVN44  VERD --> UN.VT_YK   20.9VTYA, when NWVT binds, 10/24-10/29
                        # do it one day early for 7x24
#INC.plot(4469, 611)     # LD.BENINGTN69 --> UN.VT_YK   20.9VTYA
INC.plot(4330, 611)     # LD.BENINGTN69 --> UN.VT_YK   20.9VTYA
#INC.plot(4469, 4464)    # LD.BENINGTN69 --> LD.CHELSEAV115
#INC.plot(598, 4446)     # UN.NEW_HAVN44  VERD --> LD.STJHNSBY115
#INC.plot(363, 4446)      # UN.QUEENCTY13.8BURL --> LD.STJHNSBY115
#INC.plot(4463, 4458)     # LD.HARTFDVT44 --> LD.BLISVILL115
#INC.plot(4446, 4444)     # LD.STJHNSBY115 --> LD.SO_HERO 115 (not mapped/wait)
#INC.plot(335, 611)     # UN.BELLWSFL115 BELL --> UN.VT_YK   20.9VTYA -- blows up when here is
                        # hub congestion, issues with Flag Pond.

INC.plot(4003, 4006)





#-------------------------- WESTERN MA paths
#INC.plot(4504, 4007)  # WOODLAND
#INC.plot(4479, 4007)  # continues until 3/16, ASHBURN
#aux = INC.plot(379, 4007)   # COBBLE MT
#aux=INC.plot(630, 4007, 3)   # W_SPRFLD13.8
#aux = INC.plot(4527, 4526)  # Springfield MA, BRECKWOD115 --> E_SPRFLD115
#INC.plot(628, 323)
INC.plot(4527, 4526)  # LD.BRECKWOD115  -->  LD.E_SPRFLD115 # A good option!
#INC.plot(4526, 4489)  # until there are problems at Ludlow, 10/14, 10/21, 10/24, 10/27

#INC.plot(4521, 4530)  # LD.SOUTHWCK115  -->  LD.SCITICO 115
#INC.plot(538, 4262)   # EW interface, UN.FLAGG_PD13.8PINE --> LD.PRAT_J  69, outage of I135S

INC.plot(4804, 1210) # LD.AMHRST  115 WEST LD --> UN.MILLENUM16  MILN  # to look!

INC.plot(4489, 4007)

INC.plot(538, 4000)

"UN.EDGAR   18.0EDGR"

# if Brayton Pt output is about 400 MW, and load +17K, flow on A127-6 binds.
# the path below makes money
#INC.plot(4477, 538)     # LD.PAXTON  115 --> UN.FLAGG_PD13.8PINE  

#INC.plot(359, 4310)  # BEARSWMP.BSW1 --> LD.ADAMS   23, RT onpeak BearSwamp prints!
INC.plot(4310, 359)  # LD.ADAMS   23 --> BEARSWMP.BSW1, RT offpeak BearSwamp tanks!


INC.plot(4011, 4000)  # Roseton --> Hub

#INC.plot(4615, 1210) # LD.PODICK  115 --> UN.MILLENUM16  MILN
#INC.plot(4507, 4506) # LD.PLAINFLD115 --> LD.BLANFORD115, big blowouts from 11/07. 
#INC.plot(4251, 4479)  #  LD.SANDY_PD23 --> LD.ASHBRNHM115, offpeak 326 issues


#--------------------------- INTERFACES 
INC.plot(4011, 4017)  # inc Roseton, dec Northport 1385  # exposes me to CT IMP!
INC.plot(4000, 4011)  # inc Hub, dec Roseton
INC.plot(4000, 4010)  # dec Salisbury
INC.plot(4014, 4017)  # Shoreham -> Northport

#--------------------------- ZONES
INC.plot(4000, 4004)

INC.plot(4002, 4001)

#INC.plot(4328, 4264) # Litchfield --> Comerford  # Check for tomorrow!
INC.plot(4264, 4328) # Comerford -> Litchfield   # Check for tomorrow!

## INC.plot(4264, 4379)  # Comeford to Greggs, play on WardHill, ends 3/16!

## INC.plot(4262, 538)   # EW interface, Pratts Junction -> Flag Pond

## INC.plot(4122, 625)   # NORWOOD --> WEST MEDWAY

## INC.plot(4523, 4489)  # Ludlow to Piper, East Springfield congestion

trd <- list()
#-------------------------- MAINE PATHS
## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=332,  bucket="1X8")))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4001, bucket="1X8")))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1216, bucket="1X24", mw=15)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=629,   bucket="1X24", mw=15)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4002, mw=20, hrs=c("1-8"))))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4001, mw=20, hrs=c("1-8"))))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4002, mw=40, bucket="1X24")))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4001, mw=40, bucket="1X24")))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4001,  mw=25, bucket="1X16")))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=1216,  mw=25, bucket="1X16")))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4002, bucket="1X8", mw=50)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4001, bucket="1X8", mw=50)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4001, mw=75, bucket="1X16")))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4002, mw=75, bucket="1X16")))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4002, mw=50, bucket="1X8")))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4001, mw=50, bucket="1X8")))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=14177, mw=15, bucket="1X16")))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=367,   mw=15, bucket="1X16")))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4002, mw=30, hrs=c("6-20"))))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4001, mw=30, hrs=c("6-20"))))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=557,  mw=30, hrs=c("15-21"))))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4001, mw=30, hrs=c("15-21"))))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=440,  bucket="1X16", mw=15)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=432,  bucket="1X16", mw=15)))

#-------------------------- BOSTON paths
## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=10347, bucket="1X24", mw=20)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=10349, bucket="1X24", mw=20)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1616, bucket="1X16", mw=50)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4008, bucket="1X16", mw=50)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1478, bucket="1X16", mw=40)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4008, bucket="1X16", mw=40)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4006, bucket="1X16", mw=50)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4008, bucket="1X16", mw=50)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=555, bucket="1X24", mw=35)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=547, bucket="1X24", mw=35)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1616, bucket="1X16", mw=50)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4008, bucket="1X16", mw=50)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1616, bucket="1X16", mw=25)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4001, bucket="1X16", mw=25)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1616, bucket="1X16", mw=25)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4156, bucket="1X16", mw=25)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1616, bucket="1X16", mw=25)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4102, bucket="1X16", mw=25)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1616, bucket="1X16", mw=25)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4103, bucket="1X16", mw=25)))


## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4108, bucket="1X24", mw=15)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4085, bucket="1X24", mw=15)))


#-------------------------- new HAMPSHIRE PATHS
## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1625,  bucket="1X16", mw=15)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=342, bucket="1X16", mw=15)))

trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=14595,   bucket="1X16", mw=25)))
trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4002,   bucket="1X16",  mw=25)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4006, bucket="1X16", mw=25)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4002, bucket="1X16", mw=25)))


#-------------------------- CONNECTICUT paths
## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=485,  bucket="1X16", mw=40)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4600, bucket="1X16", mw=40)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4004, bucket="1X16", mw=25)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=412,  bucket="1X16", mw=25)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1342, hrs=c("11-21"), mw=50)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=485,  hrs=c("11-21"), mw=50)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4006, bucket="1X16", mw=15)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4004, bucket="1X16", mw=15)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4000, bucket="1X16", mw=65)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4004, bucket="1X16", mw=65)))

#trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1342, bucket="1X16", mw=100)))
#trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4004, bucket="1X16", mw=100)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1691, bucket="1X16", mw=50)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4004, bucket="1X16", mw=50)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=572, hrs=c("17-21"), mw=20)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4544, hrs=c("17-21"), mw=20)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1032, bucket="1X8", mw=20)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=412,  bucket="1X8", mw=20)))

#-------------------------- Rhode Island paths
## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=322, bucket="1X8")))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=1342, bucket="1X8")))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1630, bucket="1X16", mw=30)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4005, bucket="1X16", mw=30)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=321,  bucket="1X8", mw=21)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4005, bucket="1X8", mw=21)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4000, bucket="1X24", mw=50)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4005, bucket="1X24", mw=50)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4005,  bucket="1X24", mw=20)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4255,  bucket="1X24", mw=20)))


#-------------------------- Seacoast paths
## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=508, bucket="1X24", mw=23)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=10038, bucket="1X24", mw=23)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=10038, bucket="1X16", mw=23)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=508, bucket="1X16", mw=23)))



#-------------------------- SEMA paths
## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=365, bucket="1X24", mw=35)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=366, bucket="1X24", mw=35)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1005, bucket="1X24")))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=507, bucket="1X24")))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4202,  bucket="1X16", mw=20)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4235, bucket="1X16", mw=20)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=16255, bucket="1X16", mw=8)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=537,  bucket="1X16", mw=8)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4006,  bucket="1X16", mw=50)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4000,  bucket="1X16", mw=50)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1691,  bucket="1X16", mw=100)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4000,  bucket="1X16", mw=100)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=1691,  bucket="1X16", mw=25)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4202,  bucket="1X16", mw=25)))


## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=537,  bucket="1X24", mw=15)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4839, bucket="1X24", mw=15)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=352,  bucket="1X24", mw=25)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4214, bucket="1X24", mw=25)))


#-------------------------- VERMONT paths
## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=611,  bucket="1X24")))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=329, bucket="1X24")))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=611, bucket="1X24", mw=21)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=598, bucket="1X24", mw=21)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4006,  mw=30, bucket="1X16")))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4003,  mw=30, bucket="1X16")))

#-------------------------- WESTERN MA paths
## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4007, hrs=c("14-21"), mw=35)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4008, hrs=c("14-21"), mw=35)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4526, bucket="1X16", mw=10))) # pays RT! 
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4527, bucket="1X16", mw=10))) 

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4000, bucket="1X16", mw=50)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4007, bucket="1X16", mw=50)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=359, mw=15, hrs=c("17-22"))))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4310, mw=15, hrs=c("17-22"))))

#-------------------------- OTHER paths
## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4017, bucket="1X16", mw=25)))
## trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4014, bucket="1X16", mw=25)))

## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4000, bucket="1X8", mw=50, maxPrice=42)))
## trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4000, bucket="1X8", mw=25))) 



##-------------------------------------for Matt Marsh -------------------------
#trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4001, bucket="1X8", mw=100, book="PWRSTR")))


##-------------------------------------for Kevin Telford  -------------------------
#trd <- c(trd, list(list(inc.dec="DECREMENT", ptid=4010, bucket="1X16", mw=25, book="NSINCDEC",maxPrice=150)))
#trd <- c(trd, list(list(inc.dec="INCREMENT", ptid=4001, bucket="1X16", mw=25, book="NSINCDEC", minPrice=50)))


source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/lib.inc-dec.R")
res <- format.next.day.trades(trd, save=TRUE, asOfDate=Sys.Date()+1)

UN.PARIS   34.5GRPW,DECREMENT,1x16,,,25,300,2,VIRTSTAT
.Z.NEWHAMPSHIRE,INCREMENT,1x16,,,25, 10,2,VIRTSTAT


pnl <- calc_portfolio_pnl(trd)  # the PnL of the INC/DEC portfolio for tomorrow
report_portfolio_pnl(pnl, MAP)


out <- check.node.mapping(res, email=FALSE)

source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/lib.inc-dec.R")
check.node.mapping(res, email=TRUE)


#######################################################################
# PRICE some virtuals
#######################################################################

source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/lib.inc-dec.R")

days <- as.Date(c("2012-03-03"))

trd <- get_booked_virtuals(days=days, books="VIRTSTAT")
pnlVV <- price_pfolio_incdec(trd)


 



searchEvents(days=as.Date("2008-06-10"))


searchEvents("353")



# MIDDLETON INC NOT CLEARING?!
aux <- INC.plot(482, 493)          #  UN.MIDDLETN22 MID4 --> UN.MONTVLLE13.8MON5 
aux <- aux[which(aux[,4]!=0), ]    #  rt prices are zero if not in yet
aux <- aux[which(format(index(aux), "%H") %in% 8:23), ]
avg <- aggregate(aux, format(index(aux), "%Y-%m-%d"), mean)
(PnL.dec.493 <- 25*16*(avg[,4]-avg[,3]))
(PnL.inc.482 <- 25*16*(avg[,1]-avg[,2]))




# check the hours RT blew up
dRT=aux[,3] - aux[,4]
table(format(index(dRT[which(dRT > 10)]), "%H"))


# check the hours RT/DA blew up
dRT=aux[,3] - aux[,1]
table(format(index(dRT[which(dRT > 20)]), "%H"))


# offpeak hours stats
dRToff <- subset(dRT, as.numeric(format(index(dRT), "%H")) %in% 1:7)
aggregate(dRToff, )






hrs <- FTR.bucketHours(start.dt=as.POSIXct("2010-05-01"), end.dt=as.POSIXct("2010-12-31 23:00:00"))
cast(hrs, class.type ~ ., sum, value="hours")











































########################################################################
########################################################################
require(SecDb); require(zoo)







source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/toHourly.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/lib.inc-dec.R")

ptids <- c(563, 1030, 4225, 4224, 10571)
ptids <- NULL
aux <- get.incdec.hprices(buckets=c("1X8", "1X16"), ptids=ptids,
  do.split=TRUE, do.melt=FALSE, add.RT5M=TRUE)
bPP <- aux[[2]]
#bPP.0 <- aux[[2]]; hPP.0 <- aux[[1]]

options <- NULL
options$asOfDate <- Sys.Date()
options$trades.file <-
  "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/inddec.trades.RData"

#------------------------------------------------------------
# trigger.1 = see if you can trade if there is persistence,
#   if a inc/dec worked today, will it work tomorrow...
# trigger.2 = see if there is panic in DA after a strong RT day
#
TRD.PnL <- inc.get.trading.signals( bPP,
  c("trigger.1", "trigger.2", "trigger.3"))

NXT <- next.day.trades( TRD.PnL, options)

merge(MAP[,c("ptid", "name", "zone", "area")], NXT, all.y=TRUE)

format.next.day.trades( NXT, ptids=c(388),
                       quantity=15)








#################################################################################
#################################################################################
# Check the inc-dec PnL

pnlFileArchive <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/AAD_PnL_history.RData"
load(pnlFileArchive)

aux <- PnL.Day[["2009-04-08"]]

pnl <- unlist(sapply(PnL.Day, "[", "NEPOOL.incdec"))


## # get the history for the proposed trades
## hNXT <- merge(TRD.PnL, NXT)


## #X <- bPP[[1]]


## NXT.1 <- NXT[c("11809", "11822"), ]


## sum(TRD$PnL, na.rm=TRUE)


## # plot a node
## windows()
## node <- "1X8_388"
## x <- bPP[[node]][,1]-bPP[[node]][,2]
## plot(x, type="o", col=c("red"), plot.type="single",
##      main=node, cex.main=1, ylab="RT / DA")
## abline(h=0, col="gray")
## abline(h=c(-10,10), lty=2, col="gray")


## acf(as.numeric(na.omit(x)))



## ## source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.incdec.PnL.R")
## ## FTR.incdec.PnL(TRD, "NEPOOL", hPP.0)




## # calculate basic statistics
## BS <- sapply(bPP, inc.basic.stats)
## BS[,1:5]

## (ind <- which(BS["avg.spread.all", ] > BS["sd.spread.all", ]))
## (ind <- which(abs(BS["avg.spread.last3", ]) > 10))
## (ind <- which(abs(BS["avg.spread.last1", ]) > 10))
## if (length(ind)>0) print(BS[,ind])




## attr(bPP, "varname") <- "bucket"
## aux <- melt(bPP)


source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.loadPositions.R")
FTR.loadPositions(books=c("NESTAT", "VIRTSTAT"), asOfDate=(Sys.Date()-1))






