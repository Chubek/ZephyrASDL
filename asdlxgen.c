#include "asdl2c.h"

static struct 
Lhs 
{
	struct Lhs* 
	next;
	
	char*	      
	name;

	struct Rhs
	{
		struct Rhs*
		next;

		union 
		Decl
		{
			struct 
			Con 
			{
				char*	     
				name;

				struct 
				Param 
				{
					struct Param*
					next;
					
					struct Lhs*
					type;

					char*
					name;
					
					enum Opt
					{
						OPTIONAL,
						SEQUENCE,
						NONE,
					}
					Option;
			
				}	  
				*ParamLs;
			}		  
			ConsDecl;

			struct Tuple
			{
				struct Lhs* 
				a_tyy;
				struct Lhs*
				b_tyy;

				char*
				a_name;
				char*
				b_name;
				
				enum Opt
				a_opt;
				enum Opt
				b_opt;

			}
			TupleDecl;

			char* enum_decl;

			struct Param*
			attribute_decl;
		}
		Decl;
	
		enum
		DeclType
		{
			CONS,
			TUPLE,
			ENUM,
			ATTR,
		}
		DeclType;

	}
	*RhsLs;
	
	enum BuiltIn
	{
		IDENT,
		INT,
		STRING,
		BOOLEAN,
		FLOAT,
		NOT_BUILTIN,
	}
	BuiltInType;
} 
*LhsLs, *IntLhs, *StingLhs, *IdentLhs, *FloatLhs;

static struct
Bank
{
	void*		allocptr;
	bool		freed;
	struct Bank*	next;
}
*MemoryBank;

static inline void
init_memorybank(void)
{
	MemoryBank	= calloc(1, sizeof(Bank));
}

static inline void
free_memorybank(void)
{
	struct Bank*	node;
	for (node = MemoryBank; node != NULL; node = node->next)
	{
		free(node->allocptr);
		free(node);
	}
}

static inline void* 
request_memory(size_t size)
{
	void* res;
	struct Bank*	node 	= calloc(1, sizeof(struct Bank));
	node->next		= MemoryBank;
	node->allocptr = res	= calloc(1, size);
	MemoryBank		= node;
	return res;
}

void
init_builtins(void)
{
	IntLhs 		= request_memory(sizeof(struct Lhs));
	StringLhs	= request_memory(sizeof(struct Lhs));
	IdentLhs	= request_memory(sizeof(struct Lhs));
	FloatLhs	= request_memory(sizeof(struct Lhs));

	IntLhs->name		= "tyy_int";
	StringLhs->name		= "tyy_str";
	IdentLhs->name		= "tyy_ident";
	FloatLhs->name		= "tyy_float";

	IntLhs->BuiltinType	= INT;
	StringLhs-BuiltinType	= STRING;
	IdentLhs->BuiltinType	= IDENT;
	FloatLhs->BuiltinType	= FLOAT;
}

char*
duplicate_string(char* s)
{
	size_t len		= strlen(s);
	char*  str 		= request_memory(len + 1);
	strncpy(&str[0], &str[0], len);
	return str;
	
}

void
init_lhsls(void)
{
	LhsLs	= request_memory(sizeof(struct Lhs));
}

void
append_lhsls(void)
{
	struct Lhs*  node 	= request_memory(sizeof(struct Lhs));
	node->next		= LhsLs;
	LhsLs			= node;
}

void
init_rhsls(void)
{
	LhsLs->RhsLs		= request_memory(sizeof(struct Rhs));
}

void
append_rhsls(void)
{
	struct Rhs*  node	= request_memory(sizeof(struct Rhs));
	node->next		= RhsLs;
	RhsLs			= node;
}

void
init_cons_decl(char* name)
{
	LhsLs->RhsLs->Decl.ConsDecl.name = duplicate_string(name);
	LhsLs->RhsLs->Decl.ConsDecl.ParamLs = 
			request_memory(sizeof(struct Param));
	LhsLs->RhsLs.DeclType = CONS;

}

void
init_tuple_decl(void) 
{
	LhsLs->RhsLs->Decl.DeclType = TUPLE;
}

void
init_enum_decl(void)
{
	LhsLs->RhsLs->Decl.DeclType = ENUM;
}

void
init_attr_decl(void)
{
	LhsLs->RhsLs->Decl.attribute_decl = 
			request_memory(sizeof(struct Param));
	LhsLs->RhsLs->Decl.DeclType = ATTR;
}

void
add_cons_param(char* name, struct Lhs* type, enum Opt opt)
{
	struct Param* node	= request_memory(sizeof(struct Param));
	node->next		= LhsLs->RhsLs->Decl.ConsDecl.ParamsLs;
	node->name		= duplicate_string(name);
	node->type		= type;
	node->opt		= opt;
	LhsLs->RhsLs->Decl.ConsDecl.ParamsLs = node;
}

void
add_tuple_atype(struct Lhs* tyy)
{
	LhsLs->RhsLs->Decl.TupleDecl.a_tyy = tyy;
}

void
add_tuple_btype(struct Lhs* tyy)
{
	LhsLs->RhsLs->Decl.TupleDecl.b_tyy = tyy;
}

void
add_tuple_aname(char* name)
{
	LhsLs->RhsLs->Decl.TupleDecl.a_name = duplcate_string(name);
}

void
add_tuple_bname(char* name)
{
	LhsLs->RhsLs->Decl.TupleDecl.b_name = duplicate_string(name);
}

void
add_tuple_aopt(enum Opt opt)
{
	LhsLs->RhsLs->Decl.TupleDecl.a_opt  = opt;
}

void
add_tuple_bopt(enum Opt opt)
{
	LhsLs->RhsLs->Decl.TupleDecl.b_opt  = opt;
}

void
set_builtin(enum BuiltIn tyy)
{
	LhsLs->BuiltinType	= tyy;
}

void
set_enumeration(char* enumeration)
{
	LhsLs->RhsLs->Decl.enum_decl = duplicate_string(enumeration);
}

static struct Lhs*
lhs_current = NULL;

bool
set_curr_lhs(char* name)
{
	size_t 		len	= strlen(name);
	struct Lhs*	lhs	= NULL;

	for (lhs = Lhs; lhs != NULL; lhs = lhs->next)
	{
		if (!strncmp(lhs->name, name, len)) 
		{
			lhs_current = lhs;
			return true;
		}
	}
	return false;
}

static struct Rhs* 
rhs_iter = NULL;

static struct Lhs* 
lhs_iter = NULL;

static struct Param*
param_iter = NULL;


static struct Rhs*
next_rhs(void)
{
	if (rhs_iter == NULL)
	{
		rhs_iter = lhs_iter->RhsLs;
		return rhs_iter;
	}
	rhs_iter = rhs_iter->next;
	return rhs_iter;
}

static struct Lhs*
next_lhs(void)
{
	if (lhs_iter == NULL)
	{
		lhs_iter = LhsLs;
		return lhs_iter;
	}
	lhs_iter = lhs_iter->next;
	return lhs_iter;
}

static struct Param*
next_param(void)
{
	if (param_iter == NULL)
	{
		param_iter = rhs_iter->Decl.ConsDecl.ParamLs;
		return param_iter;
	}
	param_iter = param_iter->next;
	return param_iter;
}

#define IS_LHS_BUILTIN		lhs_ter->BuiltinType

#define CURR_PARAM_NAME		param_iter->name
#define CURR_PARAM_TYPE		param_iter->type
#define CURR_PARAM_OPT		param_iter->opt

#define CURR_DECL_TYPE		rhs_iter->Decl.DeclType

#define CURR_TUPLE_TYPE_A	rhs_iter->Decl.TupleDecl.a_tyy
#define CURR_TUPLE_TYPE_B	rhs_iter->Decl.TupleDecl.b_tyy

#define CURR_TUPLE_NAME_A	rhs_iter->Decl.TupleDecl.a_name
#define CURR_TUPLE_NAME_B	rhs_iter->Decl.TupleDecl.b_name

#define CURR_TUPLE_OPT_A	rhs_iter->Decl.TupleDecl.a_opt
#define CURR_TUPLE_OPT_B	rhs_iter->Decl.TupleDecl.b_opt

#define CURR_ENUM_DECL		rhs_iter->Decl.enum_decl
#define CURR_ATTR_DECL		rhs_iter->Decl.attributes_decl


#define OUT(file, fmt, ...)	fprintf(file, fmt, __VA_ARGS__)


FILE* typedefs_stream = NULL;
FILE* structs_stream  = NULL;
FILE* enums_stream    = NULL;
FILE* funcs_stream    = NULL;

static void
open_outputs(void)
{
	typedefs_stream = tmpfile();
	structs_stream  = tmpfile();
	enums_stream    = tmpfile();
	funcs_stream	= tmpfile();
}

static void
close_outputs(void)
{
	fclose(typedefs_stream);
	fclose(structs_stream);
	fclose(enums_stream);
	fclose(funcs_stream);
}

static void
concat_outputs(char* path, char* init_txt){
	FILE* 	concat_stream 	= fopen(path, "w");
	int	concat_fd	= fileno(concat_stream);
	fputs(init_txt, concat);

	FILE* streams[] = { 
				typedefs_stream, 
				structs_stream,
				enums_stream,
				funcs_stream,
				NULL
			  };
	FILE** streams_ptr = &streams[0];
	for (FILE* stream = *streams_ptr; 
			stream != NULL;
			stream  = *streams_ptr++)
	{
		fseek(stream, 0, SEEK_END);
		long len = ftell(stream);
		rewind(stream);
		if (tee(concat_fd, fileno(stream),
					len, 
					SPLICE_F_NONBLOCK) < 0) 
		{
			perror("tee");
		}
	} 

	fclose(concat_stream);
}

