#include <string> 
#include <iostream>
#include <vector>
using namespace std;

/*-----------------LEXER---------------------*/

enum Token  // List of tokens
{
    tok_eof = -1,
    tok_def = -2,
    tok_extern = -3,
    tok_identifier = -4,
    tok_number = -5
};

static std::string IdentifierStr;
static double NumVal;

static int gettok() // Takes the token for processing.
{
    static int LastChar = ' ';  

    while(isspace(LastChar))  // Skips all whitespaces
    {
        LastChar = getchar();
    }

    if (isalpha(LastChar))  // For ASCII values and Identifiers
    {
        IdentifierStr = LastChar;

        while(isalnum((LastChar = getchar())))
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
        return tok_identifier;
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

    NumVal = strtod(NumStr.c_str(),0);
    return tok_number;
    }

    if (LastChar == '$')
    {
        do 
        {
            LastChar = getchar();
        }
        while (LastChar != EOF && LastChar !='\n' && LastChar !='\r');
    }

        if (LastChar != EOF)
        {
            return tok_eof;
        }

        int ThisChar = LastChar;
        LastChar = getchar();
        return ThisChar;
    
}
/*
int main()
{
    while(true)
    {
            int tok = gettok();
            cout << "Lexer Got Token:" << tok << endl;
    }
}
*/

/*----------------END OF LEXER-----------------*/

/*-----------------PARSER------------------*/

class ExpressAST
{
    public:
        virtual ~ExpressAST()
        {

        }
};

class NumExpressAST : public ExpressAST
{
    double val;

    public:
        NumExpressAST(double Val) : val(Val)
        {

        }
};

class VarExpressAST : public ExpressAST
{
    std::string burp;

    public:
        VarExpressAST(const std::string & Burp) : burp(Burp)
        {

        }
};

class BinExpressAST : public ExpressAST
{
    char Op;
    std::unique_ptr<ExpressAST> LHS, RHS;

    public:
        BinExpressAST(
            char op,
            std::unique_ptr<ExpressAST> LHS,
            std::unique_ptr<ExpressAST> RHS
        ): Op(op),LHS(std::move(LHS)),RHS(std::move(RHS))
        {

        }
};

class CallExpressAST : public ExpressAST
{
    std::string Callee;
    std::vector<std::unique_ptr<ExpressAST>> Args;

    public:
        CallExpressAST(const std::string & Callee,
        std::vector<std::unique_ptr<ExpressAST>> Args)
        : Callee(Callee), Args(std::move(Args))
        {

        }
};

class PrototypeAST
{
    std::string Burp;
    std::vector<std::string> Args;

    public:
        PrototypeAST(const std::string & Burp,
        std::vector<std::string> Args)
        :Burp(Burp), Args(std::move(Args))
        {

        }
};

class FunAST
{
    std:: unique_ptr<PrototypeAST> Pro;
    std:: unique_ptr<ExpressAST> Body;

    public:
        FunAST(std:: unique_ptr<ProAST> Pro,
        std:: unique_ptr<ExpressAST> Body)
        :Pro(std::move(Pro)),Body(std::move(Body))
        {

        }
};

static int CurrentToken;

static int getNextToken()
{
    return CurrentToken = gettok();
}

std::unique_ptr<ExpressAST> LogError(const char *Str)
{
    fprintf(stderr,"Log Eroor: %s\n", Str);
    return nullptr;
}

std::unique_ptr<PrototypeAST> LogError(const char *Str)
{
    LogError(Str);
    return nullptr;
}


static std::unique_ptr<ExpressAST> ParseNumberExpress()
{
        auto Result = std::make_unique<NumExpressAST>(NumVal);
        getNextToken();
        return std:: move(Result);
}

static std::unique_ptr<ExpressAST> ParseParanthExpress()
{
    getNextToken();
    auto V = ParseExpression();
    if (!V) 
    {
        return nullptr;
    }

    if (CurrentToken ==')')
    {
        getNextToken();
        return V;
    }
    else
    {
        return LogError("expect ')'");
    }
}

static std::unique_ptr<ExpressAST> ParseIdentifierOrCallExpress()
{
    std::string IdName = IdentifierStr;

    getNextToken();

    if (CurrentToken == '()')
    {
        getNextToken();
        std::vector<std::unique_ptr<ExpressAST>> Args;
        while(true)
        {
            auto Arg = ParseExpressio();
            if (Arg)
            {
                Args.push_back(Arg);
            }
            else
            {
                return nullptr;
            }

            if (CurrentToken == ')')
            {
                 getNextToken();
                break;
            }
            else if (CurrentToken == ',')
            {
                getNextToken();
                 continue;
            }
            else
            {
                return LogError("Expected ')' or ',' in argument list");
            }
        }
        return std::make_unique<CallExpressAST>(IdName, std::move(Args));
    }
    else
    {
        return std::make_unique<VarExpressAST>(IdName);
    }
}

static std::unique_ptr<ExpressAST> ParsePrimary()
{
    switch (CurrentToken)
    {
        case tok_identifier:
            return ParseIdentifierOrCallExpress();
        
        case tok_number:
            return ParseNumberExpress();

        case '(':
            return ParseParnthExpress();
        
        default:
            return LogError("Unknown token when expecting an Expression");
    }
}

/* ______Super buggy one , still under developmet, Until its fixed , you can play around with the lexer______*/