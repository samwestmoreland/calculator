#include "std_lib_facilities.h"

using namespace std;

class Token
{
    public:
        char kind;      // what kind of token
        double value;   // for numbers: a value
};

class Token_stream {
    public:
        Token get();
        void putback(Token t);
    private:
        bool full {false};  // is there a token in this buffer?
        Token buffer;       // keep a token put back using putback()
};

void Token_stream::putback(Token t)
{
    if (full) error("putback() into a full buffer");
    buffer = t;     // copy t to buffer
    full = true;    // buffer is now full
}

Token Token_stream::get()
{
    if (full) {
        full = false;   // remove token from buffer
        return buffer;
    }

    char ch;
    cin >> ch;
    switch (ch) {
        case ';':   // for "print"
        case 'q':   // for "quit"
        case '(': case ')': case '+': case '-': case '*': case '/':
            return Token{ch};
        case '.':
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            {
                cin.putback(ch);    // put digit back into input stream
                double val;
                cin >> val;         // read a floating point number
                return Token{'8', val};
            }
        default:
            error("Bad token");
    }
}
Token_stream ts;
double expression();
double primary()
{
    Token t = ts.get();
    switch (t.kind) {
        case '(':   // handle '(' expression ')'
            {
                double d = expression();
                t = ts.get();
                if (t.kind != ')') error("expected ')'");
                return d;
            }
        case '8':
            return t.value;
        default:
            error("primary expected");
    }
}

double term()
{
    double left = primary();
    Token t = ts.get();     // get the next token from the token stream
    while (true) {
        switch (t.kind) {
            case '*':
                left *= primary();
                t = ts.get();
                break;
            case '/':
                {
                    double d = primary();
                    if (d == 0) error("divide by zero");
                    left /= d;
                    t = ts.get();
                    break;
                }
            default:
                ts.putback(t);
                return left;
        }
    }
}

double expression()
{
    double left = term();   // read and evaluate a Term
    Token t = ts.get();     // get the next token from the token stream
    while (true) {
        switch (t.kind) {
            case '+':
                left += term();
                t = ts.get();
                break;
            case '-':
                left -= term();
                t = ts.get();
                break;
            default:
                ts.putback(t);
                return left;
        }
    }
}



int main()
try {
    double val = 0;
    while (cin) {
        Token t = ts.get();
        if (t.kind == 'q') break;
        if (t.kind == ';') cout << "=" << val << endl;
        else
            ts.putback(t);
        val = expression();
    }
}
catch (exception& e) {
    cerr << e.what() << endl;
    return 1;
}
catch (...) {
    cerr << "exception" << endl;
    return 2;
}
