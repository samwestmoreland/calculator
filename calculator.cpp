#include "std_lib_facilities.h"

using namespace std;

int main()
{
    cout << "Please enter expression (we can handle +, -, *, and /)" << endl;
    cout << "Add an x to end expression (e.g. 1+2x): ";
    int lval = 0;
    int rval;
    cin >> lval;
    if (!cin) error("no first operand");

    // Repeatedly read in "operator followed by operand"
    for (char op; cin >> op;)
    {
        if (op != 'x') cin >> rval;
        if (!cin) error("no second operand");
        switch (op)
        {
            case '+':
                lval += rval;
                break;
            case '-':
                lval -= rval;
                break;
            case '*':
                lval *= rval;
                break;
            case '/':
                lval /= rval;
                break;
            default:
                cout << "Result: " << lval << endl;
                return 0;
        }
    }
    error("bad expression");

    return 0;
}
