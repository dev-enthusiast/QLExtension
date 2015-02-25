/* $Header: /home/cvs/src/jar/src/jar.h,v 1.12 2001/11/27 22:48:39 procmon Exp $ */
/****************************************************************
**
**	JAR.H
**	
**	$Revision: 1.12 $
**
****************************************************************/

#if !defined( IN_JAR_H )
#define IN_JAR_H

		
/*
**	Declare function prototypes
*/	  

DLLEXPORT int		JarEnumerateBooks( 				char *PortfolioName, DT_VALUE *RetValue ),
					JarIteratePortfolio( 			char *PortfolioName, char *ClassName, DT_VALUE *RetValue ),
					JarGetEntityGroupFromEntity( 	const char *Portfolio, char *EntityGroup );

		
		
#endif

