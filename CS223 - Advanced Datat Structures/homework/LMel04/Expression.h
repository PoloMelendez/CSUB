#ifndef EXPRESSION_H
#define EXPRESSION_H
#include<iostream>
#include<cstring>
#include<cstdlib>
#include "TStack.h"

using namespace std;

const int MAX_LEN = 501;

class Expression {

    private:
	char infExpr[MAX_LEN];
	char postExpr[MAX_LEN];
	char originalExpr[MAX_LEN];
	char ** pstExpr;

	TStack<char> opStk;
	TStack<int> vlStk;

	bool isOperand( char c ) { return isdigit(c); }

	void popUntilLeftP( char tkn ){
	    while ( !opStk.empty() && opStk.top() != '(' )
		append( opStk.pop() );
	    if( !opStk.empty() && opStk.top() == '(' )
		opStk.pop();
	}
	void popUntilNotHigher( char tkn ) {
	    while ( !opStk.empty() && !isHigher( tkn ) ) append( opStk.pop() );
	    opStk.push( tkn );
	}
	void append( char c ) { strcat( strncat( postExpr, &c, 1 ), " "); }
	void append( char *c ) { strcat( strcat( postExpr, c ), " "); }

	void toPostFix() {
	    if( strlen( infExpr ) < 1 ) return;
	    char * token = strtok( infExpr, " " );
	    char tkn;
	    while( token != NULL ) {
		tkn = token[0];
		if( isOperand( tkn ) ) {
		    append( token );
		}
		else if ( tkn == '(' ) opStk.push( tkn );
		else if ( tkn == ')' ) popUntilLeftP( tkn );
		else if ( opStk.empty() || isHigher( tkn ) ) opStk.push( tkn );
		else popUntilNotHigher( tkn );
		token = strtok( NULL, " " );
	    }
	    while ( !opStk.empty() )
		if ( opStk.top() == '(' ) opStk.pop(); else append( opStk.pop() );
	}

	bool isHigher( char op ) {
	    if( opStk.empty() || opStk.top() == '(' ) return true;
	    if( ( op == '/' || op == '*' ) && ( opStk.top() == '+' || opStk.top() == '-' ))
		return true;
	    return false;
	}

	int value;
	bool done;

    public:

	double evaluation( const char * pstFix ) {
	    TStack<double> operandStck;
	    double result;
	    for( int i = 0; i < strlen( postExpr ); i++ )
	    {
		if( isdigit( postExpr[i] ) ) {
		    string temp;
		    temp += postExpr[i];
		    i++;
		    while(isdigit( postExpr[i])) { 
			temp += postExpr[i];
			i++;
		    }
		    operandStck.push( atof(temp.c_str()) );
		}
		else{
		    switch( postExpr[i] ) {
			double operand1, operand2;
			case '/' :
			operand2 = operandStck.pop();
			operand1 = operandStck.pop();
			result = operand1 / operand2;
			operandStck.push( result );
			break;
			case '*' :
			operand2 = operandStck.pop();
			operand1 = operandStck.pop();
			result = operand1 * operand2;
			operandStck.push( result );
			break;
			case '+' :
			operand2 = operandStck.pop();
			operand1 = operandStck.pop();
			result = operand1 + operand2;
			operandStck.push( result );
			break;
			case '-' :
			operand2 = operandStck.pop();
			operand1 = operandStck.pop();
			result = operand1 - operand2;
			operandStck.push( result );
			break;
			default :
			break;
		    }
		}
	    }
	    return result;
	}


	Expression( char * ip = "" ) { setInfix(ip); }


	void setInfix( const char * inf ) {
	    strcpy( infExpr, inf );
	    strcpy( originalExpr, inf );
	    postExpr[0] = 0;
	    opStk.clear(); vlStk.clear();
	    toPostFix();
	}
	const char * inFix () { return originalExpr; }
	const char * postFix () { return postExpr; }

};
#endif
