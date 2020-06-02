//
// Created by loyx on 2020/4/18.
//

#ifndef COMPILER_LOYX_ERRORMSG_H
#define COMPILER_LOYX_ERRORMSG_H

#include "util.h"
#include "absyn.h"

#define EM_LINE_CODE_BUF 510

extern unsigned EM_token_pos;
extern string FILE_NAME;

void EM_add_token(const char* , unsigned );
void EM_clear_buf();
char** EM_error_code();
void EM_warning(A_pos pos, char* message, ...);
void EM_error(A_pos pos, char* message, ...);


#endif //COMPILER_LOYX_ERRORMSG_H
