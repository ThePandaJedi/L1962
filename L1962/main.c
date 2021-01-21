//
//  main.c
//  L1962
//
//  Created by Matthew Haahr on 12/22/20.
//

#include <stdio.h>
#include <unistd.h>

#include "SExpr.h"
#include "eval.h"


/**
    Reads a file (can be stdin) and reads each token individually and prints them
 @param fp   The file pointer for the file to be read and tokenized
 */
void readFile(FILE *fp) {
    int isInteractive = isatty(fileno(fp)); // Checks if the given file is a console (ineractive) to allow for prompting
    int EOFBool = 1; // True while hasn't seen EOF
    int n = 1; // Environment saved variables
    char str[5]; // Variable name string
    
    while (EOFBool) {
        if (isInteractive) {
            printf("> "); // Simple prompting, prints if console is being read
        }
        TRY_CATCH(e,
            {
                SExpr expr = readSExpr(fp);
                if (expr.type == END) {
                    EOFBool = 0;
                    printf("\n");
                } else {
                    SExpr evaled = eval(expr, NILObj);
                    sprintf(str, "$%d", n);
                    evalSETBang(symbolToSExpr(struniq(str)), evaled, NILObj);
                    printf("%s = ", str);
                    printSExpr(evaled);
                    printf("\n");
                    n++;
                }
            }, {
                printf("caught: %s\n", e.message);
            });
    }
}

int main(int argc, char **argv) {
    hashInit();
    SExprInit();
    evalInit();
    if (argc == 1) {     // If one arg, tokenize stdin
        TRY_CATCH(failure,
            {
                readFile(stdin);
            }, {
                fprintf(stderr, "failure on %s: %s\n", "stdin", failure.message);
            });
    } else {            // If more than one arg, args past calling arg will be files to tokenize
        for (int i = 1; i < argc; i++) {
            TRY_CATCH(failure,
                {
                    printf("%s: starting\n", argv[i]);
                    FILE *fp = fopen(argv[i], "r");
                    if (fp == NULL) {
                        fail("can't open file: %s", argv[i]);
                    }
                    TRY_FINALLY({
                        readFile(fp);
                        }, {
                            fclose(fp);
                            printf("cleaned up after %s\n", argv[i]);
                        });
                    printf("%s: finished successfully\n", argv[i]);
                }, {
                    fprintf(stderr, "failure on %s: %s\n", argv[i], failure.message);
                });
            printf("\n");
        }
    }
    
    return 0;
}

