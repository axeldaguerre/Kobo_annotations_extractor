#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include<stdint.h>

#define internal      static 
#define global        static 
#define local_persist static 

typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef S8       B8;
typedef S16      B16;
typedef S32      B32;
typedef S64      B64;
typedef float    F32;
typedef double   F64;

typedef void VoidProc(void);

#define KB(n)  (((U64)(n)) << 10)
#define MB(n)  (((U64)(n)) << 20)
#define GB(n)  (((U64)(n)) << 30)
#define TB(n)  (((U64)(n)) << 40)

#define Thousand(n)   ((n)*1000)
#define Million(n)    ((n)*1000000)
#define Billion(n)    ((n)*1000000000)

#define Stmnt(S) do{ S }while(0)

global U64 max_U64 = 0xffffffffffffffffull;
global U32 max_U32 = 0xffffffff;
global U16 max_U16 = 0xffff;
global U8  max_U8  = 0xff;

global const U32 bitmask3  = 0x00000007;
global const U32 bitmask4  = 0x0000000F;
global const U32 bitmask5  = 0x0000001F;
global const U32 bitmask6  = 0x0000003F;
global const U32 bitmask10 = 0x0000003FF;

/* TODO: Faster ?
  #define RtlEqualMemory(Destination,Source,Length) (!memcmp((Destination),(Source),(Length)))
  #define RtlMoveMemory(Destination,Source,Length) memmove((Destination),(Source),(Length))
  #define RtlCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
  #define RtlFillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))
  #define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))
*/
#define MemoryCopy(dst, src, size)    memmove((dst), (src), (size))
#define MemoryZero(s,z) memset((s),0,(z))
#define MemoryCopyStruct(d,s)  MemoryCopy((d),(s),sizeof(*(d)))

#define MAX_PATH_SIZE 260

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#define AssertAlways(Expression) if(!(Expression)) {*(int *)0 = 0;}

#define Swap(T, a, b) Stmnt(T t__ = a; a = b; b = t__;)

internal U32 u32_from_u64(U64 x);
internal U64 u64_from_high_low_u32(U32 high, U32 low);

#if COMPILER_CL || (COMPILER_CLANG && OS_WINDOWS)
# pragma section(".rdata$", read)
# define read_only __declspec(allocate(".rdata$"))
#elif (COMPILER_CLANG && OS_LINUX)
# define read_only __attribute__((section(".rodata")))
#else
// NOTE(rjf): I don't know of a useful way to do this in GCC land.
// __attribute__((section(".rodata"))) looked promising, but it introduces a
// strange warning about malformed section attributes, and it doesn't look
// like writing to that section reliably produces access violations, strangely
// enough. (It does on Clang)
# define read_only
#endif

#endif