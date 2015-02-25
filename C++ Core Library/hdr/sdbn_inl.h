/****************************************************************
**
**	SECNODE.INL	- node inline functions
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/sdbn_inl.h,v 1.30 2014/07/16 03:36:41 khodod Exp $
**
****************************************************************/

#if !defined( IN_SECNODE_INL )
#define IN_SECNODE_INL

#include <secerror.h>



/****************************************************************
**	Class  : SDB_NODE
**	Routine: AdjustChildc
**	Returns: TRUE/Err
**	Action : Adjusts child list size preserving children already
**			 there
****************************************************************/

SDB_NODE_INLINE int SDB_NODE::AdjustChildc(
    unsigned NumChildren
)
{
	if( NumChildren > SDB_NODE_CHILDC_MAX )
		return Err( ERR_OVERFLOW, "@: Too many children, %d (max %d)", NumChildren, SDB_NODE_CHILDC_MAX );

	if( NumChildren == m_Childc )
		return TRUE;

	m_Childc = NumChildren;
	if( !( m_children = ListRealloc( m_children, child_list_size() ) ) )
		return FALSE;

	return TRUE;
}



/****************************************************************
**	Class  : SDB_NODE
**	Routine: ParentsRebuild
**	Returns: void
**	Action : Rebuild parents hash table
****************************************************************/

SDB_NODE_INLINE void SDB_NODE::ParentsRebuild(
	int InPlace
)
{
	SDB_NODE
			**new_parents,
			**Ptr;

	int		Num = 0;

	Ptr = ParentGetAddr( ParentsMax() - 1 );
	if( ParentIndexGet() < MinParentIndexForHash )
		ParentIndexSet( MinParentIndexForHash );
	else if( !InPlace && ParentsNum() > ( ParentsMax() * 3 ) / 4 )
		++m_ParentIndex;

	new_parents = ListAlloc( parent_list_size() );
	memset( new_parents, 0, ParentsMax() * sizeof( *m_parents ));

	for( ; Ptr >= ParentGetAddr( 0 ); --Ptr )
		if( *Ptr && *Ptr != SDB_NODE_INVALID_PTR )
		{
			int NewBucket;
			for( NewBucket = NodeHash( *Ptr ); new_parents[ NewBucket ]; NewBucket = ( NewBucket + 1 ) & ( ParentsMax() - 1 ));
			new_parents[ NewBucket ] = *Ptr;
			++Num;
		}

	if( InPlace )
	{
		memcpy( ParentGetAddr( 0 ), new_parents, ParentsMax() * sizeof( *m_parents ));
		free( new_parents );
	}
	else
	{
		free( m_parents );
		m_parents = new_parents;
	}
	ParentsNum() = Num;
	ParentsNumDeleted() = 0;
}



/****************************************************************
**	Class  : SDB_NODE
**	Routine: ParentLink
**	Returns: True/Err
**	Action : Add a parent link to the node
****************************************************************/

SDB_NODE_INLINE int SDB_NODE::ParentLink(
	SDB_NODE	*Parent
)
{
	SDB_NODE
			**Ptr;

	int		Bucket;

	if( !IsNode( Parent ))
		return TRUE;

	if( SecDbTraceFunc )
		(SecDbTraceFunc)( SDB_TRACE_LINK, this, Parent );

	if( RefCountedParents() )
	{
		LiteralRefsIncrement();
		return TRUE;
	}

	if( OnLRUCacheGet() )
		LRUCacheByObjectDelete();

	if( ParentIndexGet() < MinParentIndexForHash )
	{
		Ptr = NULL;
		for( Bucket = ParentsMax() - 1; Bucket >= 0; --Bucket )
			if( ParentGet( Bucket ).PtrValue() == Parent )
			{
				SDB_NODE_PARENT_PTR( Parent ).Node()->m_DuplicateChildren = TRUE;
				return TRUE;
			}
			else if( !ParentGet( Bucket ) )
				Ptr = ParentGetAddr( Bucket );

		if( Ptr )
		{
			*Ptr = Parent;

			if( ReleaseValueNeedParentHashGet() )
				InvParentsHashInc( this );

			return TRUE;
		}
		else if( ParentIndexGet() < MinParentIndexForHash - 1 )
		{
			int Pos = ParentsMax();
			++m_ParentIndex;
			m_parents = ListRealloc( m_parents, parent_list_size() );
			memset( ParentGetAddr( Pos ), 0, ( ParentsMax() - Pos ) * sizeof( *m_parents ));
			ParentSet( Pos, Parent );

			if( ReleaseValueNeedParentHashGet() )
				InvParentsHashInc( this );

			return TRUE;
		}
		else
			ParentsRebuild( FALSE );
	}

	Ptr = NULL;
	for( Bucket = NodeHash( Parent ); ParentGet( Bucket ); Bucket = ( Bucket + 1 ) & ( ParentsMax() - 1 ))
		if( ParentGet( Bucket ).PtrValue() == Parent )
		{
			SDB_NODE_PARENT_PTR( Parent ).Node()->m_DuplicateChildren = TRUE;
			return TRUE;			// already have parent
		}
		else if( ParentGet( Bucket ).PtrValue() == SDB_NODE_INVALID_PTR )
			Ptr = ParentGetAddr( Bucket );

	if( Ptr )
	{
		*Ptr = Parent;
		--ParentsNumDeleted();
	}
	else
		*ParentGetAddr( Bucket ) = Parent;
	++ParentsNum();

	if( ReleaseValueNeedParentHashGet() )
		InvParentsHashInc( this );

	if( ParentsNeedToRebuild() )
		ParentsRebuild( FALSE );

	return TRUE;
}


/****************************************************************
**	Class  : SDB_NODE
**	Routine: ParentUnlink
**	Returns: TRUE/FALSE
**	Action : Unlink a parent from the node
****************************************************************/

SDB_NODE_INLINE int SDB_NODE::ParentUnlink(
	SDB_NODE	*Parent
)
{
	int		Bucket,
			Status = FALSE;

	if( SecDbTraceFunc )
		(SecDbTraceFunc)( SDB_TRACE_UNLINK, this, Parent );

	if( RefCountedParents() )
	{
		LiteralRefsDecrement();
		return TRUE;
	}

	SDB_NODE **FirstParent = ParentGetAddr( 0 );

	if( ParentIndexGet() < MinParentIndexForHash )
	{
		int NumParents = 0;
		for( Bucket = ParentsMax(); --Bucket >= 0; )
			if( FirstParent[ Bucket ])
			{
				if( FirstParent[ Bucket ] == Parent )
				{
					Status = TRUE;
					FirstParent[ Bucket ] = NULL;
					if( NumParents == 0 )
					{
						while( --Bucket >= 0 )
							if( FirstParent[ Bucket ])
								return TRUE;
						LRUCacheByObjectInsert();
					}
					return Status;
				}
				++NumParents;
			}
		return Status;
	}
	for( Bucket = NodeHash( Parent ); FirstParent[ Bucket ]; Bucket = NextBucket( Bucket ))
		if( ParentGet( Bucket ).PtrValue() == Parent )
		{
			Status = TRUE;
			--ParentsNum();
			if( !FirstParent[ NextBucket( Bucket )])
			{
				// next bucket is empty so we can walk back emptying buckets until last used
				FirstParent[ Bucket ] = NULL;
				Bucket = PrevBucket( Bucket );
				while( FirstParent[ Bucket ] == SDB_NODE_INVALID_PTR )
				{
					FirstParent[ Bucket ] = NULL;
					Bucket = PrevBucket( Bucket );
					--ParentsNumDeleted();
				}
			}
			else
			{
				// next bucket is full or deleted, so put in fake null in order to maintain
				// the chain
				FirstParent[ Bucket ] = SDB_NODE_INVALID_PTR;
				++ParentsNumDeleted();
			}
			break;
		}
	if( ParentsNeedToRebuild() )
		ParentsRebuild( FALSE );

	if( ParentsNum() == 0 )
		LRUCacheByObjectInsert();

	return Status;
}



/****************************************************************
**	Class  : SDB_NODE
**	Routine: ParentLookup
**	Returns: TRUE/FALSE
**	Action : Is Parent a parent or not?
****************************************************************/

SDB_NODE_INLINE int SDB_NODE::ParentLookup(
	SDB_NODE	*Parent
)
{
	int Bucket;

	if( RefCountedParents() )
		return FALSE;

	if( ParentIndexGet() < MinParentIndexForHash )
	{
		for( Bucket = ParentsMax(); --Bucket >= 0; )
			if( ParentGet( Bucket ).PtrValue() == Parent )
				return TRUE;
		return FALSE;
	}
	for( Bucket = NodeHash( Parent ); ParentGet( Bucket ); Bucket = ( Bucket + 1 ) & ( ParentsMax() - 1 ))
		if( ParentGet( Bucket ).PtrValue() == Parent )
			return TRUE;
	return FALSE;
}



/****************************************************************
**	Class  : SDB_NODE
**	Routine: ParentHashStats
**	Returns: TRUE/FALSE
**	Action : computes Parent Hash table statistics
****************************************************************/

SDB_NODE_INLINE int SDB_NODE::ParentHashStats(
	double	&MeanChainLength,
	int		&MaxChainLength )
{
	if( ParentIndexGet() < MinParentIndexForHash || ParentsNum() <= 1 )
		return FALSE;

	MaxChainLength = 0;
	MeanChainLength = 0;
	SDB_NODE **Ptr = ParentGetAddr( ParentsMax() - 1 );
	for( ; Ptr >= ParentGetAddr( 0 ); --Ptr )
	{
		if( !*Ptr || *Ptr == SDB_NODE_INVALID_PTR )
			continue;

		unsigned long ActualBucket = Ptr - ParentGetAddr( 0 );
		unsigned long BestBucket = NodeHash( *Ptr );

		int ChainLength = ( ActualBucket + ParentsMax() - BestBucket ) & ( ParentsMax() - 1 );

		if( ChainLength > MaxChainLength )
			MaxChainLength = ChainLength;

		MeanChainLength += ChainLength;
	}
	MeanChainLength /= ParentsNum();

	return TRUE;
}

/****************************************************************
**	Class  : SDB_NODE
**	Routine: MakeFirstChildNonNull
**	Returns: void
**	Action : Moves a non null child into the first pos
****************************************************************/

SDB_NODE_INLINE void SDB_NODE::MakeFirstChildNonNull()
{
	if( !ChildcGet() )
		return;

	SDB_NODE
			**FirstChildAddr = ChildGetAddr( 0 );

	if( !*FirstChildAddr )
		for( SDB_NODE **ChildPtr = ChildGetAddr( ChildcGet() - 1 ); ChildPtr >= FirstChildAddr; --ChildPtr )
			if( *ChildPtr )
			{
				*FirstChildAddr = *ChildPtr;
				*ChildPtr = NULL;
				break;
			}
}

#endif 
