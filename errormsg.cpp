//
// Created by loyx on 2020/4/18.
//
#include "errormsg.h"
#include <stdarg.h>
#include <stdio.h>

#define EM_MSG_NUM 2

unsigned EM_token_pos = 0;
char EM_code_str[EM_LINE_CODE_BUF];
char *msg[EM_MSG_NUM];
c_string FILE_NAME;

void EM_add_token(const char* token, unsigned len){
    for(int i = 0; i<len; i++, EM_token_pos++){
        EM_code_str[EM_token_pos] = token[i];
    }
}

void EM_clear_buf(){
    EM_token_pos = 0;
}

char** EM_error_code(){
    EM_code_str[EM_token_pos] = 0;
    msg[0] = EM_code_str;
    msg[1] = EM_code_str + EM_token_pos + 1;
    for(unsigned i = 0, len = EM_token_pos; i <= len; i++){
        EM_code_str[++EM_token_pos] = ' ';
    }
    EM_code_str[EM_token_pos] = '^';
    EM_code_str[++EM_token_pos] = 0;
    return msg;
}
void EM_warning(A_pos pos, const char* message, ...){
    fprintf(stderr, "%s:%d:%d: warning: ", FILE_NAME, pos->first_line, pos->first_column);
    va_list ap;
    va_start(ap, message);
    vfprintf(stderr, message, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}

void EM_error(A_pos pos, const char* message, ...){
    fprintf(stderr, "%s:%d:%d: error: ", FILE_NAME, pos->first_line, pos->first_column);
    va_list ap;
    va_start(ap, message);
    vfprintf(stderr, message, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    exit(-1);
}

