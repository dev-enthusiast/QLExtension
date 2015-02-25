################################################################################
# Load Tsdb from Commod Curves
#
# Created by Wang Yu, 13-Jan-2006
# Converted to 'pure' R by John Scillieri, 24-Oct-2007


################################################################################
Load.Tsdb.DAM.CommodCurves <- function(CommodCurves, DateParms)
{
    
    #Don't think we need these anymore
    # DateParms$Start = datenum(DateParms$Start)
    # DateParms$End = datenum(DateParms$End)
    
    TsdbCurves = Commod_to_tsdb_DAM_RMG(CommodCurves)
    
    TsdbData = NULL
    for( curveIndex in seq_along(TsdbCurves) )
    {
        curve = TsdbCurves[curveIndex]
        curveData = tsdb.readCurve( curve,                         
                as.Date(DateParms$Start, format="%d%b%Y"), 
                as.Date(DateParms$End, format="%d%b%Y") )
        
        TsdbData[[curveIndex]] = curveData
    }
    
    names(TsdbData) = TsdbCurves
    
    TsdbData = Recognize_Bucket_RMG(TsdbData)
    
    return(TsdbData)
}


################################################################################
#  Covert Commod symbols to TSDB DAM symbols
#
#  commods=c('Commod PWE 5x16 Physical', ...
#          'Commod PWX 5x16 Physical', ...
#          'Commod PWX 2x16 Physical', ... 
#          'Commod PWX 7x8 Physical',  ...
#          'Commod PWJ 5x16 WEST Physical',  ...
#          'Commod PWJ 7x8 WEST Physical', ...
#         );
#
# tsdb_DAM_Symbols is also an array
# tsdb_DAM_Symbols = c( 'NEPOOL_SMD_DA_4000_LMP', ...
#                       'NYPP_DAM_ZONE_LBM_61760' );
#
Commod_to_tsdb_DAM_RMG <- function (commods)
{
    tsdb_DAM_Symbols = NULL
    
    for( currentCommod in seq_along(commods) )
    {
        cCommod = as.character(commods[currentCommod])
        splitName = strsplit(cCommod," ")[[1]]
        
        market = splitName[2]
        
        if(market == "PWX")
        {
            TsdbDAMSymbol = getPWXDAMSymbol( cCommod )
            
        } else
        {
            location = splitName[4]
            
            TsdbDAMSymbol = locationFromCurveMap(location)
            
            if( is.null(TsdbDAMSymbol) )
            {
                stop('Not supported Yet: ', location, " Commod:", cCommod);
            }
        }
        
        tsdb_DAM_Symbols[currentCommod] = TsdbDAMSymbol;
    }                                                         
    
    return(tsdb_DAM_Symbols)
}


################################################################################
locationFromCurveMap <- function( location )
{
    location = tolower(location)
    
    tsdbName = switch( location,        
            west = "NYPP_DAM_ZONE_LBM_61752",
            gen = "NYPP_DAM_ZONE_LBM_61753",
            cen = "NYPP_DAM_ZONE_LBM_61754",
            nor = "NYPP_DAM_ZONE_LBM_61755",
            MHK_VL = "NYPP_DAM_ZONE_LBM_61756",
            cap = "NYPP_DAM_ZONE_LBM_61757",
            hudv = "NYPP_DAM_ZONE_LBM_61758",
            mill = "NYPP_DAM_ZONE_LBM_61759",
            dunw = "NYPP_DAM_ZONE_LBM_61760",
            nyc = "NYPP_DAM_ZONE_LBM_61761",
            li = "NYPP_DAM_ZONE_LBM_61762",
            hq = "NYPP_DAM_ZONE_LBM_61844",
            npx = "NYPP_DAM_ZONE_LBM_61845",
            oh = "NYPP_DAM_ZONE_LBM_61846",
            pjm = "NYPP_DAM_ZONE_LBM_61847" 
            )
    
    return(tsdbName)
    
}

################################################################################
# Assign Buckets to TsdbInfo returned by LoadTsdb_RMG.m
# TsdbInfo = 
# 
#                  Young: {}
#               HisStart: 731886
#              TimeStamp: [19296x1 double]
#               TsdbHist: [19296x17 double]
#            TsdbSymbols: {1x46 cell}
#     TsdbSymbols_Unique: {1x17 cell}
#      TsdbSymbols_Index: [1x46 double]
#
Recognize_Bucket_RMG <- function(TsdbData)
{
    
#    [Y, M, D, H ] = datevec(TsdbInfo.TimeStamp);
#    Days = datenum(Y,M,D);
    for( Ccounter in seq_along(names(TsdbData)) )
    {
        
        SplitTsdbSymbol = strsplit( names(TsdbData)[Ccounter] ,'_')[[1]]
        
        # NERC region is the first for at least NYPP, NEPOOL, ERCOT, MISO and PJM
        Region = SplitTsdbSymbol[1];  
        
        Buckets = QuantityBucketPrimitives(Region);
        
        startDate = as.Date(TsdbData[[Ccounter]]$time[1])-1
        endDate = as.Date(TsdbData[[Ccounter]]$time[nrow(TsdbData[[Ccounter]])])+1

        bucketsByTime = bucketsForTime( Region, Buckets, startDate, endDate )
        
        TsdbData[[Ccounter]] = merge( TsdbData[[Ccounter]], bucketsByTime, all.x = TRUE)
        
        TsdbData$TsdbBuckets[[Ccounter]] =   Buckets;
    }
    
    return(TsdbData)
}


################################################################################
bucketsForTime <- function( region, buckets, startDate, endDate )
{
    maskData = NULL

    for( bucket in buckets )
    {
        cat(region, bucket, as.character(startDate), as.character(endDate), "\n")
        mask = secdb.getBucketMask( region, bucket, startDate, endDate )
        if(is.null(maskData))
        {
            maskData = mask
            
        } else
        {
            maskData = merge(maskData, mask, all=TRUE)
        }
    }
    
    maskData$bucket = NA
    for( bucket in buckets )
    {
        maskData$bucket[which(maskData[[bucket]])] = bucket
    }
    
    return(maskData[, c("time", "bucket")])
}


################################################################################
QuantityBucketPrimitives <- function( Region )
{
    Region = tolower(Region)
    QBT = switch( Region,
            alb=c ( '5x16', '2x16H', '7x8' ),
            ecar = c( '5x16', '2x16H', '7x8' ),
            iemo = c( '5x16', '2x16H', '7x8' ),
            main = c( '5x16', '2x16H', '7x8' ),
            mapp = c( '5x16', '2x16H', '7x8' ),
            nepool = c( '5x16', '2x16H', '7x8' ),
            nypp = c( '5x16', '2x16H', '7x8' ),
            pjm = c( '5x16', '2x16H', '7x8' ),
            serc = c( '5x16', '2x16H', '7x8' ),
            spp = c( '5x16', '2x16H', '7x8' ),
            weather = c( '7x24' ),
            wscc = c( '6x16', '1x24H', '6x8' ),
            c( '7x24' )
            )
    
    return(QBT)
}


################################################################################
getPWXDAMSymbol <- function( commod )
{
    splitName = strsplit(commod," ")[[1]]
    
    if( length(splitName) == 4 )
    {
        location = 'HUB';
    } else 
    {
        location=splitName[4];
    }
    
    Parms.Pref  = 'NEPOOL_SMD_DA_';
    Parms.Appe =  '_LMP'; 
    
    if( location == 'BOS' )
    {
        NodeID = 4008;
    
    } else
    {
        SecDB_Info = secdb.getValueType( 'NEPOOL Report Manager', 
                'location info query' )
        locationNames = lapply(SecDB_Info, function(z) {return(z$"Location Name")})
        correctLocationNumber = which( toupper(locationNames) == toupper(location) )
        NodeID = SecDB_Info[[correctLocationNumber]]$ID;
    }
    
    TsdbDAMSymbol = paste( Parms.Pref, NodeID, Parms.Appe, sep="")

    return( TsdbDAMSymbol )
}

