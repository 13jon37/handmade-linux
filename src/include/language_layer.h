/* date = November 25th 2020 6:32 am */

#ifndef LANGUAGE_LAYER_H
#define LANGUAGE_LAYER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/mman.h>

#define internal        static
#define local_persist   static
#define global_variable static

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#endif //LANGUAGE_LAYER_H
