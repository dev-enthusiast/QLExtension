################################################################################
# This function calculates a cash flow change based on a starting and ending
# exposure and takes into account certain collateral levels.
#
# It defines exposure states that are then used to special-case logic based
# on those states. 
# 	e.g. (We were ITM and moved to OTM, but we're below our threshold)
#
cashFlowChange <- function( 
        startExposure=0, endExposure=0, 
        cashPosted=0, lc_posted=0, cegThreshold=0, 
        cashHeld=0, lc_held=0, cptyThreshold=0 )
{   
    #Clean the NAs if present
    startExposure[ which( is.na(startExposure) ) ] = 0
    endExposure[ which( is.na(endExposure) ) ] = 0
    cashPosted[ which( is.na(cashPosted) ) ] = 0
    lc_posted[ which( is.na(lc_posted) ) ] = 0
    cegThreshold[ which( is.na(cegThreshold) ) ] = 0
    cashHeld[ which( is.na(cashHeld) ) ] = 0
    lc_held[ which( is.na(lc_held) ) ] = 0
    cptyThreshold[ which( is.na(cptyThreshold) ) ] = 0
    
    # Assume our threshold level is negative
    cegThreshold = abs(cegThreshold) * -1
    
    startingState = .getExposureState( startExposure, cegThreshold, cptyThreshold )
    endingState = .getExposureState( endExposure, cegThreshold, cptyThreshold )
    #DEBUG -- cat("SS:", startingState, "ES:", endingState, "\n")
    
    cashFlowChange = NA
    
    # A Cases
    aa = which( startingState == .A & endingState == .A )
    cashFlowChange[aa] = endExposure[aa] - startExposure[aa]
    
    ab = which( startingState == .A & endingState == .B )
    cashFlowChange[ab] = cegThreshold[ab] - startExposure[ab]
    
    ac = which( startingState == .A & endingState == .C )
    cashFlowChange[ac] = cegThreshold[ac] - startExposure[ac]
    
    ad = which( startingState == .A & endingState == .D )
    cashFlowChange[ad] = cegThreshold[ad] - startExposure[ad]
    
    # B Cases
    ba = which( startingState == .B & endingState == .A )
    cashFlowChange[ba] = endExposure[ba] - cegThreshold[ba]
    
    bb = which( startingState == .B & endingState == .B )
    cashFlowChange[bb] = 0
    
    bc = which( startingState == .B & endingState == .C )
    cashFlowChange[bc] = 0
    
    bdNoCH = which( startingState == .B & endingState == .D  & cashHeld == 0)
    cashFlowChange[bdNoCH] = 0 
    bdCH = which( startingState == .B & endingState == .D  & cashHeld != 0)
    cashFlowChange[bdCH] = endExposure[bdCH] - cptyThreshold[bdCH] 
    
    # C Cases
    ca = which( startingState == .C & endingState == .A )
    cashFlowChange[ca] = endExposure[ca] - cegThreshold[ca] 
    
    cb = which( startingState == .C & endingState == .B )
    cashFlowChange[cb] = 0 
    
    cc = which( startingState == .C & endingState == .C )
    cashFlowChange[cc] = 0 
    
    cdNoCH = which( startingState == .C & endingState == .D & cashHeld == 0)
    cashFlowChange[cdNoCH] = 0 
    cdCH = which( startingState == .C & endingState == .D & cashHeld != 0)        
    cashFlowChange[cdCH] = endExposure[cdCH] - cptyThreshold[cdCH] 
    
    # D Cases
    da = which( startingState == .D & endingState == .A )
    ourSide = NA
    ourSide[da] = endExposure[da] - cegThreshold[da]
    daNoCH = which( startingState == .D & endingState == .A & cashHeld == 0 )
    theirSide = NA
    theirSide[daNoCH] = 0
    daCH = which( startingState == .D & endingState == .A & cashHeld != 0 )
    theirSide[daCH] = -1 * cashHeld[daCH]
    cashFlowChange[da] = ourSide[da] + theirSide[da]
    
    dbNoCH = which( startingState == .D & endingState == .B & cashHeld == 0 )
    cashFlowChange[dbNoCH] =  0 
    dbCH = which( startingState == .D & endingState == .B & cashHeld != 0 )
    cashFlowChange[dbCH] = pmax( -1 * cashHeld[dbCH], endExposure[dbCH] - startExposure[dbCH] ) 
    
    dcNoCH = which( startingState == .D & endingState == .C & cashHeld == 0 )
    cashFlowChange[dcNoCH] =  0 
    dcCH = which( startingState == .D & endingState == .C & cashHeld != 0 )
    cashFlowChange[dcCH] = pmax( -1 * cashHeld[dcCH], endExposure[dcCH] - startExposure[dcCH] ) 
    
    ddNoCH = which( startingState == .D & endingState == .D & cashHeld == 0 )
    cashFlowChange[ddNoCH] =  0 
    ddCH = which( startingState == .D & endingState == .D & cashHeld != 0 )
    cashFlowChange[ddCH] = pmax( -1 * cashHeld[ddCH], endExposure[ddCH] - startExposure[ddCH] ) 
    
    if( length( which( is.na( cashFlowChange ) ) ) )
    {
        stop("HOW DID YOU GET HERE!!! ERROR!")
    }
    
    return( cashFlowChange )
}


################################################################################
# Exposure states
.A = "OTM"
.B = "OTM Below Threshold"
.C = "ITM Below Threshold"
.D = "ITM"

.getExposureState <- function( exposure, cegThreshold, cptyThreshold )
{
    exposureState = NA
    
    # Your case is A
    aCases = which( exposure < 0 & exposure < cegThreshold )
    exposureState[aCases] = .A
        
    bCases = which( exposure < 0 & exposure >= cegThreshold )
    exposureState[bCases] = .B
        
    cCases = which( exposure >= 0 & exposure <= cptyThreshold )
    exposureState[cCases] = .C
        
    dCases = which( exposure >= 0 & exposure > cptyThreshold )
    exposureState[dCases] = .D
    
    if( length( which( is.na( exposureState ) ) ) )
    {
        stop("Don't know how you got here... fix your bug")
    }
    
    return( exposureState )
}


.testCashFlowChange <- function()
{
    library(RUnit)

    # Handle NA in exposure
    cashChange = cashFlowChange( 
            startExposure=NA, endExposure=NA,
            cashPosted=999, lc_posted=998, cegThreshold=3, 
            cashHeld=996, lc_held=995, cptyThreshold=994 )
    checkEquals( target = 0, cashChange, 
            paste(" - Failed A->A less, current =", cashChange ) )
    
    # A -> A less
    cashChange = cashFlowChange( 
            startExposure=-5, endExposure=-4,
            cashPosted=999, lc_posted=998, cegThreshold=3, 
            cashHeld=996, lc_held=995, cptyThreshold=994 )
    checkEquals( target = 1, cashChange, 
            paste(" - Failed A->A less, current =", cashChange ) )
    
    # A -> A more
    cashChange = cashFlowChange( 
            startExposure=-5, endExposure=-6,
            cashPosted=999, lc_posted=998, cegThreshold=3, 
            cashHeld=996, lc_held=995, cptyThreshold=994 )
    checkEquals( target = -1, cashChange, 
            paste(" - Failed A->A more, current =", cashChange ) )
    
    # A -> B
    cashChange = cashFlowChange( 
            startExposure=-5, endExposure=-2,
            cashPosted=999, lc_posted=998, cegThreshold=3, 
            cashHeld=996, lc_held=995, cptyThreshold=994 )
    checkEquals( target = 2, cashChange, 
            paste(" - Failed A->B, current =", cashChange ) )
    
    # A -> C
    cashChange = cashFlowChange( 
            startExposure=-5, endExposure=1,
            cashPosted=999, lc_posted=998, cegThreshold=3, 
            cashHeld=996, lc_held=995, cptyThreshold=4 )
    checkEquals( target = 2, cashChange, 
            paste(" - Failed A->C, current =", cashChange ) )
    
    # A -> D
    cashChange = cashFlowChange( 
            startExposure=-5, endExposure=5,
            cashPosted=999, lc_posted=998, cegThreshold=3, 
            cashHeld=996, lc_held=995, cptyThreshold=4 )
    checkEquals( target = 2, cashChange, 
            paste(" - Failed A->D, current =", cashChange ) )
    
    # B -> A 
    cashChange = cashFlowChange( 
            startExposure=-3, endExposure=-6,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=996, lc_held=995, cptyThreshold=994 )
    checkEquals( target = -1, cashChange, 
            paste(" - Failed B->A, current =", cashChange ) )
    
    # B -> B 
    cashChange = cashFlowChange( 
            startExposure=-3, endExposure=-2,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=996, lc_held=995, cptyThreshold=994 )
    checkEquals( target = 0, cashChange, 
            paste(" - Failed B->B, current =", cashChange ) )
    
    # B -> C 
    cashChange = cashFlowChange( 
            startExposure=-3, endExposure=1,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=996, lc_held=995, cptyThreshold=5 )
    checkEquals( target = 0, cashChange, 
            paste(" - Failed B->C, current =", cashChange ) )
    
    # B -> D, no cash held
    cashChange = cashFlowChange( 
            startExposure=-3, endExposure=10,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=0, lc_held=995, cptyThreshold=5 )
    checkEquals( target = 0, cashChange, 
            paste(" - Failed B->D no CH, current =", cashChange ) )
    
    # B -> D, cash held
    cashChange = cashFlowChange( 
            startExposure=-3, endExposure=10,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=1, lc_held=995, cptyThreshold=5 )
    checkEquals( target = 5, cashChange, 
            paste(" - Failed B->D CH, current =", cashChange ) )
    
    # C -> A
    cashChange = cashFlowChange( 
            startExposure=1, endExposure=-10,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=0, lc_held=995, cptyThreshold=5 )
    checkEquals( target = -5, cashChange, 
            paste(" - Failed C->A, current =", cashChange ) )
    
    # C -> B
    cashChange = cashFlowChange( 
            startExposure=1, endExposure=-4,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=0, lc_held=995, cptyThreshold=5 )
    checkEquals( target = 0, cashChange, 
            paste(" - Failed C->B, current =", cashChange ) )
    
    # C -> C
    cashChange = cashFlowChange( 
            startExposure=1, endExposure=2,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=0, lc_held=995, cptyThreshold=5 )
    checkEquals( target = 0, cashChange, 
            paste(" - Failed C->C, current =", cashChange ) )
    
    # C -> D
    cashChange = cashFlowChange( 
            startExposure=1, endExposure=7,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=0, lc_held=995, cptyThreshold=4 )
    checkEquals( target = 0, cashChange, 
            paste(" - Failed C->D No CH, current =", cashChange ) )
    
    # C -> D
    cashChange = cashFlowChange( 
            startExposure=1, endExposure=7,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=1, lc_held=995, cptyThreshold=4 )
    checkEquals( target = 3, cashChange, 
            paste(" - Failed C->D CH, current =", cashChange ) )
    
    # D -> A, No CH
    cashChange = cashFlowChange( 
            startExposure=7, endExposure=-10,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=0, lc_held=995, cptyThreshold=4 )
    checkEquals( target = -5, cashChange, 
            paste(" - Failed D->A No CH, current =", cashChange ) )
    
    # D -> A, CH
    cashChange = cashFlowChange( 
            startExposure=7, endExposure=-10,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=1, lc_held=995, cptyThreshold=4 )
    checkEquals( target = -6, cashChange, 
            paste(" - Failed D->A CH, current =", cashChange ) )
    
    # D -> B, No CH
    cashChange = cashFlowChange( 
            startExposure=7, endExposure=-1,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=0, lc_held=995, cptyThreshold=4 )
    checkEquals( target = 0, cashChange, 
            paste(" - Failed D->B No CH, current =", cashChange ) )
    
    # D -> B, CH
    cashChange = cashFlowChange( 
            startExposure=7, endExposure=-1,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=1, lc_held=995, cptyThreshold=4 )
    checkEquals( target = -1, cashChange, 
            paste(" - Failed D->B CH, current =", cashChange ) )
    
    # D -> C, No CH
    cashChange = cashFlowChange( 
            startExposure=7, endExposure=1,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=0, lc_held=995, cptyThreshold=4 )
    checkEquals( target = 0, cashChange, 
            paste(" - Failed D->C No CH, current =", cashChange ) )
    
    # D -> C, CH
    cashChange = cashFlowChange( 
            startExposure=7, endExposure=1,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=1, lc_held=995, cptyThreshold=4 )
    checkEquals( target = -1, cashChange, 
            paste(" - Failed D->C CH, current =", cashChange ) )
    
    # D -> D, No CH
    cashChange = cashFlowChange( 
            startExposure=7, endExposure=10,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=0, lc_held=995, cptyThreshold=4 )
    checkEquals( target = 0, cashChange, 
            paste(" - Failed D->D No CH, current =", cashChange ) )
    
    # D -> D, CH greater
    cashChange = cashFlowChange( 
            startExposure=7, endExposure=10,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=1, lc_held=995, cptyThreshold=4 )
    checkEquals( target = 3, cashChange, 
            paste(" - Failed D->C CH greater, current =", cashChange ) )
    
    # D -> D, CH lesser
    cashChange = cashFlowChange( 
            startExposure=7, endExposure=5,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=1, lc_held=995, cptyThreshold=4 )
    checkEquals( target = -1, cashChange, 
            paste(" - Failed D->D CH lesser, current =", cashChange ) )
    
    # D -> D, CH lesser, big cash reserve
    cashChange = cashFlowChange( 
            startExposure=7, endExposure=5,
            cashPosted=999, lc_posted=998, cegThreshold=5, 
            cashHeld=10, lc_held=995, cptyThreshold=4 )
    checkEquals( target = -2, cashChange, 
            paste(" - Failed D->D CH lesser big cash, current =", cashChange ) )
        
}

#.testCashFlowChange()
