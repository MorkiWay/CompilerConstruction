#include "y.tab.h"
#include "ast.h"
int yyparse();

struct variable_node {char type; char* var_name; struct variable_node* next; int open; int close;};
struct function {char* fun_name; struct variable_node* vars; int args; char return_type;};
struct function_node  {struct function fun; struct function_node* next; int finds;};


struct function_node* fun_list = NULL;
struct function* var_list=NULL;

void check_var_against_functions(char* var_name)
{
	struct function_node* temp = fun_list;
	while (temp!=NULL)
	{
		if (strcmp(var_name, temp->fun.fun_name)==0)
		{
			printf("Variable %s has the name of a defined function\n",var_name);
			exit(0);
		}
		temp=temp->next;
	}

};


void free_vars(struct variable_node* vars)
{
	struct variable_node* temp=NULL;
	
	free(vars->var_name);
	//printf("in free_vars\n");fflush(stdout);
	while(vars->next!=NULL)
	{
		temp=vars->next;
		//free(vars->var_name);
		free(vars);
		//printf("in loop\n");fflush(stdout);
		vars=temp;
		//printf("%s\n",vars->var_name);fflush(stdout);
		free(vars->var_name);
	}
	free(vars);

	//printf("out of loop\n"); fflush(stdout);
};

void free_function(struct function f)
{
	free(f.fun_name);
	//printf("%s","HERE\n");fflush(stdout);
	if(f.vars!=NULL)
		free_vars(f.vars);
};


int insert_var(struct function* f,char *n, char t, int o, int c)
{
	f->args++;
		
	for (int p=o+1; p<=c; p++)
	{
		struct ast* temp_node=find_ast_node(p);
		if (strcmp(n, temp_node->token)==0)
		{
			//printf("FOUND: %s\n", temp_node->token);
			//printf("WAS: %c",temp_node->type);
			temp_node->type = t;
			//printf("NOW: %c",temp_node->type);
		}
	}


	check_var_against_functions(n);

	struct variable_node* new_node = malloc (sizeof(struct variable_node));

		new_node->var_name=malloc(sizeof(char)*(strlen(n)+1));
		strcpy(new_node->var_name,n);
		new_node->type=t;
		new_node->next=NULL;
		new_node->open=o;
		new_node->close=c;
	
	struct variable_node* temp = f->vars;
	
	if (f->vars==NULL)
	{
		f->vars=new_node;
	}
	else if (f->vars->next==NULL)
	{
		if (strcmp(f->vars->var_name,n)==0 && !(f->vars->close < o || c < f->vars->open))
			{printf("Variable %s is declared twice\n",n); exit(0);}
		else
			f->vars->next=new_node;

	//	printf("3\n");fflush(stdout);
	}
	else
	{
		while (temp->next!=NULL)
		{
			//printf("CURRENT NODE AGAIN: %s\n",temp->var_name);
			if (strcmp(temp->next->var_name,n)==0 && !( temp->next->close < o || c < temp->next->open) )
				{printf("Variable %s is defined twice\n",n); exit(0);}
			temp=temp->next;
		}
		temp->next=new_node;
	}

	return 0;
};

struct function_node* insert_function(struct function_node* f, char* n, char t)
{

	struct function_node* new_function=malloc(sizeof(struct function_node));

	new_function->fun.fun_name = malloc(sizeof(char)*(strlen(n)+1));
	strcpy(new_function->fun.fun_name,n);
	new_function->next=NULL;
	new_function->fun.return_type=t;
	new_function->fun.args=0;
	new_function->fun.vars=NULL;
	new_function->finds=0;

	struct ast* temp_node = find_ast_node(1);

	for (int q=2; temp_node!=NULL; q++)
	{
		if (strcmp(n, temp_node->token)==0)
		{
			//printf("FOUND: %s\n", temp_node->token);
			temp_node->type = t;
		}
		temp_node=find_ast_node(q);
	}

	struct function_node* temp = f;

	//printf("inserting_fun\n"); fflush(stdout);
	if (f==NULL) 
	{
		//printf("FIRST FUN\n");fflush(stdout);
		return new_function;
	}
	else if(f->next==NULL)
	{
		if(  strcmp(f->fun.fun_name,n)!=0 )
			f->next=new_function;
		else
			{printf("Function %s is defined twice\n",n); exit(0);}
	}
	else
	{
		while(temp->next!=NULL)
		{
			if(strcmp(n,temp->fun.fun_name)==0)
				{printf("Function %s is defined twice\n",n); exit(0);}
			temp=temp->next;
		}
		
		temp->next=new_function;
	}
	return f;
};

/*
void free_fun_list(struct function_node* N)
{
	struct function_node* temp=N;

	while(N!=NULL)
	{
		free(N->fun.fun_name);
		free(N
	}
}*/

void print_function_table(struct function_node* list)
{
	struct function_node* temp=list;

	while(temp!=NULL)
	{
		printf("FUN: %s\tTYPE: %c\tNUM_ARGS: %d\tARGS: ",temp->fun.fun_name,temp->fun.return_type,temp->fun.args);
		struct variable_node* temp2 = temp->fun.vars;
		while(temp2!=NULL)
		{
			printf("%c %s\t",temp2->type,temp2->var_name);
			temp2=temp2->next;
		}
		printf("\n");

		temp=temp->next;
	}
};

void print_var_list(struct function* var_list)
{
	struct variable_node* temp=var_list->vars;
	while (temp!=NULL)
	{
		printf("VAR: %s\tTYPE: %c\tOPEN: %d\tCLOSE: %d\n",temp->var_name,temp->type,temp->open,temp->close);
		temp=temp->next;
	}
}

char resolve_let(struct ast* node)
{
	//printf("RESOLVING NODE: %d \t%s\n",node->id,node->token); fflush(stdout);
	//printf("CHILDREN %s %s %s\n",get_child(node,1)->token,get_child(node,2)->token,get_child(node,3)->token);
	node->type=get_child(node,3)->type;
	return node->type;
}

char resolve_if(struct ast* node)
{
	//printf("at: %s\t child1: %s\t child2: %s\n",node->token,get_child(node,2)->token,get_child(node,3)->token);
	if(get_child(node,2)->type==get_child(node,3)->type && get_child(node,1)->type=='b')
		node->type=get_child(node,2)->type;
	else
		{printf("Arguements of if do not type check\n"); exit(0);}
	
	return node->type;
}


int main (int argc, char **argv) {
  int retval = yyparse();
  if (retval == 0) print_ast();

	var_list=malloc(sizeof(struct function));

	var_list->fun_name=NULL;
	var_list->return_type='v';
	var_list->vars=NULL;
	var_list->args=0;


	//insert_var(var_list,"sus",'i',1,2);
	//insert_var(var_list,"sus2",'b',3,4);
	//insert_var(var_list,"COW",'c',5,6);
	//print_var_list(var_list);
	
	struct ast* node=find_ast_node(1);


	//add all functions to the function table
	for(int i =2; node!=NULL; i++)
	{
		int kids=get_child_num(node);	
		if (strcmp(node->token,"DEFINE-FUN")==0)
		{
			char type;
			if (get_child(node, kids-1)->token[4]=='i')
				type='i';	//int
			else 
				type='b';	//bool

			fun_list=insert_function(fun_list,get_child(node,1)->token,type);
		}
		node=find_ast_node(i);
	}

	//fill in function variables
	node=find_ast_node(1);
	for (int i=2; node!=NULL; i++)
	{
		if(strcmp(node->token,"DEFINE-FUN")==0)
		{
			int kids=get_child_num(node);
			
			//Find the corresponding function in the table
			struct function_node* temp=fun_list;
			while (temp!=NULL)
			{
				if (strcmp(get_child(node,1)->token,temp->fun.fun_name)==0)
				{
					for(int p =2; p<kids-1; p++)
					{
						//please dont make me come back here						
						//printf("NAME DURING ENTRY: %sEND NAME\n",get_child(node,p)->token);
						int len=strlen(get_child(node,p)->token);
						int new_len=len-4; //integer case
						char type='i';
						if (get_child(node,p)->token[len-1]=='l')
							{new_len=len-5; type='b';}

						//printf("LEN: %d\n",len);
						//printf("NEW: %d\n",new_len);

						char* temp2=malloc(sizeof(char)*(new_len+1));

						strncpy(temp2,get_child(node,p)->token,new_len);
						temp2[new_len]='\0';

						//printf("NOW: %sENDNAME\n",temp2);

						
						insert_var(&(temp->fun),temp2,type,0,0);
						insert_var(var_list,temp2,type,get_child(node,1)->id,get_child(node,kids)->id);
						
					}
				}
				temp=temp->next;
			}

		}
		node=find_ast_node(i);
	}



	//fill in other variables, even if type not yet known 
	node=find_ast_node(1);
	for (int i=2; node!=NULL; i++)
	{
		//find vars
		if (strcmp(node->token,"let")==0) //vardec
		{

			//resolve_let(node);
			int kids=get_child_num(node);
			
			get_child(node,1)->type=get_child(node,2)->type;			

			insert_var(var_list,get_child(node,1)->token,get_child(node,1)->type,get_child(node,1)->id,get_child(node,kids)->id);
			resolve_let(node);

		}
		if(node->type=='c') // if statement
		{
			resolve_if(node);
		}
		if(node->type=='f')	// func_def
		{
			int kids=get_child_num(node);
			if (get_child(node,kids)->type!=get_child(node,kids-1)->type)
				{printf("result of %s doesnt match return type\n",get_child(node,1)->token); exit(0);}

		}
		
		node=find_ast_node(i);
	}


	//solve types of whole tree
	//int finds=0;
	node=find_ast_node(1);
	for (int i=2; node!=NULL; i++)
	{	
		if (	strcmp(node->token,"+")==0 || strcmp(node->token,"-")==0 || strcmp(node->token,"*")==0 ||
			strcmp(node->token,"div")==0 || strcmp(node->token,"mod")==0|| strcmp(node->token,"<")==0 ||
			strcmp(node->token,"<=")==0 || strcmp(node->token,"=")==0 || strcmp(node->token,">=")==0 ||
			strcmp(node->token,"<")==0 ) //jesus
		{
			int kids=get_child_num(node);
			for (int p=1; p<=kids; p++)
			{					
				if (get_child(node,p)->type!='i')
				{
					printf("Arguments of %s do not type check\n",node->token); exit(0);
				}
			}
		}
		if ( strcmp(node->token,"OR")==0 || strcmp(node->token,"AND")==0 || strcmp(node->token,"NOT")==0)
		{
			//printf("CHILD: %s\t%c\n",get_child(node,1)->token,get_child(node,1)->type);
			int kids=get_child_num(node);
			for (int p=1; p<=kids; p++)
			{
				if (get_child(node,p)->type!='b')
				{
					printf("Arguemnts of %s do not type check\n",node->token); exit(0);
				}
			}
		}
	
		struct function_node* temp_func_node = fun_list;

		while(temp_func_node!=NULL)
		{
			if(strcmp(temp_func_node->fun.fun_name,node->token)==0)
			{
				//printf("ONE MATCH\n");
				if(temp_func_node->finds>0)
				{
					//printf("%d\t%s matches a function declaration\n",node->id,node->token);
					//printf("%d %d\n",temp_func_node->fun.args,get_child_num(node));
					if (temp_func_node->fun.args!=get_child_num(node))
					{printf("Wrong number of arguments of function %s\n",node->token); exit(0);}
					else
					{
						struct variable_node* temp_var_node=temp_func_node->fun.vars;
						for(int n=1; temp_var_node!=NULL; n++)
						{
							int kids = get_child_num(node);
							//printf("%s\n",get_child(node,n)->token);
							//printf("%s\n",temp_var_node->var_name);
							if (get_child(node,n)->type!=temp_var_node->type)
							{
								printf("Argument #%d of %s does not type check with type of %s\n",kids-n+1,node->token,temp_var_node->var_name);
								exit(0);
							}
							temp_var_node=temp_var_node->next;
						}
					}
				}
				temp_func_node->finds++;
			}

			temp_func_node=temp_func_node->next;
		}
		if (node->type=='f')
			{printf("Function %s is not defined\n",node->token); exit(0);}

		
		if (node->type=='v')
			{printf("Variable %s is not declared\n",node->token); exit(0);}
	
		//printf("NODE: %d\t TYPE: %c\t NAME: %s\n",node->id,node->type,node->token);
		node=find_ast_node(i);

	
	}

	//print_function_table(fun_list);
	//print_var_list(var_list);

  free_ast();
  return retval;
};

