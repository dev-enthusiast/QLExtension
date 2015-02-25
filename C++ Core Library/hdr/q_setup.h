/* $Header: /home/cvs/src/quant/src/q_setup.h,v 1.8 2000/05/22 12:17:32 goodfj Exp $ */
/****************************************************************
**
**	q_setup.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_Q_SETUP_H )
#define IN_Q_SETUP_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif
#if !defined( IN_MEMPOOL_H )
#include	<mempool.h>
#endif

EXPORT_C_QBASIC int Q_CleanCurves(
	MEM_POOL	*MemPool,
	Q_VECTOR	*Fwds,			// Past values filtered
	Q_VECTOR	*FwdT,	  		// Checked for out-of-order points
	Q_VECTOR	*FwdsFiltered,	// Filtered fwds (normalized by spot) returned
	Q_VECTOR	*FwdTFiltered,	// Filtered fwd times returned
	Q_VECTOR	*Vols,			// Past values filtered
	Q_VECTOR	*VolT,			// Checked for out-of-order points
	Q_VECTOR	*VolsFiltered,	// Filtered vols returned
	Q_VECTOR	*VolTFiltered, 	// Filtered vol times returned
	Q_VECTOR	*Rd,			// Past values filtered, converted to DFs
	Q_VECTOR	*RdT,			// Checked for out-of-order points
	Q_VECTOR	*Discounts,		// Filtered DFs returned
	Q_VECTOR	*DiscTimes,		// Filtered DF times returned
	double		Spot			// Spot, used to normalize fwds
	);

EXPORT_C_QBASIC int Q_CleanCurves2(
	MEM_POOL	*MemPool,
	DT_CURVE	*Fwds,			// Past values filtered, normalized by spot
	Q_VECTOR	*FwdsFiltered,	// Filtered fwds (normalized by spot) returned
	Q_VECTOR	*FwdTFiltered,	// Filtered fwd times returned
	DT_CURVE	*Vols,			// Past values filtered
	Q_VECTOR	*VolsFiltered,	// Filtered vols returned
	Q_VECTOR	*VolTFiltered,	// Filtered vol times returned
	DT_CURVE	*Rd,			// Past values filtered, converted to DFs
	Q_VECTOR	*Discounts,		// Filtered DFs returned
	Q_VECTOR	*DiscTimes,		// Filtered DF times returned
	DATE        PricingDate,	// Pricing Date
	double		Spot			// Used to normalized fwds
	);

EXPORT_C_QBASIC int Q_DualCleanCurves(
	MEM_POOL	*MemPool,
	DT_CURVE	*Fwds,
	Q_VECTOR	*FwdsFiltered,
	Q_VECTOR	*FwdTFiltered,
	DT_CURVE	*Fwds2,
	Q_VECTOR	*Fwds2Filtered,
	Q_VECTOR	*FwdT2Filtered,
	DT_CURVE	*Vols,
	Q_VECTOR	*VolsFiltered,
	Q_VECTOR	*VolTFiltered,
	DT_CURVE	*Vols2,
	Q_VECTOR	*Vols2Filtered,
	Q_VECTOR	*VolT2Filtered,
	DT_CURVE	*XVols,
	Q_VECTOR	*XVolsFiltered,
	Q_VECTOR	*XVolTFiltered,
	DT_CURVE	*Rd,
	Q_VECTOR	*Discounts,
	Q_VECTOR	*DiscTimes,
	DATE		PricingDate,
	double		Spot,
	double		Spot2
	);
									  
EXPORT_C_QBASIC int Q_NormalizeForwards(
	MEM_POOL			*MemPool,
	Q_VECTOR			*Fwds,			// Past values filtered
	Q_VECTOR			*FwdT,	  		// Checked for out-of-order points
	Q_VECTOR			*FwdsFiltered,	// Filtered fwds (normalized by spot) returned
	Q_VECTOR			*FwdTFiltered,	// Filtered fwd times returned
	double				Spot			// Spot, used to normalize fwds
	);

EXPORT_C_QBASIC int Q_FilterVols(
 	 MEM_POOL			*MemPool,
 	 Q_VECTOR			*Vols,			// Past values filtered
 	 Q_VECTOR			*VolT,			// Checked for out-of-order points
 	 Q_VECTOR			*VolsFiltered,	// Filtered vols returned
	 Q_VECTOR			*VolTFiltered	// Filtered vol times returned
	);

EXPORT_C_QBASIC int Q_RatesToDiscounts(
	MEM_POOL			*MemPool,
	Q_VECTOR			*Rd,			// Past values filtered, converted to DFs
	Q_VECTOR			*RdT,			// Checked for out-of-order points
	Q_VECTOR			*Discounts,		// Filtered DFs returned
	Q_VECTOR			*DiscTimes		// Filtered DF times returned
	);

EXPORT_C_QBASIC int Q_NormalizeFwdCurve(
	MEM_POOL	*MemPool,
	DT_CURVE	*Fwds,			// Past values filtered, normalized by spot
	Q_VECTOR	*FwdsFiltered,	// Filtered fwds (normalized by spot) returned
	Q_VECTOR	*FwdTFiltered,	// Filtered fwd times returned
	DATE        PricingDate,	// Pricing Date
	double		Spot			// Used to normalized fwds
	);

EXPORT_C_QBASIC int Q_FilterVolCurve(
    MEM_POOL	*MemPool,
    DT_CURVE	*Vols,				// Past values filtered
    Q_VECTOR	*VolsFiltered,		// Filtered vols returned
    Q_VECTOR	*VolTFiltered,		// Filtered vol times returned
	DATE		PricingDate			// Pricing Date
	);

EXPORT_C_QBASIC int Q_RateCurveToDiscounts(
	MEM_POOL			*MemPool,
	DT_CURVE			*Rd,			// Past values filtered, converted to DFs
	Q_VECTOR			*Discounts,		// Filtered DFs returned
	Q_VECTOR			*DiscTimes,		// Filtered DF times returned
	DATE                PricingDate		// Pricing Date
	);

int Q_FwdFwdRates ( 
	double              time,		 // time in the future
	MEM_POOL			*MemPool,	 // pre-allocated memory pool
	Q_VECTOR			*tempdisc,	 // discount curve
	Q_VECTOR			*rdtimes,	 // nodes
	Q_VECTOR            *fwdrates,   // forward forward rates
	Q_VECTOR			*fwdtimes    // adjusted nodes
	);

#endif

