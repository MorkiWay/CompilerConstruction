#include "y.tab.h"
#include "ast.h"
int yyparse();

struct variable_node {char type; char* var_name; struct variable_node* next; int open; int close;};
struct function {char* fun_name; struct variable_node* vars; int args; char return_type;};
struct function_node  {struct function fun; struct function_node* next; int finds;};


struct function_node* fun_list = NULL;
struct function* var_list=NULL;

#include "semantics_utils.h"
#include "cfg.h"


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

	int size_ast=0;
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
		size_ast++;
	}

	//printf("SIZE OF AST: %d\n",size_ast);
	for (int i = size_ast; i>0; i--)
	{
		node=find_ast_node(i);
		if(strcmp(node->token,"let")==0)
		{
			//printf("FOUND: %d\n",i);
	
			int kids=get_child_num(node);
			
			get_child(node,1)->type=get_child(node,2)->type;			

			insert_var(var_list,get_child(node,1)->token,get_child(node,1)->type,get_child(node,1)->id,get_child(node,kids)->id);
		}
	}


	//printf("filling in other variables\n");fflush(stdout);
	//fill in other variables, even if type not yet known 
	node=find_ast_node(1);
	for (int i=2; node!=NULL; i++)
	{
		//printf("VALUE: %s\n",node->token); fflush(stdout);
		//find vars
		if (strcmp(node->token,"let")==0) //vardec
		{
			//printf("VAR IDENTIFIED\n");

			//resolve_let(node);
			resolve_let(node);

			//printf("AFTER INSERTION:\n");
			//print_var_list(var_list);
			//printf("EOL\n");

		}
		if(node->type=='c') // if statement
		{
			resolve_if(node);
		}/*
		if(node->type=='f')	// func_def
		{
			//printf("HERE\n"); fflush(stdout);
			int kids=get_child_num(node);
			//if (kids>0)
			//{
				if (get_child(node,kids)->type!=get_child(node,kids-1)->type)
					{printf("result of %s doesnt match return type\n",get_child(node,1)->token); exit(0);}
			//}
		}
		//printf("RESOLVED\n");fflush(stdout);*/
		
		node=find_ast_node(i);
	}


		
	struct cfg_node* cfg = malloc(sizeof(struct cfg_node)*size_ast);
	for (int i=0; i<size_ast; i++)
	{
			cfg[i].points1=0;
			cfg[i].points2=0;
			cfg[i].id=0;
			cfg[i].head=false;
			cfg[i].args=0;
			cfg[i].calls[0]='\0';
	}




	//printf("solving last types\n"); fflush(stdout);
	//solve types of whole tree
	//int finds=0;
	node=find_ast_node(1);
	for (int i=2; node!=NULL; i++)
	{	
		if (	strcmp(node->token,"+")==0 || strcmp(node->token,"-")==0 || strcmp(node->token,"*")==0 ||
			strcmp(node->token,"div")==0 || strcmp(node->token,"mod")==0|| strcmp(node->token,"<")==0 ||
			strcmp(node->token,"<=")==0 || strcmp(node->token,"=")==0 || strcmp(node->token,">=")==0 ||
			strcmp(node->token,">")==0 ) //jesus
		{
			int kids=get_child_num(node);
			for (int p=1; p<=kids; p++)
			{					
				if (get_child(node,p)->type!='i')
				{
					printf("1 Arguments of %s do not type check\n",node->token); exit(0);
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
					printf("2 Arguemnts of %s do not type check\n",node->token); exit(0);
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
					
					strcpy(cfg[node->id].calls,node->token);
					//printf("FUNCTION CALL FOUND %s\n", node->token);	
				
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


		//print_var_list(var_list); 


		if (node->type=='f')
			{printf("Function %s is not defined\n",node->token); exit(0);}

		
		if (node->type=='v')
			{printf("Variable %s is not declared\n",node->token); exit(0);}

	
		if (strcmp(node->token,"DEFINE-FUN")==0)	
		{	int kids=get_child_num(node);
				if (get_child(node,kids)->type!=get_child(node,kids-1)->type)
					{printf("Return type of %s does not match the type of body\n",get_child(node,1)->token); exit(0);}
		}

	
		//printf("NODE: %d\t TYPE: %c\t NAME: %s\n",node->id,node->type,node->token);
		node=find_ast_node(i);

	
	}

	//print_function_table(fun_list);
	//print_var_list(var_list);

	
	cfp= fopen("cfg.dot", "w");
 	fprintf(cfp, "digraph print {\n");
	
	node=find_ast_node(1);

	
	while (node!=NULL)
	{
		if (strcmp(node->token,"DEFINE-FUN")==0 || strcmp(node->token,"ENTRY")==0)
		//then it needs a cfg 
		{
			//printf("\nCFG FOR %s\n\n",node->token);


			if (get_child_num(node)>2)
			{
				struct ast* base_node=find_ast_node((get_child(node,get_child_num(node)-1)->id)+0);

				int offset=look_ahead(base_node);
				//printf("OFFSET: %d\n",offset);




				//printf("%s POINTS to %d\n",get_child(node,1)->token,find_ast_node(base_node->id+offset)->id);
				fprintf(cfp,"%d [label=\"%s\"];\n",get_child(node,1)->id,get_child(node,1)->token);
				fprintf(cfp,"%d->%d\n",get_child(node,1)->id,find_ast_node(base_node->id+offset)->id);

				cfg[get_child(node,1)->id].id=get_child(node,1)->id;
				cfg[get_child(node,1)->id].points1=find_ast_node(base_node->id+offset)->id;
				strcpy(cfg[get_child(node,1)->id].content,get_child(node,1)->token);
				cfg[get_child(node,1)->id].head=true;
				cfg[get_child(node,1)->id].args=get_child_num(node)-3;

				say_and_point_to(find_ast_node(base_node->id+offset),get_child(node,get_child_num(node))->id,cfg);
			}
			else
			{
				struct ast* base_node=find_ast_node((get_child(node,1)->id)+0);

				int offset=look_ahead(base_node);
				//printf("OFFSET: %d\n",offset);



				//printf("%s POINTS to %d\n",get_child(node,1)->token,find_ast_node(base_node->id+offset)->id); //name of starting node
				fprintf(cfp,"%d [label=\"%s\"];\n",get_child(node,1)->id,get_child(node,1)->token);
				fprintf(cfp,"%d->%d\n",get_child(node,1)->id,find_ast_node(base_node->id+offset)->id);
				say_and_point_to(find_ast_node(base_node->id+offset),node->id-1,cfg);

				
				cfg[get_child(node,1)->id].id=get_child(node,1)->id;
				cfg[get_child(node,1)->id].points1=find_ast_node(base_node->id+offset)->id;
				strcpy(cfg[get_child(node,1)->id].content,get_child(node,1)->token);
				cfg[get_child(node,1)->id].head=true;



			}

		}
		node=node->next;
	}
/*
 for (int i=0; i <size_ast; i++)
  {
	if (cfg[i].id!=0)
		printf("CFG NODE: %d -> %d %d \t%s\n",cfg[i].id,cfg[i].points1,cfg[i].points2,cfg[i].content);
	else
		printf("CFG NODE: %d UNUSED\n",i);

  }
*/

int cur_blocks=size_ast;
for (int i=0; i < size_ast; i++)
  {
	if (cfg[i].id!=0 && cfg[i].head==true)
	{ //handle entire function
		printf("function %s\n",cfg[i].content);
		for (int p = i; p < i + cfg[i].args; p++)
		{
			printf("bb:%d\n\tv%d := a%d\n",p+1,p+1,p-i+1);
			if (p< i + cfg[i].args-1)
				printf("\tbr bb%d\n",p+2);
			else
				printf("\tbr bb%d\n",p+3);
		}

		//printf("ENTRY: %d\n",i+1+cfg[i].args);
		int eof = 0;
		for (int p = cfg[i].points1; cfg[p].points1!=0; eof=p)
		{
			int branch=cfg[p].points1;
			//printf("YARGH: %d %c\n",p,cfg[p+1].calls[0]);
			int fall_back = p;
			while (cfg[p+1].calls[0]!='\0')
			{
				branch=++cur_blocks;
				//printf("function at p=%d %s\n",p,cfg[p+1].content);
			//need to push on arguments
				
				if (p==fall_back)
				{	
					printf("bb%d:\n\t",p);
					printf("%s\n\t",cfg[p].content);
					printf("br bb%d\n",branch);
				}
				
				
				for(int f = 1 ; f<=cfg[p+1].args; f++)
				{
					printf("bb%d:\n",cur_blocks);
					printf("\ta%d := v%d\n",f,p-cfg[p+1].args+f);
					printf("\tbr bb%d\n",++cur_blocks);
				}
				
				printf("bb%d:\n",cur_blocks);
				printf("\tcall %s\n", cfg[p+1].calls);
				printf("\tbr bb%d\n",p+1);

				printf("bb%d:\n",p+1);
				printf("\tv%d := rv\n",p+1);

				if (cfg[p+2].calls[0]!='\0')
					branch = cur_blocks+1;
				else branch = p+2;

				if (cfg[p+1].points2>0)
				{
					printf("\tbr v%d bb%d bb%d\n",p+1,branch,cfg[p+1].points2);
				}
				else
				{
					printf("\tbr bb%d\n",p+1);
				}
				p++;



			}
			if (fall_back==p)
			{
				int l_val=0;
				int r_val=0;
				char temp[10] = {0};
				if(cfg[p].points1==cfg[p+1].points1 || cfg[p].points1==cfg[p-1].points1)
				{
					//printf("FOUND\n");
					l_val=cfg[p].points1;
			
					int bound=strlen(cfg[p].content)-1;
					while (bound>0 && cfg[p].content[bound]!=' ')
						bound--;
					//printf("DIGIT %s\n",cfg[p].content+bound);
					strcpy(temp, cfg[p].content+bound+1);
					//printf("TEMP:%s\n",temp);
				}

			
				//printf("RAH: %c %c %c\n",cfg[p].content[0],cfg[p].content[1],cfg[p].content[2]);
				if(cfg[p].content[0]=='i' && cfg[p].content[1]=='f' && cfg[p].content[2]==' ')
				{
					//printf("IF THEN \n");
					l_val=cfg[p].points1;
					r_val=p;
				}


				printf("bb%d:\n\t",p);

				if(l_val!=0 && temp[0]!=0)
				{
					printf("v%d = %s\n\t",l_val,temp);
				}
				else if (l_val!=0)
				{
					printf("v%d = v%d\n\t",l_val,r_val);
				}
				else
				{
					printf("%s\n\t",cfg[p].content);
				}
				if(cfg[p].points2>0)
				{
					printf("br v%d bb%d bb%d\n",p,branch,cfg[p].points2);
				}	
				else {printf("br bb%d\n",cfg[p].points1);} 

			
				if (cfg[p].points1<p)
				{
					//printf("SPECIAL\n");
					printf("bb:%d\n\t",cfg[p].points1);
					printf("v%d := v%d\n\t",cfg[p].points1,p);
					printf("br bb%d\n",p+1);
				}
			}
			p++;

		}


		printf("bb%d:\n\t",eof);
		if(cfg[eof].content[0]=='i' && cfg[eof].content[1]=='f' && cfg[eof].content[2]==' ')
		{

		}
		else
		{
			printf("%s\n\t",cfg[eof].content);
		}
		printf("br bb%d\n",++cur_blocks);
		
		printf("bb%d:\n\t",cur_blocks);
		printf("rv =: v%d\n",eof);
		

		

		printf("\n\n\n");
	}
  }


 fprintf(cfp, "}\n ");
 fclose(cfp);
 system("dot -Tpdf cfg.dot -o cfg.pdf");

  free_ast();
  return retval;




};

