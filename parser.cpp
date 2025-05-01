#include "parserInterp.h"

map<string, bool> defVar;
map<string, Token> SymTable;

map<string, Value>
    TempsResults;  // Container of temporary locations of Value objects for
                   // results of expressions, variables values, and constants
queue<Value> *ValQue = new queue<Value>;

namespace Parser {
bool pushed_back = false;
LexItem pushed_token;

static LexItem GetNextToken(istream &in, int &line) {
    if (pushed_back) {
        pushed_back = false;
        return pushed_token;
    }
    return getNextToken(in, line);
}

static void PushBackToken(LexItem &t) {
    if (pushed_back) {
        abort();
    }
    pushed_back = true;
    pushed_token = t;
}

}  // namespace Parser

static int error_count = 0;

int ErrCount() { return error_count; }

void ParseError(int line, string msg) {
    ++error_count;
    cout << line << ": " << msg << endl;
}

bool IdentList(istream &in, int &line);

// Program is: Prog ::= PROGRAM IDENT ; DeclPart CompoundStmt .
bool Prog(istream &in, int &line) {
    bool f1, f2;
    LexItem tok = Parser::GetNextToken(in, line);

    if (tok.GetToken() == PROGRAM) {
        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == IDENT) {
            string identstr = tok.GetLexeme();
            if (!(defVar.find(identstr)->second)) {
                defVar[identstr] = true;
            }
            tok = Parser::GetNextToken(in, line);
            if (tok.GetToken() == SEMICOL) {
                f1 = DeclPart(in, line);

                if (f1) {
                    tok = Parser::GetNextToken(in, line);
                    if (tok == BEGIN) {
                        f2 = CompoundStmt(in, line);
                        if (!f2) {
                            ParseError(line, "Incorrect Program Body.");
                            return false;
                        }
                        tok = Parser::GetNextToken(in, line);
                        if (tok.GetToken() == DOT)
                            return true;  // Successful Parsing is completed
                        else {
                            ParseError(line, "Missing end of program dot.");
                            return false;
                        }
                    } else {
                        ParseError(line, "Missing program body.");
                        return false;
                    }
                } else {
                    ParseError(line, "Incorrect Declaration Section.");
                    return false;
                }
            } else {
                ParseError(line - 1, "Missing Semicolon.");
                return false;
            }
        } else {
            ParseError(line, "Missing Program Name.");
            return false;
        }
    } else if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    } else if (tok.GetToken() == DONE && tok.GetLinenum() <= 1) {
        ParseError(line, "Empty File");
        return true;
    }
    ParseError(line, "Missing PROGRAM Keyword.");
    return false;
}  // End of Prog

// DeclPart ::= VAR DeclStmt { ; DeclStmt }
bool DeclPart(istream &in, int &line) {
    bool status = false;
    LexItem tok;

    LexItem t = Parser::GetNextToken(in, line);
    if (t == VAR) {
        status = DeclStmt(in, line);

        while (status) {
            tok = Parser::GetNextToken(in, line);
            if (tok != SEMICOL) {
                ParseError(line, "Missing semicolon in Declaration Statement.");
                return false;
            }
            status = DeclStmt(in, line);
        }

        tok = Parser::GetNextToken(in, line);
        if (tok == BEGIN) {
            Parser::PushBackToken(tok);
            return true;
        } else {
            ParseError(line, "Syntactic error in Declaration Block.");
            return false;
        }
    } else {
        ParseError(line, "Non-recognizable Declaration Part.");
        return false;
    }

}  // end of DeclBlock function

// DeclStmt ::= IDENT {, IDENT } : Type [:= Rxpr]
bool DeclStmt(istream &in, int &line) {
    LexItem t;
    bool status = IdentList(in, line);
    bool flag;
    // For tracking variables and making a Value object
    Value retVal;

    if (!status) {
        ParseError(line,
                   "Incorrect identifiers list in Declaration Statement.");
        return status;
    }

    t = Parser::GetNextToken(in, line);
    if (t == COLON) {
        t = Parser::GetNextToken(in, line);
        Token varType = t.GetToken();
        if (t == INTEGER || t == REAL || t == STRING || t == BOOLEAN) {
            t = Parser::GetNextToken(in, line);
            if (t == ASSOP) {
                flag = Expr(in, line, retVal);
                if (!flag) {
                    ParseError(line, "Incorrect initialization expression.");
                    return false;
                }
                // Since the variable have been initialized, set their values in
                // tempsresults
                for (auto variable : SymTable) {
                    if (variable.second == ERR) {
                        SymTable[variable.first] = varType;
                        if (SymTable[variable.first] == INTEGER &&
                            retVal.IsReal()) {
                            TempsResults[variable.first] =
                                Value((int)retVal.GetReal());
                        } else {
                            TempsResults[variable.first] = retVal;
                        }
                    }
                }
            } else {
                // Set variable types  in SymTable WAIT NONONONON
                // DSFPLSDLCFLPFASLFPS LDSPLSSSSSS DFSCS COMMENT it sticks out a
                // lot so ill notice it
                for (auto variable : SymTable) {
                    // they all equal the type for : Type [:= Rxpr]
                    if (variable.second == ERR) {
                        SymTable[variable.first] = varType;
                    }
                }
                Parser::PushBackToken(t);
            }
            return true;
        } else {
            ParseError(line, "Incorrect Declaration Type.");
            return false;
        }
    } else {
        Parser::PushBackToken(t);

        return false;
    }

}  // End of DeclStmt

// IdList:= IDENT {,IDENT}
bool IdentList(istream &in, int &line) {
    bool status;
    string identstr;

    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == IDENT) {
        // set IDENT lexeme to the type tok value
        identstr = tok.GetLexeme();
        if (!(defVar.find(identstr)->second)) {
            defVar[identstr] = true;
            // new variable! add it to variable list and keep track of it
            SymTable[identstr] = ERR;
        } else {
            ParseError(line, "Variable Redefinition");
            return false;
        }
    } else {
        Parser::PushBackToken(tok);

        return true;
    }

    tok = Parser::GetNextToken(in, line);

    if (tok == COMMA) {
        status = IdentList(in, line);
    }

    else if (tok == COLON) {
        Parser::PushBackToken(tok);
        return true;
    } else if (tok == IDENT) {
        ParseError(line, "Missing comma in declaration statement.");
        return false;
    } else {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    return status;
}  // End of IdentList

// Stmt is either a simple statement or a structured one
// Stmt ::= SimpleStmt | StructuredStmt
bool Stmt(istream &in, int &line) {
    bool status = false;
    // cout << "in ContrlStmt" << endl;
    LexItem t = Parser::GetNextToken(in, line);
    if (t == WRITELN || t == WRITE || t == IDENT) {
        Parser::PushBackToken(t);
        status = SimpleStmt(in, line);
        if (!status) {
            ParseError(line, "Incorrect Simple Statement.");
            return status;
        }
        return status;
    } else if (t == IF || t == BEGIN) {
        Parser::PushBackToken(t);
        status = StructuredStmt(in, line);
        if (!status) {
            ParseError(line, "Incorrect Structured Statement.");
            return status;
        }
        return status;
    } else if (t == END) {
        cout << "in stmt: " << t.GetLexeme() << endl;
        Parser::PushBackToken(t);
        return false;
    }
    return status;
}  // End of Stmt

// StructuredStmt ::= IfStmt | CompoundStmt
bool StructuredStmt(istream &in, int &line) {
    bool status;

    LexItem t = Parser::GetNextToken(in, line);
    switch (t.GetToken()) {
        case IF:  // Keyword consumed
            status = IfStmt(in, line);

            break;

        case BEGIN:  // Keyword consumed
            status = CompoundStmt(in, line);
            break;

        default:;
    }

    return status;
}  // End of StructuredStmt

// CompoundStmt ::= BEGIN Stmt {; Stmt } END
bool CompoundStmt(istream &in, int &line) {
    bool status;
    LexItem tok;
    status = Stmt(in, line);
    tok = Parser::GetNextToken(in, line);
    while (status && tok == SEMICOL) {
        status = Stmt(in, line);
        tok = Parser::GetNextToken(in, line);
    }
    if (!status) {
        ParseError(line, "Syntactic error in the statement.");
        Parser::PushBackToken(tok);
        return false;
    }
    if (tok == END)
        return true;
    else if (tok == ELSE) {
        Parser::PushBackToken(tok);
        return true;
    } else {
        ParseError(line, "Missing end of compound statement.");
        return false;
    }
}  // end of CompoundStmt

// SimpleStmt ::= AssignStmt | WriteLnStmt | WriteStmt
bool SimpleStmt(istream &in, int &line) {
    bool status;

    LexItem t = Parser::GetNextToken(in, line);

    switch (t.GetToken()) {
        case WRITELN:  // Keyword is consumed
            status = WriteLnStmt(in, line);
            break;

        case WRITE:  // Keyword is consumed
            status = WriteStmt(in, line);
            break;

        case IDENT:  // Keyword is not consumed
            Parser::PushBackToken(t);
            status = AssignStmt(in, line);

            break;
        default:;
    }

    return status;
}  // End of SimpleStmt

// WriteLnStmt ::= writeln (ExprList)
// WriteLnStmt ::= writeln (ExprList)
bool WriteLnStmt(istream &in, int &line) {
    LexItem t;
    // cout << "in WriteStmt" << endl;

    t = Parser::GetNextToken(in, line);
    if (t != LPAREN) {
        ParseError(line, "Missing Left Parenthesis");
        return false;
    }

    bool ex = ExprList(in, line);

    if (!ex) {
        ParseError(line, "Missing expression list for WriteLn statement");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != RPAREN) {
        ParseError(line, "Missing Right Parenthesis");
        return false;
    }

    // Evaluate: print out the list of expressions' values
    while (!(*ValQue).empty()) {
        Value nextVal = (*ValQue).front();
        cout << nextVal;
        ValQue->pop();
    }
    cout << endl;
    return ex;
}  // End of WriteLnStmt

// WriteStmt ::= write (ExprList)
bool WriteStmt(istream &in, int &line) {
    LexItem t;

    t = Parser::GetNextToken(in, line);
    if (t != LPAREN) {
        ParseError(line, "Missing Left Parenthesis");
        return false;
    }

    bool ex = ExprList(in, line);

    if (!ex) {
        ParseError(line, "Missing expression list for Write statement");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != RPAREN) {
        ParseError(line, "Missing Right Parenthesis");
        return false;
    }

    return ex;
}  // End of WriteStmt

// IfStmt ::= IF Expr THEN Stmt [ ELSE Stmt ]
bool IfStmt(istream &in, int &line) {
    bool ex = false, status;
    LexItem t;
    Value ifStmtVal;

    ex = Expr(in, line, ifStmtVal);
    if (!ex) {
        ParseError(line, "Missing if statement Logic Expression");
        return false;
    }
    // Check if ex is valid for if statement
    if (ifStmtVal.GetType() != VBOOL) {
        ParseError(line,
                   "Run-Time Error-Illegal Type for If statement condition");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != THEN) {
        ParseError(line, "If-Stmt Syntax Error");
        return false;
    }

    // this is the part we skip if expr is false
    if (ifStmtVal.GetBool()) {
        status = Stmt(in, line);
        if (!status) {
            ParseError(line, "Missing Statement for If-Stmt Then-Part");
            return false;
        }
        t = Parser::GetNextToken(in, line);
    } else {
        // check if statment goes to compound (has begin) or isnt (just one
        // line)
        t = Parser::GetNextToken(in, line);
        // if its begin, skip till end
        if (t == BEGIN) {
            do {
                t = Parser::GetNextToken(in, line);
            } while (t != END);
        } else {
            do {
                t = Parser::GetNextToken(in, line);
            } while (t != SEMICOL);
        }
        t = Parser::GetNextToken(in, line);
    }

    if (t == ELSE) {
        if (!ifStmtVal.GetBool()) {
            status = Stmt(in, line);
            if (!status) {
                ParseError(line, "Missing Statement for If-Stmt Else-Part");
                return false;
            }
            return true;
        } else {
            do {
                t = Parser::GetNextToken(in, line);
            } while (t != END);
            t = Parser::GetNextToken(in, line);
        }
    }

    Parser::PushBackToken(t);  // semicolon pushed back or anything else
    return true;
}  // End of IfStmt function

// Var ::= ident
bool Var(istream &in, int &line, LexItem &idtok) {
    string identstr;

    LexItem tok = Parser::GetNextToken(in, line);
    idtok = tok;

    if (tok == IDENT) {
        identstr = tok.GetLexeme();

        if (!(defVar.find(identstr)->second)) {
            ParseError(line, "Undeclared Variable");
            return false;
        }
        return true;
    } else if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    return false;
}  // End of Var

// AssignStmt::= Var := Expr
bool AssignStmt(istream &in, int &line) {
    bool varstatus = false, status = false;
    LexItem t;
    // Our special token to store variable name
    LexItem idtok;

    varstatus = Var(in, line, idtok);

    if (varstatus) {
        t = Parser::GetNextToken(in, line);

        if (t == ASSOP) {
            Value retVal;
            status = Expr(in, line, retVal);
            if (!status) {
                ParseError(line, "Missing Expression in Assignment Statement");
                return status;
            }
            // if both var and expression types match, then its good. otherwise,
            // we have a problem
            if ((SymTable[idtok.GetLexeme()] == INTEGER &&
                 (retVal.GetType() == VINT)) ||
                (SymTable[idtok.GetLexeme()] == REAL &&
                 (retVal.GetType() == VREAL || retVal.GetType() == VINT)) ||
                (SymTable[idtok.GetLexeme()] == STRING &&
                 retVal.GetType() == VSTRING) ||
                (SymTable[idtok.GetLexeme()] == BOOLEAN &&
                 retVal.GetType() == VBOOL)) {
                TempsResults[idtok.GetLexeme()] = retVal;
            } else if (SymTable[idtok.GetLexeme()] == INTEGER &&
                       retVal.GetType() == VREAL) {
                TempsResults[idtok.GetLexeme()] =
                    Value(static_cast<int>(retVal.GetReal()));
            }

            else {
                ParseError(line, "Illegal mixed-mode assignment operation");
                return false;
            }
        } else if (t.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << t.GetLexeme() << ")" << endl;
            return false;
        } else {
            ParseError(line, "Missing Assignment Operator");
            return false;
        }
    } else {
        ParseError(line,
                   "Missing Left-Hand Side Variable in Assignment statement");
        return false;
    }
    return status;
}

// ExprList:= Expr {,Expr}
bool ExprList(istream &in, int &line) {
    bool status = false;
    Value retVal;
    // cout << "in ExprList and before calling Expr" << endl;
    status = Expr(in, line, retVal);
    if (!status) {
        ParseError(line, "Missing Expression");
        return false;
    }
    ValQue->push(retVal);
    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == COMMA) {
        // cout << "before calling ExprList" << endl;
        status = ExprList(in, line);
        // cout << "after calling ExprList" << endl;
    } else if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    } else {
        Parser::PushBackToken(tok);
        return true;
    }
    return status;
}  // ExprList

// Expr ::= LogORExpr ::= LogANDExpr { || LogANDRxpr }
bool Expr(istream &in, int &line, Value &retVal) {
    // LogORExpr
    LexItem tok;
    bool t1 = LogANDExpr(in, line, retVal);

    if (!t1) {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while (tok == OR) {
        t1 = LogANDExpr(in, line, retVal);
        if (!t1) {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }
    Parser::PushBackToken(tok);
    return true;
}  // End of Expr/LogORExpr

// LogAndExpr ::= RelExpr {AND RelExpr }
bool LogANDExpr(istream &in, int &line, Value &retVal) {
    LexItem tok;
    Value baseVar;
    Value extraVar;
    bool t1 = RelExpr(in, line, baseVar);

    if (!t1) {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while (tok == AND) {
        t1 = RelExpr(in, line, extraVar);
        if (!t1) {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        // NEED BASE CASE STUFF
        baseVar = baseVar && extraVar;
        if (baseVar.GetType() == VERR) {
            ParseError(line, "Illegal operand types for AND operation.");
            return false;
        }

        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }

    Parser::PushBackToken(tok);
    retVal = baseVar;
    return true;
}  // End of LogANDExpr

// RelExpr ::= SimpleExpr  [ ( = | < | > ) SimpleExpr ]
bool RelExpr(istream &in, int &line, Value &retVal) {
    Value baseTermVal;
    Value extraTermVal;
    LexItem tok;
    bool t1 = SimpleExpr(in, line, baseTermVal);

    if (!t1) {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    if (tok == LTHAN || tok == GTHAN || tok == EQ) {
        t1 = SimpleExpr(in, line, extraTermVal);
        if (!t1) {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        // Equate base to something using base and extra vals
        if (tok == LTHAN)
            baseTermVal = baseTermVal < extraTermVal;
        else if (tok == GTHAN)
            baseTermVal = baseTermVal > extraTermVal;
        else if (tok == EQ)
            baseTermVal = baseTermVal == extraTermVal;

        // check that operation didnt return an error
        if (baseTermVal.GetType() == VERR) {
            ParseError(line, "baddy thing with < > ==");
            return false;
        }
        tok = Parser::GetNextToken(in, line);

        if (tok == LTHAN || tok == GTHAN || tok == EQ) {
            ParseError(line, "Illegal Relational Expression.");
            return false;
        } else if (tok.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }
    Parser::PushBackToken(tok);
    retVal = baseTermVal;
    return true;
}  // End of RelExpr

// Expr:= Term {(+|-) Term}
bool SimpleExpr(istream &in, int &line, Value &retVal) {
    Value baseTermVal;
    Value extraTermVal;
    bool t1 = Term(in, line, baseTermVal);
    LexItem tok;

    if (!t1) {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while (tok == PLUS || tok == MINUS) {
        t1 = Term(in, line, extraTermVal);
        if (!t1) {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        // Actually do the addition / subtraction. since Term() should give
        // signs, just add

        baseTermVal = baseTermVal + extraTermVal;
        if (baseTermVal.GetType() == VERR) {
            ParseError(line,
                       "Illegal operand types for an arithmetic operator.");
            return false;
        }

        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }
    Parser::PushBackToken(tok);
    retVal = baseTermVal;
    return true;
}  // End of Expr

// Term:= SFactor {( * | / | DIV | MOD) SFactor}
bool Term(istream &in, int &line, Value &retVal) {
    Value baseTermVal;
    Value extraTermVal;
    bool t1 = SFactor(in, line, baseTermVal);
    LexItem tok;

    if (!t1) {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while (tok == MULT || tok == DIV || tok == IDIV || tok == MOD) {
        t1 = SFactor(in, line, extraTermVal);

        if (!t1) {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        // Do mult, div, etc. here
        if (tok == MULT)
            baseTermVal = baseTermVal * extraTermVal;
        else if (tok == DIV) {
            // Special case for divide by zero
            if (extraTermVal.GetInt() == 0 || extraTermVal.GetReal() == 0) {
                ParseError(line, "Run-Time Error-Illegal division by Zero");
                return false;
            }
            baseTermVal = baseTermVal / extraTermVal;
        } else if (tok == IDIV) {
            // Special case for divide by zero
            if (extraTermVal.IsInt()) {
                if (extraTermVal.GetInt() == 0) {
                    ParseError(line,
                               "Run-Time Error-Illegal integer division (div) "
                               "by Zero");
                    return false;
                }
            } else if (extraTermVal.IsReal()) {
                if (extraTermVal.GetReal() == 0) {
                    ParseError(line,
                               "Run-Time Error-Illegal integer division (div) "
                               "by Zero");
                    return false;
                }
            }
            baseTermVal = baseTermVal.div(extraTermVal);
        } else if (tok == MOD)
            baseTermVal = baseTermVal % extraTermVal;

        // check if the operation didnt return error
        if (baseTermVal.GetType() == VERR) {
            ParseError(line,
                       "Illegal operand types for an arithmetic operator.");
            return false;
        }

        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }
    Parser::PushBackToken(tok);
    retVal = baseTermVal;
    return true;
}  // End of Term

// SFactor ::= [( - | + | NOT )] Factor
bool SFactor(istream &in, int &line, Value &retVal) {
    LexItem t = Parser::GetNextToken(in, line);
    bool status;
    int sign = 1;
    if (t == MINUS) {
        sign = -1;
    } else if (t == PLUS) {
        sign = 1;
    } else if (t == NOT) {
        sign = -2;  // sign is a NOT op for logic operands
    } else
        Parser::PushBackToken(t);

    status = Factor(in, line, retVal, sign);
    return status;
}  // End of SFactor

// Factor ::= IDENT | ICONST | RCONST | SCONST | BCONST | (Expr)
bool Factor(istream &in, int &line, Value &retVal, int sign) {
    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == IDENT) {
        string lexeme = tok.GetLexeme();

        // Just for not operator
        if (sign == -2) {
            if (SymTable[lexeme] == STRING || SymTable[lexeme] == INTEGER ||
                SymTable[lexeme] == REAL) {
                ParseError(line, "Illegal Operand Type for Sign/NOT Operator");
                return false;
            }
        } else if (sign == -1) {
            if (SymTable[lexeme] == STRING || SymTable[lexeme] == BOOLEAN) {
                ParseError(line, "Illegal Operand Type for Sign/NOT Operator");
                return false;
            }
        } else if (!(defVar.find(lexeme)->second)) {
            ParseError(line, "Using Undefined Variable");
            return false;
        } else if (TempsResults[lexeme].GetType() == VERR) {
            ParseError(line, "Using uninitialized Variable");
            return false;
        }
        // if its an ident, means its another variable
        retVal = TempsResults[lexeme];
        return true;
    }
    // Reminder for basic variable types, iconst, sconst, rconst, bconst
    else if (tok == ICONST) {
        int ival = stoi(tok.GetLexeme()) * sign;
        retVal = Value(static_cast<double>(ival));
        return true;
    } else if (tok == SCONST) {
        string sval = tok.GetLexeme();
        retVal = Value(sval);
        return true;
    } else if (tok == RCONST) {
        double rval = stod(tok.GetLexeme()) * sign;
        retVal = Value(rval);
        return true;
    } else if (tok == BCONST) {
        bool bval;
        if (tok.GetLexeme() == "true") {
            bval = true;
        } else
            bval = false;
        bval *= sign;

        retVal = Value(bval);
        return true;
    } else if (tok == LPAREN) {
        bool ex = Expr(in, line, retVal);
        if (!ex) {
            ParseError(line, "Missing expression after (");
            return false;
        }
        if (Parser::GetNextToken(in, line) == RPAREN)
            return ex;
        else {
            Parser::PushBackToken(tok);
            ParseError(line, "Missing ) after expression");
            return false;
        }
    } else if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    Parser::PushBackToken(tok);

    return false;
}
