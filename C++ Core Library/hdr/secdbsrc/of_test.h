/****************************************************************
**
**	OF_TEST.H	- Test object function header information
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/of_test.h,v 1.4 2000/12/19 17:20:46 simpsk Exp $
**
****************************************************************/

#if !defined( IN_OF_TEST_H )
#define IN_OF_TEST_H


/*
**	Define test security data structure
*/

struct TEST_SECURITY_DATA
{
	char	Underlyer1[ 20 ],
			Underlyer2[ 20 ],
			Underlyer3[ 20 ];

	double	BogusValue1,
			BogusValue2;

};


#endif
