#pragma once

#ifdef __clang__
# define UNUSED_STRUCT_FIELD __attribute__((unused))
#else
// unsupported on gcc (it understands the syntax, but not putting it on struct
// fields, only function args)
# define UNUSED_STRUCT_FIELD
#endif

