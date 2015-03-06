/*
*  R : A Computer Language for Statistical Data Analysis
*  Copyright (C) 1995, 1996  Robert Gentleman and Ross Ihaka
*  Copyright (C) 1999-2015   The R Core Team.
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation; either version 2.1 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public License
*  along with this program; if not, a copy is available at
*  http://www.r-project.org/Licenses/
*/

/* This file is installed and available to packages, but only a small
part of the contents is within the API.  See chapter 6 of 'Writing
R Extensions'.
*/

#ifndef R_INTERNALS_H_
#define R_INTERNALS_H_

namespace QuantLib {

	/*------ enum_SEXPTYPE ----- */
	typedef enum {
		NILSXP = 0,	/* nil = NULL */
		SYMSXP = 1,	/* symbols */
		LISTSXP = 2,	/* lists of dotted pairs */
		CLOSXP = 3,	/* closures */
		ENVSXP = 4,	/* environments */
		PROMSXP = 5,	/* promises: [un]evaluated closure arguments */
		LANGSXP = 6,	/* language constructs (special lists) */
		SPECIALSXP = 7,	/* special forms */
		BUILTINSXP = 8,	/* builtin non-special forms */
		CHARSXP = 9,	/* "scalar" string type (internal only)*/
		LGLSXP = 10,	/* logical vectors */
		INTSXP = 13,	/* integer vectors */
		REALSXP = 14,	/* real variables */
		CPLXSXP = 15,	/* complex variables */
		STRSXP = 16,	/* string vectors */
		DOTSXP = 17,	/* dot-dot-dot object */
		ANYSXP = 18,	/* make "any" args work */
		VECSXP = 19,	/* generic vectors */
		EXPRSXP = 20,	/* expressions vectors */
		BCODESXP = 21,	/* byte code */
		EXTPTRSXP = 22,	/* external pointer */
		WEAKREFSXP = 23,	/* weak reference */
		RAWSXP = 24,	/* raw bytes */
		S4SXP = 25,	/* S4 non-vector */

		NEWSXP = 30,   /* fresh node creaed in new page */
		FREESXP = 31,   /* node released by GC */

		FUNSXP = 99	/* Closure or Builtin */
	} SEXPTYPE;

	/* These are also used with the write barrier on, in attrib.c and util.c */
#define TYPE_BITS 5
#define MAX_NUM_SEXPTYPE (1<<TYPE_BITS)

	struct sxpinfo_struct {
		SEXPTYPE type : TYPE_BITS;/* ==> (FUNSXP == 99) %% 2^5 == 3 == CLOSXP
								  * -> warning: `type' is narrower than values
								  *              of its type
								  * when SEXPTYPE was an enum */
		unsigned int obj : 1;
		unsigned int named : 2;
		unsigned int gp : 16;
		unsigned int mark : 1;
		unsigned int debug : 1;
		unsigned int trace : 1;  /* functions and memory tracing */
		unsigned int spare : 1;  /* currently unused */
		unsigned int gcgen : 1;  /* old generation number */
		unsigned int gccls : 3;  /* node class */
	}; /*		    Tot: 32 */

#define SEXPREC_HEADER \
	struct sxpinfo_struct sxpinfo; \
	struct SEXPREC *attrib; \
	struct SEXPREC *gengc_next_node, *gengc_prev_node

	/* The standard node structure consists of a header followed by the
	node data. */
	struct primsxp_struct;
	struct symsxp_struct;
	struct listsxp_struct;
	struct envsxp_struct;
	struct closxp_struct;
	struct promsxp_struct;

	typedef struct SEXPREC {
		SEXPREC_HEADER;
		union {
			struct primsxp_struct primsxp;
			struct symsxp_struct symsxp;
			struct listsxp_struct listsxp;
			struct envsxp_struct envsxp;
			struct closxp_struct closxp;
			struct promsxp_struct promsxp;
		} u;
	} SEXPREC, *SEXP;

	struct primsxp_struct {
		int offset;
	};

	struct symsxp_struct {
		struct SEXPREC *pname;
		struct SEXPREC *value;
		struct SEXPREC *internal;
	};

	struct listsxp_struct {
		struct SEXPREC *carval;
		struct SEXPREC *cdrval;
		struct SEXPREC *tagval;
	};

	struct envsxp_struct {
		struct SEXPREC *frame;
		struct SEXPREC *enclos;
		struct SEXPREC *hashtab;
	};

	struct closxp_struct {
		struct SEXPREC *formals;
		struct SEXPREC *body;
		struct SEXPREC *env;
	};

	struct promsxp_struct {
		struct SEXPREC *value;
		struct SEXPREC *expr;
		struct SEXPREC *env;
	};
}

#endif /* R_INTERNALS_H_ */