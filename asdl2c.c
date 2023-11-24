#include <asdl2c.h>

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

			struct Enum
			{
				char**
				name_enum;
				
				int
				num_enum;
			
			}
			EnumDecl;

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
*LhsLs;

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

