/****************************************************************
**
**	gsdt/format.h	- GsDt::FormatInfo class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/FormatInfo.h,v 1.2 2001/01/22 13:04:30 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSDT_FORMAT_H )
#define IN_GSDT_FORMAT_H

#include	<gsdt/GsDt.h>
#include	<dtformat.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDTLITE GsDt::FormatInfo
{
public:
	FormatInfo( DT_MSG_FORMAT_INFO *m_dtMsgFormatInfo );	// Keeps reference, but does not delete it
	~FormatInfo();

	int		width()		{ return m_dtMsgFormatInfo->OutForm.Width; }
	void	widthSet( int w )	{ m_dtMsgFormatInfo->OutForm.Width = w; }

	int		decimal()	{ return m_dtMsgFormatInfo->OutForm.Decimal; }
	void	decimalSet( int d )	{ m_dtMsgFormatInfo->OutForm.Decimal = d; }

	int		&flags()	{ return m_dtMsgFormatInfo->OutForm.Flags; }
	double	&scaleAmt()	{ return m_dtMsgFormatInfo->OutForm.ScaleAmt; }
	int		&code()		{ return m_dtMsgFormatInfo->Code; }
	char	*buffer()	{ return m_dtMsgFormatInfo->Buffer; }
	// Not including trailing '\0'
	size_t	size()		{ return m_dtMsgFormatInfo->BufferSize ? m_dtMsgFormatInfo->BufferSize-1 : 0; }

	GsStatus	resize( size_t NewSize )
	{
		if( size() >= NewSize )
			return GS_OK;
		else
			return ReallyResize( NewSize );
	}

private:
	FormatInfo( const FormatInfo& );
	void operator=( const FormatInfo& );

	GsStatus ReallyResize( size_t NewSize );

	DT_MSG_FORMAT_INFO	*m_dtMsgFormatInfo;

	bool				m_freeDmfi;	// true to free m_dtMsgFormatInfo
};

CC_END_NAMESPACE
#endif

