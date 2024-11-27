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
		{
		//printf(
		printf("Arguements of if do not type check\n"); exit(0);

		}
	
	return node->type;
}
