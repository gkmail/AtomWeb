/******************************************************************************
 *                AtomWeb: An embedded web server                             *
 *                                                                            *
 * Copyright (c) 2015 Gong Ke                                                 *
 * All rights reserved.                                                       *
 *                                                                            *
 * Redistribution and use in source and binary forms, with or without         *
 * modification, are permitted provided that the following conditions are     *
 * met:                                                                       *
 * 1.Redistributions of source code must retain the above copyright notice,   *
 * this list of conditions and the following disclaimer.                      *
 * 2.Redistributions in binary form must reproduce the above copyright        *
 * notice, this list of conditions and the following disclaimer in the        *
 * documentation and/or other materials provided with the distribution.       *
 * 3.Neither the name of the Gong Ke; nor the names of its contributors may   *
 * be used to endorse or promote products derived from this software without  *
 * specific prior written permission.                                         *
 *                                                                            *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS    *
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  *
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR           *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,      *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,        *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR         *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF     *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING       *
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               *
 *****************************************************************************/

#ifndef _AW_TYPES_H_
#define _AW_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_config.h"

#ifdef AW_MEMWATCH
#define MEMWATCH
#define MEMWATCH_STDIO
#include "memwatch.h"
#endif

typedef void * AW_Ptr;

#ifdef AW_64BITS
typedef long long          AW_IntPtr;
typedef unsigned long long AW_UIntPtr;
#else
typedef int                AW_IntPtr;
typedef unsigned int       AW_UIntPtr;
#endif

typedef int    AW_Size;
typedef char   AW_S8;
typedef short  AW_S16;
typedef int    AW_S32;
typedef char   AW_Char;
typedef unsigned char  AW_U8;
typedef unsigned short AW_U16;
typedef unsigned int   AW_U32;

typedef AW_U8  AW_Bool;
#define AW_TRUE  1
#define AW_FALSE 0

typedef int    AW_Result;
#define AW_OK            1
#define AW_NONE          0
#define AW_FAILED       -1
#define AW_ERR_SYNTAX   -2
#define AW_ERR_NOTSUPP  -3
#define AW_ERR_NOTEXIST -4

typedef struct AW_Uri_s     AW_Uri;
typedef struct AW_Hash_s    AW_Hash;
typedef struct AW_Manager_s AW_Manager;
typedef struct AW_HashEntry_s AW_HashEntry;
typedef struct AW_Server_s  AW_Server;
typedef struct AW_Session_s AW_Session;
typedef struct AW_Object_s  AW_Object;
typedef struct AW_Class_s   AW_Class;
typedef struct AW_Map_s     AW_Map;

typedef void    (*AW_FreeFunc) (AW_Ptr ptr);
typedef AW_Bool (*AW_EqualFunc) (AW_Ptr k1, AW_Ptr k2);
typedef AW_U32  (*AW_KeyFunc) (AW_Ptr key);

#define AW_MAX(a, b) ((a)>(b)?(a):(b))
#define AW_MIN(a, b) ((a)<(b)?(a):(b))

#define AW_MACRO_BEGIN do {
#define AW_MACRO_END   } while(0)

#ifdef __cplusplus
}
#endif

#endif

