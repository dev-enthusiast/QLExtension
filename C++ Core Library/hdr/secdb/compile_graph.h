/****************************************************************
**
**	GRAPH_COMPILE.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdb/compile_graph.h,v 1.5 2001/11/27 23:25:03 procmon Exp $
**
****************************************************************/

#ifndef IN_SECDB_GRAPH_COMPILE_H
#define IN_SECDB_GRAPH_COMPILE_H

#include <secdb/base.h>
#include <secnodei.h>
#include <secdb/SecDbCompiledGraph.h>

EXPORT_CPP_SECDB bool SecDbCompileGraph( SDB_NODE* f, sdb_node_vec_t const& args_vec, sdb_node_vec_t& compile_deps, CC_NS(Gs,SecDbCompiledGraph)& compiled_graph );

#endif 
