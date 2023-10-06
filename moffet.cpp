#include <string> 
#include <iostream>
using namespace std;

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

int main()
{
    while(true)
    {
            int tok = gettok();
            cout << "Lexer Got Token:" << tok << endl;
    }
}
