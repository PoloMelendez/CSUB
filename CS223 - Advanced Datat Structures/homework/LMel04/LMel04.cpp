#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include "TStack.h"
#include "Expression.h"

using namespace std;

int main( ) {

    char exp[201];

    cout << "Enter an expression: ";

    cin.getline(exp, 201);

    cout <<"Expr read: " << exp << endl;

    Expression e(exp);

    //e.setInfix( exp );

    cout << "Infix  : " << e.inFix() << endl;
    cout << "Postfix: " << e.postFix() << endl;

    cout << "Evaluation: " << e.evaluation( e.postFix() ) << endl;

    return 0;
    
}
