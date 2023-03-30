

/**********
GRAMMAR for the Parser:

program → declaration program | compound
declaration → type idlist SEMICOLON
idlist → ID | ID COMMA idlist
type → INTEGER | FLOAT | VOID
compound → BEGIN stmtlist END
stmtlist → stmt | stmt SEMICOLON stmtlist
stmt → ID | ID LPAREN exprlist RPAREN |
ID ASSIGNOP expr |
IF expr THEN compound ELSE compound |
WHILE LPAREN expr RPAREN compound |
compound
exprlist → expr | expr COMMA exprlist
expr → simpexpr | simpexpr RELOP simpexpr
simpexpr → term | term ADDOP simpexpr
term → factor | factor MULOP term
factor → ID | ID LPAREN exprlist RPAREN |
NUM REAL | NUM INT |
LPAREN expr RPAREN

 **********/
 

#include <iostream>
#include <fstream>

#include "Token.hpp"

using namespace std;

//Forward Declaration of all the function used for the parser
void factor(ifstream&);
void term(ifstream&);
void simpexpr(ifstream&);
void expr(ifstream&);
void exprlist(ifstream&);
void stmt(ifstream&);
void stmtlist(ifstream&);
void compound(ifstream&);
void type(ifstream&);
void idlist(ifstream&);
void declaration(ifstream&);
void program(ifstream&);

//Check if the program runs succesfully, false if not
bool isValid = 	
//string that will store the parsed code
string result;
//token that will be readed each time
Token tok;
//hold the number of indentations we want for each line
int indent = 0;

//function that drop a line and increments or decrement the indentation
void newline(int increment)
{
	//drop a line
	result += "\n";
	//if the parameter is negative
	if(increment < 0)
		indent -= (increment * -1); //subtract it
	else
		indent += increment;//else add it
	//loop until we have printed the desired number of indentation
	for(int i = 1; i <= indent; i++)
	{
		result += "  ";//Indentation will be represented with two spaces
	}
}


int main(int argc, char *argv[])
{
  // check to make sure an input file was specified. 
  if (argc!=2)
    {
      cerr << "USAGE: ./tokenify <file>" << endl;
      return -1;
    }

  // open input file
  ifstream ifile(argv[1]);

  // if open was not successful, let user know. 
  if (!ifile)
    {
      cerr << "ERROR: Could not open file:\"" << argv[1] << "\"" <<endl;
      return -1;
    }
   
  // (try to) get the first token
  program(ifile); 
  
  //if the programs runs succesfully
  if(isValid)
  {
  	//print this
  	cout << "# successful code ============================================" << endl;
  	//print the parsed code
  	cout << result;
  	//drop a line
  	cout <<endl;
  }
  else //else the code does not run succesfully
 	cout << "# UNsuccessful code ============================================" << endl;

  return 0;
}

/********************
program → declaration program | compound
declaration → type idlist SEMICOLON
idlist → ID | ID COMMA idlist
********************/
void program(ifstream &ifile)
{
	tok.get(ifile);//get the next token
	if(tok.type() != BEGIN) //if the first token is not "BEGIN" we are in a declaration
	{
		//call declaration
		declaration(ifile);
		//and then call itself
		program(ifile);
	}
	else if(tok.type() == BEGIN)//if the token is begin we are in a compound
	{
		result += "\n";//drop a lines to separate body and declaration
		compound(ifile);//call compound
	}
	else//else, we have an error
		isValid = false;
	
}

/********************
declaration → type idlist SEMICOLON
********************/

void declaration(ifstream &ifile)
{
	type(ifile); //call type
	idlist(ifile);//call idlist
	if(tok.type() == SEMICOLON)//las token of declaration must be a semicolon
	{
		result += tok.value();//get the next token
		result += "\n";//drop a line
	}
	else//else, we have an error
		isValid = false;
	

}
/********************
type → INTEGER | FLOAT | VOID
********************/
void type(ifstream &ifile){
	if(tok.type() == INTEGER || tok.type() == FLOAT || tok.type() == VOID)//if the token is an INTEGERn FLOAT or VOID
	{
		result += tok.value();//add it value to the result
		result += " ";//add spacing
		tok.get(ifile);//get next token
	}
	else//else, we have an error
		isValid = false;
}
/********************
idlist → ID | ID COMMA idlist
********************/
void idlist(ifstream &ifile)
{
	if(tok.type() == ID)//if first token is an ID
	{
		result += tok.value();//add it to the result
		tok.get(ifile);//get the next token
		if(tok.type() == COMMA)//if it is a comme
		{
			result += tok.value();//add it to the result
			result += " ";//add correct spacing
			tok.get(ifile);//read next token
			idlist(ifile);//call idlist
			
		}//else do nothing
	}
	else//if it is not an ID, we have an error
		isValid = false;	
}
/********************
compound → BEGIN stmtlist END
********************/
void compound(ifstream &ifile){
	if(tok.type() == BEGIN)//double check that the first token is begin
	{
		result += tok.value();//add it to the result
		newline(1);//add spacing and indentation
		tok.get(ifile);//read next token
		stmtlist(ifile);//call stmtlist
		if(tok.type() == END)//if the next token is "END" 
		{
			newline(-1);//dedent
			result += tok.value();//add the value to the result
			indent-=1;//reduce the indentation but without spacing
		}
	}
	else//if it is not Begin, we have an error
		isValid = false;
}
/********************
stmtlist → stmt | stmt SEMICOLON stmtlist
********************/
void stmtlist(ifstream &ifile){
	stmt(ifile);//call stmt
	if(tok.type() == SEMICOLON)//if next token is a SEMICOLON
	{
		result += tok.value();//add it to the result
		newline(0);//add spacing without indentation
		tok.get(ifile);//get next token
		stmtlist(ifile);//call stmtlist
		
	}
	if(tok.type() != SEMICOLON)//if there is no semicolon, do nothing (useful for debuggin purposes)
	{
		
	}
	else //(Also useful for debugging purposes)
		isValid = false;

}
/********************
stmtlist → stmt | stmt SEMICOLON stmtlist
stmt → ID | ID LPAREN exprlist RPAREN |
ID ASSIGNOP expr |
IF expr THEN compound ELSE compound |
WHILE LPAREN expr RPAREN compound |
compound
********************/
void stmt(ifstream &ifile)
{
	if(tok.type() == ID)//if first token is ID
	{
		result += tok.value();//add it to the result
		tok.get(ifile);//get the next token
		if(tok.type() == LPAREN)//if it is a LPAREN
		{
			result += tok.value();//add it to the result
			result += " ";//add spacing
			tok.get(ifile);//read the next token
			exprlist(ifile);//call exprlist
			if(tok.type() == RPAREN)//If next token is a RPAREN
			{
				result += " ";//add spacing
				result += tok.value();//add it to the result
				result += " ";//add spacing
				tok.get(ifile);//read the next token
			}
			else//if it is not an RPAREN, we have an error
				isValid = false;
		}
		else if(tok.type() == ASSIGNOP)//else if the next token is a ASSIGNOP(= or ==)
		{
			result += " ";//add correct spacing
			result += tok.value();//add it to the result
			result += " ";//add spacing
			tok.get(ifile);//get the next token
			expr(ifile);//call expr
		}
	}	
	else if(tok.type() == IF)//else if the next token is if
	{
		result += tok.value();//add it to the result
		result += " ";//add spacing
		tok.get(ifile);//get next token
		expr(ifile);//call expr
		if(tok.type() == THEN)//if next token is THEN
		{
			result += tok.value();//add it to the result
			newline(1);//Drop one line and add correct indentation
			tok.get(ifile);//get the next token
			compound(ifile);//call compound
			tok.get(ifile);//get the next token
			if(tok.type() == ELSE)//if next token is ELSE
			{
				newline(0);//add line without indent
				result += tok.value();//add it to the result
				newline(1);//add spacing and drop a line
				tok.get(ifile);//get the next token
				compound(ifile);//call compound
				tok.get(ifile);//get the next token
			}
			else//else, we have an error
				isValid = false;
		}	
		else//else, we have an error
			isValid = false;
		
	}
	else if(tok.type() == WHILE)//else if next token is a while
	{
		result += tok.value();//add it to the result
		result += " ";//add spacing
		tok.get(ifile);//get the next token
		if(tok.type() == LPAREN)//if next token is LPAREN
		{
			result += tok.value();//add it to the result
			result += " ";//add spacing
			tok.get(ifile);//get the next token
			expr(ifile);//call expr
			if(tok.type() == RPAREN)//if next token is RPAREN
			{
				result += " ";//add spacing
				result += tok.value();//add it to the result
				newline(1);//add spacing and indentation
				tok.get(ifile);//get the next token
				compound(ifile);//call compound
				tok.get(ifile);//get the next token
				
			}
			else//else we have an erro
				isValid = false;
		}
		else//if there is no LPAREN we have an erro
			isValid = false;
	}
	else if(tok.type() != ID && tok.type() != IF && tok.type() != WHILE)//esle if is not any of the previous
	{
		compound(ifile);//call compound
		tok.get(ifile);//get the next token
	}
	else//esle we have an error
		isValid = false;

}
/********************
exprlist → expr | expr COMMA exprlist
********************/
void exprlist(ifstream &ifile){
	expr(ifile);//call expr
	if(tok.type() == COMMA)//if the next token is a COMMA
	{	
		result += tok.value();//add it to the result
		result += " ";// add spacing
		exprlist(ifile);//call exprlist
	}
	else if(tok.type() != COMMA)//(DEBUGGING PURPOSES)
	{
	
	}
	else //(DEBUGGING PURPOSE)
		isValid = false;
}
/********************
expr → simpexpr | simpexpr RELOP simpexpr
********************/
void expr(ifstream &ifile){
	simpexpr(ifile);//call simpexpr
	if(tok.type() == RELOP)
	{
		result += " ";//add spacing
		result += tok.value();//add it to the result
		result += " ";//add spacing
		tok.get(ifile);//get the next token
		simpexpr(ifile);//call simpexpr
	}
	else if(tok.type() != RELOP)//(DEBUGGING PURPOSES)
	{
	
	}
	else //(DEBUGGING PURPOSES)
		isValid = false;
}
/********************
simpexpr → term | term ADDOP simpexpr
********************/
void simpexpr(ifstream &ifile) {
	term(ifile);//call term
	if(tok.type() == ADDOP)//if next token is ADDOP (+ or -)
	{
		result += " ";//add spacing
		result += tok.value();//add it to the result
		result += " ";//add spacing
		tok.get(ifile);//get the next token
		simpexpr(ifile);//call simpexpr
	}
	else if(tok.type() != ADDOP)//(DEBUGGING PURPOSES)
	{
	
	}
	else //(DEBUGGING PURPOSES)
		isValid = false;
	
}
/********************
term → factor | factor MULOP term
********************/
void term(ifstream &ifile) {
	factor(ifile);//call factor
	if(tok.type() == MULOP)//if the next token is MULOP (+ or /)
	{
		result += " ";//add spacing
		result += tok.value();//add it to the result
		result += " ";//add spacing
		tok.get(ifile);//get the next token
		term(ifile);//call term
	}
	else if(tok.type() != MULOP)//(DEBUGGING PURPOSES)
	{
	
	}
	else //(DEBUGGING PURPOSES)
		isValid = false;
}
/********************
factor → ID | ID LPAREN exprlist RPAREN |
NUM REAL | NUM INT |
LPAREN expr RPAREN
********************/

void factor(ifstream &ifile) {
	
	if(tok.type() == ID)//if the first token is an ID
	{
		result += tok.value();//add it to the result
		tok.get(ifile);//read the next token
		if(tok.type() == LPAREN)//if it is LPAREN
		{
			result += tok.value();//add it tohe result
			result += " ";//add spacing
			tok.get(ifile);//get the next token
			exprlist(ifile);//call exprlist
			if(tok.type() == RPAREN)//if the next toke in RPAREN
			{
				result += " ";//add spacing
				result += tok.value();//read the next token
				result += " ";//add spacing
				tok.get(ifile);//read the next token
			}
			else//If it is not a RPAREN we have an error
				isValid = false;
		}
	}	
	else if(tok.type() == NUM_REAL || tok.type() == NUM_INT)//IF the next token is NUMREAL or NUMINT
	{
		result += tok.value();//add it to result
		tok.get(ifile);//read the next token
	}
	else if(tok.type() == LPAREN)//if the nexto token is a LPAREN
	{
		result += tok.value();//add it to result
		result += " ";//add spacing
		tok.get(ifile);//get the next token
		expr(ifile);//call expr
		if(tok.type() == RPAREN)//if the next token is RPAREN
		{
			result += " ";//add spacing
			result += tok.value();//add it to the result
			result += " ";//add spacing
			tok.get(ifile);//get the next token
		}
		else//if not RPAREN we have an error
			isValid = false;
		
	}
	else//if not LPAREN we have an error
		isValid = false;
}
