FILE *cfp;

struct callass {char content[50]; int func_id;};
struct branch {int var; int points1; int points2;};

struct cfg_node{ int id; char content[100]; int points1; int points2; char calls[30]; bool head; int args;};

int look_ahead(struct ast* node)
{
	struct variable_node* temp=var_list->vars;

	int points_to=0;
	int ahead=1;
	while(temp!=NULL)
	{
		//printf("VAR NAME: %s\n",temp->var_name);
		if(strcmp(find_ast_node((node->id)+ahead)->token,temp->var_name)==0 &&
		   strcmp(find_ast_node((node->id)+ahead)->parent->token,"let")==0)
		{
			ahead+=1;
			temp=var_list->vars;
			//printf("DEBUG: %s %s\n",temp->var_name);
		}
		temp=temp->next;
	}

	return ahead;
}


int say_and_point_to(struct ast* node, int stop, struct cfg_node* cfg)
{
	node->stat='d';
	bool done=false;
	//printf("TOKEN: %s\n",node->token);
	int points_to=0;
	int also_points_to=0;

	int ahead=look_ahead(node);

	if(strcmp(node->parent->token,"let")==0)
	{
		if((node->id)==get_child(node->parent,2)->id)
		{
			//printf("DEBUG\n");
			points_to=get_child(node->parent,1)->id;
		}
		if((node->id)==get_child(node->parent,1)->id)
		{
			if(strcmp(get_child(node->parent,3)->token,"let")==0)
			{
				points_to=get_child(get_child(node->parent,3),2)->id;
			}

			else{points_to=get_child(node->parent,3)->id;}
			//printf("%s := v%d POINTS to %d\n", node->token, get_child(node->parent,2)->id,points_to);
			fprintf(cfp,"%d [label=\"%s := v%d\"];\n",node->id,node->token,get_child(node->parent,2)->id);
			if(points_to!=0)
			{
				fprintf(cfp,"%d->%d\n",node->id,points_to);
				cfg[node->id].points1=points_to;
			}
		
			if(also_points_to!=0)
			{
				fprintf(cfp,"%d->%d\n",node->id,also_points_to);
				cfg[node->id].points2=also_points_to;
			}
			//node->stat='d';
			
			cfg[node->id].id=node->id;
			snprintf(cfg[node->id].content,50,"%s := v%d",node->token,get_child(node->parent,2)->id);
			

			done=true;
			
		}
		if((node->id)==get_child(node->parent,3)->id)
		{
			points_to=node->id+1;
		}
	}
	else if (node->id!=stop)
	{
		points_to=(node->id)+ahead;
		//printf("DEBUG POINTS TO: %d\n", points_to);
	}
	else
	{
		//printf("END OF THE LINE\n");
	}

	if(strcmp(node->parent->token,"if")==0)
	{
		if(node->id==get_child(node->parent,1)->id)
		//left most
		{
			//points_to=get_child(node->parent,2)->id;
			also_points_to=get_child(node->parent,2)->id+1;
		}
		else
		{
			points_to=node->parent->id;
		}



	}



	if (node->is_leaf==true && done==false)
	{
		//printf("SEE: %s %c\n",node->token,node->type);
		//printf("v%d := %s POINTS to %d and %d\n",node->id,node->token, points_to, also_points_to);
		fprintf(cfp,"%d [label=\"v%d := %s\"];\n",node->id,node->id,node->token);
		if(points_to!=0)
		{
			fprintf(cfp,"%d->%d\n",node->id,points_to);
		}
		
		if(also_points_to!=0)
		{
			fprintf(cfp,"%d->%d\n",node->id,also_points_to);
		}

		snprintf(cfg[node->id].content,50,"v%d := %s",node->id,node->token);
		cfg[node->id].id=node->id;
		cfg[node->id].points1=points_to;
		cfg[node->id].points2=also_points_to;

	}

	else if (strcmp(node->token,"let")==0)
	{
		//printf("v%d := v%d POINTS to %d and %d\n",node->id,node->id-1,points_to, also_points_to);
		
		fprintf(cfp,"%d [label=\"v%d := v%d\"];\n",node->id,node->id,node->id-1);
		if(points_to!=0)
		{
			fprintf(cfp,"%d->%d\n",node->id,points_to);
		}
		
		if(also_points_to!=0)
		{
			fprintf(cfp,"%d->%d\n",node->id,also_points_to);
		}

		cfg[node->id].id=node->id;
		cfg[node->id].points1=points_to;
		cfg[node->id].points2=also_points_to;
		snprintf(cfg[node->id].content,50,"v%d := v%d",node->id,node->id-1);
		
	}
	else if (strcmp(node->token,"if")==0)
	{
		//printf("if v%d = true, then v%d := v%d else v%d := v%d POINTS to %d and %d\n", 
		//	get_child(node,1)->id,node->id,get_child(node,2)->id,node->id, get_child(node,3)->id,points_to,also_points_to);
			
		fprintf(cfp,"%d [label=\"if v%d = true, then v%d := v%d else v%d := v%d \"];\n",node->id,get_child(node,1)->id,node->id,get_child(node,2)->id,node->id, get_child(node,3)->id);
		if(points_to!=0)
		{
			fprintf(cfp,"%d->%d\n",node->id,points_to);
		}
		
		if(also_points_to!=0)
		{
			fprintf(cfp,"%d->%d\n",node->id,also_points_to);
		}

		
		cfg[node->id].id=node->id;
		cfg[node->id].points1=points_to;
		cfg[node->id].points2=also_points_to;
		snprintf(cfg[node->id].content,50,"if v%d = true, then v%d := v%d else v%d := v%d",get_child(node,1)->id,node->id,get_child(node,2)->id,node->id, get_child(node,3)->id);



	}
	else if (strcmp(node->token,"NOT")==0)
	{
		//printf("v%d := %s v%d POINTS to %d and %d\n",node->id,node->token, points_to, points_to, also_points_to);
		fprintf(cfp,"%d [label=\"v%d := %s v%d\"];\n",node->id,node->id,node->token,node->id);
		if(points_to!=0)
		{
			fprintf(cfp,"%d->%d\n",node->id,points_to);
		}
		
		if(also_points_to!=0)
		{
			fprintf(cfp,"%d->%d\n",node->id,also_points_to);
		}
	
		cfg[node->id].id=node->id;
		cfg[node->id].points1=points_to;
		cfg[node->id].points2=also_points_to;
		snprintf(cfg[node->id].content,50,"v%d := %s v%d",node->id,node->token,node->id);


	}
	else if (	strcmp(node->token,"+")==0 || strcmp(node->token,"-")==0 || strcmp(node->token,"*")==0 ||
			strcmp(node->token,"div")==0 || strcmp(node->token,"mod")==0|| strcmp(node->token,">")==0 ||
			strcmp(node->token,"<=")==0 || strcmp(node->token,"=")==0 || strcmp(node->token,">=")==0 ||
			strcmp(node->token,"<")==0 || strcmp(node->token,"and")==0 || strcmp(node->token,"or")==0 ) //jesus
	{
		//printf("v%d := ",node->id);
		fprintf(cfp,"%d [label=\"v%d := ",node->id,node->id);
	
		cfg[node->id].id=node->id;
		cfg[node->id].points1=points_to;
		cfg[node->id].points2=also_points_to;
		snprintf(cfg[node->id].content,50,"v%d := ",node->id);

		for (int i=1; i<=get_child_num(node)-1; i++)
		{
			//printf ("v%d %s ",get_child(node,i)->id,node->token);
			fprintf(cfp,"v%d %s ",get_child(node,i)->id,node->token);
			snprintf(cfg[node->id].content+strlen(cfg[node->id].content),50,"v%d %s ",get_child(node,i)->id,node->token);
		}
		//printf("v%d POINTS to %d and %d\n",get_child(node,get_child_num(node))->id, points_to, also_points_to);
		fprintf(cfp,"v%d\"];\n",get_child(node,get_child_num(node))->id);
		snprintf(cfg[node->id].content+strlen(cfg[node->id].content),50,"v%d",get_child(node,get_child_num(node))->id);


		if(points_to!=0)
		{
			fprintf(cfp,"%d->%d\n",node->id,points_to);
		}
		
		if(also_points_to!=0)
		{
			fprintf(cfp,"%d->%d\n",node->id,also_points_to);
		}
	}
	else if (done==false)
	{
		
		cfg[node->id].id=node->id;
		cfg[node->id].points1=points_to;
		cfg[node->id].points2=also_points_to;
		cfg[node->id].args=get_child_num(node);		
		strcpy(cfg[node->id].calls,node->token);
		//printf("IT BE THE FUNCTION %s\n",node->token);

		snprintf(cfg[node->id].content,50,"v%d := %s (",node->id,node->token);	
		
		//printf("v%d := %s (",node->id,node->token);
		fprintf(cfp,"%d [label=\"v%d := %s (",node->id,node->id,node->token);
		for (int i=1; i<=get_child_num(node); i++)
		{
			//printf(" v%d,",get_child(node,i)->id);
			fprintf(cfp," v%d,",get_child(node,i)->id);
			snprintf(cfg[node->id].content+strlen(cfg[node->id].content),50," v%d,", get_child(node,i)->id);
		}
		fprintf(cfp," )\"];\n");
		snprintf(cfg[node->id].content+strlen(cfg[node->id].content),50," )");


		//printf(") POINTS to %d and %d\n", points_to, also_points_to);
		if(points_to!=0)
		{
			fprintf(cfp,"%d->%d\n",node->id,points_to);
		}
		
		if(also_points_to!=0)
		{
			fprintf(cfp,"%d->%d\n",node->id,also_points_to);
		}
	}


	//printf("DEBUG %d %d\n",points_to,also_points_to);	
	if (points_to!=0)
	{
		if (find_ast_node(points_to)->stat=='u')
			say_and_point_to(find_ast_node(points_to),stop,cfg);
	}
	if (also_points_to!=0)
	{
		if (find_ast_node(also_points_to)->stat=='u')
			say_and_point_to(find_ast_node(also_points_to),stop,cfg);
	}

	return 0;
}
