%{    
    #include "source_files/symexpr.h"

    // Declare stuff from Flex that Bison needs to know about:
    extern int yylex();
    extern int yyparse();    
    void yyerror(const char *s);
    
    bool v = false;
%}

%union
{
    struct Symbolic_expression* pSymexpr;
    int ival;
}

%token nl sin cos tan cotan log sqrt sinh cosh tanh cotanh verbose
%token <ival>num
%type <pSymexpr>expr

%left '+' '-'
%left '/'
%left '*'
%left '^'
%left '(' ')'

%%

p : p stmt nl
    | ;

stmt : expr     {
                    PSYMEXPR e = simplify_repeatedly($1, false);
                    PSYMEXPR de = simplify_repeatedly(derivative(e), v);

                    printf("\nThe derivative of\n--------------------\n");
                    print(e);
                    printf("\n--------------------\n\nis\n--------------------\n");
                    print(de);
                    printf("\n--------------------\n");
                    printf("________________________________________________________________________________\n");

                    deallocate(e);
                    deallocate(de);
                }
     | verbose  {
                    if (v)
                            {
                                v = false;
                                printf("verbose mode deactivated\n");                        
                                printf("________________________________________________________________________________\n");
                            }
                            else
                            {
                                v = true;
                                printf("verbose mode activated\n");                         
                                printf("________________________________________________________________________________\n");
                            }
                    }



expr    : sin '(' expr ')'                  {$$ = new_symexpr(SIN, $3, NULL, 0);}
        | cos '(' expr ')'                  {$$ = new_symexpr(COS, $3, NULL, 0);}
        | tan '(' expr ')'                  {$$ = new_symexpr(TAN, $3, NULL, 0);}
        | cotan '(' expr ')'                {$$ = new_symexpr(COTAN, $3, NULL, 0);}

        | sinh '(' expr ')'                 {
                                                $$ = new_symexpr(
                                                    DIV,
                                                    new_symexpr(
                                                        SUB,
                                                        new_symexpr(EXP, new_symexpr(E, NULL, NULL, 0), $3, 0),
                                                        new_symexpr(EXP, new_symexpr(E, NULL, NULL, 0), new_symexpr(MUL, new_symexpr(CONST, NULL, NULL, -1), copy($3), 0), 0),
                                                        0
                                                    ),
                                                    new_symexpr(CONST, NULL, NULL, 2),
                                                    0
                                                );
                                            }

        | cosh '(' expr ')'                 {
                                                $$ = new_symexpr(
                                                    DIV,
                                                    new_symexpr(
                                                        ADD,
                                                        new_symexpr(EXP, new_symexpr(E, NULL, NULL, 0), $3, 0),
                                                        new_symexpr(EXP, new_symexpr(E, NULL, NULL, 0), new_symexpr(MUL, new_symexpr(CONST, NULL, NULL, -1), copy($3), 0), 0),
                                                        0
                                                    ),
                                                    new_symexpr(CONST, NULL, NULL, 2),
                                                    0
                                                );
                                            }
        
        | tanh '(' expr ')'                 {
                                                $$ = new_symexpr(
                                                    DIV,
                                                    new_symexpr(
                                                        SUB,
                                                        new_symexpr(EXP, new_symexpr(E, NULL, NULL, 0), $3, 0),
                                                        new_symexpr(EXP, new_symexpr(E, NULL, NULL, 0), new_symexpr(MUL, new_symexpr(CONST, NULL, NULL, -1), copy($3), 0), 0),
                                                        0
                                                    ),
                                                    new_symexpr(
                                                        ADD,
                                                        new_symexpr(EXP, new_symexpr(E, NULL, NULL, 0), copy($3), 0),
                                                        new_symexpr(EXP, new_symexpr(E, NULL, NULL, 0), new_symexpr(MUL, new_symexpr(CONST, NULL, NULL, -1), copy($3), 0), 0),
                                                        0
                                                    ),
                                                    0
                                                );
                                            }

        | cotanh '(' expr ')'               {
                                                $$ = new_symexpr(
                                                    DIV,
                                                    new_symexpr(
                                                        ADD,
                                                        new_symexpr(EXP, new_symexpr(E, NULL, NULL, 0), $3, 0),
                                                        new_symexpr(EXP, new_symexpr(E, NULL, NULL, 0), new_symexpr(MUL, new_symexpr(CONST, NULL, NULL, -1), copy($3), 0), 0),
                                                        0
                                                    ),
                                                    new_symexpr(
                                                        SUB,
                                                        new_symexpr(EXP, new_symexpr(E, NULL, NULL, 0), copy($3), 0),
                                                        new_symexpr(EXP, new_symexpr(E, NULL, NULL, 0), new_symexpr(MUL, new_symexpr(CONST, NULL, NULL, -1), copy($3), 0), 0),
                                                        0
                                                    ),
                                                    0
                                                );
                                            }


        | expr '*' expr                     {$$ = new_symexpr(MUL, $1, $3, 0);}
        | '(' expr ')' '*' '(' expr ')'     {$$ = new_symexpr(MUL, $2, $6, 0);}
        | '(' expr ')' '*' expr             {$$ = new_symexpr(MUL, $2, $5, 0);}
        | expr '*' '(' expr ')'             {$$ = new_symexpr(MUL, $1, $4, 0);}

        | expr '/' expr                     {$$ = new_symexpr(DIV, $1, $3, 0);}
        | '(' expr ')' '/' '(' expr ')'     {$$ = new_symexpr(DIV, $2, $6, 0);}
        | '(' expr ')' '/' expr             {$$ = new_symexpr(DIV, $2, $5, 0);}
        | expr '/' '(' expr ')'             {$$ = new_symexpr(DIV, $1, $4, 0);}

        | expr '+' expr                     {$$ = new_symexpr(ADD, $1, $3, 0);}
        | '(' expr ')' '+' '(' expr ')'     {$$ = new_symexpr(ADD, $2, $6, 0);}
        | '(' expr ')' '+' expr             {$$ = new_symexpr(ADD, $2, $5, 0);}
        | expr '+' '(' expr ')'             {$$ = new_symexpr(ADD, $1, $4, 0);}
        
        | expr '-' expr                     {$$ = new_symexpr(SUB, $1, $3, 0);}
        | '(' expr ')' '-' '(' expr ')'     {$$ = new_symexpr(SUB, $2, $6, 0);}
        | '(' expr ')' '-' expr             {$$ = new_symexpr(SUB, $2, $5, 0);}
        | expr '-' '(' expr ')'             {$$ = new_symexpr(SUB, $1, $4, 0);}

        | log '(' expr ')'                  {$$ = new_symexpr(LOG, $3, NULL, 0);}
        | sqrt '(' expr ')'                 {$$ = new_symexpr(SQRT, $3, NULL, 0);}

        | expr '^' expr                     {$$ = new_symexpr(EXP, $1, $3, 0);}
        | '(' expr ')' '^' '(' expr ')'     {$$ = new_symexpr(EXP, $2, $6, 0);}
        | '(' expr ')' '^' expr             {$$ = new_symexpr(EXP, $2, $5, 0);}
        | expr '^' '(' expr ')'             {$$ = new_symexpr(EXP, $1, $4, 0);}

        | '-' expr                          {$$ = new_symexpr(MUL, new_symexpr(CONST, NULL, NULL, -1), $2, 0);}
        | '-' '(' expr ')'                  {$$ = new_symexpr(MUL, new_symexpr(CONST, NULL, NULL, -1), $3, 0);}

        | 'x'                               {$$ = new_symexpr(VAR, NULL, NULL, 0);}
        | 'e'                               {$$ = new_symexpr(E, NULL, NULL, 0);}
        | num                               {$$ = new_symexpr(CONST, NULL, NULL, $1);}

%%

int main()
{    
    printf("________________________________________________________________________________\n");    
    yyparse();
    return 0;
}

void yyerror(const char *s) {
  printf("Incorrect or unsupported expression! Message: %s\n", s);
  // might as well halt now:
  exit(-1);
}