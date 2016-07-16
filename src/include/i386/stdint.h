#ifndef __STDINT_H
#define __STDINT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef char                int8_t;
typedef short int           int16_t;
typedef long                int32_t;
typedef long long           int64_t;

typedef unsigned char       uint8_t;
typedef unsigned short int  uint16_t;
typedef unsigned long       uint32_t;
typedef unsigned long long  uint64_t;

typedef int8_t              int_least8_t;
typedef int16_t             int_least16_t;
typedef int32_t             int_least32_t;
typedef int64_t             int_least64_t;

typedef uint8_t             uint_least8_t;
typedef uint16_t            uint_least16_t;
typedef uint32_t            uint_least32_t;
typedef uint64_t            uint_least64_t;

typedef int32_t             int_fast8_t;
typedef int32_t             int_fast16_t;
typedef int32_t             int_fast32_t;
typedef int64_t             int_fast64_t;

typedef uint32_t            uint_fast8_t;
typedef uint32_t            uint_fast16_t;
typedef uint32_t            uint_fast32_t;
typedef uint64_t            uint_fast64_t;

typedef uint32_t            intptr_t;
typedef int64_t             intmax_t;
typedef uint64_t            uintmax_t;

typedef uint32_t            size_t;
typedef int32_t             ssize_t;

#define SSIZE_MAX           ((ssize_t)0x7FFFFFFF)
#define SSIZE_MIN           ((ssize_t)0x80000000)

#ifdef __cplusplus
}
#endif

#endif
