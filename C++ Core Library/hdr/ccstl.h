/****************************************************************
**
**	ccstl.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/portable/src/ccstl.h,v 1.19 2004/10/18 16:28:38 khodod Exp $
**
****************************************************************/

#ifndef IN_CCSTL_H
#define IN_CCSTL_H


/*
**	One platform does not provide default template arguments.
**  The STL relies heavily on this language feature, so we kludge around
**  this by using macros. ugh.
**
**	Note: Always leave spaces around Types because they might be Temp<Arg>.
*/

#ifdef CC_NO_TEMPLATE_DEFAULTS

//i.e. SW4.2
#define CC_STL_PAIR(T1,T2)                    pair< T1, T2 >
#define CC_STL_LIST(T)					      list< T, allocator >
#define CC_STL_VECTOR(T)				      vector< T, allocator >
#define CC_STL_DEQUE(T)                       deque< T, allocator >
#define CC_STL_MAP(T1,T2)				      map< T1, T2, less< T1 >, allocator >
#define CC_STL_MAP_WITH_COMP(T1,T2,T3)	      map< T1, T2, T3, allocator >
#define CC_STL_MULTIMAP(T1,T2)                multimap< T1, T2, less< T1 >, allocator >
#define CC_STL_MULTIMAP_WITH_COMP(T1,T2,T3)   multimap< T1, T2, T3, allocator >
#define CC_STL_SET(T)					      set< T, less< T >, allocator >
#define CC_STL_SET_WITH_COMP(T1,T2)           set< T1, T2, allocator >
#define CC_STL_MULTISET(T)                    multiset< T, less< T >, allocator >
#define CC_STL_MULTISET_WITH_COMP(T1,T2)      multiset< T1, T2, allocator >
#define CC_STL_ISTREAM_ITERATOR(T)            istream_iterator< T , ptrdiff_t >

#define CC_STL_STACK(T)                       stack< T , deque< T, allocator > , allocator >
#define CC_STL_QUEUE(T)                       queue< T , deque< T, allocator > , allocator >
#define CC_STL_PRIORITY_QUEUE(T)              priority_queue< T, vector< T, allocator >, less< T >, allocator >
#define CC_STL_PRIORITY_QUEUE_WITH_COMP(T,C)  priority_queue< T, vector< T, allocator >, C , allocator >

#else

#define CC_STL_PAIR(T1,T2)                    std::pair< T1, T2 >
#define CC_STL_LIST(T)					      std::list< T >
#define CC_STL_VECTOR(T)				      std::vector< T >
#define CC_STL_DEQUE(T)                       std::deque< T >
#define CC_STL_MAP(T1,T2)				      std::map< T1, T2 >
#define CC_STL_MAP_WITH_COMP(T1,T2,T3)	      std::map< T1, T2, T3 >
#define CC_STL_MULTIMAP(T1,T2)                std::multimap< T1, T2 >
#define CC_STL_MULTIMAP_WITH_COMP(T1,T2,T3)   std::multimap< T1, T2, T3 >
#define CC_STL_SET(T)					      std::set< T >
#define CC_STL_SET_WITH_COMP(T1,T2)           std::set< T1, T2 >
#define CC_STL_MULTISET(T1)                   std::multiset< T1 >
#define CC_STL_MULTISET_WITH_COMP(T1,T2)      std::multiset< T1, T2 >
#define CC_STL_ISTREAM_ITERATOR(T)            std::istream_iterator< T >

#define CC_STL_STACK(T)                       std::stack< T >
#define CC_STL_QUEUE(T)                       std::queue< T >
#define CC_STL_PRIORITY_QUEUE(T)              std::priority_queue< T >
#define CC_STL_PRIORITY_QUEUE_WITH_COMP(T,C)  std::priority_queue< T, std::vector< T >, C >

#endif

#define CC_STL_EXCEPTION CC_NS(std,exception)


#endif // !defined( IN_CCSTL_H )

