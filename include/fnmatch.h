/* -*- Mode: C; tab-width: 2; c-basic-offset: 2 -*- */
/* vim:set softtabstop=2 shiftwidth=2: */
/* 
 * Copyright (c) 2012, Jonas Pommerening <jonas.pommerening@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _FNMATCH_H_
#define _FNMATCH_H_
#include <unistd.h>
#include <stddef.h>

#define FNMATCH_ESCAPE '\\'
#define FNMATCH_CHARS_START '['
#define FNMATCH_CHARS_END ']'
#define FNMATCH_ONE '?'
#define FNMATCH_ANY '*'
#define FNMATCH_DEEP '*'
#define FNMATCH_SEP '/'
#define FNMATCH_EOP '\0'

#define FNMATCH_EXTERN extern

typedef enum {
  FNMATCH_NOMATCH,
  FNMATCH_MATCH,
  FNMATCH_PUSH,
  FNMATCH_POP,
  FNMATCH_CONTINUE,
  FNMATCH_STOP,
  FNMATCH_ERROR
} fnmatch_state_t;

typedef enum {
  FNMATCH_OP_FIXED,
  FNMATCH_OP_CHARS,
  FNMATCH_OP_ONE,
  FNMATCH_OP_ANY,
  FNMATCH_OP_DEEP,
  FNMATCH_OP_SEP,
  FNMATCH_OP_END
} fnmatch_opcode_t;

typedef struct fnmatch_pattern_s fnmatch_pattern_t;
typedef struct fnmatch_context_s fnmatch_context_t;
typedef struct fnmatch_scanner_s fnmatch_scanner_t;
typedef struct fnmatch_match_s   fnmatch_match_t;

typedef fnmatch_state_t (*fnmatch_push_cb)( fnmatch_context_t* ctx, void* info );
typedef fnmatch_state_t (*fnmatch_pop_cb)( fnmatch_context_t* ctx, void* info );
typedef fnmatch_state_t (*fnmatch_match_cb)( fnmatch_context_t* ctx, fnmatch_match_t* match, void* info );

struct fnmatch_pattern_s {
  char*  pattern;
  char*  program;
  size_t proglen;
  size_t alloc;
  size_t mchars; /* minimum chars to match (sum of all fixed, sep, one, chars) */
  size_t groups; /* number of groups */
  size_t parts;  /* number of parts */
};

struct fnmatch_context_s {
  fnmatch_pattern_t* pattern;
  char*  buffer;
  size_t buflen;
  size_t alloc;
  
  fnmatch_state_t  state;
  fnmatch_opcode_t opcode;
  size_t match;
  size_t opptr;
  size_t offset;
  size_t mark_opptr;
  size_t mark_offset;
};

struct fnmatch_scanner_s {
  fnmatch_context_t context;
  fnmatch_push_cb   push_cb;
  fnmatch_pop_cb    pop_cb;
  fnmatch_match_cb  match_cb;
};

struct fnmatch_match_s {
  size_t argc;
  char*  argv[1];
};

FNMATCH_EXTERN void fnmatch_pattern_init( fnmatch_pattern_t* pattern );
FNMATCH_EXTERN void fnmatch_pattern_destroy( fnmatch_pattern_t* pattern );
FNMATCH_EXTERN fnmatch_state_t fnmatch_pattern_compile( fnmatch_pattern_t* pattern, const char* expr );
FNMATCH_EXTERN fnmatch_state_t fnmatch_pattern_match( fnmatch_pattern_t* pattern, const char* str );

FNMATCH_EXTERN void fnmatch_context_init( fnmatch_context_t* context, fnmatch_pattern_t* pattern );
FNMATCH_EXTERN void fnmatch_context_destroy( fnmatch_context_t* context );
FNMATCH_EXTERN fnmatch_state_t fnmatch_context_match( fnmatch_context_t* context );
FNMATCH_EXTERN void fnmatch_context_reset( fnmatch_context_t* context );
FNMATCH_EXTERN void fnmatch_context_push( fnmatch_context_t* context, const char* str );
FNMATCH_EXTERN const char * fnmatch_context_pop( fnmatch_context_t* context );

FNMATCH_EXTERN void fnmatch_scanner_init( fnmatch_scanner_t* scanner, fnmatch_pattern_t* pattern,
  fnmatch_push_cb push_cb, fnmatch_pop_cb pop_cb, fnmatch_match_cb match_cb );
FNMATCH_EXTERN void fnmatch_scanner_destroy( fnmatch_scanner_t* scanner );
FNMATCH_EXTERN fnmatch_state_t fnmatch_scanner_match( fnmatch_scanner_t* scanner, void* info );
FNMATCH_EXTERN void fnmatch_scanner_reset( fnmatch_scanner_t* scanner );

#endif