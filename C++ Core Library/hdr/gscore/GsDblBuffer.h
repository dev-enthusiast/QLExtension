/****************************************************************
**
**	gscore/GsDblBuffer.h	- GsDblBuffer class
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsDblBuffer.h,v 1.17 2004/12/01 17:04:22 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_DBLBUFFER_H )
#define IN_GSCORE_DBLBUFFER_H

#include <memory.h>
#include <gscore/types.h>
#include <gscore/GsException.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	GsDblBuffer is a light-weight reference-counted buffer of doubles 
**	to be used for GsVector, GsTensor, GsMatrix
*/


class EXPORT_CLASS_GSBASE GsDblBuffer 
{
	double	*m_buffer;

	void	link()		{ if( m_buffer ) m_buffer[0]++; }
	void	unlink()	{ if( m_buffer && !--m_buffer[0] ) delete [] m_buffer; }

public:

	// We make no claim to be able to support a buffer with any more than
	// this number of elements.  Of course, far smaller buffers could fail
	// too, if we run out of memory.

	static size_t MaxElements() { return 0x70000000; }


	GsDblBuffer() : m_buffer( 0 ) {}
	GsDblBuffer( const GsDblBuffer& buf ) : m_buffer( buf.m_buffer )	{ link(); }
	~GsDblBuffer()		{ unlink(); }

	void operator=( const GsDblBuffer& buf )
	{
		if( &buf != this ) 
		{
			unlink();
			m_buffer = buf.m_buffer;
			link();
		}
	}

	void create( size_t size )
	{
		m_buffer = new double[ size + 1 ];

		if( !m_buffer )
			GSX_THROW( GsXAllocationError, "GsDblBuffer::create(): Unable to allocate memory." );

		m_buffer[0] = 1;
	}

	void cow( size_t size )
	{
		if( m_buffer && m_buffer[0] > 1 ) 
		{
			m_buffer[0]--;
		
			double *old_data = data();
			create( size );
			memcpy( data(), old_data, size * sizeof( double ));
		}
	}

	double * data() const	{ return m_buffer ? m_buffer + 1 : 0; }

	friend EXPORT_CPP_GSBASE void swap( GsDblBuffer& a, GsDblBuffer& b)
	{
		double *tmp = a.m_buffer;
		a.m_buffer = b.m_buffer;
		b.m_buffer = tmp;
	}


	// Class representing strided reference. 

	class EXPORT_CLASS_GSBASE StridedRef
	{
	public:

		StridedRef( GsDblBuffer &Buffer, size_t BufferSize, size_t ElemBegin, size_t Size, int Stride )
		{
			Buffer.cow( BufferSize );

			m_Begin  = Buffer.data() + ElemBegin;
			m_Size   = Size;
			m_Stride = Stride;
		};

		class EXPORT_CLASS_GSBASE iterator
		{
		public:
			iterator( StridedRef &Ref, size_t Elem )
			:
				m_Stride(  Ref.Stride() ),
				m_Current( Ref.Begin() + Ref.Stride() * Elem )
			{				
			}
			 
			iterator &operator++() 	 	{ m_Current += m_Stride; return *this; }
			iterator  operator++(int) 	{ iterator Old( *this ); m_Current += m_Stride; return Old; } 

			iterator &operator--()		{ m_Current -= m_Stride; return *this; }
			iterator  operator--(int)	{ iterator Old( *this ); m_Current -= m_Stride; return Old; }

			double &operator*() 		{ return *m_Current; }

			bool operator==( iterator rhs ) const { return ( m_Current == rhs.m_Current ); }
			bool operator!=( iterator rhs ) const { return ( m_Current != rhs.m_Current ); }

			bool operator<(  iterator rhs ) const { if( m_Stride > 0 ) return ( m_Current < rhs.m_Current ); else return ( m_Current > rhs.m_Current ); }
							 
			bool operator>(  iterator rhs ) const { return  (  rhs  < *this ); }
			bool operator>=( iterator rhs ) const { return !( *this <  rhs  ); }
			bool operator<=( iterator rhs ) const { return !(  rhs  < *this ); }

			iterator &operator+=( int Offset )	{ m_Current += m_Stride * Offset; return *this; }
			iterator &operator-=( int Offset )	{ m_Current -= m_Stride * Offset; return *this; }

			iterator operator+( int Offset ) const { iterator Result( *this ); Result += Offset; return Result; }
			iterator operator-( int Offset ) const { iterator Result( *this ); Result -= Offset; return Result; }

		private:
			int 	m_Stride;		

			double	*m_Current;
		};

		iterator begin() { return iterator( *this, 0	  ); }		
		iterator end()   { return iterator( *this, size() ); }

		double 		 *Begin() 		{ return m_Begin;  }
		const double *Begin() const { return m_Begin;  }

		size_t 	size()   const { return m_Size; 	}
		int		Stride() const { return m_Stride;	}

		double &operator[]( int Index ) { return *( m_Begin + m_Stride * Index ); }

		operator GsVector() const;

		StridedRef &operator=( const GsVector &Vector );

	private:
		double 	*m_Begin;
		
		size_t 	m_Size;							

		int 	m_Stride;		
	};

	class EXPORT_CLASS_GSBASE ConstStridedRef
	{
	public:

		ConstStridedRef( const GsDblBuffer &Buffer, size_t ElemBegin, size_t Size, int Stride )
		:
			m_Begin(  	Buffer.data() + ElemBegin 	),
			m_Size(	  	Size					  	),
			m_Stride(	Stride					 	)
		{
		}
		
		ConstStridedRef( const StridedRef &Other )
		:
			m_Begin( 	Other.Begin()	),
			m_Size(  	Other.size()	),
			m_Stride(	Other.Stride()	)
		{			
		}		
		
		class EXPORT_CLASS_GSBASE const_iterator
		{
		public:

			const_iterator( const ConstStridedRef &Ref, size_t Elem )
			:
				m_Stride(  Ref.Stride() ),
				m_Current( Ref.Begin() + Ref.Stride() * Elem )
			{				
			}
			 
			const_iterator &operator++() 	 	{ m_Current += m_Stride; return *this; }
			const_iterator  operator++(int) 	{ const_iterator Old( *this ); m_Current += m_Stride; return Old; } 

			const_iterator &operator--()		{ m_Current -= m_Stride; return *this; }
			const_iterator  operator--(int)		{ const_iterator Old( *this ); m_Current -= m_Stride; return Old; }

			double operator*() 					const { return *m_Current; }

			bool operator==( const_iterator rhs ) const { return ( m_Current == rhs.m_Current ); }
			bool operator!=( const_iterator rhs ) const { return ( m_Current != rhs.m_Current ); }

			bool operator<(  const_iterator rhs ) const { if( m_Stride > 0 ) return ( m_Current < rhs.m_Current ); else return ( m_Current > rhs.m_Current ); }

			bool operator>(  const_iterator rhs ) const { return  (  rhs  < *this ); }
			bool operator>=( const_iterator rhs ) const { return !( *this <  rhs  ); }
			bool operator<=( const_iterator rhs ) const { return !(  rhs  < *this ); }

			const_iterator &operator+=( int Offset )	{ m_Current += m_Stride * Offset; return *this; }
			const_iterator &operator-=( int Offset )	{ m_Current -= m_Stride * Offset; return *this; }

			const_iterator operator+( int Offset ) const { const_iterator Result( *this ); Result += Offset; return Result; }
			const_iterator operator-( int Offset ) const { const_iterator Result( *this ); Result -= Offset; return Result; }

		private:
			int 	m_Stride;		

			const double	
					*m_Current;
		};

		const_iterator begin() const { return const_iterator( *this, 0	  ); }		
		const_iterator end()   const { return const_iterator( *this, size() ); }

		const double *Begin() const { return m_Begin;  }

		size_t 	size()   const { return m_Size; 	}
		int		Stride() const { return m_Stride;	}

		double operator[]( int Index ) const { return *( m_Begin + m_Stride * Index ); }

		operator GsVector() const;

	private:
		const double 	
				*m_Begin;
		
		size_t 	m_Size;							

		int 	m_Stride;		
	};
};

CC_END_NAMESPACE


#endif
