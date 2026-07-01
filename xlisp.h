#include <stdio.h>

/* xlisp - a small subset of lisp */

/* system specific definitions */

/* DEFEXT       define to enable default extension of '.lsp' on 'load' */
/* FGETNAME     define if system supports 'fgetname' */
/* NNODES       number of nodes to allocate in each request */
/* xlisp - a small subset of lisp */

/* system specific definitions */
#define UNIX

#ifdef AZTEC
#include "setjmp.h"
#include "stdio.h"
#else
#include <ctype.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

/* NNODES       number of nodes to allocate in each request */
/* TDEPTH       trace stack depth */
/* FORWARD      type of a forward declaration (usually "") */
/* LOCAL        type of a local function (usually "static") */

/* for the Computer Innovations compiler */
#ifdef CI
#define NNODES 1000
#define TDEPTH 500
#endif

/* for the CPM68K compiler */
#ifdef CPM68K
#define NNODES 1000
#define TDEPTH 500
#define LOCAL
#define AFMT "%lx"
#undef NULL
#define NULL (char *)0
#endif

/* for the DeSmet compiler */
#ifdef DESMET
#define NNODES 1000
#define TDEPTH 500
#define LOCAL
#define getc(fp) getcx(fp)
#define putc(ch, fp) putcx(ch, fp)
#define EOF -1
#endif

/* for the MegaMax compiler */
#ifdef MEGAMAX
#define NNODES 200
#define TDEPTH 100
#define LOCAL
#define AFMT "%lx"
#define TSTKSIZE (4 * TDEPTH)
#endif

/* for the VAX-11 C compiler */
#ifdef vms
#define NNODES 2000
#define TDEPTH 1000
#endif

/* for the DECUS C compiler */
#ifdef decus
#define NNODES 200
#define TDEPTH 100
#define FORWARD extern
#endif

/* for unix compilers */
#ifdef unix
#define NNODES 200
#define TDEPTH 100
#endif

/* for the AZTEC C compiler */
#ifdef AZTEC
#define NNODES 200
#define TDEPTH 100
#define getc(fp) agetc(fp)
#define putc(ch, fp) aputc(ch, fp)
#endif

/* default important definitions */
#ifndef NNODES
#define NNODES 200
#endif
#ifndef TDEPTH
#define TDEPTH 100
#endif
#ifndef FORWARD
#define FORWARD
#endif
#ifndef LOCAL
#define LOCAL static
#endif
#ifndef AFMT
#define AFMT "%p"
#endif
#ifndef TSTKSIZE
#define TSTKSIZE (sizeof(NODE *) * TDEPTH)
#endif

/* useful definitions */
#define TRUE 1
#define FALSE 0
#define NIL (NODE *)0

/* program limits */
#define STRMAX 100 /* maximum length of a string constant */

/* node types */
#define FREE 0
#define SUBR 1
#define FSUBR 2
#define LIST 3
#define SYM 4
#define INT 5
#define STR 6
#define OBJ 7
#define FPTR 8

/* node flags */
#define MARK 1
#define LEFT 2

/* string types */
#define DYNAMIC 0
#define STATIC 1

/* new node access macros */
#define ntype(x) ((x)->n_type)
#define atom(x) ((x) == NIL || (x)->n_type != LIST)
#define null(x) ((x) == NIL)
#define listp(x) ((x) == NIL || (x)->n_type == LIST)
#define consp(x) ((x) && (x)->n_type == LIST)
#define subrp(x) ((x) && (x)->n_type == SUBR)
#define fsubrp(x) ((x) && (x)->n_type == FSUBR)
#define stringp(x) ((x) && (x)->n_type == STR)
#define symbolp(x) ((x) && (x)->n_type == SYM)
#define filep(x) ((x) && (x)->n_type == FPTR)
#define objectp(x) ((x) && (x)->n_type == OBJ)
#define fixp(x) ((x) && (x)->n_type == INT)
#define car(x) ((x)->n_car)
#define cdr(x) ((x)->n_cdr)
#define rplaca(x, y) ((x)->n_car = (y))
#define rplacd(x, y) ((x)->n_cdr = (y))

/* symbol node */
#define n_symplist n_info.n_xsym.xsy_plist
#define n_symvalue n_info.n_xsym.xsy_value

/* subr/fsubr node */
#define n_subr n_info.n_xsubr.xsu_subr

/* list node */
#define n_car n_info.n_xlist.xl_car
#define n_cdr n_info.n_xlist.xl_cdr
#define n_ptr n_info.n_xlist.xl_car

/* integer node */
#define n_int n_info.n_xint.xi_int

/* string node */
#define n_str n_info.n_xstr.xst_str
#define n_strtype n_info.n_xstr.xst_type

/* object node */
#define n_obclass n_info.n_xobj.xo_obclass
#define n_obdata n_info.n_xobj.xo_obdata

/* file pointer node */
#define n_fp n_info.n_xfptr.xf_fp
#define n_savech n_info.n_xfptr.xf_savech

/* node structure */
typedef struct node {
  char n_type;                /* type of node */
  char n_flags;               /* flag bits */
  union {                     /* value */
    struct xsym {             /* symbol node */
      struct node *xsy_plist; /* symbol plist - (name . plist) */
      struct node *xsy_value; /* the current value */
    } n_xsym;
    struct xsubr {                /* subr/fsubr node */
      struct node *(*xsu_subr)(); /* pointer to an internal routine */
    } n_xsubr;
    struct xlist {         /* list node (cons) */
      struct node *xl_car; /* the car pointer */
      struct node *xl_cdr; /* the cdr pointer */
    } n_xlist;
    struct xint { /* integer node */
      int xi_int; /* integer value */
    } n_xint;
    struct xstr {    /* string node */
      int xst_type;  /* string type */
      char *xst_str; /* string pointer */
    } n_xstr;
    struct xobj {              /* object node */
      struct node *xo_obclass; /* class of object */
      struct node *xo_obdata;  /* instance data */
    } n_xobj;
    struct xfptr {   /* file pointer node */
      FILE *xf_fp;   /* the file pointer */
      int xf_savech; /* lookahead character for input files */
    } n_xfptr;
  } n_info;
} NODE;

/* execution context flags */
#define CF_GO 1
#define CF_RETURN 2
#define CF_THROW 4
#define CF_ERROR 8

/* execution context */
typedef struct context {
  int c_flags;                       /* context type flags */
  struct node *c_expr;               /* expression (type dependant) */
  jmp_buf c_jmpbuf;                  /* longjmp context */
  struct context *c_xlcontext;       /* old value of xlcontext */
  struct node *c_xlstack;            /* old value of xlstack */
  struct node *c_xlenv, *c_xlnewenv; /* old values of xlenv and xlnewenv */
  int c_xltrace;                     /* old value of xltrace */
} CONTEXT;

/* function table entry structure */
struct fdef {
  char *f_name;            /* function name */
  int f_type;              /* function type SUBR/FSUBR */
  struct node *(*f_fcn)(); /* function code */
};

/* memory segment structure definition */
struct segment {
  int sg_size;
  struct segment *sg_next;
  struct node sg_nodes[1];
};

/* external procedure declarations */
extern struct node *xleval(struct node *expr);
extern struct node *xlapply(struct node *fun, struct node *args);
extern struct node *xlevlist(struct node *args);
extern struct node *xlarg(struct node **pargs);
extern struct node *xlevarg(struct node **pargs);
extern struct node *xlmatch(int type, struct node **pargs);
extern struct node *xlevmatch(int type, struct node **pargs);
extern struct node *xlsend(struct node *obj, struct node *args);
extern struct node *xlenter(char *name, int type);
extern struct node *xlsenter(char *name);
extern struct node *xlintern(char *name, int type);
extern struct node *xlmakesym(char *name, int type);
extern struct node *xlsave(struct node *n,
                           ...); /* generate a stack frame (NULL-terminated) */
extern struct node *xlobsym(struct node *sym);
extern struct node *xlgetprop(struct node *sym, struct node *prp);
extern char *xlsymname(struct node *sym);

extern struct node *newnode(int type);
extern char *stralloc(int size);
extern char *strsave(char *str);

/* void-returning internal routines */
extern void xlinit(void);
extern void xlminit(void);
extern void xlsinit(void);
extern void xldinit(void);
extern void xloinit(void);
extern void xlsubr(char *sname, int type, struct node *(*subr)());
extern void xlfail(char *emsg);
extern void xlabort(char *emsg);
extern void xlbreak(char *emsg, struct node *arg);
extern void xlerror(char *emsg, struct node *arg);
extern void xlcerror(char *cmsg, char *emsg, struct node *arg);
extern void xlerrprint(char *hdr, char *cmsg, char *emsg, struct node *arg);
extern void xlsignal(char *emsg, struct node *arg);
extern void xltpush(struct node *nptr);
extern void xltpop(void);
extern void xlbaktrace(int n);
extern void xlbegin(CONTEXT *cptr, int flags, struct node *expr);
extern void xlend(CONTEXT *cptr);
extern void xlgo(struct node *label);
extern void xlreturn(struct node *val);
extern void xlthrow(struct node *tag, struct node *val);
extern void xlunbind(struct node *env);
extern void xlsbind(struct node *sym, struct node *val);
extern void xlabind(struct node *fargs, struct node *aargs);
extern void xlfixbindings(void);
extern void xlbind(struct node *sym, struct node *val);
extern void xlunbound(struct node *sym);
extern void xlputprop(struct node *sym, struct node *val, struct node *prp);
extern void xlremprop(struct node *sym, struct node *prp);
extern void xlprint(struct node *fptr, struct node *vptr, int flag);
extern void xlterpri(struct node *fptr);
extern void xlputc(struct node *fptr, int ch);
extern int xlgetc(struct node *fptr);
extern int xlpeek(struct node *fptr);
extern void xlflush(void);
extern int xlread(struct node *fptr, struct node **pval);
extern int xlload(char *name, int vflag, int pflag);
extern void stdprint(struct node *expr);
extern void xllastarg(struct node *args);
extern void assign(struct node *sym, struct node *val);
extern void gc(void);
extern int addseg(void);
extern void stats(void);
extern void strfree(char *str);

/* xlobj.c public functions */
extern struct node *xlgetivar(struct node *obj, int num);
extern struct node *xlsetivar(struct node *obj, int num, struct node *val);
extern struct node *xlivar(struct node *obj, int num);
extern struct node *xlcvar(struct node *cls, int num);
extern struct node *xlxsend(struct node *obj, struct node *msg,
                            struct node *args);
extern struct node *xlsend(struct node *obj, struct node *args);
extern void xladdmsg(struct node *cls, char *msg, struct node *(*code)());
extern struct node *xlclass(char *name, int vcnt);
