#ifndef __LIB_DEFINE_H__
#define __LIB_DEFINE_H__

#include <assert.h>

#define __USE_LIB_MEMORY_SYSTEM

#ifdef __USE_LIB_MEMORY_SYSTEM
#define NEW(tag, ...) new(tag, ##__VA_ARGS__)
#define NEW_ARRAY(tag, ...) new(tag, ##__VA_ARGS__)
#else
#define NEW(...) new
#define NEW_ARRAY(...) new
#endif
#define SAFE_DELETE(ptr) do{ if(ptr){ delete (ptr); (ptr)=NULL; } }while(false)
#define SAFE_DELETE_ARRAY(ptr) do{ if(ptr){ delete[] (ptr); (ptr)=NULL; } }while(false)

#define ASSERT(val)	assert(val)
#define ASSERT_MES(val, mes, ...) do{ DEBUG_PRINT_VAL(!(val), mes, ##__VA_ARGS__); ASSERT(val); }while(false)

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

#ifdef _DEBUG
#define DEBUG_PRINT(mes, ...) printf(mes, ##__VA_ARGS__)
#define DEBUG_PRINT_VAL(val, mes, ...) do{ if (val) { DEBUG_PRINT(mes, ##__VA_ARGS__); } }while(false)
#define DEBUG_PAUSE() do{ DEBUG_PRINT("##########DEBUG_PAUSE##########"); getchar(); }while(false)
#else
#define DEBUG_PRINT(mes, ...) do{}while(false)
#define DEBUG_PRINT_VAL(val, mes, ...) do{}while(false)
#define DEBUG_PAUSE() do{}while(false)
#endif

#endif // __LIB_DEFINE_H__