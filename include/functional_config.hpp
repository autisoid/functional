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

//Define FUNCTIONAL_FIX_FLTUSED_NOT_FOUND if you're not using CRT or any of standard libraries in order to compile our header.
//#define FUNCTIONAL_FIX_FLTUSED_NOT_FOUND

//You may adjust these defines to suit your needs.
#define FUNCTIONAL_HEAP_SIZE 4 * 1024 * 1024
#define FUNCTIONAL_BLOCK_SIZE 4096

//#define FUNCTIONAL_NO_ALLOCATOR
//if FUNCTIONAL_NO_ALLOCATOR is defined, you must introduce your own Q_malloc and Q_free functions using FUNCTIONAL_CUSTOM_MALLOC and FUNCTIONAL_CUSTOM_FREE defines.
//#define FUNCTIONAL_CUSTOM_MALLOC malloc
//#define FUNCTIONAL_CUSTOM_FREE free

//#define FUNCTIONAL_USE_CPP_BOOL
//Use inbuilt "bool" type instead of our Q_bool.

//#define FUNCTIONAL_DONT_DEFINE_VARARGS
//Disables definition of va_list, va_start, va_arg, va_end.

//#define FUNCTIONAL_DONT_USE_ANONYMOUS_NAMESPACE
//Don't use anonymous namespace: name it "functional" instead. (To call our functions you must add functional:: before the function name: functional::Q_sprintf)

#endif //FUNCTIONAL_CONFIG_HPP_GUARD

#undef FUNCTIONAL_CONFIG_HPP_RECURSE_GUARD
#endif //FUNCTIONAL_CONFIG_HPP_RECURSE_GUARD