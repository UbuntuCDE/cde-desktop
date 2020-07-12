/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: parse.c /main/3 1995/11/01 16:50:11 rswiston $ */
/***************************************************************
*                                                              *
*                      AT&T - PROPRIETARY                      *
*                                                              *
*        THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF        *
*                    AT&T BELL LABORATORIES                    *
*         AND IS NOT TO BE DISCLOSED OR USED EXCEPT IN         *
*            ACCORDANCE WITH APPLICABLE AGREEMENTS             *
*                                                              *
*                Copyright (c) 1995 AT&T Corp.                 *
*              Unpublished & Not for Publication               *
*                     All Rights Reserved                      *
*                                                              *
*       The copyright notice above does not evidence any       *
*      actual or intended publication of such source code      *
*                                                              *
*               This software was created by the               *
*           Advanced Software Technology Department            *
*                    AT&T Bell Laboratories                    *
*                                                              *
*               For further information contact                *
*                    {research,attmail}!dgk                    *
*                                                              *
***************************************************************/

/* : : generated by proto : : */

#if !defined(__PROTO__)
#if defined(__STDC__) || defined(__cplusplus) || defined(_proto) || defined(c_plusplus)
#if defined(__cplusplus)
#define __MANGLE__	"C"
#else
#define __MANGLE__
#endif
#define __STDARG__
#define __PROTO__(x)	x
#define __OTORP__(x)
#define __PARAM__(n,o)	n
#if !defined(__STDC__) && !defined(__cplusplus)
#if !defined(c_plusplus)
#define const
#endif
#define signed
#define void		int
#define volatile
#define __V_		char
#else
#define __V_		void
#endif
#else
#define __PROTO__(x)	()
#define __OTORP__(x)	x
#define __PARAM__(n,o)	o
#define __MANGLE__
#define __V_		char
#define const
#define signed
#define void		int
#define volatile
#endif
#if defined(__cplusplus) || defined(c_plusplus)
#define __VARARG__	...
#else
#define __VARARG__
#endif
#if defined(__STDARG__)
#define __VA_START__(p,a)	va_start(p,a)
#else
#define __VA_START__(p,a)	va_start(p)
#endif
#endif
#ifdef KSHELL
#include	"defs.h"
#else
#include	<shell.h>
#endif
#include	<ctype.h>
#include	<fcin.h>
#include	<error.h>
#include	"shlex.h"
#include	"history.h"
#include	"builtins.h"
#include	"test.h"

/* These routines are local to this module */

static union anynode	*makeparent __PROTO__((int, union anynode*));
static union anynode	*makelist __PROTO__((int, union anynode*, union anynode*));
static struct argnod	*qscan __PROTO__((struct comnod*, int));
static struct ionod	*inout __PROTO__((struct ionod*, int));
static union anynode	*sh_cmd __PROTO__((int,int));
static union anynode	*term __PROTO__((int));
static union anynode	*list __PROTO__((int));
static struct regnod	*syncase __PROTO__((int));
static union anynode	*item __PROTO__((int));
static union anynode	*simple __PROTO__((int, struct ionod*));
static int		skipnl __PROTO__((void));
static union anynode	*test_expr __PROTO__((int));
static union anynode	*test_and __PROTO__((void));
static union anynode	*test_or __PROTO__((void));
static union anynode	*test_primary __PROTO__((void));

#define	sh_getlineno()	(sh.inlineno)

#ifndef NIL
#   define NIL(type)	((type)0)
#endif /* NIL */
#define CNTL(x)		((x)&037)


#ifndef KSHELL
static struct stdata
{
	struct slnod    *staklist;
	int	cmdline;
} st;
#endif

static int		loop_level;
static struct argnod	*label_list;
static struct argnod	*label_last;

#define getnode(type)	((union anynode*)stakalloc(sizeof(struct type)))

#ifdef SHOPT_KIA
/*
 * write out entities for each item in the list
 * type=='V' for variable assignment lists
 * Otherwise type is determined by the command */
static void writedefs __PARAM__((struct argnod *arglist, int line, int type, struct argnod *cmd), (arglist, line, type, cmd)) __OTORP__(struct argnod *arglist; int line; int type; struct argnod *cmd;){
	struct argnod *argp = arglist;
	char *cp;
	int n;
	int width=0;
	static char atbuff[20];
	static char justify[2];
	char *attribute = atbuff;
	char *parent = "0";
	justify[0] = 0;
	if(type==0)
	{
		parent = shlex.parent;
		type = 'v';
		switch(*argp->argval)
		{
		    case 'a':
			type='a';
			break;
		    case 'e':
			*attribute++ =  'x';
			break;
		    case 'r':
			*attribute++ = 'r';
			break;
		    case 'l':
			break;
		}
		while(argp = argp->argnxt.ap)
		{
			if((n= *(cp=argp->argval))!='-' && n!='+')
				break;
			if(cp[1]==n)
				break;
			while((n= *++cp))
			{
				if(isdigit(n))
					width = 10*width + n-'0';
				else if(n=='L' || n=='R' || n =='Z')
					justify[0]=n;
				else
					*attribute++ = n;
			}
		}
	}
	else if(cmd)
		parent = sh_argstr(cmd);
	*attribute = 0;
	while(argp)
	{
		if(cp=strchr(argp->argval,'='))
			n = cp-argp->argval;
		else
			n = strlen(argp->argval);
		sfprintf(shlex.kiafile,"%d;%c;%.*s;%d;%d;%s;%s;%d;%s\n",++shlex.entity,type,n,argp->argval,line,sh.inlineno-(shlex.token==NL),parent,justify,width,atbuff);
		argp = argp->argnxt.ap;
	}
}
#endif /* SHOPT_KIA */
/*
 * Make a parent node for fork() or io-redirection
 */
static union anynode	*makeparent __PARAM__((int flag, union anynode *child), (flag, child)) __OTORP__(int flag; union anynode *child;){
	union anynode	*par = getnode(forknod);
	par->fork.forktyp = flag;
	par->fork.forktre = child;
	par->fork.forkio = 0;
	par->fork.forkline = sh_getlineno()-1;
	return(par);
}

/*
 *  Make a node corresponding to a command list
 */
static union anynode	*makelist __PARAM__((int type, union anynode *l, union anynode *r), (type, l, r)) __OTORP__(int type; union anynode *l; union anynode *r;){
	union anynode	*t;
	if(!l || !r)
		sh_syntax();
	else
	{
		if((type&COMMSK) == TTST)
			t = getnode(tstnod);
		else
			t = getnode(lstnod);
		t->lst.lsttyp = type;
		t->lst.lstlef = l;
		t->lst.lstrit = r;
	}
	return(t);
}

/*
 * entry to shell parser
 * Flag can be the union of SH_EOF|SH_NL
 */

union anynode	*sh_parse __PARAM__((Sfio_t *iop, int flag), (iop, flag)) __OTORP__(Sfio_t *iop; int flag;){
	union anynode	*t;
	Fcin_t	sav_input;
	int	sav_prompt = sh.nextprompt;
	if(sh.binscript && sffileno(iop)==sh.infd)
		return(sh_trestore(iop));
	fcsave(&sav_input);
	sh.st.staklist = 0;
	shlex.heredoc = 0;
	shlex.inlineno = sh.inlineno;
	shlex.firstline = sh.st.firstline;
	sh.nextprompt = 1;
	loop_level = 0;
	label_list = label_last = 0;
	sh_onstate(sh_isoption(SH_INTERACTIVE));
	sh_lexinit(0);
	if(fcfopen(iop) < 0)
		return(NIL(union anynode*));
	if(fcfile())
	{
		char *cp = fcfirst();
		if( cp[0]==CNTL('k') &&  cp[1]==CNTL('s') && cp[2]==CNTL('h') && cp[3]==0) 
		{
			int version;
			fcseek(4);
			fcgetc(version);
			fcclose();
			fcrestore(&sav_input);
			if(version > 2)
				error(ERROR_exit(1),e_lexversion);
			if(sffileno(iop)==sh.infd)
				sh.binscript = 1;
			sfgetc(iop);
			return(sh_trestore(iop));
		}
	}
	if((flag&SH_NL) && (sh.inlineno=error_info.line+sh.st.firstline)==0)
		sh.inlineno=1;
#ifdef KSHELL
	sh.nextprompt = 2;
#endif
	t = sh_cmd((flag&SH_EOF)?EOFSYM:'\n',SH_EMPTY|(flag&SH_NL));
	fcclose();
	fcrestore(&sav_input);
	sh.nextprompt = sav_prompt;
	if(flag&SH_NL)
	{
		sh.st.firstline = shlex.firstline;
		sh.inlineno = shlex.inlineno;
	}
	stakseek(0);
	return(t);
}

/*
 * This routine parses up the matching right parenthesis and returns
 * the parse tree
 */
union anynode *sh_dolparen __PARAM__((void), ()){
	union anynode *t=0;
	Sfio_t *sp = fcfile();
	int line = sh.inlineno;
	sh.inlineno = error_info.line+sh.st.firstline;
	sh_lexinit(1);
	switch(sh_lex())
	{
	    /* ((...)) arithmetic expression */
	    case EXPRSYM:
		t = getnode(arithnod);
		t->ar.artyp = TARITH;
		t->ar.arline = sh_getlineno();
		t->ar.arexpr = shlex.arg;
		break;
	    case LPAREN:
		t = sh_cmd(RPAREN,SH_NL);
		break;
	}
	if(!sp && (sp=fcfile()))
	{
		/*
		 * This code handles the case where string has been converted
		 * to a file by an alias setup
		 */
		int c;
		char *cp;
		if(fcgetc(c) > 0)
			fcseek(-1);
		cp = fcseek(0);
		fcclose();
		fcsopen(cp);
		sfclose(sp);
	}
	sh.inlineno = line;
	return(t);
}

/*
 * remove temporary files and stacks
 */

void	sh_freeup __PARAM__((void), ()){
	if(sh.st.staklist)
		sh_funstaks(sh.st.staklist,-1);
	sh.st.staklist = 0;
}

/*
 * increase reference count for each stack in function list when flag>0
 * decrease reference count for each stack in function list when flag<=0
 * stack is freed when reference count is zero
 */

void sh_funstaks __PARAM__((struct slnod *slp,int flag), (slp, flag)) __OTORP__(struct slnod *slp;int flag;){
	struct slnod *slpold;
	while(slpold=slp)
	{
		if(slp->slchild)
			sh_funstaks(slp->slchild,flag);
		slp = slp->slnext;
		if(flag<=0)
			stakdelete(slpold->slptr);
		else
			staklink(slpold->slptr);
	}
}
/*
 * cmd
 *	empty
 *	list
 *	list & [ cmd ]
 *	list [ ; cmd ]
 */

static union anynode	*sh_cmd __PARAM__((int sym, int flag), (sym, flag)) __OTORP__(int sym; int flag;){
	union anynode	*left, *right;
	int type = FINT|FAMP;
	if(sym==NL)
		shlex.lasttok = 0;
	left = list(flag);
	if(shlex.token==NL)
	{
		if(flag&SH_NL)
			shlex.token=';';
	}
	else if(!left && !(flag&SH_EMPTY))
		sh_syntax();
	switch(shlex.token)
	{
	    case COOPSYM:		/* set up a cooperating process */
		type |= (FPIN|FPOU|FPCL|FCOOP);
		/* FALL THRU */		
	    case '&':
		if(left)
			left = makeparent(TFORK|type, left);
		else
			 sh_syntax();
		/* FALL THRU */		
	    case ';':
		if(right=sh_cmd(sym,flag|SH_EMPTY))
			left=makelist(TLST, left, right);
		else if(!left)
		{
			shlex.token = ';';
			sh_syntax();
		}
		break;
	    case EOFSYM:
		if(sym==NL)
			break;
	    default:
		if(sym && sym!=shlex.token)
		{
			if(sym!=ELSESYM || (shlex.token!=ELIFSYM && shlex.token!=FISYM))
				sh_syntax();
		}
	}
	return(left);
}

/*
 * list
 *	term
 *	list && term
 *	list || term
 *      unfortunately, these are equal precedence
 */
static union anynode	*list __PARAM__((int flag), (flag)) __OTORP__(int flag;){
	union anynode	*t = term(flag);
	int 	token;
	while(t && ((token=shlex.token)==ANDFSYM || token==ORFSYM))
		t = makelist((token==ANDFSYM?TAND:TORF), t, term(SH_NL));
	return(t);
}

/*
 * term
 *	item
 *	item | term
 */
static union anynode	*term __PARAM__((int flag), (flag)) __OTORP__(int flag;){
	union anynode	*t;
	int token;
	if(flag&SH_NL)
		token = skipnl();
	else
		token = sh_lex();
	/* check to see if pipeline is to be timed */
	if(token==TIMESYM || token==NOTSYM)
	{
		t = getnode(parnod);
		t->par.partyp=TTIME;
		if(shlex.token==NOTSYM)
			t->par.partyp |= COMSCAN;
		t->par.partre = term(0);
	}
	else if((t=item(SH_NL|SH_EMPTY)) && shlex.token=='|')
	{
		union anynode	*tt;
		t = makeparent(TFORK|FPOU,t);
		if(tt=term(SH_NL))
		{
			switch(tt->tre.tretyp&COMMSK)
			{
			    case TFORK:
				tt->tre.tretyp |= FPIN|FPCL;
				break;
			    case TFIL:
				tt->lst.lstlef->tre.tretyp |= FPIN|FPCL;
				break;
			    default:
				tt= makeparent(TSETIO|FPIN|FPCL,tt);
			}
			t=makelist(TFIL,t,tt);
		}
		else if(shlex.token)
			sh_syntax();
	}
	return(t);
}

/*
 * case statement
 */
static struct regnod*	syncase __PARAM__((int esym), (esym)) __OTORP__(int esym;){
	int tok = skipnl();
	struct regnod	*r;
	if(tok==esym)
		return(NIL(struct regnod*));
	r = (struct regnod*)stakalloc(sizeof(struct regnod));
	r->regptr=0;
	r->regflag=0;
	if(tok==LPAREN)
		skipnl();
	while(1)
	{
		if(!shlex.arg)
			sh_syntax();
		shlex.arg->argnxt.ap=r->regptr;
		r->regptr = shlex.arg;
		if((tok=sh_lex())==RPAREN)
			break;
		else if(tok=='|')
			sh_lex();
		else
			sh_syntax();
	}
	r->regcom=sh_cmd(0,SH_NL|SH_EMPTY);
	if((tok=shlex.token)==BREAKCASESYM)
		r->regnxt=syncase(esym);
	else if(tok==FALLTHRUSYM)
	{
		r->regflag++;
		r->regnxt=syncase(esym);
	}
	else
	{
		if(tok!=esym && tok!=EOFSYM)
			sh_syntax();
		r->regnxt=0;
	}
	if(shlex.token==EOFSYM)
		return(NIL(struct regnod*));
	return(r);
}

/*
 * This routine creates the parse tree for the arithmetic for
 * When called, shlex.arg contains the string inside ((...))
 * When the first argument is missing, a while node is returned
 * Otherise a list containing an arithmetic command and a while
 * is returned.
 */
static union anynode	*arithfor __PARAM__((union anynode *tf), (tf)) __OTORP__(union anynode *tf;){
	union anynode	*t, *tw = tf;
	int	offset;
	struct argnod *argp;
	int n;
	int argflag = shlex.arg->argflag;
	/* save current input */
	Fcin_t	sav_input;
	fcsave(&sav_input);
	fcsopen(shlex.arg->argval);
	/* split ((...)) into three expressions */
	for(n=0; ; n++)
	{
		int c;
		argp = (struct argnod*)stakseek(ARGVAL);
		argp->argnxt.ap = 0;
		argp->argflag = argflag;
		if(n==2)
			break;
		/* copy up to ; onto the stack */
		sh_lexskip(';',1,ST_NESTED);
		offset = staktell()-1;
		if((c=fcpeek(-1))!=';')
			break;
		/* remove trailing white space */
		while(offset>ARGVAL && ((c= *stakptr(offset-1)),isspace(c)))
			offset--;
		/* check for empty initialization expression  */
		if(offset==ARGVAL && n==0)
			continue;
		stakseek(offset);
		/* check for empty condition and treat as while((1)) */
		if(offset==ARGVAL)
			stakputc('1');
		argp = (struct argnod*)stakfreeze(1);
		t = getnode(arithnod);
		t->ar.artyp=TARITH;
		t->ar.arline = sh_getlineno();
		t->ar.arexpr = argp;
		if(n==0)
			tf = makelist(TLST,t,tw);
		else
			tw->wh.whtre = t;
	}
	while((offset=fcpeek(0)) && isspace(offset))
		fcseek(1);
	stakputs(fcseek(0));
	argp = (struct argnod*)stakfreeze(1);
	fcrestore(&sav_input);
	if(n<2)
	{
		shlex.token = RPAREN|SYMREP;
		sh_syntax();
	}
	/* check whether the increment is present */
	if(*argp->argval)
	{
		t = getnode(arithnod);
		t->ar.artyp=TARITH;
		t->ar.arline = sh_getlineno();
		t->ar.arexpr = argp;
		tw->wh.whinc = (struct arithnod*)t;
	}
	else
		tw->wh.whinc = 0;
	sh_lexinit(1);
	if((n=sh_lex())==NL)
		n = skipnl();
	else if(n==';')
		n = sh_lex();
	if(n!=DOSYM && n!=LBRACE)
		sh_syntax();
	tw->wh.dotre = sh_cmd(n==DOSYM?DONESYM:RBRACE,SH_NL);
	tw->wh.whtyp = TWH;
	return(tf);

}

static union anynode *funct __PARAM__((void), ()){
	union anynode *t;
	int flag;
	struct slnod *volatile slp=0;
	Stak_t *savstak;
#ifdef SHOPT_KIA
	int current = shlex.current;
	char *parent = shlex.parent;
#endif /* SHOPT_KIA */
#ifdef KSHELL
	int histon = sh_isstate(SH_HISTORY);
#endif
	int saveloop = loop_level;
	struct argnod *savelabel = label_last;
	int jmpval;
	struct  checkpt buff;
	t = getnode(functnod);
	t->funct.functloc = -1;
	t->funct.functline = sh.inlineno;
	t->funct.functtyp=TFUN;
	t->funct.functargs = 0;
	if(!(flag = (shlex.token==FUNCTSYM)))
		t->funct.functtyp |= FPOSIX;
	else if(sh_lex())
		sh_syntax();
	t->funct.functnam= (char*)shlex.arg->argval;
#ifdef SHOPT_KIA
	shlex.parent = t->funct.functnam;
	shlex.current = ++shlex.entity;
#endif /* SHOPT_KIA */
	if(t->funct.functtyp&FPOSIX)
		skipnl();
	else
	{
		if((shlex.token=sh_lex())==0)
			t->funct.functargs = (struct comnod*)simple(SH_NOIO|SH_FUNDEF,NIL(struct ionod*));
		while(shlex.token==NL)
			shlex.token = sh_lex();
	}
	if(flag && shlex.token!=LBRACE)
		sh_syntax();
#ifdef KSHELL
	/* log function definitions in history file */
	if(sh_isoption(SH_INTERACTIVE) && !sh_isoption(SH_NOLOG)
		&& fcfile() && sffileno(fcfile())>=0 && sh_histinit())
	{
		fcfill();
		fcseek(-1);
		t->funct.functloc = sfseek(sh.hist_ptr->histfp,(off_t)0,SEEK_CUR)+1;
		sh_onstate(SH_HISTORY);
	}
#endif
	sh_pushcontext(&buff,1);
	jmpval = sigsetjmp(buff.buff,0);
	if(jmpval == 0)
	{
		/* create a new stak frame to compile the command */
		savstak = stakcreate(STAK_SMALL);
		savstak = stakinstall(savstak, 0);
		slp = (struct slnod*)stakalloc(sizeof(struct slnod));
		slp->slchild = 0;
		slp->slnext = sh.st.staklist;
		sh.st.staklist = 0;
		t->funct.functstak = (struct slnod*)slp;
		loop_level = 0;
		label_last = label_list;
		if(!flag && shlex.token==0)
		{
			/* copy current word token to current stak frame */
			struct argnod *ap;
			flag = ARGVAL + strlen(shlex.arg->argval);
			ap = (struct argnod*)stakalloc(flag);
			memcpy(ap,shlex.arg,flag);
			shlex.arg = ap;
		}
		t->funct.functtre = item(SH_NOIO);
	}
	sh_popcontext(&buff);
	loop_level = saveloop;
	label_last = savelabel;
	/* restore the old stack */
	if(slp)
	{
		slp->slptr =  stakinstall(savstak,0);
		slp->slchild = sh.st.staklist;
	}
#ifdef SHOPT_KIA
	shlex.parent = parent;
	shlex.current = current;
#endif /* SHOPT_KIA */
	if(jmpval && slp)
	{
		sh.st.staklist = slp->slnext;
		stakdelete(slp->slptr);
		siglongjmp(*sh.jmplist,jmpval);
	}
	sh.st.staklist = (struct slnod*)slp;
#ifdef KSHELL
	if(sh.hist_ptr && sh_isstate(SH_HISTORY) && !histon)
	{
		if(fcfill()>0)
			fcseek(-1);
		hist_flush(sh.hist_ptr);
		hist_cancel(sh.hist_ptr);
		sh_offstate(SH_HISTORY);
	}
#endif
#ifdef SHOPT_KIA
	if(shlex.kiafile)
		sfprintf(shlex.kiafile,"%d;p;%s;%d;%d;%s;p;;\n",shlex.current,t->funct.functnam,t->funct.functline,sh.inlineno-1,parent);
	shlex.parent = parent;
	shlex.current = current;
#endif /* SHOPT_KIA */
	return(t);
}

/*
 * Compound assignment
 */
static struct argnod *assign __PARAM__((struct argnod *ap), (ap)) __OTORP__(struct argnod *ap;){
	int n;
	union anynode *t ,**tp;
	struct comnod *ac;
	int array=0;
	Namval_t *np;
	n = strlen(ap->argval)-1;
	if(ap->argval[n]!='=' || ap->argval[n-1]==']')
		sh_syntax();
#ifdef SHOPT_APPEND
	if(ap->argval[n-1]=='+')
	{
		n--;
		array = ARG_APPEND;
	}
#endif /* SHOPT_APPEND */
	/* shift right */
	while(n > 0)
	{
		ap->argval[n] = ap->argval[n-1];
		n--;
	}
	*ap->argval=0;
	t = getnode(fornod);
	t->for_.fornam = (char*)(ap->argval+1);
	t->for_.fortyp = sh_getlineno();
	tp = &t->for_.fortre;
	ap->argchn.ap = (struct argnod*)t;
	ap->argflag = array;
	shlex.assignok = SH_ASSIGN;
	if(skipnl())
		sh_syntax();
	array=0;
	if(!(shlex.arg->argflag&ARG_ASSIGN) && !((np=nv_search(shlex.arg->argval,sh.fun_tree,0)) && nv_isattr(np,BLT_DCL)))
		array=1;
	while(1)
	{
		if(shlex.token==RPAREN)
			break;
		ac = (struct comnod*)simple(SH_NOIO|SH_ASSIGN,NIL(struct ionod*));
		if((n=shlex.token)==RPAREN)
			break;
		if(n!=NL && n!=';')
			sh_syntax();
		shlex.assignok = 1;
		if(skipnl() || array)
		{
			if(shlex.token==RPAREN)
				break;
			sh_syntax();
		}
		if(!(shlex.arg->argflag&ARG_ASSIGN) && !((np=nv_search(shlex.arg->argval,sh.fun_tree,0)) && nv_isattr(np,BLT_DCL)))
			sh_syntax();
		t = makelist(TLST,(union anynode*)ac,t);
		*tp = t;
		tp = &t->lst.lstrit;
	}
	*tp = (union anynode*)ac;
	return(ap);
}

/*
 * item
 *
 *	( cmd ) [ < in ] [ > out ]
 *	word word* [ < in ] [ > out ]
 *	if ... then ... else ... fi
 *	for ... while ... do ... done
 *	case ... in ... esac
 *	begin ... end
 */

static union anynode	*item __PARAM__((int flag), (flag)) __OTORP__(int flag;){
	union anynode	*t;
	struct ionod	*io;
	int tok = (shlex.token&0xff);
	int savwdval = shlex.lasttok;
	int savline = shlex.lastline;
	if(!(flag&SH_NOIO) && (tok=='<' || tok=='>'))
		io=inout(NIL(struct ionod*),1);
	else
		io=0;
	if((tok=shlex.token) && tok!=EOFSYM && tok!=FUNCTSYM)
	{
		shlex.lastline =  sh_getlineno();
		shlex.lasttok = shlex.token;
	}
	switch(tok)
	{
	    /* [[ ... ]] test expression */
	    case BTESTSYM:
		t = test_expr(ETESTSYM);
		t->tre.tretyp &= ~TTEST;
		break;
	    /* ((...)) arithmetic expression */
	    case EXPRSYM:
		t = getnode(arithnod);
		t->ar.artyp=TARITH;
		t->ar.arline = sh_getlineno();
		t->ar.arexpr = shlex.arg;
		sh_lex();
		goto done;

	    /* case statement */
	    case CASESYM:
	    {
		int savetok = shlex.lasttok;
		int saveline = shlex.lastline;
		t = getnode(swnod);
		if(sh_lex())
			sh_syntax();
		t->sw.swarg=shlex.arg;
		if((tok=skipnl())!=INSYM && tok!=LBRACE)
			sh_syntax();
		if(!(t->sw.swlst=syncase(tok==INSYM?ESACSYM:RBRACE)) && shlex.token==EOFSYM)
		{
			shlex.lasttok = savetok;
			shlex.lastline = saveline;
			sh_syntax();
		}
		t->sw.swtyp=TSW;
		break;
	    }

	    /* if statement */
	    case IFSYM:
	    {
		union anynode	*tt;
		t = getnode(ifnod);
		t->if_.iftyp=TIF;
		t->if_.iftre=sh_cmd(THENSYM,SH_NL);
		t->if_.thtre=sh_cmd(ELSESYM,SH_NL);
		tok = shlex.token;
		t->if_.eltre=(tok==ELSESYM?sh_cmd(FISYM,SH_NL):
			(tok==ELIFSYM?(shlex.token=IFSYM, tt=item(SH_NOIO)):0));
		if(tok==ELIFSYM)
		{
			if(tt->tre.tretyp!=TSETIO)
				goto done;
			t->if_.eltre = tt->fork.forktre;
			tt->fork.forktre = t;
			t = tt;
			goto done;
		}
		break;
	    }

	    /* for and select statement */
	    case FORSYM:
	    case SELECTSYM:
	    {
		t = getnode(fornod);
		t->for_.fortyp=(shlex.token==FORSYM?TFOR:TSELECT);
		t->for_.forlst=0;
		if(sh_lex())
		{
			if(shlex.token!=EXPRSYM || t->for_.fortyp!=TFOR)
				sh_syntax();
			/* arithmetic for */
			t = arithfor(t);
			break;
		}
		t->for_.fornam=(char*) shlex.arg->argval;
#ifdef SHOPT_KIA
		if(shlex.kiafile)
			sfprintf(shlex.kiafile,"%d;v;%s;%d;%d;%s;;0\n",++shlex.entity,t->for_.fornam,sh.inlineno,sh.inlineno,shlex.parent);
#endif /* SHOPT_KIA */
		while((tok=sh_lex())==NL);
		if(tok==INSYM)
		{
			if(sh_lex())
				sh_syntax();
			t->for_.forlst=(struct comnod*)simple(SH_NOIO,NIL(struct ionod*));
			if(shlex.token != NL && shlex.token !=';')
				sh_syntax();
			tok = skipnl();
		}
		/* 'for i;do cmd' is valid syntax */
		else if(tok==';')
			tok=sh_lex();
		if(tok!=DOSYM && tok!=LBRACE)
			sh_syntax();
		loop_level++;
		t->for_.fortre=sh_cmd(tok==DOSYM?DONESYM:RBRACE,SH_NL);
		if(--loop_level==0)
			label_last = label_list;
		break;
	    }

	    /* This is the code for parsing function definitions */
	    case FUNCTSYM:
		return(funct());

	    /* while and until */
	    case WHILESYM:
	    case UNTILSYM:
		t = getnode(whnod);
		t->wh.whtyp=(shlex.token==WHILESYM ? TWH : TUN);
		loop_level++;
		t->wh.whtre = sh_cmd(DOSYM,SH_NL);
		t->wh.dotre = sh_cmd(DONESYM,SH_NL);
		if(--loop_level==0)
			label_last = label_list;
		t->wh.whinc = 0;
		break;

	    case LABLSYM:
	    {
		struct argnod *argp = label_list;
		while(argp)
		{
			if(strcmp(argp->argval,shlex.arg->argval)==0)
				error(ERROR_exit(3),e_lexsyntax3,sh.inlineno,argp->argval);
			argp = argp->argnxt.ap;
		}
		shlex.arg->argnxt.ap = label_list;
		label_list = shlex.arg;
		label_list->argchn.len = sh_getlineno();
		label_list->argflag = loop_level;
		skipnl();
		if(!(t = item(SH_NL)))
			sh_syntax();
		tok = t->tre.tretyp;
		if(sh_isoption(SH_NOEXEC) && tok!=TWH && tok!=TUN && tok!=TFOR && tok!=TSELECT)
			error(ERROR_warn(0),e_lexlabignore,label_list->argchn.len,label_list->argval);
		return(t);
	    }

	    /* command group with {...} */
	    case LBRACE:
		t = sh_cmd(RBRACE,SH_NL);
		break;

	    case LPAREN:
		t = getnode(parnod);
		t->par.partre=sh_cmd(RPAREN,SH_NL);
		t->par.partyp=TPAR;
		break;

	    default:
		if(io==0)
			return(0);

	    /* simple command */
	    case 0:
		return((union anynode*)simple(flag,io));
	}
	sh_lex();
	if(io=inout(io,0))
	{
		int type = t->tre.tretyp&COMMSK;
		if((tok=t->tre.tretyp&COMMSK) != TFORK)
			tok = TSETIO;
		t=makeparent(tok,t);
		t->tre.treio=io;
	}
done:
	shlex.lasttok = savwdval;
	shlex.lastline = savline;
	return(t);
}

/*
 * This is for a simple command, for list, or compound assignment
 */
static union anynode *simple __PARAM__((int flag, struct ionod *io), (flag, io)) __OTORP__(int flag; struct ionod *io;){
	struct comnod *t;
	struct argnod	*argp;
	int tok;
	struct argnod	**argtail;
	struct argnod	**settail;
	int 	keywd=1;
	int	argno = 0;
	int	assignment = 0;
	int	key_on = (!(flag&SH_NOIO) && sh_isoption(SH_KEYWORD));
	int	assign_type=1;
	if((argp=shlex.arg) && (argp->argflag&ARG_ASSIGN) && argp->argval[0]=='[')
		assign_type = SH_ASSIGN;	/* x=([subscript]=value ...) */
	t = (struct comnod*)getnode(comnod);
	t->comio=io; /*initial io chain*/
	/* set command line number for error messages */
	t->comline = sh_getlineno();
	argtail = &(t->comarg);
	t->comset = 0;
	t->comnamp = 0;
	settail = &(t->comset);
	while(shlex.token==0)
	{
		argp = shlex.arg;
		if(*argp->argval==LBRACE && (flag&SH_FUNDEF) && argp->argval[1]==0)
		{
			shlex.token = LBRACE;
			break;
		}
		if(assign_type==SH_ASSIGN && (!argp || argp->argval[0]!='['))
			sh_syntax();
		/* check for assignment argument */
		if(argp->argflag&ARG_ASSIGN && assignment!=2)
		{
			*settail = argp;
			settail = &(argp->argnxt.ap);
			shlex.assignok = assign_type;
			if(assignment)
			{
				struct argnod *ap=argp;
				char *last, *cp;
				if(assignment==1)
				{
					last = strchr(argp->argval,'=');
					if((cp=strchr(argp->argval,'[')) && (cp < last))
						last = cp;
					stakseek(ARGVAL);
					stakwrite(argp->argval,last-argp->argval);
					ap=(struct argnod*)stakfreeze(1);
					ap->argflag = ARG_RAW;
					ap->argchn.ap = 0;
				}
				*argtail = ap;
				argtail = &(ap->argnxt.ap);
				if(argno>=0)
					argno++;
			}
			else /* alias substitutions allowed */
				shlex.aliasok = 1;
		}
		else
		{
			if(!(argp->argflag&ARG_RAW))
				argno = -1;
			if(argno>=0 && argno++==0)
			{
				/* check for builtin command */
				Namval_t *np = nv_search(argp->argval,sh.fun_tree,0);
				if((t->comnamp=(__V_*)np) &&
					nv_isattr(np,BLT_DCL))
				{
					assignment = 1+(*argp->argval=='a');
					key_on = 1;
				}
			}
			*argtail = argp;
			argtail = &(argp->argnxt.ap);
			shlex.assignok = keywd= key_on;
			shlex.aliasok = 0;
		}
	retry:
		tok = sh_lex();
#ifdef SHOPT_DEVFD
		if((tok==IPROCSYM || tok==OPROCSYM))
		{
			union anynode *t;
			int mode = (tok==OPROCSYM);
			t = sh_cmd(RPAREN,SH_NL);
			argp = (struct argnod*)stakalloc(sizeof(struct argnod));
			*argp->argval = 0;
			argno = -1;
			*argtail = argp;
			argtail = &(argp->argnxt.ap);
			argp->argchn.ap = (struct argnod*)makeparent(mode?TFORK|FPIN|FAMP|FPCL:TFORK|FPOU,t);
			argp->argflag =  (ARG_EXP|mode);
			goto retry;
		}
#endif	/* SHOPT_DEVFD */
		if(tok==LPAREN)
		{
			if(argp->argflag&ARG_ASSIGN)
			{
				argp = assign(argp);
				goto retry;
			}
			if(argno==1 && !t->comset)
			{
				/* SVR2 style function */
				if(sh_lex() == RPAREN)
				{
					shlex.arg = argp;
					return(funct());
				}
				shlex.token = LPAREN;
			}
		}
		else if(tok==RPAREN && (flag&SH_ASSIGN))
			break;
		else if(tok==NL && shlex.assignok==SH_ASSIGN)
			goto retry;
		if(!(flag&SH_NOIO))
		{
			if(io)
			{
				while(io->ionxt)
					io = io->ionxt;
				io->ionxt = inout((struct ionod*)0,0);
			}
			else
				t->comio = io = inout((struct ionod*)0,0);
		}
	}
	*argtail = 0;
	t->comtyp = TCOM;
#ifdef SHOPT_KIA
	if(shlex.kiafile)
	{
		Namval_t *np=(Namval_t*)t->comnamp;
		int line = t->comline;
		argp = t->comarg;
		if(argp)
		{
			sfprintf(shlex.kiatmp,"%c;%d;p;%s;%d;%d;;\n",(shlex.current?'p':'s'),shlex.current,sh_argstr(argp),line,line);
		}
		if(np && nv_size(np)==0)
		{
			nv_setsize(np,1);
			sfprintf(shlex.kiafile,"%d;p;%s;0;0;0;b;;\n",++shlex.entity,nv_name(np));
		}
		if(t->comset && argno==0)
			writedefs(t->comset,line,'v',t->comarg);
		else if(np && nv_isattr(np,BLT_DCL))
			writedefs(argp,line,0,NIL(struct argnod*));
		else if(argp && strcmp(argp->argval,"read")==0)
			writedefs(argp,line,0,NIL(struct argnod*));
		else if(argp && *argp->argval=='.' && argp->argval[1]==0 && (argp=argp->argnxt.ap))
		{
			if((argp->argflag&ARG_RAW) && !nv_search(argp->argval,shlex.file_tree,0))
			{
				sfprintf(shlex.kiafile,"%d;f;%s;0;0;0;d;;\n",++shlex.entity,argp->argval);
				nv_search(argp->argval,shlex.file_tree,NV_ADD);
			}
			sfprintf(shlex.kiatmp,"%c;%d;f;%s;%d;%d;%c;\n",(shlex.current?'p':'s'),shlex.current,sh_argstr(argp),line,line,'d');
		}
	}
#endif /* SHOPT_KIA */
	if(t->comnamp && (argp=t->comarg->argnxt.ap))
	{ 
		Namval_t *np=(Namval_t*)t->comnamp;
		if((np==SYSBREAK || np==SYSCONT) && (argp->argflag&ARG_RAW) && !isdigit(*argp->argval))
		{
			char *cp = argp->argval;
			/* convert break/continue labels to numbers */
			tok = 0;
			for(argp=label_list;argp!=label_last;argp=argp->argnxt.ap)
			{
				if(strcmp(cp,argp->argval))
					continue;
				tok = loop_level-argp->argflag;
				if(tok>=1)
				{
					argp = t->comarg->argnxt.ap;
					if(tok>9)
					{
						argp->argval[1] = '0'+tok%10;
						argp->argval[2] = 0;
						tok /= 10;
					}
					else
						argp->argval[1] = 0;
					*argp->argval = '0'+tok;
				}
				break;
			}
			if(sh_isoption(SH_NOEXEC) && tok==0)
				error(ERROR_warn(0),e_lexlabunknown,sh.inlineno-(shlex.token=='\n'),cp);
		}
		else if(sh_isoption(SH_NOEXEC) && np==SYSSET && ((tok= *argp->argval)=='-'||tok=='+') &&
			(argp->argval[1]==0||strchr(argp->argval,'k')))
			error(ERROR_warn(0),e_lexobsolete5,sh.inlineno-(shlex.token=='\n'),argp->argval);
	}
	/* expand argument list if possible */
	if(argno>0)
		t->comarg = qscan(t,argno);
	else if(t->comarg)
		t->comtyp |= COMSCAN;
	shlex.aliasok = 0;
	return((union anynode*)t);
}

/*
 * skip past newlines but issue prompt if interactive
 */
static int	skipnl __PARAM__((void), ()){
	int token;
	while((token=sh_lex())==NL);
	if(token==';')
		sh_syntax();
	return(token);
}

/*
 * check for and process and i/o redirections
 * if flag is set then an alias can be in the next word
 */
static struct ionod	*inout __PARAM__((struct ionod *lastio,int flag), (lastio, flag)) __OTORP__(struct ionod *lastio;int flag;){
	int 		iof = shlex.digits, token=shlex.token;
	struct ionod	*iop;
	switch(token&0xff)
	{
	    case '<':
		if(token==IODOCSYM)
			iof |= (IODOC|IORAW);
		else if(token==IOMOV0SYM)
			iof |= IOMOV;
		else if(token==IORDWRSYM)
			iof |= IORDW;
		break;

	    case '>':
		iof |= IOPUT;
		if(token==IOAPPSYM)
			iof |= IOAPP;
		else if(token==IOMOV1SYM)
			iof |= IOMOV;
		else if(token==IOCLOBSYM)
			iof |= IOCLOB;
		break;

	    default:
		return(lastio);
	}
	shlex.digits=0;
	if(sh_lex())
		sh_syntax();
	iop=(struct ionod*) stakalloc(sizeof(struct ionod));
	iop->ioname=shlex.arg->argval;
	iop->iodelim = 0;
	if(iof&IODOC)
	{
		iop->iolst=shlex.heredoc;
		shlex.heredoc=iop;
		if(shlex.arg->argflag&ARG_QUOTED)
			iof |= IOQUOTE;
		if(shlex.digits)
			iof |= IOSTRIP;
	}
	else
	{
		iop->iolst = 0;
		if(shlex.arg->argflag&ARG_RAW)
			iof |= IORAW;
	}
	iop->iofile=iof;
	if(flag)
		/* allow alias substitutions and parameter assignments */
		shlex.aliasok = shlex.assignok = 1;
#ifdef SHOPT_KIA
	if(shlex.kiafile)
	{
		flag = sh.inlineno-(shlex.token=='\n');
		if((iof&IORAW) && !(iof&IOMOV) && !nv_search(iop->ioname,shlex.file_tree,0))
		{
			sfprintf(shlex.kiafile,"%d;f;%s;0;0;0;d;;\n",++shlex.entity,iop->ioname);
			nv_search(iop->ioname,shlex.file_tree,NV_ADD);
		}
		sfprintf(shlex.kiatmp,"%c;%d;f;%s;%d;%d;%c;%d\n",(shlex.current?'p':'s'),shlex.current,(iof&IORAW)?sh_fmtq(iop->ioname):iop->ioname,flag,flag,(iof&IOPUT)?((iof&IOAPP)?'a':'w'):((iof&IODOC)?'h':'r'),iof&IOUFD);
	}
#endif /* SHOPT_KIA */
	sh_lex();
	iop->ionxt=inout(lastio,flag);
	return(iop);
}

/*
 * convert argument chain to argument list when no special arguments
 */

static struct argnod *qscan __PARAM__((struct comnod *ac,int argn), (ac, argn)) __OTORP__(struct comnod *ac;int argn;){
	char **cp;
	struct argnod *ap;
	struct dolnod* dp;
	int special=0;
	/* special hack for test -t compatibility */
	if((Namval_t*)ac->comnamp==SYSTEST)
		special = 2;
	else if(*(ac->comarg->argval)=='[' && ac->comarg->argval[1]==0)
		special = 3;
	if(special)
	{
		ap = ac->comarg->argnxt.ap;
		if(argn==(special+1) && ap->argval[1]==0 && *ap->argval=='!')
			ap = ap->argnxt.ap;
		else if(argn!=special)
			special=0;
	}
	if(special)
	{
		const char *message;
		if(strcmp(ap->argval,"-t"))
		{
			message = "line %d: Invariant test";
			special=0;
		}
		else
		{
			message = "line %d: -t requires argument";
			argn++;
		}
		if(sh_isoption(SH_NOEXEC))
			error(ERROR_warn(0),message,ac->comline);
	}
	/* leave space for an extra argument at the front */
	dp = (struct dolnod*)stakalloc((unsigned)sizeof(struct dolnod) + ARG_SPARE*sizeof(char*) + argn*sizeof(char*));
	cp = dp->dolval+ARG_SPARE;
	dp->dolnum = argn;
	dp->dolbot = ARG_SPARE;
	ap = ac->comarg;
	while(ap)
	{
		*cp++ = ap->argval;
		ap = ap->argnxt.ap;
	}
	if(special==3)
	{
		cp[0] = cp[-1];
		cp[-1] = "1";
		cp++;
	}
	else if(special)
		*cp++ = "1";
	*cp = 0;
	return((struct argnod*)dp);
}

static union anynode *test_expr __PARAM__((int sym), (sym)) __OTORP__(int sym;){
	union anynode *t = test_or();
	if(shlex.token!=sym)
		sh_syntax();
	return(t);
}

static union anynode *test_or __PARAM__((void), ()){
	union anynode *t = test_and();
	while(shlex.token==ORFSYM)
		t = makelist(TORF|TTEST,t,test_and());
	return(t);
}

static union anynode *test_and __PARAM__((void), ()){
	union anynode *t = test_primary();
	while(shlex.token==ANDFSYM)
		t = makelist(TAND|TTEST,t,test_primary());
	return(t);
}

static union anynode *test_primary __PARAM__((void), ()){
	struct argnod *arg;
	union anynode *t;
	int num,token;
	token = skipnl();
	num = shlex.digits;
	switch(token)
	{
	    case '(':
		t = test_expr(')');
		t = makelist(TTST|TTEST|TPAREN ,t, (union anynode*)sh.inlineno);
		break;
	    case '!':
		t = test_primary();
		t->tre.tretyp |= TNEGATE;
		return(t);
	    case TESTUNOP:
		if(sh_lex())
			sh_syntax();
#ifdef SHOPT_KIA
		if(shlex.kiafile && !strchr("sntzoOG",num))
		{
			int line = sh.inlineno- (shlex.token==NL);
			if((shlex.arg->argflag&ARG_RAW) && !nv_search(shlex.arg->argval,shlex.file_tree,0))
			{
				sfprintf(shlex.kiafile,"%d;f;%s;0;0;0;t;;\n",++shlex.entity,shlex.arg->argval);
				nv_search(shlex.arg->argval,shlex.file_tree,NV_ADD);
			}
			sfprintf(shlex.kiatmp,"%c;%d;f;%s;%d;%d;%c;\n",(shlex.current?'p':'s'),shlex.current,sh_argstr(shlex.arg),line,line,'t');
		}
#endif /* SHOPT_KIA */
		t = makelist(TTST|TTEST|TUNARY|(num<<TSHIFT),
			(union anynode*)shlex.arg,(union anynode*)shlex.arg);
		t->tst.tstline =  sh.inlineno;
		break;
	    /* binary test operators */
	    case 0:
		arg = shlex.arg;
		if((token=sh_lex())==TESTBINOP)
			num = shlex.digits;
		else if(token=='<')
			num = TEST_SLT;
		else if(token=='>')
			num = TEST_SGT;
		else if(token==ANDFSYM||token==ORFSYM||token==ETESTSYM)
		{
			t = makelist(TTST|TTEST|TUNARY|('n'<<TSHIFT),
				(union anynode*)arg,(union anynode*)arg);
			t->tst.tstline =  sh.inlineno;
			return(t);
		}
		else
			sh_syntax();
#ifdef SHOPT_KIA
		if(shlex.kiafile && (num==TEST_EF||num==TEST_NT||num==TEST_OT))
		{
			int line = sh.inlineno- (shlex.token==NL);
			sfprintf(shlex.kiatmp,"%c;%d;f;%s;%d;%d;%c;\n",(shlex.current?'p':'s'),shlex.current,sh_argstr(shlex.arg),line,line,'t');
		}
#endif /* SHOPT_KIA */
		if(sh_lex())
			sh_syntax();
		if(num&TEST_PATTERN)
		{
			if(shlex.arg->argflag&(ARG_EXP|ARG_MAC))
				num &= ~TEST_PATTERN;
		}
		t = getnode(tstnod);
		t->lst.lsttyp = TTST|TTEST|TBINARY|(num<<TSHIFT);
		t->lst.lstlef = (union anynode*)arg;
		t->lst.lstrit = (union anynode*)shlex.arg;
		t->tst.tstline =  sh.inlineno;
#ifdef SHOPT_KIA
		if(shlex.kiafile && (num==TEST_EF||num==TEST_NT||num==TEST_OT))
		{
			int line = sh.inlineno-(shlex.token==NL);
			sfprintf(shlex.kiatmp,"%c;%d;f;%s;%d;%d;%c;\n",(shlex.current?'p':'s'),shlex.current,sh_argstr(shlex.arg),line,line,'t');
		}
#endif /* SHOPT_KIA */
	}
	skipnl();
	return(t);
}

#ifdef SHOPT_KIA
int kiaclose __PARAM__((void), ()){
	off_t off1,off2;
	int n;
	if(shlex.kiafile)
	{
		sfseek(shlex.kiafile,shlex.kiabegin,SEEK_SET);
		sfprintf(shlex.kiafile,"%6d", sh.inlineno-1);
		off1 = sfseek(shlex.kiafile,(off_t)0,SEEK_END);
		sfseek(shlex.kiatmp,(off_t)0,SEEK_SET);
		sfmove(shlex.kiatmp,shlex.kiafile,SF_UNBOUND,-1);
		off2 = sfseek(shlex.kiafile,(off_t)0,SEEK_END);
		n= sfprintf(shlex.kiafile,"DIRECTORY\n;SYM;0;%d\n;REF;%d;%d\n;DIRECTORY;",off1,off1,off2-off1);
		sfprintf(shlex.kiafile,"%010d;%010d\n",off2+10, n+22);
	}
	return(sfclose(shlex.kiafile));
}
#endif /* SHOPT_KIA */