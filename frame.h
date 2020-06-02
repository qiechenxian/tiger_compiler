//
// Created by loyx on 2020/5/7.
//

#ifndef COMPILER_LOYX_FRAME_H
#define COMPILER_LOYX_FRAME_H

#include "util.h"
#include "temp.h"

typedef struct F_frame_* F_frame;
typedef struct F_access_* F_access;

typedef struct F_accessList_ *F_accessList;
struct F_accessList_{F_access head; F_accessList tail;};

F_frame F_newFrame(Temp_label name, U_boolList formals);
Temp_label F_getName(F_frame frame);
F_accessList F_getFormals(F_frame frame);
F_access F_allocLocal(F_frame frame, bool escape);


//Temp_temp /** Frame pointer */

#endif //COMPILER_LOYX_FRAME_H
