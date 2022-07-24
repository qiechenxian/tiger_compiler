//
// Created by loyx on 2020/4/18.
//

#ifndef COMPILER_LOYX_ERRORMSG_H
#define COMPILER_LOYX_ERRORMSG_H

#include "util.h"
#include "ast.h"

#define EM_LINE_CODE_BUF 510

extern unsigned EM_token_pos;
extern c_string FILE_NAME;

void EM_add_token(const char* , unsigned );
void EM_clear_buf();
char** EM_error_code();
void EM_warning(A_pos pos, const char* message, ...);
void EM_error(A_pos pos, const char* message, ...);
void EM_errorWithExitCode(int code, A_pos pos, const char* message, ...);

/// 需要制定assert退出码时，在assert前，对该全局变量赋值
extern int EM_ASSERT_CODE;

#ifdef assert
#undef assert
#define assert(exp) \
    (static_cast <bool> (exp) \
    ? void(0)       \
    : exit(EM_ASSERT_CODE))
#endif

#endif //COMPILER_LOYX_ERRORMSG_H
