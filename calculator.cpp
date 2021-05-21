#include "std_lib_facilities.h"

using namespace std;

class Token {
public:
  char kind;    // what kind of token
  double value; // for numbers: a value
  string name;
  Token() {}
  Token(char ch) : kind{ch} {} // initialize kind with ch
  Token(char ch, double val)
      : kind{ch}, value{val} {} // initialize kind and value

  Token(char ch, string n) : kind{ch}, name{n} {} // initialize kind and name
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

class Variable {
public:
  string name;
  double value;
};

vector<Variable> var_table;

// return the value of the Variable named s
double get_value(string s) {
  for (const Variable &v : var_table) {
    if (v.name == s) {
      return v.value;
    }
  }
  error("get: undefined variable ", s);
}

// set the variable named s to d
void set_value(string s, double d) {
  for (Variable &v : var_table) {
    if (v.name == s) {
      v.value = d;
      return;
    }
  }
  error("set: undefined variable", s);
}

// is var already in var_table?
bool is_declared(string var) {
  for (const Variable &v : var_table) {
    if (v.name == var) {
      return true;
    }
  }
  return false;
}

// add {var,val} to var_table
double define_name(string var, double val) {
  if (is_declared(var))
    error(var, " declared twice");
  var_table.push_back(Variable{var, val});
  return val;
}

Token_stream ts;
double expression();

const char number = '8';
const char quit = 'q';
const char print = ';';
const char name = 'a';        // name token
const char let = 'L';         // declaration token
const string declkey = "let"; // declaration keyword

// handle: name = expression
double declaration() {
  Token t = ts.get();
  if (t.kind != name)
    error("name expected in declaration");
  string var_name = t.name;

  Token t2 = ts.get();
  if (t2.kind != '=')
    error("= missing in declaration of ", var_name);

  double d = expression();
  define_name(var_name, d);
  return d;
}

double statement() {
  Token t = ts.get();
  switch (t.kind) {
  case let:
    return declaration();
  default:
    ts.putback(t);
    return expression();
  }
}

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
  case '=':
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
    if (isalpha(ch)) {
      string s;
      s += ch;
      while (cin.get(ch) && (isalpha(ch) || isdigit(ch)))
        s += ch;
      cin.putback(ch);
      if (s == declkey)
        return Token{let}; // declaration keyword
      return Token{name, s};
    }
    error("Bad token");
  }
}

double primary() {
  Token t = ts.get();
  switch (t.kind) {
  case 'a': // handle variables
    return get_value(t.name);
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
      while (t.kind == print) // discard all "prints"
        t = ts.get();
      if (t.kind == quit) { // quit
        return;
      }
      ts.putback(t);
      cout << result << statement() << endl;
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
