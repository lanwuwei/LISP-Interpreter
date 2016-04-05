#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <stack>
using namespace std;

typedef struct Token{
	string get_token;
	int get_type;
}token;

const int literal_atom=0, numeric_atom=1, eof=2,open_parenthesis=3,closing_parenthesis=4,literal_open_parenthesis=5,numeric_open_paenthesis=6,literal_closing_parenthesis=7,numeric_closing_parenthesis=8,bad_prefix=9;
const int Keywords_num=17;
const string Keywords[Keywords_num]={"T","NIL","CAR","CDR","CONS","ATOM","EQ","NULL","INT","PLUS","MINUS","TIMES","LESS","GREATER","COND","QUOTE","DEFUN"};

class Scanner
{
	public:
	
	static bool Is_literal_atom(string s)
	{
		if(s.empty())
			return false;
		if(s.at(0)<'A' || s.at(0)>'Z')
			return false;
		for (int i=0;i<s.size();)
		{
			if((s.at(i)>='0' && s.at(i)<='9')||(s.at(i)>='A' && s.at(i)<='Z'))
				i++;
			else
				return false;
		}
		return true;
	}
	
	static bool Is_numeric_atom(string s)
	{
		if(s.empty())
		{
			return false;			
		}
		else
		{
			for (int i=0;i<s.size();i++)
			{
				if (s.at(i)>'9' || s.at(i)<'0')
				{
					if(i==0 && s.at(i)=='-')
						continue;
					else
						return false;					
				}
			}
			return true;			
		}
	}
	
	static token getNextToken(istream & in)
	{
		char temp;
		token my_token;
		while(1)
		{
			temp=in.get();
			if(temp==' ' || temp=='\r' || temp=='\n' ||temp=='\t' || in.eof()) 
			{
				if(my_token.get_token.size()==0)
				{
					if(in.eof())
					{
						break;
					}
					continue;
				}
				else
				{
					if(Is_literal_atom(my_token.get_token))
					{
						my_token.get_type=literal_atom;
						return my_token;
					}
					else if(Is_numeric_atom(my_token.get_token))
					{
						my_token.get_type=numeric_atom;
						return my_token;
					}
					else
					{
						cout<<"ERROR: Invalid token "<<my_token.get_token<<endl;
						my_token.get_token.clear();
						exit(1);
					}
				}
			}
			else if(temp=='(')
			{
				if(my_token.get_token.size()!=0)
				{
					if(Is_literal_atom(my_token.get_token))
						my_token.get_type=literal_open_parenthesis;
					else if(Is_numeric_atom(my_token.get_token))
						my_token.get_type=numeric_open_paenthesis;
					else
						my_token.get_type=bad_prefix;
					return my_token;
				}
				my_token.get_token='(';
				my_token.get_type=open_parenthesis;
				return my_token;
			}
			else if(temp==')')
			{
				if(my_token.get_token.size()!=0)
				{
					if(Is_literal_atom(my_token.get_token))
					{
						my_token.get_type=literal_closing_parenthesis;		
	//					cout<<my_token.get_token<<endl;
					}
					else if(Is_numeric_atom(my_token.get_token))
					{
						my_token.get_type=numeric_closing_parenthesis;	
	//					cout<<my_token.get_token<<endl;
					}
					else
						my_token.get_type=bad_prefix;
					return my_token;
				}
				my_token.get_token=')';
				my_token.get_type=closing_parenthesis;
				return my_token;
			}
			else
			{
				my_token.get_token+=temp;
			}
		}
		my_token.get_type=eof;
		my_token.get_token="";
		return my_token;
	}
	
	void Display()
	{
		int num_literal_atom=0, num_numeric_atom=0, num_open_parenthesis=0;
		int num_closing_parenthesis=0;
		queue<string> buf_literal_atom;
		token my_token;
		while(!cin.eof())
		{
			my_token=getNextToken(cin);
			if (my_token.get_type==open_parenthesis || my_token.get_type==literal_open_parenthesis || my_token.get_type==numeric_open_paenthesis)
			{
				num_open_parenthesis++;
				if(my_token.get_type==literal_open_parenthesis)
				{
					num_literal_atom++;
					buf_literal_atom.push(my_token.get_token);
				}
				if(my_token.get_type==numeric_open_paenthesis)
				{
					num_numeric_atom++;
				}
			}
			else if(my_token.get_type==closing_parenthesis || my_token.get_type==literal_closing_parenthesis || my_token.get_type==numeric_closing_parenthesis)
			{
				num_closing_parenthesis++;
				if(my_token.get_type==literal_closing_parenthesis)
				{
					num_literal_atom++;
					buf_literal_atom.push(my_token.get_token);				
				}
				if(my_token.get_type==numeric_closing_parenthesis)
				{
					num_numeric_atom++;				
				}
			}
			else if(my_token.get_type==literal_atom )
			{
				num_literal_atom++;
				buf_literal_atom.push(my_token.get_token);
			}
			else if(my_token.get_type==numeric_atom )
			{
				num_numeric_atom++;
			}
			else if(my_token.get_type==eof)
			{
				break;
			}
		}
		cout<<"LITERAL ATOMS: "<<num_literal_atom<<" ";
		while(buf_literal_atom.size()!=0)
		{
			cout<<buf_literal_atom.front()<<", ";
			buf_literal_atom.pop();
		}
		cout<<endl;
		cout<<"NUMERIC ATOMS: "<<num_numeric_atom<<", "<<num_literal_atom+num_numeric_atom<<endl;
		cout<<"OPEN PARENTHESIS: "<<num_open_parenthesis<<endl;
		cout<<"CLOSING PARENTHESIS: "<<num_closing_parenthesis<<endl;		
	}
};

class Parser : public Scanner
{
	public:
	token my_token;
	struct bitnode{
		string data;
		struct bitnode *lchild, *rchild;
	};
	bitnode * root;
	
	void Init()
	{
		my_token=Scanner::getNextToken(cin);
		
	}
	
	token GetCurrent()
	{
		return my_token;
	}
	
	void MoveToNext()
	{
		my_token=Scanner::getNextToken(cin);
	}
	
	void Expr(bitnode * T)
	{
		bitnode * TL,  * TR;
		if(GetCurrent().get_type==literal_atom || GetCurrent().get_type==numeric_atom)
		{
			T->data=GetCurrent().get_token;
			MoveToNext();
		}
		else if(GetCurrent().get_type==literal_open_parenthesis || GetCurrent().get_type==numeric_open_paenthesis)
		{
			T->data=GetCurrent().get_token;
			my_token.get_token='(';
			my_token.get_type=open_parenthesis;
		}
		else if(GetCurrent().get_type==open_parenthesis || GetCurrent().get_type==literal_open_parenthesis || GetCurrent().get_type ==numeric_open_paenthesis)
		{
			MoveToNext();
			if(GetCurrent().get_type!=closing_parenthesis)
			{
				TL=new bitnode;
				TL->data="";
				TL->lchild=NULL;
				TL->rchild=NULL;
				T->lchild=TL;
				TR=new bitnode;
				TR->data="";
				TR->lchild=NULL;
				TR->rchild=NULL;
				T->rchild=TR;				
			}
			while(GetCurrent().get_type!=closing_parenthesis && GetCurrent().get_type!=literal_closing_parenthesis && GetCurrent().get_type!=numeric_closing_parenthesis)
			{
				Expr(T->lchild);
				T=T->rchild;	
				T->data="";
				if(GetCurrent().get_type!=closing_parenthesis )
				{
					TL=new bitnode;
					TL->data="";
					TL->lchild=NULL;
					TL->rchild=NULL;
					T->lchild=TL;
					TR=new bitnode;
					TR->data="";
					TR->lchild=NULL;
					TR->rchild=NULL;
					T->rchild=TR;					
				}	
			}
			if(GetCurrent().get_type==closing_parenthesis)
			{
				if(T->lchild!=NULL)
				{
					T->rchild->data="NIL";		
				}		
				else
				{
					T->data="NIL";				
				}
			}
			else if(GetCurrent().get_type==literal_closing_parenthesis || GetCurrent().get_type==numeric_closing_parenthesis)
			{
				if(T->lchild!=NULL)
				{
					T->lchild->data=GetCurrent().get_token;		
					T->rchild->data="NIL";
				}
				else
				{
					T->data=GetCurrent().get_token;	
				}
			}
			MoveToNext();
		}
		else
		{
			if(GetCurrent().get_type==closing_parenthesis || GetCurrent().get_type==literal_closing_parenthesis || GetCurrent().get_type==numeric_closing_parenthesis || GetCurrent().get_type==eof)
			{
				cout<<"ERROR: Bad grammar..."<<endl;
				exit(1);
			}
			else
			{
				cout<<"ERROR: Unknown token "<<GetCurrent().get_token<<" ..."<<endl;
				exit(1);
			}
				
		}
	}
	
	void Print(bitnode * root)
	{
		if(root!=NULL)
		{
			if(root->data=="")
			{
				cout<<"(";
				Print(root->lchild);		
				cout<<" . ";
				Print(root->rchild);
				cout<<")";				
			}
			else
			{
				cout<<root->data;
			}
		}
	}
	void Start()
	{
		Init();
		do{
			root=new bitnode;
			root->data="";
			root->lchild=NULL;
			root->rchild=NULL;
			Expr(root);
			if(root->lchild==NULL) //one node tree
			{
				if(root->data!="")
				{
					cout<< root->data <<" "<<endl;
				}
				else
				{
					cout<<"NIL"<<" "<<endl;
				}
			}
			else //multiple nodes tree
			{
				Print(root);
				cout<<endl;
			}
		}while(GetCurrent().get_type!=eof);
	}
};

class Interpreter: public Parser
{
	public:
	bitnode * new_root;	
	vector<bitnode *> func_list;
	vector<vector<string> > param_list;
	vector<vector<bitnode *> > parameter_exp_list;
	vector<string> func_name_list;
	int iterator;
	stack<int> my_stack;
	vector<stack<vector<bitnode *> > > my_exp_stack;
	
	bool IsKeyword(string s)
	{
		for (int i=0;i<Keywords_num;i++)
		{
			if (s==Keywords[i])
				return true;
		}
		return false;
	}
	
	
	void Apply(bitnode * & old_root)
	{
		if(old_root->rchild!=NULL)
		{
			bitnode *TL, *TR;
			TL=old_root->lchild;
			TR=old_root->rchild;
			if(TL->data=="DEFUN")
			{
				if(TR->rchild!=NULL)
				{
					if(Is_literal_atom(TR->lchild->data) && !IsKeyword(TR->lchild->data))
					{
						if(TR->rchild->rchild!=NULL && TR->rchild->rchild->rchild!=NULL && TR->rchild->rchild->rchild->data=="NIL")
						{
							bitnode *s1=TR->rchild->lchild;
							vector<string> parameter_list;
							vector<bitnode *> temp_exp_list;
							stack<vector<bitnode *> > temp_exp_stack;
							if(s1->lchild!=NULL)
							{
								while(s1->rchild!=NULL)
								{
									if(Is_literal_atom(s1->lchild->data))
									{
										if(!IsKeyword(s1->lchild->data))
										{
											if(find(parameter_list.begin(),parameter_list.end(),s1->lchild->data)==parameter_list.end())
											{
												parameter_list.push_back(s1->lchild->data);
												s1=s1->rchild;
											}
											else
											{
												cout<<"ERROR: "<<"The function parameter list must contain distinct elements."<<endl;
												exit(1);
											}
										}
										else
										{
											cout<<"ERROR: "<<"The function paprameter must not be keyword."<<endl;
											exit(1);
										}
									}
									else
									{
										cout<<"ERROR: "<<"The function paprameter must be literal atom."<<endl;
										exit(1);
									}
								}
							}
							func_list.push_back(TR->rchild->rchild->lchild);
							param_list.push_back(parameter_list);
							func_name_list.push_back(TR->lchild->data);
							parameter_exp_list.push_back(temp_exp_list);
							my_exp_stack.push_back(temp_exp_stack);
							cout<<TR->lchild->data<<endl;	
							root->data="";
							root->lchild=NULL;
							root->rchild=NULL;
						}
						else
						{
							cout<<"ERROR: "<<"A DEFUN expression must be of the form (DEFUN F s1 s2)."<<endl;
							exit(1);
						}
					}
					else
					{
						if(IsKeyword(TR->lchild->data))
						{
							cout<<"ERROR: "<<"The function name "<<TR->lchild->data<<" is keyword."<<endl;
						}
						else
						{
							cout<<"ERROR: "<<"The function name "<<TR->lchild->data<<" is not qualified."<<endl;
						}
						exit(1);
					}
				}
				else
				{
					cout<<"ERROR: "<<"A DEFUN expression must be of the form (DEFUN F s1 s2)."<<endl;
					exit(1);
				}
			}
			
			else if(find(func_name_list.begin(),func_name_list.end(),TL->data)!=func_name_list.end())
			{
				iterator=find(func_name_list.begin(),func_name_list.end(),TL->data)-func_name_list.begin();
				int param_num=0;
				vector<bitnode *> temp_exp_list;
				temp_exp_list.clear();
				bitnode * start=TR;
				while(start->rchild!=NULL)
				{
					param_num++;
					start=start->rchild;
				}
				if(param_num!=param_list[iterator].size())
				{
//					cout<<"test1:param_num"<<param_num<<endl;
					cout<<"ERROR: "<<"The number of parameter expressions must be equal to the number of formal parameters."<<endl;
//					cout<<"test2:exp_num"<<param_list[iterator].size()<<endl;
					exit(1);
				}	
				start=TR;
				while(start->rchild!=NULL)
				{
					bitnode *temp;
					Eval(temp,start->lchild);
					temp_exp_list.push_back(temp);
					start=start->rchild;
				}	
				parameter_exp_list[iterator]=temp_exp_list;
				old_root=func_list[iterator];
			}
		
		}
	}
	
	void Eval(bitnode * & new_root, bitnode * & old_root)
	{
		bitnode *TL, *TR;
		if(old_root->lchild==NULL)
		{
			if (Is_numeric_atom(old_root->data) || old_root->data=="NIL" || old_root->data=="T")
			{
				new_root=old_root;
			}
			else if(iterator!=-1 && (find(param_list[iterator].begin(),param_list[iterator].end(),old_root->data)!=param_list[iterator].end()))
			{
				int offset=find(param_list[iterator].begin(),param_list[iterator].end(),old_root->data)-param_list[iterator].begin();
				new_root=parameter_exp_list[iterator][offset];
			}
			else
			{
				cout<<"ERROR:"<<"cannot evaluate this literal atom: "<<old_root->data<<endl;
				exit(1);
			}
		}
		else
		{
			TL=old_root->lchild;
			TR=old_root->rchild;
			if(TL->data=="PLUS" || TL->data=="MINUS" || TL->data=="TIMES" || TL->data=="LESS" || TL->data=="GREATER")
			{
				bitnode *a1,*a2;
				if(TR->lchild==NULL)
				{
					cout<<"ERROR:"<<"There is no operand for"<<TL->data<<endl;
					exit(1);
				}
				else if(TR->rchild->lchild==NULL)
				{
					cout<<"ERROR:"<<"Operand is needed for "<<TL->data<<endl;	
					exit(1);					
				}
				else if(TR->rchild->rchild->data!="NIL")
				{
					cout<<"ERROR: "<<"Too many operands for "<<TL->data<<endl;
					exit(1);
				}
				else
				{
					Eval(a1,TR->lchild);
					Eval(a2,TR->rchild->lchild);
					if(Is_numeric_atom(a1->data) && Is_numeric_atom(a2->data))
					{
						new_root=new bitnode;
						new_root->lchild=NULL;
						new_root->rchild=NULL;
						ostringstream os;
						istringstream is;	
						int	b1,b2;
						is.str(a1->data);
						is>>b1;
						is.clear();
						is.str(a2->data);
						is>>b2;
						if(TL->data=="PLUS")
						{
							os<<b1+b2;
							new_root->data=os.str();
						}
						else if(TL->data=="MINUS")
						{
							os<<b1-b2;
							new_root->data=os.str();
						}
						else if(TL->data=="TIMES")
						{
							os<<b1*b2;
							new_root->data=os.str();
						}
						else if(TL->data=="LESS")
						{
							if (b1<b2)
								new_root->data="T";
							else
								new_root->data="NIL";
						}
						else
						{
							if (b1>b2)
								new_root->data="T";
							else
								new_root->data="NIL";								
						}
					}
					else
					{
						cout<<"ERROR: "<<"Numeric atom operand is needed for "<<TL->data<<endl;
						exit(1);
					}
				}
			}
			else if(TL->data=="EQ")
			{
				bitnode *a1,*a2;
				if(TR->lchild==NULL)
				{
					cout<<"ERROR: "<<"There is no operand for "<<TL->data<<endl;
					exit(1);
				}
				else if(TR->rchild->lchild==NULL)
				{
					cout<<"ERROR: "<<"One more operand is needed for "<<TL->data<<endl;		
					exit(1);
				}
				else if(TR->rchild->rchild->data!="NIL")
				{
					cout<<"ERROR: "<<"Too many operands for "<<TL->data<<endl;
					exit(1);
				}
				else
				{
					Eval(a1,TR->lchild);
					Eval(a2,TR->rchild->lchild);					
					if(a1->data!="" && a2->data!="")
					{
						new_root=new bitnode;
						new_root->lchild=NULL;
						new_root->rchild=NULL;
						if(a1->data==a2->data)
						{
							new_root->data="T";
						}
						else
						{
							new_root->data="NIL";
						}
					}
					else
					{
						cout<<"ERROR: "<<"The operand for EQ is not atom."<<endl;
						exit(1);
					}
				}
			}
			else if(TL->data=="ATOM" || TL->data=="INT" || TL->data=="NULL")
			{
				bitnode *a1;
				if(TR->lchild!=NULL && TR->rchild->data=="NIL")
				{
					Eval(a1,TR->lchild);
					new_root=new bitnode;
					new_root->lchild=NULL;
					new_root->rchild=NULL;					
					if(TL->data=="ATOM")
					{						
						if(a1->data!="")
							new_root->data="T";
						else
							new_root->data="NIL";
					}
					else if(TL->data=="INT")
					{
						if(Is_numeric_atom(a1->data))
							new_root->data="T";	
						else
							new_root->data="NIL";
					}
					else
					{					
						if(a1->data=="NIL")
							new_root->data="T";
						else
							new_root->data="NIL";
					}
				}
				else
				{
					cout<<"ERROR:"<<"Only one operand is needed for "<<TL->data<<endl;
					exit(1);
				}
			}
			else if(TL->data=="CAR" || TL->data=="CDR")
			{
				if(TR->lchild!=NULL && TR->rchild->data=="NIL")
				{
					bitnode * a1;
					Eval(a1,TR->lchild);
					if(a1->data=="")
					{
						if(TL->data=="CAR")
							new_root=a1->lchild;						
						else
							new_root=a1->rchild;
					}
					else
					{
						if(a1->data=="NIL")
						{
							cout<<"ERROR: "<<"empty list for "<<TL->data<<endl;
							exit(1);
						}
						else
						{
							cout<<"ERROR: "<<"In (CAR s1), eval(s1) is an atom."<<endl;
//							cout<<a1->data<<endl;
							exit(1);
						}
					}
				}
				else
				{
					cout<<"ERROR:"<<"The operand for "<<TL->data<<" must be a list."<<endl;
					exit(1);
				}
			}
			else if(TL->data=="CONS")
			{
				if(TR->lchild!=NULL && TR->rchild->lchild!=NULL && TR->rchild->rchild->data=="NIL")
				{
					bitnode *a1,*a2;
					Eval(a1,TR->lchild);
					Eval(a2,TR->rchild->lchild);
					new_root=new bitnode;
					new_root->data="";
					new_root->lchild=a1;
					new_root->rchild=a2;
				}
				else
				{
					cout<<"ERROR:"<<"The length of eval(s) for CONS is not 3."<<endl;
					exit(1);
				}
			}
			else if(TL->data=="QUOTE")
			{
				if(TR->lchild!=NULL && TR->rchild->data=="NIL")
				{
					new_root=TR->lchild;
				}
				else
				{
					cout<<"ERROR:"<<"The length of eval(s) for QUOTE is not 2."<<endl;
					exit(1);
				}
			}
			else if(TL->data=="COND")
			{
				bitnode *a1;
				a1=TR;
				if(a1->rchild==NULL)
				{
					cout<<"ERROR:Condition is needed for COND."<<endl;
					exit(1);
				}
				while(a1->rchild!=NULL)
				{
					if(a1->lchild->rchild!=NULL && a1->lchild->rchild->rchild!=NULL && a1->lchild->rchild->rchild->data=="NIL")
					{
						a1=a1->rchild;
					}
					else						
					{
						cout<<"ERROR: The condition for COND is not eligible."<<endl;
						exit(1);
					}
				}
				a1=TR;
				while(a1->rchild!=NULL)
				{
					bitnode *b1;
					Eval(b1,a1->lchild->lchild);
					if(b1->data!="NIL")
					{
						Eval(new_root,a1->lchild->rchild->lchild);
						break;
					}
					else
					{
						a1=a1->rchild;
					}
				}
				if(a1->rchild==NULL)
				{
					cout<<"ERROR:None of the condition (bi,ei) is qualified."<<endl;
					exit(1);
				}
			}
			else if(find(func_name_list.begin(),func_name_list.end(),TL->data)!=func_name_list.end())
			{
				int offset=find(func_name_list.begin(),func_name_list.end(),TL->data)-func_name_list.begin();
				int param_num=0;
				vector<bitnode *> temp_exp_list;
				temp_exp_list.clear();
				bitnode * start=TR;
				while(start->rchild!=NULL)
				{
					param_num++;
					start=start->rchild;
				}
				if(param_num!=param_list[offset].size())
				{
//					cout<<"test3:param_num"<<param_num<<endl;
					cout<<"ERROR: "<<"The number of parameter expressions must be equal to the number of formal parameters."<<endl;
//					cout<<"test4:exp_num"<<param_list[offset].size()<<endl;
					exit(1);
				}	
				
				start=TR;
				while(start->rchild!=NULL)
				{
					bitnode *temp;
					Eval(temp,start->lchild);
					temp_exp_list.push_back(temp);
					start=start->rchild;
				}	
				my_exp_stack[offset].push(parameter_exp_list[offset]);
				parameter_exp_list[offset]=temp_exp_list;			
				bitnode * subtree;
				subtree=func_list[offset];
				my_stack.push(iterator);
				iterator=offset;
				Eval(new_root,subtree);
				iterator=my_stack.top();
				my_stack.pop();
				parameter_exp_list[offset].clear();
				parameter_exp_list[offset]=my_exp_stack[offset].top();
				my_exp_stack[offset].pop();
			}
			else
			{
				cout<<"ERROR: "<<"Undefined rules, which can not be evaluated."<<endl;
//				cout<<TL->data<<endl;
				exit(1);
			}
		}
	}
	
	
	void Print(bitnode * new_root)
	{
		if(new_root->lchild==NULL)
		{
			cout<<new_root->data;
		}
		else
		{
			cout<<"(";
			while(new_root->rchild!=NULL)
			{
				Print(new_root->lchild);
				if(new_root->rchild->rchild!=NULL)
				{
					cout<<" ";
				}
				new_root=new_root->rchild;				
			}
			if(new_root->data=="NIL")
			{
				cout<<") ";
			}
			else
			{
				cout<<". "<<new_root->data<<") ";				
			}			
		}
	}
	
	void Start()
	{
		Init();
		do{
			iterator=-1;
			root=new bitnode;
			root->data="";
			root->lchild=NULL;
			root->rchild=NULL;
			Expr(root);	
//			Parser::Print(root);
//			cout<<endl;
			Apply(root);
			if(!(root->lchild==NULL && root->data==""))
			{
				Eval(new_root,root);
				Print(new_root);
				cout<<endl;				
			}
		}while(GetCurrent().get_type!=eof);		
	}
};

int main() {
	// your code goes here
	Interpreter my_interpreter;
	my_interpreter.Start();
	return 0;
}