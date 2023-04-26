/**
 * Copyright - xWhitey, 2023.
 * functional_config.hpp - Here you can configure the functional of our functional.hpp
 */

#ifdef FUNCTIONAL_CONFIG_HPP_RECURSE_GUARD
#error Recursive header files inclusion detected in functional_config.hpp
#else //FUNCTIONAL_CONFIG_HPP_RECURSE_GUARD

#define FUNCTIONAL_CONFIG_HPP_RECURSE_GUARD

#ifndef FUNCTIONAL_CONFIG_HPP_GUARD
#define FUNCTIONAL_CONFIG_HPP_GUARD
#pragma once

/* Uncomment those lines to make it automatically detect if you're building in Debug variant. (MSVC specific, we didn't test it with GCC/Clang)
#ifndef _DEBUG
#define FUNCTIONAL_NO_ASSERTS
#endif //_DEBUG
*/
//Define FUNCTIONAL_NO_ASSERTS to disable all assertions (not recommended when you're debugging your app)
//Default: undefined

//#define FUNCTIONAL_DEBUG_PARANOID
//Define FUNCTIONAL_DEBUG_PARANOID to make asserts literally at each line (not recommended when you're doing a fast-debug)
//Default: undefined

//#define FUNCTIONAL_FIX_FLTUSED_NOT_FOUND
//Usually needed by MSVC or Clang
//Define FUNCTIONAL_FIX_FLTUSED_NOT_FOUND if you're not using CRT or any of standard libraries in order to compile our header.
//Default: undefined

//You may adjust these defines to suit your needs.
#define FUNCTIONAL_HEAP_SIZE 2 * 1024 * 1024 * 512
//Default: 2 * 1024 * 1024 * 512
#define FUNCTIONAL_BLOCK_SIZE 4096
//Default: 4096

//#define FUNCTIONAL_NO_ALLOCATOR
//if FUNCTIONAL_NO_ALLOCATOR is defined, you must introduce your own Q_malloc and Q_free functions using FUNCTIONAL_CUSTOM_MALLOC and FUNCTIONAL_CUSTOM_FREE defines.
//Default: undefined
//#define FUNCTIONAL_CUSTOM_MALLOC malloc
//#define FUNCTIONAL_CUSTOM_FREE free
//#define FUNCTIONAL_CUSTOM_REALLOC realloc

//#define FUNCTIONAL_USE_CPP_BOOL
//Use inbuilt "bool" type instead of our Q_bool.
//Default: undefined

//#define FUNCTIONAL_DONT_DEFINE_VARARGS
//Disables definition of Q_va_list, Q_va_start, Q_va_arg, Q_va_end.
//Default: undefined

//#define FUNCTIONAL_DONT_USE_ANONYMOUS_NAMESPACE
//Don't use anonymous namespace: name it "functional" instead. (To call our functions you must add functional:: before the function name: functional::Q_sprintf)
//Default: undefined

//#define FUNCTIONAL_USE_FASTEST_STRLEN
//Use fastest strlen function, compares four bytes with zero instead of per-byte comparison
//Default: undefined

#endif //FUNCTIONAL_CONFIG_HPP_GUARD

#undef FUNCTIONAL_CONFIG_HPP_RECURSE_GUARD
#endif //FUNCTIONAL_CONFIG_HPP_RECURSE_GUARD