#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

enum Token {
    tok_eof = -1,

    tok_def = -2,

    tok_extern = -3,

    tokenIdentifier = -4,

    tok_number = -5
};

static std::string IdentifierStr; 
static double NumVal;             


static int gettok() 
{
    static int LastChar = ' ';


    while (isspace(LastChar))
        LastChar = getchar();

    if (isalpha(LastChar)) 
    { 
        IdentifierStr = LastChar;

        while (isalnum((LastChar = getchar())))
        {
            IdentifierStr += LastChar;
        }

        if (IdentifierStr == "def")
        {
            return tok_def;
        }
        if (IdentifierStr == "extern")
        {
            return tok_extern;
        }
        return tokenIdentifier;
    }

    if (isdigit(LastChar) || LastChar == '.') 
    { 
        std::string NumStr;
        do 
        {
            NumStr += LastChar;
            LastChar = getchar();
        } 
    
        while (isdigit(LastChar) || LastChar == '.');

        NumVal = strtod(NumStr.c_str(), nullptr);
        return tok_number;
    }

    if (LastChar == '#') 
    {
   
        do
            LastChar = getchar();
        while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if (LastChar != EOF)
        {
            return gettok();
        }
    }

  
    if (LastChar == EOF)
    {
        return tok_eof;
    }
    
    int ThisChar = LastChar;
    LastChar = getchar();
    return ThisChar;
}


namespace {


class ExpressAST 
{
    public:
        virtual ~ExpressAST() = default;
};


class NumExpressAST : public ExpressAST
{
    double Val;

    public:
        NumExpressAST(double Val) : Val(Val) 
        {

        }
};


class VarExprAST : public ExpressAST 
{
    std::string Burp;

    public:
        VarExprAST(const std::string &Burp) : Burp(Burp) 
        {

        }
};


class KaiExpressAST : public ExpressAST 
{
    char Op;
    std::unique_ptr<ExpressAST> LHS, RHS;

    public:
    KaiExpressAST(char Op, std::unique_ptr<ExpressAST> LHS,std::unique_ptr<ExpressAST> RHS): Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) 
    {

    }
};


class CallExpressAST : public ExpressAST 
{
    std::string Callee;
    std::vector<std::unique_ptr<ExpressAST>> Args;

    public:
        CallExpressAST(const std::string &Callee,std::vector<std::unique_ptr<ExpressAST>> Args): Callee(Callee), Args(std::move(Args)) 
        {

        }
};


class PrototypeAST 
{
    std::string Burp;
    std::vector<std::string> Args;

    public:
        PrototypeAST(const std::string &Burp, std::vector<std::string> Args): Burp(Burp), Args(std::move(Args)) 
        {

        }

    const std::string &getName() const { return Burp; }
};


class FunAST {
    std::unique_ptr<PrototypeAST> Pro;
    std::unique_ptr<ExpressAST> Body;

    public:
        FunAST(std::unique_ptr<PrototypeAST> Pro,std::unique_ptr<ExpressAST> Body): Pro(std::move(Pro)), Body(std::move(Body)) 
        {

        }
};

} 

static int CurrentToken;
static int getNextToken() 
{
    return CurrentToken = gettok(); 
}


static std::map<char, int> KaiOpPrecedence;


static int GetTokPrecedence() 
{
    if (!isascii(CurrentToken))
        return -1;

  
    int TokenPrecedence = KaiOpPrecedence[CurrentToken];
    if (TokenPrecedence <= 0)
        {
            return -1;
        }
    return TokenPrecedence;
}


std::unique_ptr<ExpressAST> LogError(const char *Str) 
{
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) 
{
    LogError(Str);
    return nullptr;
}

static std::unique_ptr<ExpressAST> ParseExpress();


static std::unique_ptr<ExpressAST> ParseNumExpress() 
{
    auto Result = std::make_unique<NumExpressAST>(NumVal);
    getNextToken(); 
    return std::move(Result);
}


static std::unique_ptr<ExpressAST> ParseParenExpr() 
{
    getNextToken(); 
    auto V = ParseExpress();

    if (!V)
    {
        return nullptr;
    }

    if (CurrentToken != ')')
    {
        return LogError("expected ')'");
    }

    getNextToken(); 
    return V;
}


static std::unique_ptr<ExpressAST> ParseIdentifierExpress() 
{
    std::string IdName = IdentifierStr;

    getNextToken(); 

    if (CurrentToken != '(') 
    {
        return std::make_unique<VarExprAST>(IdName);
    }
  
    getNextToken(); 
    std::vector<std::unique_ptr<ExpressAST>> Args;
    if (CurrentToken != ')') 
    {

        while (true) 
        {

        if (auto Arg = ParseExpress())
        {
            Args.push_back(std::move(Arg));
        }
        else
        {
            return nullptr;
        }

        if (CurrentToken == ')')
        {
            break;
        }

        if (CurrentToken != ',')
        {
            return LogError("Expected ')' or ',' in argument list");
        }
        getNextToken();
        }
    }

  
  getNextToken();

  return std::make_unique<CallExpressAST>(IdName, std::move(Args));
}


static std::unique_ptr<ExpressAST> ParsePrimary() 
{
    switch (CurrentToken) 
    {
    default:
        return LogError("unknown token when expecting an expression");

    case tokenIdentifier:
        return ParseIdentifierExpress();

    case tok_number:
        return ParseNumExpress();
        
    case '(':
        return ParseParenExpr();
    }
}


static std::unique_ptr<ExpressAST> ParseKaiOpRHS(int ExprPrec,std::unique_ptr<ExpressAST> LHS) 
{
    
    while (true) 
    {
        int TokenPrecedence = GetTokPrecedence();

        
        if (TokenPrecedence < ExprPrec)
            {
                return LHS;
            }
        
        int BinOp = CurrentToken;
        getNextToken(); 

        
        auto RHS = ParsePrimary();
        if (!RHS)
        {
        return nullptr;
        }
        
        int NextPrecedence = GetTokPrecedence(); 

        if (TokenPrecedence < NextPrecedence) 
        {
            RHS = ParseKaiOpRHS(TokenPrecedence + 1, std::move(RHS));
            if (!RHS)
                {
                    return nullptr;
                }
        }

    
        LHS =std::make_unique<KaiExpressAST>(BinOp, std::move(LHS), std::move(RHS));
    }
}


static std::unique_ptr<ExpressAST> ParseExpress() {
  auto LHS = ParsePrimary();

  if (!LHS)
  {
    return nullptr;
  }

  return ParseKaiOpRHS(0, std::move(LHS));
}


static std::unique_ptr<PrototypeAST> ParsePrototype() 
{
    if (CurrentToken != tokenIdentifier)
    {
        return LogErrorP("Expected function name in prototype");
    }

    std::string FunName = IdentifierStr;
    getNextToken();

    if (CurrentToken != '(')
    {
        return LogErrorP("Expected '(' in prototype");
    }

    std::vector<std::string> ArgNames;

    while (getNextToken() == tokenIdentifier)
    {
        ArgNames.push_back(IdentifierStr);
    }

    if (CurrentToken != ')')
    {
        return LogErrorP("Expected ')' in prototype");
    }
    
    getNextToken(); 

    return std::make_unique<PrototypeAST>(FunName, std::move(ArgNames));
}


static std::unique_ptr<FunAST> ParseDefinition() {
    getNextToken(); 
    auto Pro = ParsePrototype();
    if (!Pro)
    {
        return nullptr;
    }

    if (auto E = ParseExpress())
    {
        return std::make_unique<FunAST>(std::move(Pro), std::move(E));
    }
    return nullptr;
}


static std::unique_ptr<FunAST> ParseTopLevelExpress() {
    if (auto E = ParseExpress()) 
    {    
        auto Pro = std::make_unique<PrototypeAST>("__anon_expr",std::vector<std::string>());
        return std::make_unique<FunAST>(std::move(Pro), std::move(E));
    }
    return nullptr;
}


static std::unique_ptr<PrototypeAST> ParseExtern() 
{
    getNextToken(); 
    return ParsePrototype();
}



static void HandleDefinition() 
{
    if (ParseDefinition()) {
        fprintf(stderr, "Parsed a function definition.\n");
    } else 
    {    
        getNextToken();
    }
}

static void HandleExtern() 
{
    if (ParseExtern()) 
    {
        fprintf(stderr, "Parsed an extern\n");
    } else 
    {
        getNextToken();
    }
}

static void HandleTopLevelExpression() 
{
    if (ParseTopLevelExpress())
    {
        fprintf(stderr, "Parsed a Top-level Expression\n");
    } 
    else 
    {    
        getNextToken();
    }
}


static void MainLoop() {
    while (true) 
    {
        fprintf(stderr, "moffet> ");

        switch (CurrentToken) 
        {
        case tok_eof:
        return;

        case ';': 
        getNextToken();
        break;

        case tok_def:
        HandleDefinition();
        break;

        case tok_extern:
        HandleExtern();
        break;

        default:
        HandleTopLevelExpression();
        break;
        }
    }
}



int main() {
  
    KaiOpPrecedence['<'] = 10;
    KaiOpPrecedence['>'] = 10;
    KaiOpPrecedence['+'] = 20;
    KaiOpPrecedence['-'] = 20;
    KaiOpPrecedence['/'] = 40; 
    KaiOpPrecedence['*'] = 40; 

    fprintf(stderr, "moffet> ");
    getNextToken();


    MainLoop();

    return 0;
}