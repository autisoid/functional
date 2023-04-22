/**
 * Copyright - xWhitey, 2023.
 * functional.hpp - Various functional as like varargs, allocating mem &c.
 */

#ifdef FUNCTIONAL_HPP_RECURSE_GUARD
#error Recursive header files inclusion detected in functional.hpp
#else //FUNCTIONAL_HPP_RECURSE_GUARD

#define FUNCTIONAL_HPP_RECURSE_GUARD

#ifndef FUNCTIONAL_HPP_GUARD
#define FUNCTIONAL_HPP_GUARD
#pragma once

/**
 * TL;DR - README
 * If you're going to contribute, please follow for our naming & coding plan (read it downwards)
 
 * If you found a bug, please open an issue with such pattern:
 
 ** 1. Describe the bug 
 ** 2. Expected behavior
 ** 3. Example code to test the bug

 */

/**
 * Our naming & coding plan

 ** The main rule is to make the library maximally customizable. (Though I didn't follow this rule inb4 kekw)
 
 ** Please try to follow Hungarian Notation where possible!
 ** Private struct members should start with _m_ instead of _ like Hungarian Notation writes. Example: int _m_iApplesEaten;

 ** A macro, which can be changed in our config or is a function should be CAPS_LOCKED and start with FUNCTIONAL_ (e.g: FUNCTIONAL_HEAP_SIZE (constant), FUNCTIONAL_STRINGIZE (function))
 ** Or, you can add Q_ prefix. (I don't really care about which prefix do you use, but you SHOULD use one of them)
  
 ** We don't have classes. We use class for a typename. We have typedef struct as a class:
 ** typedef struct CMyKlass {} CMyKlass;
 
 ** Anything you got from std (e.g we have is_pointer) should be named as-is, as like you implemented std::is_pointer then you should name it is_pointer
 
 ** Avoid using any compiler-specific and OS-specific codes, especially WinAPI. We're making fully cross-platform header, aren't we?
 
 ** Insert SAL Annotations to mark parameters where possible 
 
 ** Don't forget about asserts as well, insert them where possible
  
 ** Any function parameter should start with _ and uppercase letter afterwards. Example: _Pointer, _String, _Number
 
 ** Avoid using 'typename' in template<> expression, use 'class' keyword instead. Example: typename _Ty -> class _Ty
  
 ** Any template typename should start with _ and uppercase letter afterwards. Example: class _Ty, class... _Ts, functional_unsigned_size_t _Entropy = 0
  
 ** Since I pull this commit, please avoid using reinterpret_cast. Use union_cast or indirect_cast (provided by our header). Any code I wrote before still will use C++ (or C-style) casts.
 
 ** Do not copy-paste any code directly from another library, make sure it meets our coding & naming conditions before.
 
 */

#ifdef __cplusplus

#ifndef FUNCTIONAL_DONT_INCLUDE_CONFIG
#include "functional_config.hpp"
#endif //FUNCTIONAL_DONT_INCLUDE_CONFIG

#ifdef FUNCTIONAL_DONT_INCLUDE_CONFIG
#define FUNCTIONAL_HEAP_SIZE 2 * 1024 * 1024 * 512
#define FUNCTIONAL_BLOCK_SIZE 4096
#endif //FUNCTIONAL_DONT_INCLUDE_CONFIG

#ifndef FUNCTIONAL_HEAP_SIZE
#define FUNCTIONAL_HEAP_SIZE 2 * 1024 * 1024 * 512
#endif //FUNCTIONAL_HEAP_SIZE

#ifndef FUNCTIONAL_BLOCK_SIZE
#define FUNCTIONAL_BLOCK_SIZE 4096
#endif //FUNCTIONAL_BLOCK_SIZE

#define Q_NULL reinterpret_cast<void*>(0)

inline namespace {
	constexpr const struct {
		template<class _Ty> operator _Ty* () const {
			return static_cast<_Ty*>(Q_NULL);
		}

		template<class _Ty, class _Holder> operator _Ty _Holder::* () const {
			return static_cast<_Ty*>(Q_NULL);
		}

		operator void* () const {
			return Q_NULL;
		}

	private:
		void operator&() const;
	} Q_nullptr = {};
}

#ifndef FUNCTIONAL_DONT_DEFINE_VARARGS

typedef char* Q_va_list;

#define Q_va_start(_List, lastParam) _List = (Q_va_list) &lastParam + sizeof(lastParam)
#define Q_va_arg(_List, _Type) *((_Type*)_List); _List = (Q_va_list) _List + sizeof(_Type)
#define Q_va_end(_List) _List = Q_nullptr

#endif //FUNCTIONAL_DONT_DEFINE_VARARGS

template<class T, T v> struct integral_constant {
	static constexpr T value = v;
	typedef T value_type;
	typedef integral_constant type;
	constexpr operator value_type() const noexcept { return value; }
	constexpr value_type operator()() const noexcept { return value; }
};

typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;

template<class T, class U> struct is_same : false_type {};

template<class T> struct is_same<T, T> : true_type {};

template<class T, class U> inline constexpr bool is_same_v = is_same<T, U>::value;

template<bool _Condition, class _Ty = void> struct enable_if {};

template<class _Ty> struct enable_if<true, _Ty> { typedef _Ty type; };

template<bool _Condition, class _Ty = void> using enable_if_t = typename enable_if<_Condition, _Ty>::type;

template<class T> struct is_pointer : false_type {};

template<class T> struct is_pointer<T*> : true_type {};

template<class T> struct is_pointer<T* const> : true_type {};

template<class T> struct is_pointer<T* volatile> : true_type {};

template<class T> struct is_pointer<T* const volatile> : true_type {};

template<class T> inline constexpr bool is_pointer_v = is_pointer<T>::value;

template<class _Ty> struct remove_reference { typedef _Ty type; };
template<class _Ty> struct remove_reference<_Ty&> { typedef _Ty type; };
template<class _Ty> struct remove_reference<_Ty&&> { typedef _Ty type; }; 

template<class _Ty> using remove_reference_t = typename remove_reference<_Ty>::type;

#ifdef FUNCTIONAL_USE_CPP_BOOL
typedef bool Q_bool;
#define Q_FALSE false
#define Q_TRUE true
#else //FUNCTIONAL_USE_CPP_BOOL
typedef enum : unsigned char {
	Q_FALSE = 0x0,
	Q_TRUE = 0x1
} Q_bool;
#endif //FUNCTIONAL_USE_CPP_BOOL

template<bool _Condition, class _Ty = void, class _Ty2 = void> struct two_enable_if {};

template<class _Ty, class _Ty2> struct two_enable_if<true, _Ty, _Ty2> { typedef _Ty type; };
template<class _Ty, class _Ty2> struct two_enable_if<false, _Ty, _Ty2> { typedef _Ty2 type; };

template<bool _Condition, class _Ty, class _Ty2> using two_enable_if_t = typename two_enable_if<_Condition, _Ty, _Ty2>::type;

template<typename _Tester = two_enable_if_t<sizeof(void*) == 4, int, long long>> struct CPointerSizeDetector {
	typedef _Tester m_tType;
};

//We're getting rid outta int on 32 bit platforms and long long on 64 bit platforms, so typedef the right underlying type to suit our needs. - xWhitey
typedef CPointerSizeDetector<two_enable_if_t<sizeof(void*) == 4, int, long long>>::m_tType functional_size_t;
typedef CPointerSizeDetector<two_enable_if_t<sizeof(void*) == 4, unsigned int, unsigned long long>>::m_tType functional_unsigned_size_t;
typedef functional_unsigned_size_t functional_uintptr_t;
typedef functional_uintptr_t functional_ptrdiff_t;

//Simple constexpr bool. Though we don't use it ourselves. - xWhitey
template<bool _Condition> using constexpr_bool = integral_constant<bool, _Condition>;
/* Usage:
constexpr constexpr_bool<true == false> someConstexprBool;
*/

template<class _Ty> struct is_array : false_type {};
template<class _Ty> struct is_array<_Ty[]> : true_type {};
template<class _Ty, functional_size_t _Size> struct is_array<_Ty[_Size]> : true_type {};

template<class _Ty> struct remove_extent { typedef _Ty type; };
template<class _Ty> struct remove_extent<_Ty[]> { typedef _Ty type; };
template<class _Ty, functional_size_t _Size> struct remove_extent<_Ty[_Size]> { typedef _Ty type; };

template<class _Ty> struct typeof { typedef _Ty type; };

template<class _Ty> struct add_pointer {
	typedef typeof<typename remove_reference<_Ty>::type*> type;
};

template<class _Ty> struct remove_const { typedef _Ty type; };
template<class _Ty> struct remove_const<const _Ty> { typedef _Ty type; };

template<class _Ty> struct remove_volatile { typedef _Ty type; };
template<class _Ty> struct remove_volatile<volatile _Ty> { typedef _Ty type; };

template<class _Ty> struct remove_cv { typedef _Ty type; };
template<class _Ty> struct remove_cv<const _Ty> { typedef _Ty type; };
template<class _Ty> struct remove_cv<volatile _Ty> { typedef _Ty type; };
template<class _Ty> struct remove_cv<const volatile _Ty> { typedef _Ty type; };

template<class _Ty> struct is_const : false_type {};
template<class _Ty> struct is_const<const _Ty> : true_type {};

template<class _Ty> struct is_reference : false_type {};
template<class _Ty> struct is_reference<_Ty&> : true_type {};
template<class _Ty> struct is_reference<_Ty&&> : true_type {};

template<class _Ty> struct is_function : integral_constant<bool, !is_const<const _Ty>::value && !is_reference<_Ty>::value> {};

template<class _Ty> struct decay {
private:
	typedef typename remove_reference<_Ty>::type U;
public:
	typedef typename two_enable_if_t<is_array<U>::value, typename add_pointer<typename remove_extent<U>::type>::type, two_enable_if_t<is_function<U>::value, typename add_pointer<U>::type, typename remove_cv<U>::type>> type;
};

template<class _Ty> using decay_t = typename decay<_Ty>::type;

template <class> inline constexpr Q_bool is_lvalue_reference_v = Q_FALSE;

template <class _Ty> inline constexpr Q_bool is_lvalue_reference_v<_Ty&> = Q_TRUE;

template <class _Ty> constexpr _Ty&& forward(remove_reference_t<_Ty>& _Arg) {
	return static_cast<_Ty&&>(_Arg);
}

template <class _Ty> constexpr _Ty&& forward(remove_reference_t<_Ty>&& _Arg) {
	static_assert(!is_lvalue_reference_v<_Ty>, "bad forward call");
	return static_cast<_Ty&&>(_Arg);
}

template <class _Ty> constexpr remove_reference_t<_Ty>&& move(_Ty&& _Arg) {
	return static_cast<remove_reference_t<_Ty>&&>(_Arg);
}

template<class _Callee, class... _Ts> auto Q_bind(_Callee(*_Function)(_Ts... _Args)) {
	return ([&](_Ts... _Placeholders) {
		return _Function(_Placeholders...);
	});
}

#define FUNCTIONAL_DIRECT_ALIAS(_Original, _Shadow) auto _Shadow = _Original;
#define FUNCTIONAL_STRONG_ALIAS(_Original, _Shadow) auto _Shadow = Q_bind(_Original);

#ifndef _CRT_HYBRIDPATCHABLE
#define _CRT_HYBRIDPATCHABLE
#endif //_CRT_HYBRIDPATCHABLE

/* Fake SAL */

#ifndef FUNCTIONAL_FAKE_SAL_DEFINED
#define FUNCTIONAL_FAKE_SAL_DEFINED
#ifdef _When_
#undef _When_
#endif
#define _When_(c,a)
#ifdef _At_
#undef _At_
#endif
#define _At_(t,a)
#ifdef _At_buffer_
#undef _At_buffer_
#endif
#define _At_buffer_(t,i,c,a)
#ifdef _Group_
#undef _Group_
#endif
#define _Group_(a)
#ifdef _Pre_
#undef _Pre_
#endif
#define _Pre_
#ifdef _Post_
#undef _Post_
#endif
#define _Post_
#ifdef _Deref_
#undef _Deref_
#endif
#define _Deref_
#ifdef _Null_
#undef _Null_
#endif
#define _Null_
#ifdef _Notnull_
#undef _Notnull_
#endif
#define _Notnull_
#ifdef _Maybenull_
#undef _Maybenull_
#endif
#define _Maybenull_
#ifdef _Const_
#undef _Const_
#endif
#define _Const_
#ifdef _Check_return_
#undef _Check_return_
#endif
#define _Check_return_
#ifdef _Must_inspect_result_
#undef _Must_inspect_result_
#endif
#define _Must_inspect_result_
#ifdef _Pre_satisfies_
#undef _Pre_satisfies_
#endif
#define _Pre_satisfies_(e)
#ifdef _Post_satisfies_
#undef _Post_satisfies_
#endif
#define _Post_satisfies_(e)
#ifdef _Writable_elements_
#undef _Writable_elements_
#endif
#define _Writable_elements_(s)
#ifdef _Writable_bytes_
#undef _Writable_bytes_
#endif
#define _Writable_bytes_(s)
#ifdef _Readable_elements_
#undef _Readable_elements_
#endif
#define _Readable_elements_(s)
#ifdef _Readable_bytes_
#undef _Readable_bytes_
#endif
#define _Readable_bytes_(s)
#ifdef _Null_terminated_
#undef _Null_terminated_
#endif
#define _Null_terminated_
#ifdef _NullNull_terminated_
#undef _NullNull_terminated_
#endif
#define _NullNull_terminated_
#ifdef _Valid_
#undef _Valid_
#endif
#define _Valid_
#ifdef _Notvalid_
#undef _Notvalid_
#endif
#define _Notvalid_
#ifdef _Success_
#undef _Success_
#endif
#define _Success_(c)
#ifdef _Return_type_success_
#undef _Return_type_success_
#endif
#define _Return_type_success_(c)
#ifdef _On_failure_
#undef _On_failure_
#endif
#define _On_failure_(a)
#ifdef _Always_
#undef _Always_
#endif
#define _Always_(a)
#ifdef _Use_decl_annotations_
#undef _Use_decl_annotations_
#endif
#define _Use_decl_annotations_
#ifdef _Pre_defensive_
#undef _Pre_defensive_
#endif
#define _Pre_defensive_
#ifdef _Post_defensive_
#undef _Post_defensive_
#endif
#define _Post_defensive_
#ifdef _Pre_unknown_
#undef _Pre_unknown_
#endif
#define _Pre_unknown_
#ifdef _Acquires_lock_
#undef _Acquires_lock_
#endif
#define _Acquires_lock_(e)
#ifdef _Releases_lock_
#undef _Releases_lock_
#endif
#define _Releases_lock_(e)
#ifdef _Requires_lock_held_
#undef _Requires_lock_held_
#endif
#define _Requires_lock_held_(e)
#ifdef _Requires_lock_not_held_
#undef _Requires_lock_not_held_
#endif
#define _Requires_lock_not_held_(e)
#ifdef _Requires_no_locks_held_
#undef _Requires_no_locks_held_
#endif
#define _Requires_no_locks_held_
#ifdef _Guarded_by_
#undef _Guarded_by_
#endif
#define _Guarded_by_(e)
#ifdef _Write_guarded_by_
#undef _Write_guarded_by_
#endif
#define _Write_guarded_by_(e)
#ifdef _Interlocked_
#undef _Interlocked_
#endif
#define _Interlocked_
#ifdef _Post_same_lock_
#undef _Post_same_lock_
#endif
#define _Post_same_lock_(e1,e2)
#ifdef _Benign_race_begin_
#undef _Benign_race_begin_
#endif
#define _Benign_race_begin_
#ifdef _Benign_race_end_
#undef _Benign_race_end_
#endif
#define _Benign_race_end_
#ifdef _No_competing_thread_
#undef _No_competing_thread_
#endif
#define _No_competing_thread_
#ifdef _No_competing_thread_begin_
#undef _No_competing_thread_begin_
#endif
#define _No_competing_thread_begin_
#ifdef _No_competing_thread_end_
#undef _No_competing_thread_end_
#endif
#define _No_competing_thread_end_
#ifdef _Acquires_shared_lock_
#undef _Acquires_shared_lock_
#endif
#define _Acquires_shared_lock_(e)
#ifdef _Releases_shared_lock_
#undef _Releases_shared_lock_
#endif
#define _Releases_shared_lock_(e)
#ifdef _Requires_shared_lock_held_
#undef _Requires_shared_lock_held_
#endif
#define _Requires_shared_lock_held_(e)
#ifdef _Acquires_exclusive_lock_
#undef _Acquires_exclusive_lock_
#endif
#define _Acquires_exclusive_lock_(e)
#ifdef _Releases_exclusive_lock_
#undef _Releases_exclusive_lock_
#endif
#define _Releases_exclusive_lock_(e)
#ifdef _Requires_exclusive_lock_held_
#undef _Requires_exclusive_lock_held_
#endif
#define _Requires_exclusive_lock_held_(e)
#ifdef _Has_lock_kind_
#undef _Has_lock_kind_
#endif
#define _Has_lock_kind_(n)
#ifdef _Create_lock_level_
#undef _Create_lock_level_
#endif
#define _Create_lock_level_(n)
#ifdef _Has_lock_level_
#undef _Has_lock_level_
#endif
#define _Has_lock_level_(n)
#ifdef _Lock_level_order_
#undef _Lock_level_order_
#endif
#define _Lock_level_order_(n1,n2)
#ifdef _Analysis_assume_lock_acquired_
#undef _Analysis_assume_lock_acquired_
#endif
#define _Analysis_assume_lock_acquired_(e)
#ifdef _Analysis_assume_lock_released_
#undef _Analysis_assume_lock_released_
#endif
#define _Analysis_assume_lock_released_(e)
#ifdef _Analysis_assume_lock_held_
#undef _Analysis_assume_lock_held_
#endif
#define _Analysis_assume_lock_held_(e)
#ifdef _Analysis_assume_lock_not_held_
#undef _Analysis_assume_lock_not_held_
#endif
#define _Analysis_assume_lock_not_held_(e)
#ifdef _Analysis_assume_same_lock_
#undef _Analysis_assume_same_lock_
#endif
#define _Analysis_assume_same_lock_(e)
#ifdef _In_
#undef _In_
#endif
#define _In_
#ifdef _Out_
#undef _Out_
#endif
#define _Out_
#ifdef _Inout_
#undef _Inout_
#endif
#define _Inout_
#ifdef _In_z_
#undef _In_z_
#endif
#define _In_z_
#ifdef _Inout_z_
#undef _Inout_z_
#endif
#define _Inout_z_
#ifdef _In_reads_
#undef _In_reads_
#endif
#define _In_reads_(s)
#ifdef _In_reads_bytes_
#undef _In_reads_bytes_
#endif
#define _In_reads_bytes_(s)
#ifdef _In_reads_z_
#undef _In_reads_z_
#endif
#define _In_reads_z_(s)
#ifdef _In_reads_or_z_
#undef _In_reads_or_z_
#endif
#define _In_reads_or_z_(s)
#ifdef _Out_writes_
#undef _Out_writes_
#endif
#define _Out_writes_(s)
#ifdef _Out_writes_bytes_
#undef _Out_writes_bytes_
#endif
#define _Out_writes_bytes_(s)
#ifdef _Out_writes_z_
#undef _Out_writes_z_
#endif
#define _Out_writes_z_(s)
#ifdef _Inout_updates_
#undef _Inout_updates_
#endif
#define _Inout_updates_(s)
#ifdef _Inout_updates_bytes_
#undef _Inout_updates_bytes_
#endif
#define _Inout_updates_bytes_(s)
#ifdef _Inout_updates_z_
#undef _Inout_updates_z_
#endif
#define _Inout_updates_z_(s)
#ifdef _Out_writes_to_
#undef _Out_writes_to_
#endif
#define _Out_writes_to_(s,c)
#ifdef _Out_writes_bytes_to_
#undef _Out_writes_bytes_to_
#endif
#define _Out_writes_bytes_to_(s,c)
#ifdef _Out_writes_all_
#undef _Out_writes_all_
#endif
#define _Out_writes_all_(s)
#ifdef _Out_writes_bytes_all_
#undef _Out_writes_bytes_all_
#endif
#define _Out_writes_bytes_all_(s)
#ifdef _Inout_updates_to_
#undef _Inout_updates_to_
#endif
#define _Inout_updates_to_(s,c)
#ifdef _Inout_updates_bytes_to_
#undef _Inout_updates_bytes_to_
#endif
#define _Inout_updates_bytes_to_(s,c)
#ifdef _Inout_updates_all_
#undef _Inout_updates_all_
#endif
#define _Inout_updates_all_(s)
#ifdef _Inout_updates_bytes_all_
#undef _Inout_updates_bytes_all_
#endif
#define _Inout_updates_bytes_all_(s)
#ifdef _In_reads_to_ptr_
#undef _In_reads_to_ptr_
#endif
#define _In_reads_to_ptr_(p)
#ifdef _In_reads_to_ptr_z_
#undef _In_reads_to_ptr_z_
#endif
#define _In_reads_to_ptr_z_(p)
#ifdef _Out_writes_to_ptr_
#undef _Out_writes_to_ptr_
#endif
#define _Out_writes_to_ptr_(p)
#ifdef _Out_writes_to_ptr_z_
#undef _Out_writes_to_ptr_z_
#endif
#define _Out_writes_to_ptr_z_(p)
#ifdef _In_opt_
#undef _In_opt_
#endif
#define _In_opt_
#ifdef _Out_opt_
#undef _Out_opt_
#endif
#define _Out_opt_
#ifdef _Inout_opt_
#undef _Inout_opt_
#endif
#define _Inout_opt_
#ifdef _In_opt_z_
#undef _In_opt_z_
#endif
#define _In_opt_z_
#ifdef _Inout_opt_z_
#undef _Inout_opt_z_
#endif
#define _Inout_opt_z_
#ifdef _In_reads_opt_
#undef _In_reads_opt_
#endif
#define _In_reads_opt_(s)
#ifdef _In_reads_opt_z_
#undef _In_reads_opt_z_
#endif
#define _In_reads_opt_z_(s)
#ifdef _In_reads_bytes_opt_
#undef _In_reads_bytes_opt_
#endif
#define _In_reads_bytes_opt_(s)
#ifdef _Out_writes_opt_
#undef _Out_writes_opt_
#endif
#define _Out_writes_opt_(s)
#ifdef _Out_writes_bytes_opt_
#undef _Out_writes_bytes_opt_
#endif
#define _Out_writes_bytes_opt_(s)
#ifdef _Out_writes_opt_z_
#undef _Out_writes_opt_z_
#endif
#define _Out_writes_opt_z_(s)
#ifdef _Inout_updates_opt_
#undef _Inout_updates_opt_
#endif
#define _Inout_updates_opt_(s)
#ifdef _Inout_updates_bytes_opt_
#undef _Inout_updates_bytes_opt_
#endif
#define _Inout_updates_bytes_opt_(s)
#ifdef _Inout_updates_opt_z_
#undef _Inout_updates_opt_z_
#endif
#define _Inout_updates_opt_z_(s)
#ifdef _Out_writes_to_opt_
#undef _Out_writes_to_opt_
#endif
#define _Out_writes_to_opt_(s,c)
#ifdef _Out_writes_bytes_to_opt_
#undef _Out_writes_bytes_to_opt_
#endif
#define _Out_writes_bytes_to_opt_(s,c)
#ifdef _Out_writes_all_opt_
#undef _Out_writes_all_opt_
#endif
#define _Out_writes_all_opt_(s)
#ifdef _Out_writes_bytes_all_opt_
#undef _Out_writes_bytes_all_opt_
#endif
#define _Out_writes_bytes_all_opt_(s)
#ifdef _Inout_updates_to_opt_
#undef _Inout_updates_to_opt_
#endif
#define _Inout_updates_to_opt_(s,c)
#ifdef _Inout_updates_bytes_to_opt_
#undef _Inout_updates_bytes_to_opt_
#endif
#define _Inout_updates_bytes_to_opt_(s,c)
#ifdef _Inout_updates_all_opt_
#undef _Inout_updates_all_opt_
#endif
#define _Inout_updates_all_opt_(s)
#ifdef _Inout_updates_bytes_all_opt_
#undef _Inout_updates_bytes_all_opt_
#endif
#define _Inout_updates_bytes_all_opt_(s)
#ifdef _In_reads_to_ptr_opt_
#undef _In_reads_to_ptr_opt_
#endif
#define _In_reads_to_ptr_opt_(p)
#ifdef _In_reads_to_ptr_opt_z_
#undef _In_reads_to_ptr_opt_z_
#endif
#define _In_reads_to_ptr_opt_z_(p)
#ifdef _Out_writes_to_ptr_opt_
#undef _Out_writes_to_ptr_opt_
#endif
#define _Out_writes_to_ptr_opt_(p)
#ifdef _Out_writes_to_ptr_opt_z_
#undef _Out_writes_to_ptr_opt_z_
#endif
#define _Out_writes_to_ptr_opt_z_(p)
#ifdef _Outptr_
#undef _Outptr_
#endif
#define _Outptr_
#ifdef _Outptr_opt_
#undef _Outptr_opt_
#endif
#define _Outptr_opt_
#ifdef _Outptr_result_maybenull_
#undef _Outptr_result_maybenull_
#endif
#define _Outptr_result_maybenull_
#ifdef _Outptr_opt_result_maybenull_
#undef _Outptr_opt_result_maybenull_
#endif
#define _Outptr_opt_result_maybenull_
#ifdef _Outptr_result_z_
#undef _Outptr_result_z_
#endif
#define _Outptr_result_z_
#ifdef _Outptr_opt_result_z_
#undef _Outptr_opt_result_z_
#endif
#define _Outptr_opt_result_z_
#ifdef _Outptr_result_maybenull_z_
#undef _Outptr_result_maybenull_z_
#endif
#define _Outptr_result_maybenull_z_
#ifdef _Outptr_opt_result_maybenull_z_
#undef _Outptr_opt_result_maybenull_z_
#endif
#define _Outptr_opt_result_maybenull_z_
#ifdef _COM_Outptr_
#undef _COM_Outptr_
#endif
#define _COM_Outptr_
#ifdef _COM_Outptr_opt_
#undef _COM_Outptr_opt_
#endif
#define _COM_Outptr_opt_
#ifdef _COM_Outptr_result_maybenull_
#undef _COM_Outptr_result_maybenull_
#endif
#define _COM_Outptr_result_maybenull_
#ifdef _COM_Outptr_opt_result_maybenull_
#undef _COM_Outptr_opt_result_maybenull_
#endif
#define _COM_Outptr_opt_result_maybenull_
#ifdef _Outptr_result_buffer_
#undef _Outptr_result_buffer_
#endif
#define _Outptr_result_buffer_(s)
#ifdef _Outptr_result_buffer_maybenull_
#undef _Outptr_result_buffer_maybenull_
#endif
#define _Outptr_result_buffer_maybenull_(s)
#ifdef _Outptr_result_bytebuffer_
#undef _Outptr_result_bytebuffer_
#endif
#define _Outptr_result_bytebuffer_(s)
#ifdef _Outptr_result_bytebuffer_maybenull_
#undef _Outptr_result_bytebuffer_maybenull_
#endif
#define _Outptr_result_bytebuffer_maybenull_(s)
#ifdef _Outptr_opt_result_buffer_
#undef _Outptr_opt_result_buffer_
#endif
#define _Outptr_opt_result_buffer_(s)
#ifdef _Outptr_opt_result_buffer_maybenull_
#undef _Outptr_opt_result_buffer_maybenull_
#endif
#define _Outptr_opt_result_buffer_maybenull_(s)
#ifdef _Outptr_opt_result_bytebuffer_
#undef _Outptr_opt_result_bytebuffer_
#endif
#define _Outptr_opt_result_bytebuffer_(s)
#ifdef _Outptr_opt_result_bytebuffer_maybenull_
#undef _Outptr_opt_result_bytebuffer_maybenull_
#endif
#define _Outptr_opt_result_bytebuffer_maybenull_(s)
#ifdef _Outptr_result_buffer_to_
#undef _Outptr_result_buffer_to_
#endif
#define _Outptr_result_buffer_to_(s,c)
#ifdef _Outptr_result_bytebuffer_to_
#undef _Outptr_result_bytebuffer_to_
#endif
#define _Outptr_result_bytebuffer_to_(s,c)
#ifdef _Outptr_opt_result_buffer_to_
#undef _Outptr_opt_result_buffer_to_
#endif
#define _Outptr_opt_result_buffer_to_(s,c)
#ifdef _Outptr_opt_result_bytebuffer_to_
#undef _Outptr_opt_result_bytebuffer_to_
#endif
#define _Outptr_opt_result_bytebuffer_to_(s,c)
#ifdef _Ret_
#undef _Ret_
#endif
#define _Ret_
#ifdef _Ret_valid_
#undef _Ret_valid_
#endif
#define _Ret_valid_
#ifdef _Ret_z_
#undef _Ret_z_
#endif
#define _Ret_z_
#ifdef _Ret_writes_
#undef _Ret_writes_
#endif
#define _Ret_writes_(s)
#ifdef _Ret_writes_bytes_
#undef _Ret_writes_bytes_
#endif
#define _Ret_writes_bytes_(s)
#ifdef _Ret_writes_z_
#undef _Ret_writes_z_
#endif
#define _Ret_writes_z_(s)
#ifdef _Ret_writes_to_
#undef _Ret_writes_to_
#endif
#define _Ret_writes_to_(s,c)
#ifdef _Ret_writes_bytes_to_
#undef _Ret_writes_bytes_to_
#endif
#define _Ret_writes_bytes_to_(s,c)
#ifdef _Ret_writes_to_ptr_
#undef _Ret_writes_to_ptr_
#endif
#define _Ret_writes_to_ptr_(p)
#ifdef _Ret_writes_to_ptr_z_
#undef _Ret_writes_to_ptr_z_
#endif
#define _Ret_writes_to_ptr_z_(p)
#ifdef _Ret_writes_maybenull_
#undef _Ret_writes_maybenull_
#endif
#define _Ret_writes_maybenull_(s)
#ifdef _Ret_writes_bytes_maybenull_
#undef _Ret_writes_bytes_maybenull_
#endif
#define _Ret_writes_bytes_maybenull_(s)
#ifdef _Ret_writes_to_maybenull_
#undef _Ret_writes_to_maybenull_
#endif
#define _Ret_writes_to_maybenull_(s,c)
#ifdef _Ret_writes_bytes_to_maybenull_
#undef _Ret_writes_bytes_to_maybenull_
#endif
#define _Ret_writes_bytes_to_maybenull_(s,c)
#ifdef _Ret_writes_maybenull_z_
#undef _Ret_writes_maybenull_z_
#endif
#define _Ret_writes_maybenull_z_(s)
#ifdef _Ret_null_
#undef _Ret_null_
#endif
#define _Ret_null_
#ifdef _Ret_notnull_
#undef _Ret_notnull_
#endif
#define _Ret_notnull_
#ifdef _Ret_maybenull_
#undef _Ret_maybenull_
#endif
#define _Ret_maybenull_
#ifdef _Ret_maybenull_z_
#undef _Ret_maybenull_z_
#endif
#define _Ret_maybenull_z_
#ifdef _Field_size_
#undef _Field_size_
#endif
#define _Field_size_(s)
#ifdef _Field_size_opt_
#undef _Field_size_opt_
#endif
#define _Field_size_opt_(s)
#ifdef _Field_size_bytes_
#undef _Field_size_bytes_
#endif
#define _Field_size_bytes_(s)
#ifdef _Field_size_bytes_opt_
#undef _Field_size_bytes_opt_
#endif
#define _Field_size_bytes_opt_(s)
#ifdef _Field_size_part_
#undef _Field_size_part_
#endif
#define _Field_size_part_(s,c)
#ifdef _Field_size_part_opt_
#undef _Field_size_part_opt_
#endif
#define _Field_size_part_opt_(s,c)
#ifdef _Field_size_bytes_part_
#undef _Field_size_bytes_part_
#endif
#define _Field_size_bytes_part_(s,c)
#ifdef _Field_size_bytes_part_opt_
#undef _Field_size_bytes_part_opt_
#endif
#define _Field_size_bytes_part_opt_(s,c)
#ifdef _Field_size_full_
#undef _Field_size_full_
#endif
#define _Field_size_full_(s)
#ifdef _Field_size_full_opt_
#undef _Field_size_full_opt_
#endif
#define _Field_size_full_opt_(s)
#ifdef _Field_size_bytes_full_
#undef _Field_size_bytes_full_
#endif
#define _Field_size_bytes_full_(s)
#ifdef _Field_size_bytes_full_opt_
#undef _Field_size_bytes_full_opt_
#endif
#define _Field_size_bytes_full_opt_(s)
#ifdef _Printf_format_string_
#undef _Printf_format_string_
#endif
#define _Printf_format_string_
#ifdef _Scanf_format_string_
#undef _Scanf_format_string_
#endif
#define _Scanf_format_string_
#ifdef _Scanf_s_format_string_
#undef _Scanf_s_format_string_
#endif
#define _Scanf_s_format_string_
#ifdef _Printf_format_string_params_
#undef _Printf_format_string_params_
#endif
#define _Printf_format_string_params_(x)
#ifdef _Scanf_format_string_params_
#undef _Scanf_format_string_params_
#endif
#define _Scanf_format_string_params_(x)
#ifdef _Scanf_s_format_string_params_
#undef _Scanf_s_format_string_params_
#endif
#define _Scanf_s_format_string_params_(x)
#ifdef _In_range_
#undef _In_range_
#endif
#define _In_range_(l,h)
#ifdef _Out_range_
#undef _Out_range_
#endif
#define _Out_range_(l,h)
#ifdef _Ret_range_
#undef _Ret_range_
#endif
#define _Ret_range_(l,h)
#ifdef _Deref_in_range_
#undef _Deref_in_range_
#endif
#define _Deref_in_range_(l,h)
#ifdef _Deref_out_range_
#undef _Deref_out_range_
#endif
#define _Deref_out_range_(l,h)
#ifdef _Deref_inout_range_
#undef _Deref_inout_range_
#endif
#define _Deref_inout_range_(l,h)
#ifdef _Field_range_
#undef _Field_range_
#endif
#define _Field_range_(l,h)
#ifdef _Pre_equal_to_
#undef _Pre_equal_to_
#endif
#define _Pre_equal_to_(e)
#ifdef _Post_equal_to_
#undef _Post_equal_to_
#endif
#define _Post_equal_to_(e)
#ifdef _Struct_size_bytes_
#undef _Struct_size_bytes_
#endif
#define _Struct_size_bytes_(s)
#ifdef _Analysis_assume_
#undef _Analysis_assume_
#endif
#define _Analysis_assume_
#ifdef _Analysis_assume_nullterminated_
#undef _Analysis_assume_nullterminated_
#endif
#define _Analysis_assume_nullterminated_(s)
#ifdef _Analysis_mode_
#undef _Analysis_mode_
#endif
#define _Analysis_mode_(m)
#ifdef _Analysis_noreturn_
#undef _Analysis_noreturn_
#endif
#define _Analysis_noreturn_
#ifdef _Raises_SEH_exception_
#undef _Raises_SEH_exception_
#endif
#define _Raises_SEH_exception_
#ifdef _Maybe_raises_SEH_exception_
#undef _Maybe_raises_SEH_exception_
#endif
#define _Maybe_raises_SEH_exception_
#ifdef _Function_class_
#undef _Function_class_
#endif
#define _Function_class_(n)
#ifdef _Literal_
#undef _Literal_
#endif
#define _Literal_
#ifdef _Notliteral_
#undef _Notliteral_
#endif
#define _Notliteral_
#ifdef _Enum_is_bitflag_
#undef _Enum_is_bitflag_
#endif
#define _Enum_is_bitflag_
#ifdef _Strict_type_match_
#undef _Strict_type_match_
#endif
#define _Strict_type_match_
#ifdef _Points_to_data_
#undef _Points_to_data_
#endif
#define _Points_to_data_
#ifdef _Interlocked_operand_
#undef _Interlocked_operand_
#endif
#define _Interlocked_operand_
#ifdef _Outref_
#undef _Outref_
#endif
#define _Outref_
#ifdef _Outref_result_maybenull_
#undef _Outref_result_maybenull_
#endif
#define _Outref_result_maybenull_
#ifdef _Outref_result_buffer_
#undef _Outref_result_buffer_
#endif
#define _Outref_result_buffer_(s)
#ifdef _Outref_result_bytebuffer_
#undef _Outref_result_bytebuffer_
#endif
#define _Outref_result_bytebuffer_(s)
#ifdef _Outref_result_buffer_to_
#undef _Outref_result_buffer_to_
#endif
#define _Outref_result_buffer_to_(s,c)
#ifdef _Outref_result_bytebuffer_to_
#undef _Outref_result_bytebuffer_to_
#endif
#define _Outref_result_bytebuffer_to_(s,c)
#ifdef _Outref_result_buffer_all_
#undef _Outref_result_buffer_all_
#endif
#define _Outref_result_buffer_all_(s)
#ifdef _Outref_result_bytebuffer_all_
#undef _Outref_result_bytebuffer_all_
#endif
#define _Outref_result_bytebuffer_all_(s)
#ifdef _Outref_result_buffer_maybenull_
#undef _Outref_result_buffer_maybenull_
#endif
#define _Outref_result_buffer_maybenull_(s)
#ifdef _Outref_result_bytebuffer_maybenull_
#undef _Outref_result_bytebuffer_maybenull_
#endif
#define _Outref_result_bytebuffer_maybenull_(s)
#ifdef _Outref_result_buffer_to_maybenull_
#undef _Outref_result_buffer_to_maybenull_
#endif
#define _Outref_result_buffer_to_maybenull_(s,c)
#ifdef _Outref_result_bytebuffer_to_maybenull_
#undef _Outref_result_bytebuffer_to_maybenull_
#endif
#define _Outref_result_bytebuffer_to_maybenull_(s,c)
#ifdef _Outref_result_buffer_all_maybenull_
#undef _Outref_result_buffer_all_maybenull_
#endif
#define _Outref_result_buffer_all_maybenull_(s)
#ifdef _Outref_result_bytebuffer_all_maybenull_
#undef _Outref_result_bytebuffer_all_maybenull_
#endif
#define _Outref_result_bytebuffer_all_maybenull_(s)
#ifdef _In_defensive_
#undef _In_defensive_
#endif
#define _In_defensive_(a)
#ifdef _Out_defensive_
#undef _Out_defensive_
#endif
#define _Out_defensive_(a)
#ifdef _Inout_defensive_
#undef _Inout_defensive_
#endif
#define _Inout_defensive_(a)
#ifdef _Outptr_result_nullonfailure_
#undef _Outptr_result_nullonfailure_
#endif
#define _Outptr_result_nullonfailure_
#ifdef _Outptr_opt_result_nullonfailure_
#undef _Outptr_opt_result_nullonfailure_
#endif
#define _Outptr_opt_result_nullonfailure_
#ifdef _Outref_result_nullonfailure_
#undef _Outref_result_nullonfailure_
#endif
#define _Outref_result_nullonfailure_
#ifdef _Result_nullonfailure_
#undef _Result_nullonfailure_
#endif
#define _Result_nullonfailure_
#ifdef _Result_zeroonfailure_
#undef _Result_zeroonfailure_
#endif
#define _Result_zeroonfailure_
#ifdef _Acquires_nonreentrant_lock_
#undef _Acquires_nonreentrant_lock_
#endif
#define _Acquires_nonreentrant_lock_(e)
#ifdef _Releases_nonreentrant_lock_
#undef _Releases_nonreentrant_lock_
#endif
#define _Releases_nonreentrant_lock_(e)
#ifdef _Function_ignore_lock_checking_
#undef _Function_ignore_lock_checking_
#endif
#define _Function_ignore_lock_checking_(e)
#ifdef _Analysis_suppress_lock_checking_
#undef _Analysis_suppress_lock_checking_
#endif
#define _Analysis_suppress_lock_checking_(e)
#undef _Reserved_
#define _Reserved_           _Pre_equal_to_(0) _Pre_ _Null_
#undef _Pre_z_
#define _Pre_z_              _Pre_ _Null_terminated_
#undef _Post_z_
#define _Post_z_             _Post_ _Null_terminated_
#undef _Prepost_z_
#define _Prepost_z_          _Pre_z_ _Post_z_
#undef _Pre_null_
#define _Pre_null_           _Pre_ _Null_
#undef _Pre_maybenull_
#define _Pre_maybenull_      _Pre_ _Maybenull_
#undef _Pre_notnull_
#define _Pre_notnull_        _Pre_ _Notnull_
#undef _Pre_valid_
#define _Pre_valid_          _Pre_notnull_ _Pre_ _Valid_
#undef _Pre_opt_valid_
#define _Pre_opt_valid_      _Pre_maybenull_ _Pre_ _Valid_
#undef _Post_valid_
#define _Post_valid_         _Post_ _Valid_
#undef _Post_invalid_
#define _Post_invalid_       _Post_ _Deref_ _Notvalid_
#undef _Post_ptr_invalid_
#define _Post_ptr_invalid_   _Post_ _Notvalid_
#undef _Pre_readable_size_
#define _Pre_readable_size_(s)      _Pre_ _Readable_elements_(s) _Pre_ _Valid_
#undef _Pre_writable_size_
#define _Pre_writable_size_(s)      _Pre_ _Writable_elements_(s)
#undef _Pre_readable_byte_size_
#define _Pre_readable_byte_size_(s) _Pre_ _Readable_bytes_(s) _Pre_ _Valid_
#undef _Pre_writable_byte_size_
#define _Pre_writable_byte_size_(s) _Pre_ _Writable_bytes_(s)
#undef _Post_readable_size_
#define _Post_readable_size_(s)     _Post_ _Readable_elements_(s) _Post_ _Valid_
#undef _Post_writable_size_
#define _Post_writable_size_(s)     _Post_ _Writable_elements_(s)
#undef _Post_readable_byte_size_
#define _Post_readable_byte_size_(s) _Post_ _Readable_bytes_(s) _Post_ _Valid_
#undef _Post_writable_byte_size_
#define _Post_writable_byte_size_(s) _Post_ _Writable_bytes_(s)
#endif //FUNCTIONAL_FAKE_SAL_DEFINED

/* Fake SAL end */

typedef struct CString {
	CString() {
		this->_m_lp_cStorage = Q_nullptr;
		this->_m_iLength = 0;
	}

	CString(_In_z_ char* _Which);

	CString(_In_z_ const char* _Which);

	CString(_In_ functional_unsigned_size_t _Length);

	CString(_In_ CString& _Other) {
		this->_m_lp_cStorage = _Other._m_lp_cStorage;
		this->_m_iLength = _Other._m_iLength;
	}

	CString(_In_ CString&& _Other) noexcept {
		this->_m_lp_cStorage = _Other._m_lp_cStorage;
		this->_m_iLength = _Other._m_iLength;
	}

	CString(_In_ const CString& _Other) {
		this->_m_lp_cStorage = _Other._m_lp_cStorage;
		this->_m_iLength = _Other._m_iLength;
	}

	CString(_In_ const CString&& _Other) noexcept {
		this->_m_lp_cStorage = _Other._m_lp_cStorage;
		this->_m_iLength = _Other._m_iLength;
	}

	~CString();

	CString& operator=(_In_ CString _Rhs) {
		this->_m_lp_cStorage = _Rhs._m_lp_cStorage;
		this->_m_iLength = _Rhs._m_iLength;
	}

	CString& operator=(_In_ CString& _Rhs) {
		this->_m_lp_cStorage = _Rhs._m_lp_cStorage;
		this->_m_iLength = _Rhs._m_iLength;
	}

	CString& operator=(_In_ CString&& _Rhs) noexcept {
		this->_m_lp_cStorage = _Rhs._m_lp_cStorage;
		this->_m_iLength = _Rhs._m_iLength;
	}

	CString& operator=(_In_ const CString& _Rhs) {
		this->_m_lp_cStorage = _Rhs._m_lp_cStorage;
		this->_m_iLength = _Rhs._m_iLength;
	}

	CString& operator=(_In_ const CString&& _Rhs) noexcept {
		this->_m_lp_cStorage = _Rhs._m_lp_cStorage;
		this->_m_iLength = _Rhs._m_iLength;
	}

	CString& operator=(_In_z_ const char* _String);

	CString& operator=(_In_ char _Character);

	CString& operator+=(_In_z_ const char* _String);

	CString& operator+=(_In_ char _Character);

	template<class... _Ts> static CString& Format(_Printf_format_string_ _In_z_ const char* const _Format, _In_opt_ _Ts... _Args);

	CString& operator+(_In_z_ const char* _Other);

	CString& operator+(_In_ char _Character);

	Q_bool operator==(_In_ CString& _Rhs);

	Q_bool operator==(_In_ const char* _Rhs);

	Q_bool operator!=(_In_ CString& _Rhs);

	Q_bool operator!=(_In_ const char* _Rhs);

	const char* c_str() {
		return this->_m_lp_cStorage;
	}

	functional_unsigned_size_t length() {
		return this->_m_iLength;
	}
private:
	char* _m_lp_cStorage = Q_nullptr;
	functional_unsigned_size_t _m_iLength = 0;
} CString, Q_string;

//Bypassing compiler checks when dividing by zero: 1337 / 0 will (or may) produce a warning/error for some compilers (MSVC, G++, Clang do such).
int g_zeroTest = 0;
//Once an assertion has failed, please consider using Q_GetAssertionFailureReason 
//or just directly read reason from gs_lpszAssertionFailureReason using a debugger (when using Q_GetAssertionFailureReason, call it in a 'critical section' or so-called crash handler).
static const char* gs_lpszAssertionFailureReason = Q_nullptr;

const char* Q_GetAssertionFailureReason() {
	//No reason, no assertions have failed.
	if (!gs_lpszAssertionFailureReason) return "(null)";

	return gs_lpszAssertionFailureReason;
}

#define Q_ARRAYSIZE(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#ifndef __LINE__
#warning __LINE__ macro unsupported
#define __LINE__ -1
#endif //__LINE__
#ifndef __FILE__
#warning __FILE__ macro unsupported
#define __FILE__ "Compiler unsupported"
#endif //__FILE__
#ifndef FUNCTIONAL_NO_ASSERTS
//Checking passed expression for truth, if false - setting failure reason, crashing application using access violation and divide by zero exceptions.
#define Q_ASSERT(_Expr) do { (!!(_Expr)) || (gs_lpszAssertionFailureReason = Q_string::Format("Assertion failed at %d line in file %s with code %s.", __LINE__, __FILE__, #_Expr).c_str(), (*((unsigned int*)0) = 0xCAFE), (*((unsigned int*)0) = 0xCAFE / g_zeroTest)); } while (0)
#ifdef FUNCTIONAL_DEBUG_PARANOID
#define Q_SLOWASSERT(_Expr) Q_ASSERT(_Expr)
#else
#define Q_SLOWASSERT(_Expr)
#endif //FUNCTIONAL_DEBUG_PARANOID
#else
#define Q_ASSERT(_Expr)
#define Q_SLOWASSERT(_Expr)
#endif //FUNCTIONAL_NO_ASSERTS

void* Q_memcpy(_Out_writes_bytes_all_(_Size) void* _Dst, _In_reads_bytes_(_Size) const void* _Src, _In_ unsigned int _Size) {
	auto dest = static_cast<char*>(_Dst);
	auto source = static_cast<const char*>(_Src);

	Q_SLOWASSERT(dest && source && "Q_memcpy: Where should I store copied values?");

	if (dest && source) {
		while (_Size) {
			*dest++ = *source++;
			--_Size;
		}
	}

	return dest;
}

void* Q_memset(_Out_writes_bytes_all_(_Size) void* _Dst, _In_reads_bytes_(_Size) functional_unsigned_size_t _Value, _In_ unsigned int _Size) {
	auto dest = static_cast<char*>(_Dst);

	Q_SLOWASSERT(dest && "Q_memset: Where should I store your values?");

	if (dest) {
		while (_Size) {
			*dest++ = _Value;
			--_Size;
		}
	}

	return dest;
}

#ifndef FUNCTIONAL_NO_ALLOCATOR
inline namespace YouShouldNotUseThisFunctional {
	typedef struct CAllocatedSegment {
		Q_bool m_bIsFree;
		functional_size_t m_iSize;
		CAllocatedSegment* m_lpNext, * m_lpPrevious;
	} CAllocatedSegment;
}

typedef struct CAllocator {
	static CAllocator* Init() {
		static CAllocator allocator = CAllocator{};
		allocator.ClearPool();
		allocator._m_lpSegments = (CAllocatedSegment*)allocator._m_acMemoryPool;
		allocator._m_lpSegments->m_bIsFree = Q_TRUE;
		allocator._m_lpSegments->m_iSize = FUNCTIONAL_HEAP_SIZE / FUNCTIONAL_BLOCK_SIZE;
		allocator._m_lpSegments->m_lpNext = Q_nullptr;
		allocator._m_lpSegments->m_lpPrevious = Q_nullptr;
		allocator._m_lpOldFreeSegment = Q_nullptr;

		return &allocator;
	}

	void ClearPool() {
		Q_memset(this->_m_acMemoryPool, 0, FUNCTIONAL_HEAP_SIZE);
	}

	CAllocatedSegment* SearchFreeSegment(_In_ CAllocatedSegment* _Segment, _In_ functional_size_t _MinSize) {
		while (_Segment) {
			if (_Segment->m_bIsFree && _Segment->m_iSize >= _MinSize) return _Segment;
			_Segment = _Segment->m_lpNext;
		}
		return _Segment;
	}

	functional_size_t GetNumBlock(_In_ functional_size_t size) {
		if (size % FUNCTIONAL_BLOCK_SIZE) size += FUNCTIONAL_BLOCK_SIZE;
		return size / FUNCTIONAL_BLOCK_SIZE;
	}

	CAllocatedSegment* CutSegment(_In_ CAllocatedSegment* _Segment, _In_ functional_size_t _Size) {
		functional_uintptr_t addr = reinterpret_cast<functional_uintptr_t>(_Segment);
		addr += (_Segment->m_iSize - _Size) * FUNCTIONAL_BLOCK_SIZE;
		CAllocatedSegment* result = (CAllocatedSegment*)addr;
		_Segment->m_iSize -= _Size;
		result->m_iSize = _Size;
		result->m_lpPrevious = _Segment;
		result->m_lpNext = _Segment->m_lpNext;
		if (_Segment->m_lpNext) _Segment->m_lpNext->m_lpPrevious = result;
		_Segment->m_lpNext = result;
		result->m_bIsFree = _Segment->m_bIsFree;
		return result;
	}

	CAllocatedSegment* MergeSegment(_In_ CAllocatedSegment* _Segment, _In_ CAllocatedSegment* _OldSegment) {
		if (this->_m_lpOldFreeSegment == _OldSegment) this->_m_lpOldFreeSegment = _Segment;
		_Segment->m_iSize += _OldSegment->m_iSize;
		_Segment->m_lpNext = _OldSegment->m_lpNext;
		if (_OldSegment->m_lpNext) _OldSegment->m_lpNext->m_lpPrevious = _Segment;

		return _Segment;
	}

	void* SegmentToPtr(_In_ CAllocatedSegment* _Segment) {
		Q_SLOWASSERT(_Segment && "Expected _Segment to be non-nullptr");

		return reinterpret_cast<char*>(_Segment) + sizeof(CAllocatedSegment);
	}

	CAllocatedSegment* PtrToSegment(_In_ void* _Pointer) {
		Q_SLOWASSERT(_Pointer && "Expected _Pointer to be non-nullptr");

		return (CAllocatedSegment*)(reinterpret_cast<char*>(_Pointer) - sizeof(CAllocatedSegment));
	}

	void* Allocate(_In_ functional_size_t _Size) {
		int s = GetNumBlock(_Size + sizeof(CAllocatedSegment));
		CAllocatedSegment* it = SearchFreeSegment(this->_m_lpOldFreeSegment, s);
		if (!it) it = SearchFreeSegment(this->_m_lpSegments, s);
		if (!it) {
			return Q_nullptr;
		}

		it->m_bIsFree = Q_FALSE;

		if (it->m_iSize > s + GetNumBlock(sizeof(CAllocatedSegment))) {
			CAllocatedSegment* n = CutSegment(it, it->m_iSize - s);
			n->m_bIsFree = Q_TRUE;
			this->_m_lpOldFreeSegment = n;
		}

		return SegmentToPtr(it);
	}

	void Free(_In_ void* _Pointer) {
		if (!_Pointer) return;
		CAllocatedSegment* segment = PtrToSegment(_Pointer);
		segment->m_bIsFree = Q_TRUE;
		Q_memset(_Pointer, 0, segment->m_iSize);

		if (segment->m_lpNext && segment->m_lpNext->m_bIsFree) MergeSegment(segment, segment->m_lpNext);
		if (segment->m_lpPrevious && segment->m_lpPrevious->m_bIsFree) MergeSegment(segment->m_lpPrevious, segment);
	}

	void* Reallocate(_In_ void* _Pointer, _In_ functional_size_t _Size) {
		if (!_Size) {
			Free(_Pointer);

			return Q_nullptr;
		}

		CAllocatedSegment* segment = PtrToSegment(_Pointer);
		int block = GetNumBlock(_Size + sizeof(CAllocatedSegment));
		if (segment->m_iSize >= block) {
			return _Pointer;
		} else {
			if (segment->m_lpNext && segment->m_lpNext->m_bIsFree && segment->m_iSize + segment->m_lpNext->m_iSize >= block) {
				MergeSegment(segment, segment->m_lpNext);
				if (segment->m_iSize > block + GetNumBlock(sizeof(CAllocatedSegment))) {
					CAllocatedSegment* n = CutSegment(segment, segment->m_iSize - block);
					n->m_bIsFree = Q_TRUE;
				}

				return _Pointer;
			} else {
				auto storage = Allocate(_Size);
				Q_memcpy(storage, _Pointer, _Size);
				Free(_Pointer);

				return storage;
			}
		}
	}
private:
	CAllocator() {
		Q_memset(this->_m_acMemoryPool, 0, Q_ARRAYSIZE(this->_m_acMemoryPool));
		this->_m_lpSegments = Q_nullptr;
		this->_m_lpOldFreeSegment = Q_nullptr;
	}
		 
	char _m_acMemoryPool[FUNCTIONAL_HEAP_SIZE];
	CAllocatedSegment* _m_lpSegments;
	CAllocatedSegment* _m_lpOldFreeSegment;
} CAllocator;

static CAllocator* gs_lpAllocator = CAllocator::Init();

void* Q_malloc(_In_ functional_size_t _Size) {
	if (!gs_lpAllocator || reinterpret_cast<functional_uintptr_t>(gs_lpAllocator) == 1) gs_lpAllocator = CAllocator::Init();

	return gs_lpAllocator->Allocate(_Size);
}

void Q_free(_In_ void* _Pointer) {
	//How did you manage to use free without initializing the allocator, huh?
	//Is your code bugsafe/bugless?
	Q_SLOWASSERT(gs_lpAllocator && reinterpret_cast<functional_uintptr_t>(gs_lpAllocator) != 1);
	if (!gs_lpAllocator || reinterpret_cast<functional_uintptr_t>(gs_lpAllocator) == 1) gs_lpAllocator = CAllocator::Init();

	gs_lpAllocator->Free(_Pointer);
}

void* Q_realloc(_In_ void* _Pointer, _In_ functional_size_t _Size) {
	//How did you manage to use realloc without initializing the allocator, huh?
	//Is your code bugsafe/bugless?
	Q_SLOWASSERT(gs_lpAllocator && reinterpret_cast<functional_uintptr_t>(gs_lpAllocator) != 1);
	if (!gs_lpAllocator || reinterpret_cast<functional_uintptr_t>(gs_lpAllocator) == 1) gs_lpAllocator = CAllocator::Init();

	return gs_lpAllocator->Reallocate(_Pointer, _Size);
}

void Q_clear_allocator() {
	gs_lpAllocator->ClearPool();
	gs_lpAllocator = CAllocator::Init();
}
#endif //FUNCTIONAL_NO_ALLOCATOR

#ifdef FUNCTIONAL_NO_ALLOCATOR
#ifndef FUNCTIONAL_CUSTOM_MALLOC
#error You must provide custom malloc function in order to use FUNCTIONAL_NO_ALLOCATOR definition.
#endif //FUNCTIONAL_CUSTOM_MALLOC
#ifndef FUNCTIONAL_CUSTOM_FREE
#error You must provide custom free function in order to use FUNCTIONAL_NO_ALLOCATOR definition.
#endif //FUNCTIONAL_CUSTOM_FREE
#ifndef FUNCTIONAL_CUSTOM_REALLOC
#error You must provide custom free function in order to use FUNCTIONAL_NO_ALLOCATOR definition.
#endif //FUNCTIONAL_CUSTOM_REALLOC

void* Q_malloc(_In_ functional_size_t _Size) {
	return FUNCTIONAL_CUSTOM_MALLOC(_Size);
}

void Q_free(_In_ void* _Pointer) {
	FUNCTIONAL_CUSTOM_FREE(_Pointer);
}

void* Q_realloc(_In_ void* _Pointer, _In_ functional_size_t _Size) {
	FUNCTIONAL_CUSTOM_REALLOC(_Pointer, _Size);
}

#endif //FUNCTIONAL_NO_ALLOCATOR

typedef struct INewWrapper {} INewWrapper;
inline void* operator new(_In_opt_ functional_unsigned_size_t _Count, _In_ INewWrapper _Wrapper, _In_ void* _Pointer) {
	(void)_Count;
	(void)_Wrapper;

	return _Pointer; 
}
inline void  operator delete(void*, INewWrapper, void*) {}

#define Q_new(_Type) new (INewWrapper(), Q_malloc(sizeof(_Type))) _Type

template<class _Ty> void Q_delete(_In_ _Ty* _Pointer) {
	if (_Pointer) {
		_Pointer->~_Ty();

		Q_free(_Pointer);
	}
}

template<class _To, class _From> _To union_cast(_From&& _What) {
	union {
		remove_reference_t<_From>* m_lpFrom;
		remove_reference_t<_To>* m_lpTo;
	} c_uCaster;

	c_uCaster.m_lpFrom = &_What;

	return (*c_uCaster.m_lpTo);
}

template<class _Callee> union UStorage {
	UStorage() {}
	decay_t<_Callee> callee;
};

template<class _Callee, class _ReturnType, class... _Ts> auto functional_cast_internal(_Callee&& c, _ReturnType(*)(_Ts...)) {
	static bool used = false;
	static UStorage<_Callee> s;
	typedef decltype(s.callee) type;

	if (used)
		s.callee.~type();

	new (INewWrapper(), &s.callee) type(forward<_Callee>(c));
	used = true;

	return [](_Ts... _Args) -> _ReturnType {
		return _ReturnType(s.callee(forward<_Ts>(_Args)...));
	};
}

//Deduction is disabled here using enable_if and typeof - xWhitey
template<class _ReturnType, class _Which, class = enable_if_t<true, _ReturnType>, class = typename typeof<_ReturnType>::type> _ReturnType* functional_cast(_Which&& c) {
	return functional_cast_internal(forward<_Which>(c), (_ReturnType*)Q_nullptr);
}

template<class _To, class _From, class = enable_if_t<true, _To>, class = typeof<_To>> _To indirect_cast(_From&& _What) {
	return functional_cast<_To()>([_What]() { return _What; })();
}

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif //INT_MAX

#ifdef FUNCTIONAL_FIX_FLTUSED_NOT_FOUND
extern "C" Q_bool _fltused = Q_TRUE;
#endif //FUNCTIONAL_FIX_FLTUSED_NOT_FOUND

#ifndef BYTE_SIZE_IN_BITS
#define BYTE_SIZE_IN_BITS 8
#endif //BYTE_SIZE_IN_BITS

#ifndef CHAR_BIT
#define CHAR_BIT (sizeof(char) * BYTE_SIZE_IN_BITS)
#endif //CHAR_BIT

//(-)2147483647
#define INT_STR_SIZE (sizeof(int) * CHAR_BIT / 3 + 3)

//(-)3.402823466e+38F
#define FLOAT_STR_SIZE (sizeof(float) * CHAR_BIT / 3 + 18)

template<class _Function> void expand(_Function&& _Func) {

}

template<class _Function, class _Arg, class... _Rest> void expand(_Function&& _Func, _Arg& _Argument, _Rest&... _RestArgs) {
	_Func(_Argument);
	expand(_Func, _RestArgs...);
}

typedef struct CParameterPackExpander {
	functional_size_t m_iArgsSize, m_iCurrentArg;
	void** m_a_lpTotalArgs;

	template<class _ResultType> _ResultType& at(_In_ functional_size_t _Where);
} CParameterPackExpander, Q_pp_list;

#define Q_pp_start(_Klass, _Pack) do {																				\
		_Klass->m_iArgsSize = sizeof...(_Pack);																		\
		_Klass->m_iCurrentArg = 0;																					\
		_Klass->m_a_lpTotalArgs	= reinterpret_cast<void**>(Q_malloc(_Klass->m_iArgsSize * sizeof(void*)));			\
																													\
		expand([&](auto& arg) {																						\
			_Klass->m_a_lpTotalArgs[_Klass->m_iCurrentArg] = reinterpret_cast<void*>(&arg);							\
			++_Klass->m_iCurrentArg;																				\
		}, _Pack...);																								\
		_Klass->m_iCurrentArg = 0;																					\
	} while (0)

#define Q_pp_arg(_Klass, _Type) *static_cast<_Type*>(_Klass->m_a_lpTotalArgs[_Klass->m_iCurrentArg++])

#define Q_pp_end(_Klass) do {					\
		Q_free(_Klass->m_a_lpTotalArgs);		\
		Q_free(_Klass);							\
	} while (0)

//Usage example
/*template<class... _Ts> void do_shit(_Ts... _Args) {
	auto list = Q_new(Q_pp_list)();
	Q_pp_start(list, _Args);

	int my_arg1 = Q_pp_arg(list, int);

	float my_arg2 = Q_pp_arg(list, float);

	const char* my_arg3 = Q_pp_arg(list, const char*);

	void* my_arg4 = Q_pp_arg(list, void*);

	Q_msvc_printf("%d %f %s 0x%p\n", my_arg1, my_arg2, my_arg3, my_arg4);

	Q_pp_end(list);
}*/

#define Q_max(_Which, _To) ((_Which < _To) ? _To : _Which)
#define FUNCTIONAL_abs(_Which) (_Which > 0 ? _Which : -_Which)

#define RandomNumber_Stage1(_Counter) (_Counter * ((int) (_Counter / 2)) + __COUNTER__)
#define RandomNumber_Stage2(_Counter) (RandomNumber_Stage1(_Counter) + RandomNumber_Stage1((__COUNTER__ % (_Counter + 1)) + 1))
#define RandomNumber_Stage3(_Counter) (_Counter % 2 == 0 ? RandomNumber_Stage2(_Counter + ((int) (__COUNTER__ / 2))) : RandomNumber_Stage1(_Counter + ((int) (__COUNTER__ * 2))))
#define RandomNumber_Stage4(_Counter) (__LINE__ % 2 == 0 ? RandomNumber_Stage3(_Counter + __LINE__) : RandomNumber_Stage2(_Counter + __LINE__ / 3))
#define RandomNumber_Stage5(_Counter) (RandomNumber_Stage4(_Counter) * RandomNumber_Stage2(__LINE__))
#define RandomNumber_Stage6(_Counter) (RandomNumber_Stage5(_Counter / 2) * RandomNumber_Stage4(_Counter / 3))
#define RandomNumber_Stage7(_Counter) (RandomNumber_Stage6((_Counter)) + RandomNumber_Stage3(__LINE__ / 2))
#define RandomNumber(_Min, _Max) (FUNCTIONAL_abs((RandomNumber_Stage7(__COUNTER__ + _Min + _Max) % (_Max + 1 - _Min) + _Min)))

//FIXME
#define RandomSeed() (RandomNumber_Stage7(((int) (__TIMESTAMP__[9] - '0' + __TIMESTAMP__[10] - '0' + __TIMESTAMP__[12] - '0' + __TIMESTAMP__[13] - '0' + __TIMESTAMP__[15] - '0' + __TIMESTAMP__[16] - '0' + __TIMESTAMP__[18] - '0' + __TIMESTAMP__[19] - '0'))) / 16777215)

#define Q_RAND_MAX 32767

typedef struct CTrustedRandom {
	CTrustedRandom() {
		this->_m_iSeed = this->_m_iInvocationCounter = 0;
		this->_m_lp_iSeedTable = Q_nullptr;
	}

	~CTrustedRandom() {
		if (this->_m_lp_iSeedTable != Q_nullptr) {
			Q_free(this->_m_lp_iSeedTable);
		}
	}

	void srand(_In_ functional_unsigned_size_t _Position) {
		if (this->_m_lp_iSeedTable == Q_nullptr) {
			this->_m_iSeed = this->_m_a_iDefaultSeedTable[_Position];
		} else {
			this->_m_iSeed = this->_m_lp_iSeedTable[_Position];
		}
	}

	functional_size_t rand(_In_ functional_unsigned_size_t _Seed, _In_ functional_unsigned_size_t _Min, _In_ functional_unsigned_size_t _Max) {
		return ((_worker(_Seed, 0, Q_RAND_MAX) - _worker(_Seed + 1, Q_RAND_MAX / 2, Q_RAND_MAX)) - _worker(_Seed + 3, _Min, _Max) + _worker(_Seed / 2, _Min, _Max)) % (_Max + 1 - _Min) + _Min;
	}

	functional_size_t autorand(_In_ functional_unsigned_size_t _Min, _In_ functional_unsigned_size_t _Max) {
		return rand(rand(rand(this->_m_iInvocationCounter, 0, Q_ARRAYSIZE(this->_m_a_iDefaultSeedTable) - 1), 0, Q_ARRAYSIZE(this->_m_a_iDefaultSeedTable) - 1), _Min, _Max);
	}

	CTrustedRandom* GenerateNewDevice() {
		CTrustedRandom* result = Q_new(CTrustedRandom)();
		result->_m_lp_iSeedTable = reinterpret_cast<int*>(Q_malloc(sizeof(int) * Q_ARRAYSIZE(this->_m_a_iDefaultSeedTable)));
		result->_m_iSeed = this->autorand(0, Q_ARRAYSIZE(this->_m_a_iDefaultSeedTable) - 1);

		for (int idx = 0; idx < Q_ARRAYSIZE(this->_m_a_iDefaultSeedTable); idx++) {
			result->_m_lp_iSeedTable[idx] = this->autorand(0, 70000);
		}

		result->generate_seed();

		return result;
	}
private:
	unsigned char get_u1() {
		return this->_m_iSeed & 0xFF;
	}

	functional_unsigned_size_t generate_seed() {
		this->_m_iSeed *= (_m_iSeed % 2 == 0 ? __MAGIC_WORD__ : __SECONDARY_MAGIC_WORD__);
		if (this->_m_lp_iSeedTable == Q_nullptr) {
			this->_m_iSeed += this->_m_a_iDefaultSeedTable[this->_m_iSeed & 0xFF] + 1;
		} else {
			this->_m_iSeed += this->_m_lp_iSeedTable[this->_m_iSeed & 0xFF] + 1;
		}

		return this->_m_iSeed & 0x7FFFFFFF;
	}

	functional_unsigned_size_t _worker(_In_ functional_unsigned_size_t _Seed, _In_ functional_unsigned_size_t _Min, _In_ functional_unsigned_size_t _Max) {
		if (_m_iInvocationCounter >= Q_ARRAYSIZE(this->_m_a_iDefaultSeedTable) - 1) {
			_m_iInvocationCounter = 0;
		}

		_m_iInvocationCounter++;

		srand(_Seed - get_u1() + 1);

		if (_Max == _Min)
			return _Min;

		return generate_seed() % (_Max + 1 - _Min) + _Min;
	}

	int _m_iSeed, _m_iInvocationCounter;

	//Hardcoded to be with capacity of 256 integers!!! (Q_ARRAYSIZE(_m_a_iDefaultSeedTable))
 	int* _m_lp_iSeedTable = Q_nullptr;

	const static int _m_a_iDefaultSeedTable[1024];

	static const inline constexpr auto __MAGIC_WORD__ = 0xDEAD;
	static const inline constexpr auto __SECONDARY_MAGIC_WORD__ = 0xBABE;
} CTrustedRandom;

const int CTrustedRandom::_m_a_iDefaultSeedTable[1024] = { 
	31106, 30253, 54371, 8959, 44853, 19451, 5615, 5615, 54403, 22057, 58310, 52951, 38452, 27995, 66160, 43935, 6908, 20492, 33682, 36510, 62280, 54371, 53825, 47916, 41058, 22057, 37871, 34824, 49633, 35425, 45794, 18589, 916, 68616, 8454, 13061, 29941, 44341, 64022, 44383, 11686, 47498, 5120, 20678, 44882, 10364, 48352, 36333, 28628, 39558, 6908, 29806, 18526, 48163, 44848, 23598, 64022, 18403, 35892, 13823, 37578, 58454, 26653, 47557, 67119, 30449, 52986, 53638, 52788, 9383, 4996, 39558, 33019, 49556, 25998, 51322, 41144, 34988, 3667, 50086, 41743, 14928, 2084, 55251, 16700, 42160, 30253, 54371, 8959, 44853, 19451, 5615, 5615, 54403, 22057, 58310, 52951, 38452, 27995, 66160, 43935, 6908, 20492, 33682, 36510, 62280, 54371, 53825, 47916, 41058, 22057, 37871, 34824, 49633, 35425, 45794, 18589, 916, 68616, 8454, 13061, 29941, 44341, 64022, 44383, 11686, 47498, 5120, 20678, 44882, 10364, 48352, 36333, 28628, 39558, 6908, 29806, 18526, 48163, 44848, 23598, 64022, 18403, 35892, 13823, 37578, 58454, 26653, 47557, 67119, 30449, 52986, 53638, 52788, 9383, 4996, 39558, 33019, 49556, 25998, 51322, 41144, 34988, 3667, 50086, 41743, 14928, 2084, 55251, 16700, 42160, 30253, 54371, 8959, 44853, 19451, 5615, 5615, 54403, 22057, 58310, 52951, 38452, 27995, 66160, 43935, 6908, 20492, 33682, 36510, 62280, 54371, 53825, 47916, 41058, 22057, 37871, 34824, 49633, 35425, 45794, 18589, 916, 68616, 8454, 13061, 29941, 44341, 64022, 44383, 11686, 47498, 5120, 20678, 44882, 10364, 48352, 36333, 28628, 39558, 6908, 29806, 18526, 48163, 44848, 23598, 64022, 18403, 35892, 13823, 37578, 58454, 26653, 47557, 67119, 30449, 52986, 53638, 52788, 9383, 4996, 39558, 33019, 49556, 25998, 51322, 41144, 34988, 3667, 50086, 41743, 14928, 2084, 55251, 16700, 42160, 30253, 54371, 8959, 44853, 19451, 5615, 5615, 54403, 22057, 58310, 52951, 38452, 27995, 66160, 43935, 6908, 20492, 33682, 36510, 62280, 54371, 53825, 47916, 41058, 22057, 37871, 34824, 49633, 35425, 45794, 18589, 916, 68616, 8454, 13061, 29941, 44341, 64022, 44383, 11686, 47498, 5120, 20678, 44882, 10364, 48352, 36333, 28628, 39558, 6908, 29806, 18526, 48163, 44848, 23598, 64022, 18403, 35892, 13823, 37578, 58454, 26653, 47557, 67119, 30449, 52986, 53638, 52788, 9383, 4996, 39558, 33019, 49556, 25998, 51322, 41144, 34988, 3667, 50086, 41743, 14928, 2084, 55251, 16700, 42160, 30253, 54371, 8959, 44853, 19451, 5615, 5615, 54403, 22057, 58310, 52951, 38452, 27995, 66160, 43935, 6908, 20492, 33682, 36510, 62280, 54371, 53825, 47916, 41058, 22057, 37871, 34824, 49633, 35425, 45794, 18589, 916, 68616, 8454, 13061, 29941, 44341, 64022, 44383, 11686, 47498, 5120, 20678, 44882, 10364, 48352, 36333, 28628, 39558, 6908, 29806, 18526, 48163, 44848, 23598, 64022, 18403, 35892, 13823, 37578, 58454, 26653, 47557, 67119, 30449, 52986, 53638, 52788, 9383, 4996, 39558, 33019, 49556, 25998, 51322, 41144, 34988, 3667, 50086, 41743, 14928, 2084, 55251, 16700, 42160, 30253, 54371, 8959, 44853, 19451, 5615, 5615, 54403, 22057, 58310, 52951, 38452, 27995, 66160, 43935, 6908, 20492, 33682, 36510, 62280, 54371, 53825, 47916, 41058, 22057, 37871, 34824, 49633, 35425, 45794, 18589, 916, 68616, 8454, 13061, 29941, 44341, 64022, 44383, 11686, 47498, 5120, 20678, 44882, 10364, 48352, 36333, 28628, 39558, 6908, 29806, 18526, 48163, 44848, 23598, 64022, 18403, 35892, 13823, 37578, 58454, 26653, 47557, 67119, 30449, 52986, 53638, 52788, 9383, 4996, 39558, 33019, 49556, 25998, 51322, 41144, 34988, 3667, 50086, 41743, 14928, 2084, 55251, 16700, 42160, 30253, 54371, 8959, 44853, 19451, 5615, 5615, 54403, 22057, 58310, 52951, 38452, 27995, 66160, 43935, 6908, 20492, 33682, 36510, 62280, 54371, 53825, 47916, 41058, 22057, 37871, 34824, 49633, 35425, 45794, 18589, 916, 68616, 8454, 13061, 29941, 44341, 64022, 44383, 11686, 47498, 5120, 20678, 44882, 10364, 48352, 36333, 28628, 39558, 6908, 29806, 18526, 48163, 44848, 23598, 64022, 18403, 35892, 13823, 37578, 58454, 26653, 47557, 67119, 30449, 52986, 53638, 52788, 9383, 4996, 39558, 33019, 49556, 25998, 51322, 41144, 34988, 3667, 50086, 41743, 14928, 2084, 55251, 16700, 42160, 30253, 54371, 8959, 44853, 19451, 5615, 5615, 54403, 22057, 58310, 52951, 38452, 27995, 66160, 43935, 6908, 20492, 33682, 36510, 62280, 54371, 53825, 47916, 41058, 22057, 37871, 34824, 49633, 35425, 45794, 18589, 916, 68616, 8454, 13061, 29941, 44341, 64022, 44383, 11686, 47498, 5120, 20678, 44882, 10364, 48352, 36333, 28628, 39558, 6908, 29806, 18526, 48163, 44848, 23598, 64022, 18403, 35892, 13823, 37578, 58454, 26653, 47557, 67119, 30449, 52986, 53638, 52788, 9383, 4996, 39558, 33019, 49556, 25998, 51322, 41144, 34988, 3667, 50086, 41743, 14928, 2084, 55251, 16700, 42160, 30253, 54371, 8959, 44853, 19451, 5615, 5615, 54403, 22057, 58310, 52951, 38452, 27995, 66160, 43935, 6908, 20492, 33682, 36510, 62280, 54371, 53825, 47916, 41058, 22057, 37871, 34824, 49633, 35425, 45794, 18589, 916, 68616, 8454, 13061, 29941, 44341, 64022, 44383, 11686, 47498, 5120, 20678, 44882, 10364, 48352, 36333, 28628, 39558, 6908, 29806, 18526, 48163, 44848, 23598, 64022, 18403, 35892, 13823, 37578, 58454, 26653, 47557, 67119, 30449, 52986, 53638, 52788, 9383, 4996, 39558, 33019, 49556, 25998, 51322, 41144, 34988, 3667, 50086, 41743, 14928, 2084, 55251, 16700, 42160, 30253, 54371, 8959, 44853, 19451, 5615, 5615, 54403, 22057, 58310, 52951, 38452, 27995, 66160, 43935, 6908, 20492, 33682, 36510, 62280, 54371, 53825, 47916, 41058, 22057, 37871, 34824, 49633, 35425, 45794, 18589, 916, 68616, 8454, 13061, 29941, 44341, 64022, 44383, 11686, 47498, 5120, 20678, 44882, 10364, 48352, 36333, 28628, 39558, 6908, 29806, 18526, 48163, 44848, 23598, 64022, 18403, 35892, 13823, 37578, 58454, 26653, 47557, 67119, 30449, 52986, 53638, 52788, 9383, 4996, 39558, 33019, 49556, 25998, 51322, 41144, 34988, 3667, 50086, 41743, 14928, 2084, 55251, 16700, 42160, 30253, 54371, 8959, 44853, 19451, 5615, 5615, 54403, 22057, 58310, 52951, 38452, 27995, 66160, 43935, 6908, 20492, 33682, 36510, 62280, 54371, 53825, 47916, 41058, 22057, 37871, 34824, 49633, 35425, 45794, 18589, 916, 68616, 8454, 13061, 29941, 44341, 64022, 44383, 11686, 47498, 5120, 20678, 44882, 10364, 48352, 36333, 28628, 39558, 6908, 29806, 18526, 48163, 44848, 23598, 64022, 18403, 35892, 13823, 37578, 58454, 26653, 47557, 67119, 30449, 52986, 53638, 52788, 9383, 4996, 39558, 33019, 49556, 25998, 51322, 41144, 34988, 3667, 50086, 41743, 14928, 2084, 55251, 16700, 42160, 30253, 54371, 8959, 44853, 19451, 5615, 5615, 54403, 22057, 58310, 52951, 38452, 27995, 66160, 43935, 6908, 20492, 33682, 36510, 62280, 54371, 53825, 47916, 41058, 22057, 37871, 34824, 49633, 35425, 45794, 18589, 916, 68616, 8454, 13061, 29941, 44341, 64022, 44383, 11686, 47498, 5120, 20678, 44882, 10364, 48352, 36333, 28628, 39558, 6908, 29806, 18526, 48163, 44848, 23598, 64022, 18403, 35892, 13823, 37578, 58454, 26653, 47557, 67119, 30449, 52986, 53638, 52788, 9383, 4996, 39558, 33019, 49556, 25998, 51322, 41144, 34988, 3667, 50086, 41743, 14928, 2084, 55251, 16700, 42160, 30253, 54371, 8959 
};

namespace
#ifdef FUNCTIONAL_DONT_USE_ANONYMOUS_NAMESPACE
	functional
#endif //FUNCTIONAL_DONT_USE_ANONYMOUS_NAMESPACE
{
	functional_size_t Q_strlen(_In_z_ const char* _Str) {
		const char* p = const_cast<char*>(&_Str[0]);

#ifdef FUNCTIONAL_USE_FASTEST_STRLEN
		//Be careful: it was reverse engineered from hidden Windows API internals.
		if (((unsigned char)(_Str) & 3) == 0)
			goto main_loop;
		do {
			if (!*p++)
				return p - 1 - _Str;
		} while (((unsigned char) (p) & 3) != 0);
		while (true) {
			int v3;
			do {
			main_loop:
				v3 = (*(unsigned long*)p + 0x7EFEFEFF) ^ ~*(unsigned long*)p;
				p += 4;
			} while ((v3 & 0x81010100) == 0);
			int v4 = *((unsigned long*)p - 1);
			if (!v4)
				break;
			if (!(*((unsigned char*)&(v4)+1)))
				return p - 3 - _Str;
			if ((v4 & 0xFF0000) == 0)
				return p - 2 - _Str;
			if ((v4 & 0xFF000000) == 0)
				return p - 1 - _Str;
		}
		return p - 4 - _Str;
#else
		functional_size_t count = 0;

		for (; p[0] != '\0'; ++p, count++);

		return count;
#endif //FUNCTIONAL_USE_FASTEST_STRLEN
	}

	char* Q_strcpy(_Always_(_Post_z_) _Out_ char* _Destination, _In_z_ const char* _Source) {
		if (!_Source) return _Destination;

		if (_Source) {
			const functional_size_t length = Q_strlen(_Source) + 1;
			for (int idx = 0; idx < length; idx++) {
				_Destination[idx] = _Source[idx];
			}
			_Destination[length + 1] = '\0';
		}

		return _Destination;
	}

	char* Q_strcat(_Inout_ char* _Destination, _In_z_ const char* _Source) {
		if (!_Source) return _Destination;

		if (_Source) {
			const int srcLength = Q_strlen(_Source);
			const int destLength = Q_strlen(_Destination);
			for (int idx = destLength; idx < destLength + srcLength; idx++) {
				_Destination[idx] = _Source[idx - destLength];
			}
		}

		return _Destination;
	}

	functional_size_t Q_strcmp(_In_z_ const char* _Str1, _In_z_ const char* _Str2) {
		for (; *_Str1 == *_Str2; ++_Str1, ++_Str2) {
			if (*_Str1 == '\0') return 0;
		}

		return Q_strlen(_Str1) < Q_strlen(_Str2) ? -1 : 1;
	}

	functional_size_t Q_strncmp(_In_reads_or_z_(_MaxCount) const char* _Str1, _In_reads_or_z_(_MaxCount) const char* _Str2, _In_ functional_unsigned_size_t _MaxCount) {
		while (_MaxCount && *_Str1 && (*_Str1 == *_Str2)) ++_Str1, ++_Str2, --_MaxCount;

		if (_MaxCount == 0) return 0;

		return(*(unsigned char*)_Str1 - *(unsigned char*)_Str2);
	}

	char* Q_strdup(_In_z_ const char* _Source) {
		const auto buffer = static_cast<char*>(Q_malloc(Q_strlen(_Source) + 1));
		Q_SLOWASSERT(buffer && "Failed to allocate buffer (size is dynamic) at Q_strdup");

		Q_memcpy(buffer, _Source, Q_strlen(_Source));

		buffer[Q_strlen(_Source) + 1] = '\0';

		return buffer;
	}

	int Q_abs(_In_ functional_size_t _Number) {
		return _Number < 0 ? -_Number : _Number;
	}

	char* Q_itoa_internal(_Pre_notnull_ _Always_(_Post_z_) _Out_opt_ char* _Dest, _In_ functional_unsigned_size_t _Size, _In_ int _Value) {
		char buf[INT_STR_SIZE];
		char* p = &buf[INT_STR_SIZE - 1];
		*p = '\0';
		int i = _Value;

		do {
			*(--p) = Q_abs(i % 10) + '0';
			i /= 10;
		} while (i);

		if (_Value < 0) {
			*(--p) = '-';
		}
		const auto len = static_cast<functional_unsigned_size_t>(&buf[INT_STR_SIZE] - p);
		if (len > _Size) {
			return Q_nullptr;
		}
		return static_cast<char*>(Q_memcpy(_Dest, p, len));
	}

	functional_size_t Q_integer_to_octal(_In_ functional_size_t _Number) {
		functional_size_t modulo, octal = 0, idx = 1;

		while (_Number != 0) {
			modulo = _Number % 8;
			_Number = _Number / 8;
			octal = octal + (modulo * idx);
			idx = idx * 10;
		}

		return octal;
	}

	char* Q_itoa(_In_ functional_size_t _Number) {
		const auto buffer = static_cast<char*>(Q_malloc(INT_STR_SIZE));
		Q_SLOWASSERT(buffer && "Failed to allocate buffer (size=INT_STR_SIZE) at Q_itoa");
		Q_itoa_internal(buffer, INT_STR_SIZE, _Number);
		return buffer;
	}

	functional_size_t Q_atoi(char* s) {
		int c = 1, a = 0, sign, end, base = 1;

		if (s[0] == '-')
			sign = -1;
		else if (s[0] <= '9' && s[0] >= '0')
			sign = 1;
		else if (s[0] == '+')
			sign = 2;
		else
			return 0;

		while (s[c] != '\n' && s[c] <= '9' && s[c] >= '0')
			c++;

		if (sign == -1)
			end = 1;
		else if (sign == 1)
			end = 0;
		else {
			end = 1;
			sign = 1;
		}

		for (int i = c - 1; i >= end; i--) {
			a += (s[i] - '0') * base;
			base *= 10;
		}

		return sign * a;
	}

	functional_size_t Q_pow(_In_ functional_size_t _Base, _In_ functional_size_t _Power) {
		if (_Power == 0) return 1;
		else if ((_Power % 2) == 0)
			return Q_pow(_Base, _Power / 2) * Q_pow(_Base, _Power / 2);
		else
			return _Base * Q_pow(_Base, _Power / 2) * Q_pow(_Base, _Power / 2);
	}

	char* Q_ftoa(_In_ float _Value, _In_opt_ functional_size_t _Precision = 2) {
		const auto buffer = static_cast<char*>(Q_malloc(FLOAT_STR_SIZE));
		int b, l, i = 0;
		if (_Value < 0.f) {
			buffer[i++] = '-';
			_Value *= -1;
		}
		int a = (int)_Value;
		_Value -= a;
		int k = 0;
		while (true) {
			l = Q_pow(10, k);
			int m = a / l;
			if (m == 0) {
				break;
			}
			k++;
		}
		k--;
		for (l = k + 1; l > 0; l--) {
			b = Q_pow(10, l - 1);
			const int c = a / b;
			buffer[i++] = c + '0';
			a %= b;
		}

		if (_Precision != 0)
			buffer[i++] = '.';

		for (l = 0; l < _Precision; l++) {
			_Value *= 10.0;
			b = (int)_Value;
			buffer[i++] = b + '0';
			_Value -= b;
		}

		buffer[i] = '\0';

		return buffer;
	}

	functional_size_t Q_toupper(_In_ functional_size_t _C) {
		if (_C >= 'a' && _C <= 'z') {
			_C -= ('a' - 'A');
		}

		return _C;
	}

	char Q_toupper(_In_ char _C) {
		if (_C >= 'a' && _C <= 'z') {
			_C -= ('a' - 'A');
		}

		return _C;
	}

	char* Q_itohexa_helper(_In_ char* _Dest, _In_ functional_uintptr_t _Val) {
		if (_Val >= 16) {
			_Dest = Q_itohexa_helper(_Dest, _Val / 16);
		}
		*_Dest++ = "0123456789abcdef"[_Val & 15];
		return _Dest;
	}

	char* Q_itohexa(_In_ functional_uintptr_t _Val) {
		const auto buffer = static_cast<char*>(Q_malloc(32));
		Q_SLOWASSERT(buffer && "Failed to allocate buffer (size=32) at Q_itohexa");
		*Q_itohexa_helper(buffer, _Val) = '\0';

		return buffer;
	}

	char* Q_itohexa_upper(_In_ functional_uintptr_t _Val) {
		char* buffer = Q_itohexa(_Val);
		const functional_size_t len = Q_strlen(buffer);

		for (functional_size_t idx = 0; idx < len; idx++) {
			buffer[idx] = Q_toupper(buffer[idx]);
		}

		return buffer;
	}

	template<class... _Ts> _Success_(return >= 0) functional_size_t Q_sprintf(_Always_(_Post_z_) _Out_ char* const _Buffer,
		_Printf_format_string_ _In_z_ const char* const _Format, _In_opt_ _Ts... _Args) {
		Q_SLOWASSERT(_Buffer && "Q_sprintf: Where do you want me to store the output string?");
		Q_SLOWASSERT(_Format && "Q_sprintf: What should I print into your buffer?");
		if constexpr (sizeof...(_Args) > 0) {
			CParameterPackExpander* expander = Q_new(CParameterPackExpander)();
			Q_pp_start(expander, _Args);

			auto p = const_cast<char*>(&_Format[0]);
			int fmtCnt = 0;
			for (; p[0] != '\0'; ++p) {
				if (p[0] == '%') fmtCnt++;
			}
			//Handling the case when not all arguments are present, i.e there are too much format specifiers but not enough args to format the string
			if (fmtCnt > sizeof...(_Args)) {
				Q_strcpy(_Buffer, "Q_sprintf: Not all arguments are present.\n");
				Q_pp_end(expander);
				return sizeof("Q_sprintf: Not all arguments are present.\n") - 1;
			}

			p = const_cast<char*>(&_Format[0]);

			int writtenChars = 0;

			const auto buffer = static_cast<char*>(Q_malloc(2048));
			for (; p[0] != '\0'; ++p, ++writtenChars) {
				if (p[0] == '%') {
					p++;
					switch (p[0]) {
					case '.': {
						++p;
						functional_size_t count = 0;
						if (p[0] == '*') {
							count = expander->at<functional_size_t>(expander->m_iCurrentArg);
							++expander->m_iCurrentArg;
							++p;
						}
						char* countBuffer = reinterpret_cast<char*>(Q_malloc(3));
						Q_memset(countBuffer, 0, 3);
						if (!count) {
							for (functional_size_t idx = 0; p[0] != 'f' && idx < 3; ++p, idx++) {
								countBuffer[idx] = p[0];
							}
							countBuffer[2] = '\0';
							count = Q_atoi(countBuffer);
							Q_free(countBuffer);
						}
						const auto it = expander->at<float>(expander->m_iCurrentArg);
						const char* converted = Q_ftoa(it, count);
						if (it < 1.f) Q_strcat(buffer, "0");
						Q_strcat(buffer, converted);
						if (it < 1.f) writtenChars++;
						writtenChars += Q_strlen(converted);
						++expander->m_iCurrentArg;
						++p;
					}
							break;
					case 'o': {
						const auto it = Q_integer_to_octal(expander->at<functional_size_t>(expander->m_iCurrentArg));
						const char* converted = Q_itoa(it);
						Q_strcat(buffer, converted);
						writtenChars += Q_strlen(converted);
						++expander->m_iCurrentArg;
						++p;
					}
							break;
					case 'u':
					case 'i':
					case 'd': {
						const auto it = expander->at<functional_size_t>(expander->m_iCurrentArg);
						const char* converted = Q_itoa(it);
						Q_strcat(buffer, converted);
						writtenChars += Q_strlen(converted);
						++expander->m_iCurrentArg;
						++p;
					}
							break;
					case 'f': {
						const auto it = expander->at<float>(expander->m_iCurrentArg);
						const char* converted = Q_ftoa(it, 6);
						if (it < 1.f) Q_strcat(buffer, "0");
						Q_strcat(buffer, converted);
						if (it < 1.f) writtenChars++;
						writtenChars += Q_strlen(converted);
						++expander->m_iCurrentArg;
						++p;
					}
							break;
					case 'c': {
						buffer[writtenChars] = expander->at<char>(expander->m_iCurrentArg);
						++writtenChars;
						++expander->m_iCurrentArg;
						++p;
					}
							break;
					case 'x': {
						if (const auto num = expander->at<functional_uintptr_t>(expander->m_iCurrentArg)) {
							const char* str = Q_itohexa(num);
							writtenChars += Q_strlen(str);
							Q_strcat(buffer, str);
						} else {
							writtenChars += sizeof("0x0") - 1;
							Q_strcat(buffer, "0x0");
						}
						++expander->m_iCurrentArg;
						++p;
					}
							break;
					case 'X': {
						if (const auto num = expander->at<functional_uintptr_t>(expander->m_iCurrentArg)) {
							const char* str = Q_itohexa_upper(num);
							writtenChars += Q_strlen(str);
							Q_strcat(buffer, str);
						} else {
							writtenChars += sizeof("0x0") - 1;
							Q_strcat(buffer, "0x0");
						}
						++expander->m_iCurrentArg;
						++p;
					}
							break;
					case 's': {
						const auto str = expander->at<const char*>(expander->m_iCurrentArg);
						writtenChars += Q_strlen(str);
						Q_strcat(buffer, str);
						++expander->m_iCurrentArg;
						++p;
					}
							break;
					case 'p': {
						if (const auto addr = reinterpret_cast<functional_uintptr_t>(expander->at<void*>(expander->m_iCurrentArg))) {
							const char* str = Q_itohexa(addr);
							writtenChars += Q_strlen(str);
							Q_strcat(buffer, str);
						}
						else {
							writtenChars += sizeof("(null)") - 1;
							Q_strcat(buffer, "(null)");
						}
						++expander->m_iCurrentArg;
						++p;
					}
							break;
					default: {
						buffer[writtenChars] = p[0];
						++writtenChars;
						++p;
					}
						   break;
					}
				}
				buffer[writtenChars] = p[0];
			}

			buffer[writtenChars + 1] = '\0';

			Q_memcpy(_Buffer, buffer, Q_strlen(buffer) + 1);
			Q_pp_end(expander);
			Q_free(buffer);

			return writtenChars;
	} else {
			if (_Format) {
				const auto toWrite = Q_strlen(_Format);
				_Buffer[toWrite + 1] = '\0';
				Q_memcpy(_Buffer, _Format, toWrite);
				return toWrite;
			}
		}

		return -1;
	}
}

CString::CString(_In_z_ char* _Which) {
	Q_ASSERT(_Which && "Expected a non-null string at CString::CString(char*)");
	this->_m_iLength = Q_strlen(_Which);
	this->_m_lp_cStorage = reinterpret_cast<char*>(Q_malloc(this->_m_iLength + 1));
	Q_strcpy(this->_m_lp_cStorage, _Which);
	this->_m_lp_cStorage[this->_m_iLength + 1] = '\0';
}

CString::CString(_In_z_ const char* _Which) {
	Q_ASSERT(_Which && "Expected a non-null string at CString::CString(const char*)");
	this->_m_iLength = Q_strlen(_Which);
	this->_m_lp_cStorage = reinterpret_cast<char*>(Q_malloc(this->_m_iLength + 1));
	Q_strcpy(this->_m_lp_cStorage, _Which);
	this->_m_lp_cStorage[this->_m_iLength + 1] = '\0';
}

CString::CString(_In_ functional_unsigned_size_t _Length) : _m_iLength(_Length) {
	Q_ASSERT(_Length > 0 && "Expected positive _Length at CString::CString(functional_unsigned_size_t)");
	this->_m_lp_cStorage = reinterpret_cast<char*>(Q_malloc(_Length + 1));
	this->_m_lp_cStorage[_Length + 1] = '\0';
}

CString::~CString() {
	if (this->_m_lp_cStorage) Q_free(this->_m_lp_cStorage);
	this->_m_iLength = 0;
}

CString& CString::operator=(_In_z_ const char* _String) {
	Q_ASSERT(_String && "Expected a non-null string.");
	this->_m_iLength = Q_strlen(_String);

	if (this->_m_lp_cStorage) {
		Q_free(this->_m_lp_cStorage);
	}

	this->_m_lp_cStorage = reinterpret_cast<char*>(Q_malloc(this->_m_iLength + 1));

	Q_strcpy(this->_m_lp_cStorage, _String);
	this->_m_lp_cStorage[this->_m_iLength + 1] = '\0';

	return *this;
}

CString& CString::operator=(_In_ char _Character) {
	this->_m_iLength = 1;

	if (this->_m_lp_cStorage) {
		Q_free(this->_m_lp_cStorage);
	}

	this->_m_lp_cStorage = reinterpret_cast<char*>(Q_malloc(2));

	this->_m_lp_cStorage[0] = _Character;
	this->_m_lp_cStorage[1] = '\0';

	return *this;
}

CString& CString::operator+=(_In_z_ const char* _String) {
	Q_ASSERT(_String && "Expected a non-null string. To add a character into CString, refer to CString#operator+=(char)");
	this->_m_iLength += Q_strlen(_String);

	if (this->_m_lp_cStorage) {
		Q_realloc(this->_m_lp_cStorage, this->_m_iLength + 1);
	}
	else {
		this->_m_lp_cStorage = reinterpret_cast<char*>(Q_malloc(this->_m_iLength + 1));
	}

	Q_strcat(this->_m_lp_cStorage, _String);
	this->_m_lp_cStorage[this->_m_iLength + 1] = '\0';

	return *this;
}

CString& CString::operator+=(_In_ char _Character) {
	this->_m_iLength++;

	if (this->_m_lp_cStorage) {
		Q_realloc(this->_m_lp_cStorage, this->_m_iLength + 1);
	}
	else {
		this->_m_lp_cStorage = reinterpret_cast<char*>(Q_malloc(this->_m_iLength + 1));
	}

	this->_m_lp_cStorage[this->_m_iLength - 1] = _Character;
	this->_m_lp_cStorage[this->_m_iLength] = '\0';

	return *this;
}

CString& CString::operator+(_In_z_ const char* _Other) {
	Q_ASSERT(_Other && "Expected a non-null string. To add a character into CString, refer to CString#operator+(char)");
	CString* result = Q_new(CString)(this->_m_iLength + Q_strlen(_Other));

	Q_strcat(result->_m_lp_cStorage, this->_m_lp_cStorage);
	Q_strcat(result->_m_lp_cStorage, _Other);
	result->_m_lp_cStorage[result->_m_iLength + 1] = '\0';

	return *result;
}

CString& CString::operator+(_In_ char _Character) {
	CString* result = Q_new(CString)(this->_m_iLength + 1);

	Q_strcat(result->_m_lp_cStorage, this->_m_lp_cStorage);
	result->_m_lp_cStorage[this->_m_iLength] = _Character;
	result->_m_lp_cStorage[result->_m_iLength + 1] = '\0';

	return *result;
}

template<class... _Ts> _Success_(return != Q_nullptr) CString& CString::Format(_Printf_format_string_ _In_z_ const char* const _Format, _In_opt_ _Ts... _Args) {
	CString* result = Q_new(CString)();
	result->_m_lp_cStorage = reinterpret_cast<char*>(Q_malloc(2048));
	result->_m_iLength = Q_sprintf(result->_m_lp_cStorage, _Format, _Args...) + 1;
	Q_realloc(result->_m_lp_cStorage, result->_m_iLength);

	return *result;
}

Q_bool CString::operator==(_In_ CString& _Rhs) {
	return (Q_strcmp(this->_m_lp_cStorage, _Rhs._m_lp_cStorage) == 0) ? Q_TRUE : Q_FALSE;
}

Q_bool CString::operator==(_In_ const char* _Rhs) {
	return (Q_strcmp(this->_m_lp_cStorage, _Rhs) == 0) ? Q_TRUE : Q_FALSE;
}

Q_bool CString::operator!=(_In_ CString& _Rhs) {
	return (Q_strcmp(this->_m_lp_cStorage, _Rhs._m_lp_cStorage) != 0) ? Q_TRUE : Q_FALSE;
}

Q_bool CString::operator!=(_In_ const char* _Rhs) {
	return (Q_strcmp(this->_m_lp_cStorage, _Rhs) != 0) ? Q_TRUE : Q_FALSE;
}

//Non-copyable
template<class _Ty> struct CUniquePointer {
	explicit CUniquePointer(_In_opt_ _Ty* _Pointer = Q_nullptr) {
		this->_m_lpStorage = _Pointer;
	}

	~CUniquePointer() {
		if (this->_m_lpStorage) {
			this->_m_lpStorage->~_Ty();
			Q_free(this->_m_lpStorage);
			this->_m_lpStorage = Q_nullptr;
		}
	}

	_Ty& operator*() const {
		Q_ASSERT(this->_m_lpStorage);
		return *this->_m_lpStorage;
	}

	_Ty* operator->() const {
		Q_ASSERT(this->_m_lpStorage);
		return this->_m_lpStorage;
	}

	_Ty* get() const {
		return this->_m_lpStorage;
	}

	void reset() {
		if (this->_m_lpStorage) {
			this->_m_lpStorage->~_Ty();
			Q_free(this->_m_lpStorage);
			this->_m_lpStorage = Q_nullptr;
		}
	}
private:
	_Ty* _m_lpStorage;

	CUniquePointer(CUniquePointer const&);
	CUniquePointer& operator=(CUniquePointer const&);

	void operator==(CUniquePointer const&) const;
	void operator!=(CUniquePointer const&) const;
};

template<class _Ty, class... _Ts> CUniquePointer<_Ty> make_unique(_In_opt_ _Ts&&... _Args) {
	return CUniquePointer<_Ty>(Q_new(_Ty)(forward<_Ts>(_Args)...));
}

//The CString type is defined after our namespace which is defined a bit later than CParameterPackExpander. (refer to Q_ASSERT)
template<class _ResultType> _ResultType& CParameterPackExpander::at(_In_ functional_size_t _Where) {
	Q_ASSERT(_Where < this->m_iArgsSize);
	_ResultType& _Which = (_ResultType&)(*static_cast<_ResultType*>(this->m_a_lpTotalArgs[_Where]));

	return _Which;
}

#else //__cplusplus
#error C++ compiler required to compile functional.hpp.
#endif //__cplusplus

#endif //FUNCTIONAL_HPP_GUARD

#undef FUNCTIONAL_HPP_RECURSE_GUARD
#endif //FUNCTIONAL_HPP_RECURSE_GUARD
