/* $Header: /home/cvs/src/num/src/random.h,v 1.8 2000/12/05 16:08:25 lowthg Exp $ */
/****************************************************************
**
**	random.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_RANDOM_H )
#define IN_RANDOM_H

#include <n_aiarr.h>

EXPORT_C_NUM int N_MultiGaussDeviate(
	int				*idum, 		//Seed
	int				N,			//Size of the vector
	AIA_DOUBLE_2D	*corr,		//Correlation Matrix
	AIA				*r			//Result as a vector of random number
);

EXPORT_C_NUM double N_BrownianBridgeMin(
	int		*idum, 
	double 	S0,				 //Start Value
	double 	S1,				 //End Value
	double 	sigma, 			 //Volatility
	double 	T				 //Elapsed time between S1 and S2
);

EXPORT_C_NUM double N_BrownianBridgeMax(
	int		*idum, 
	double 	S0,				 //Start Value
	double 	S1,				 //End Value
	double 	sigma, 			 //Volatility
	double 	T				 //Elapsed time between S1 and S2
);

EXPORT_C_NUM int N_BrownianBridge(
	int		*idum,
	int		N,			 //Size of the returned arrays
	double 	*S0,   		 //Start Value
	double 	*sigma,		 //Volatility
	double 	*T,	  		 //Elapsed time between S1 and S2
	double	*Min,		 //Array of Min
	double	*Max,		 //Array of Max
	int		FwdVolIndic, //Interpret vol as a fwd vol rather than a term vol
	int		FwdTIndic 	 //Interpret T as a fwd T rather than a term T
);

EXPORT_C_NUM int N_RandomBit(
	unsigned long 	*piseed	  // seed for random number generation
);

#endif

