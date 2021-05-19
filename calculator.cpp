#include "std_lib_facilities.h"

using namespace std;

class Token {
public:
  char kind;    // what kind of token
  double value; // for numbers: a value
};

class Token_stream {
public:
  Token get();
  void putback(Token t);
  void ignore(char c);

private:
  bool full{false}; // is there a token in this buffer?
  Token buffer;     // keep a token put back using putback()
};

void Token_stream::ignore(char c) {
  // first look in buffer
  if (full && c == buffer.kind) {
    full = false;
    return;
  }
  full = false;

  // now search input
  char ch = 0;
  while (cin >> ch)
    if (ch == c)
      return;
}

void Token_stream::putback(Token t) {
  if (full)
    error("putback() into a full buffer");
  buffer = t;  // copy t to buffer
  full = true; // buffer is now full
}

const char number = '8';
const char quit = 'q';
const char print = ';';
Token Token_stream::get() {
  if (full) {
    full = false; // remove token from buffer
    return buffer;
  }

  char ch;
  cin >> ch;
  switch (ch) {
  case print: // for "print"
  case quit:  // for "quit"
  case '(':
  case ')':
  case '+':
  case '-':
  case '*':
  case '/':
    return Token{ch};
  case '.':
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9': {
    cin.putback(ch); // put digit back into input stream
    double val;
    cin >> val; // read a floating point number
    return Token{number, val};
  }
  default:
    error("Bad token");
  }
}

Token_stream ts;
double expression();

double primary() {
  Token t = ts.get();
  switch (t.kind) {
  case '(': // handle '(' expression ')'
  {
    double d = expression();
    t = ts.get();
    if (t.kind != ')')
      error("expected ')'");
    return d;
  }
  case number:
    return t.value;
  case '-':
    return -primary();
  case '+':
    return primary();
  default:
    error("primary expected");
  }
}

double term() {
  double left = primary();
  Token t = ts.get(); // get the next token from the token stream
  while (true) {
    switch (t.kind) {
    case '%': {
      double d = primary();
      if (d == 0)
        error("%:divide by zero");
      left = fmod(left, d);
      t = ts.get();
      break;
    }
    case '*':
      left *= primary();
      t = ts.get();
      break;
    case '/': {
      double d = primary();
      if (d == 0)
        error("divide by zero");
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

double expression() {
  double left = term(); // read and evaluate a Term
  Token t = ts.get();   // get the next token from the token stream
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

void clean_up_mess() { ts.ignore(print); }

void calculate() {
  const string prompt = "> ";
  const string result = "=";
  double val = 0;
  while (cin)
    try {
      cout << prompt;
      Token t = ts.get();
      while (t.kind == print)
        t = ts.get();
      if (t.kind == quit) {
        return;
      }
      ts.putback(t);
      cout << result << expression() << endl;
    } catch (exception &e) {
      cerr << e.what() << endl;
      clean_up_mess();
    }
}

int main() try {
  calculate();
  return 0;
} catch (exception &e) {
  cerr << e.what() << endl;
  return 1;
} catch (...) {
  cerr << "exception" << endl;
  return 2;
}
