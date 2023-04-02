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

#ifdef __cplusplus

#include "functional_config.hpp"

#ifndef FUNCTIONAL_DONT_DEFINE_VARARGS

#define va_list
#undef va_list

#ifndef va_list
typedef char* va_list;
#endif //va_list

#ifdef va_start
#undef va_start
#define va_start(list, lastParam) list = (va_list) &lastParam + sizeof(lastParam)
#else
#define va_start(list, lastParam) list = (va_list) &lastParam + sizeof(lastParam)
#endif //va_start
#ifdef va_arg
#undef va_arg
#define va_arg(list, type) *((type*)list); list = (va_list) list + sizeof(type)
#else
#define va_arg(list, type) *((type*)list)
#endif //va_arg
#ifdef va_end
#undef va_end
#define va_end(list) list = NULL
#endif //va_end

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
typedef functional_size_t functional_uintptr_t;
typedef functional_uintptr_t functional_ptrdiff_t;

//Simple constexpr bool. Though we don't use it ourselves. - xWhitey
template<bool _Condition> using constexpr_bool = integral_constant<bool, _Condition>;
/* Usage:
constexpr constexpr_bool<true == false> someConstexprBool;
*/

#ifndef FUNCTIONAL_NO_ALLOCATOR
inline namespace YouShouldNotUseThisFunctional {
	typedef struct CAllocatedSegment {
		Q_bool m_bIsFree;
		functional_size_t m_iSize;
		CAllocatedSegment* m_lpNext, *m_lpPrevious;
	} CAllocatedSegment;
}

typedef struct CAllocator {
	static CAllocator* Init() {
		static CAllocator allocator;
		allocator._m_lpSegments = (CAllocatedSegment*)allocator._m_acMemoryPool;
		allocator._m_lpSegments->m_bIsFree = Q_TRUE;
		allocator._m_lpSegments->m_iSize = FUNCTIONAL_HEAP_SIZE / FUNCTIONAL_BLOCK_SIZE;
		allocator._m_lpSegments->m_lpNext = nullptr;
		allocator._m_lpSegments->m_lpPrevious = nullptr;
		allocator._m_lpOldFreeSegment = nullptr;

		return &allocator;
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
		return reinterpret_cast<char*>(_Segment) + sizeof(CAllocatedSegment);
	}

	CAllocatedSegment* PtrToSegment(_In_ void* _Pointer) {
		return (CAllocatedSegment*)(reinterpret_cast<char*>(_Pointer) - sizeof(CAllocatedSegment));
	}

	void* Allocate(_In_ functional_size_t _Size) {
		int s = GetNumBlock(_Size + sizeof(CAllocatedSegment));
		CAllocatedSegment* it = SearchFreeSegment(this->_m_lpOldFreeSegment, s);
		if (!it) it = SearchFreeSegment(this->_m_lpSegments, s);
		if (!it) {
			return nullptr;
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

		if (segment->m_lpNext && segment->m_lpNext->m_bIsFree) MergeSegment(segment, segment->m_lpNext);
		if (segment->m_lpPrevious && segment->m_lpPrevious->m_bIsFree) MergeSegment(segment->m_lpPrevious, segment);
	}
private:
	char _m_acMemoryPool[FUNCTIONAL_HEAP_SIZE];
	CAllocatedSegment* _m_lpSegments;
	CAllocatedSegment* _m_lpOldFreeSegment;
} CAllocator;

static CAllocator* gs_lpAllocator = CAllocator::Init();

void* Q_malloc(_In_ functional_size_t _Size) {
	return gs_lpAllocator->Allocate(_Size);
}

void Q_free(_In_ void* _Pointer) {
	gs_lpAllocator->Free(_Pointer);
}
#endif //FUNCTIONAL_NO_ALLOCATOR

#ifdef FUNCTIONAL_NO_ALLOCATOR
#ifndef FUNCTIONAL_CUSTOM_MALLOC
#error You must provide custom malloc function in order to use FUNCTIONAL_NO_ALLOCATOR definition.
#endif //FUNCTIONAL_CUSTOM_MALLOC
#ifndef FUNCTIONAL_CUSTOM_FREE
#error You must provide custom free function in order to use FUNCTIONAL_NO_ALLOCATOR definition.
#endif //FUNCTIONAL_CUSTOM_FREE

void* Q_malloc(_In_ functional_size_t _Size) {
	return FUNCTIONAL_CUSTOM_MALLOC(_Size);
}

void Q_free(_In_ void* _Pointer) {
	FUNCTIONAL_CUSTOM_FREE(_Pointer);
}

#endif //FUNCTIONAL_NO_ALLOCATOR

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif //INT_MAX

#ifndef _CRT_HYBRIDPATCHABLE
#define _CRT_HYBRIDPATCHABLE
#endif //_CRT_HYBRIDPATCHABLE

/* Fake SAL */

#ifndef _In_
#define _In_
#endif //_In_
#ifndef _In_opt_
#define _In_opt_
#endif //_In_opt_
#ifndef _Out_
#define _Out_
#endif //_Out_
#ifndef _Out_opt_
#define _Out_opt_
#endif //_Out_opt_
#ifndef _Pre_maybenull_
#define _Pre_maybenull_
#endif //_Pre_maybenull_
#ifndef _Post_invalid_
#define _Post_invalid_
#endif //_Post_invalid_
#ifndef _Out_writes_bytes_all_
#define _Out_writes_bytes_all_
#endif //_Out_writes_bytes_all_
#ifndef _In_reads_bytes_
#define _In_reads_bytes_
#endif //_In_reads_bytes_
#ifndef _In_z_
#define _In_z_
#endif //_In_z_
#ifndef _In_reads_or_z_
#define _In_reads_or_z_
#endif //_In_reads_or_z_
#ifndef _Pre_notnull_
#define _Pre_notnull_
#endif //_Pre_notnull_
#ifndef _Success_
#define _Success_(_Expr)
#endif //_Success_
#ifndef _Always_
#define _Always_(_Expr)
#endif //_Always_
#ifndef _Post_z_
#define _Post_z_
#endif //_Post_z_
#ifndef _Printf_format_string_
#define _Printf_format_string_
#endif //_Printf_format_string_

/* Fake SAL end */

#ifdef FUNCTIONAL_FIX_FLTUSED_NOT_FOUND
extern "C" bool _fltused = false;
#endif //FUNCTIONAL_FIX_FLTUSED_NOT_FOUND

#ifndef BYTE_SIZE_IN_BITS
#define BYTE_SIZE_IN_BITS 8
#endif //BYTE_SIZE_IN_BITS

#ifndef CHAR_BIT
#define CHAR_BIT sizeof(char) * BYTE_SIZE_IN_BITS
#endif //CHAR_BIT

//(-)2147483647(e)
#define INT_STR_SIZE (sizeof(int) * CHAR_BIT / 3 + 3)
constexpr auto a = FLT_MAX;
//(-)3.402823466e+38F
#define FLOAT_STR_SIZE (sizeof(float) * CHAR_BIT / 3 + 18)

namespace
#ifdef FUNCTIONAL_DONT_USE_ANONYMOUS_NAMESPACE
functional
#endif //FUNCTIONAL_DONT_USE_ANONYMOUS_NAMESPACE
{
	void* Q_memcpy(_Out_writes_bytes_all_(_Size) void* _Dst, _In_reads_bytes_(_Size) const void* _Src, _In_ unsigned int _Size) {
		auto dest = static_cast<char*>(_Dst);
		auto source = static_cast<const char*>(_Src);

		if (dest && source) {
			while (_Size) {
				*dest++ = *source++;
				--_Size;
			}
		}

		return dest;
	}

	functional_size_t Q_strlen(_In_z_ const char* _Str) {
		const char* p = const_cast<char*>(&_Str[0]);

		functional_size_t count = 0;
		while (*p) {
			count++;
			p++;
		}

		return count;
	}

	char* Q_strcpy(_Always_(_Post_z_) _Out_ char* _Destination, _In_z_ const char* _Source) {
		if (!_Source) return _Destination;

		if (_Source) {
			const functional_size_t length = Q_strlen(_Source);
			for (int idx = 0; idx < length; idx++) {
				_Destination[idx] = _Source[idx];
			}
			_Destination[length + 1] = '\0';
		}

		return _Destination;
	}

	char* Q_strcat(_Out_ char* _Destination, _In_z_ const char* _Source) {
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
			if (*_Str1 == '\0') return(0);
		}

		return(Q_strlen(_Str1) < Q_strlen(_Str2) ? -1 : 1);
	}

	functional_size_t Q_strncmp(_In_reads_or_z_(_MaxCount) const char* _Str1, _In_reads_or_z_(_MaxCount) const char* _Str2, _In_ functional_unsigned_size_t _MaxCount) {
		while (_MaxCount && *_Str1 && (*_Str1 == *_Str2)) ++_Str1, ++_Str2, --_MaxCount;

		if (_MaxCount == 0) return(0);

		return(*(unsigned char*)_Str1 - *(unsigned char*)_Str2);
	}

	char* Q_strdup(_In_z_ const char* _Source) {
		const auto buffer = static_cast<char*>(Q_malloc(Q_strlen(_Source) + 1));

		Q_memcpy(buffer, _Source, Q_strlen(_Source));

		buffer[Q_strlen(_Source) + 1] = '\0';

		return(buffer);
	}

	int Q_abs(_In_ functional_size_t _Number) {
		return(_Number < 0 ? -_Number : _Number);
	}

	char* Q_itoa_internal(_Pre_notnull_ _Post_z_ char* _Dest, _In_ functional_unsigned_size_t _Size, _In_ int _Value) {
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
		const auto len = static_cast<size_t>(&buf[INT_STR_SIZE] - p);
		if (len > _Size) {
			return(NULL);
		}
		return(static_cast<char*>(Q_memcpy(_Dest, p, len)));
	}

	char* Q_itoa_octal(_In_ functional_size_t _Number) {
		const auto buffer = static_cast<char*>(Q_malloc(INT_STR_SIZE));

		functional_size_t modulo, octal = 0, idx = 1;

		while (_Number != 0) {
			modulo = _Number % 8;
			_Number = _Number / 8;
			octal = octal + (modulo * idx);
			idx = idx * 10;
		}

		return buffer;
	}

	char* Q_itoa(_In_ functional_size_t _Number) {
		const auto buffer = static_cast<char*>(Q_malloc(INT_STR_SIZE));
		Q_itoa_internal(buffer, INT_STR_SIZE, _Number);
		return(buffer);
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
			return(0);

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

		return(sign * a);
	}

	functional_size_t Q_pow(_In_ functional_size_t _Base, _In_ functional_size_t _Power) {
		if (_Power == 0) return(1);
		else if ((_Power % 2) == 0)
			return(Q_pow(_Base, _Power / 2) * Q_pow(_Base, _Power / 2));
		else
			return(_Base * Q_pow(_Base, _Power / 2) * Q_pow(_Base, _Power / 2));
	}

	char* Q_ftoa(_In_ float _Value, _In_opt_ functional_size_t _Precision = 2) {
		const auto buffer = static_cast<char*>(Q_malloc(FLOAT_STR_SIZE));
		int b, l, i = 0;
		if (_Value < 0.f) {
			buffer[i++] = '-';
			_Value *= -1;
		}
		int a = (int) _Value;
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
			b = (int) _Value;
			buffer[i++] = b + '0';
			_Value -= b;
		}

		buffer[i] = '\0';

		return (buffer);
	}

	functional_size_t Q_toupper(_In_ functional_size_t _C) {
		if (_C >= 'a' && _C <= 'z') {
			_C -= ('a' - 'A');
		}

		return (_C);
	}

	char Q_toupper(_In_ char _C) {
		if (_C >= 'a' && _C <= 'z') {
			_C -= ('a' - 'A');
		}

		return (_C);
	}

	char* Q_itohexa_helper(_In_ char* _Dest, _In_ uintptr_t _Val) {
		if (_Val >= 16) {
			_Dest = Q_itohexa_helper(_Dest, _Val / 16);
		}
		*_Dest++ = "0123456789abcdef"[_Val & 15];
		return (_Dest);
	}

	char* Q_itohexa(_In_ uintptr_t _Val) {
		const auto buffer = static_cast<char*>(Q_malloc(32));
		*Q_itohexa_helper(buffer, _Val) = '\0';
	
		return (buffer);
	}

	char* Q_itohexa_upper(_In_ uintptr_t _Val) {
		char* buffer = Q_itohexa(_Val);
		const int len = Q_strlen(buffer);

		for (int idx = 0; idx < len; idx++) {
			buffer[idx] = Q_toupper(buffer[idx]);
		}

		return buffer;
	}

	template<class... _Ts> _Success_(return >= 0) functional_size_t Q_sprintf(_Always_(_Post_z_) _Out_ char* const _Buffer,
		_Printf_format_string_ _In_z_ const char* const _Format, _In_opt_ _Ts... _Args) {
		if constexpr (sizeof...(_Args) > 0) {
			const auto toBeFreed = Q_malloc(sizeof...(_Args) * sizeof(void*));
			const auto args = static_cast<void**>(toBeFreed);
			int thiz = 0;

			([&]() {
				if constexpr (is_pointer_v<_Ts> || is_same_v<_Ts, const char*>) {
					args[thiz] = (void*)(_Args);
					++thiz;
				} else {
					args[thiz] = reinterpret_cast<void*>(&_Args);
					++thiz;
				}
			}(), ...);

			auto p = const_cast<char*>(&_Format[0]);
			int fmtCnt = 0;
			for (; *p != '\0'; ++p) {
				if (*p == '%') fmtCnt++;
			}
			if (fmtCnt != sizeof...(_Args)) {
				Q_strcpy(_Buffer, "Q_sprintf: Not all arguments/fmt specifiers are present.\n");
				Q_free(toBeFreed);
				return -2;
			}

			p = const_cast<char*>(&_Format[0]);

			int writtenChars = 0;
			thiz = 0;

			const auto buffer = static_cast<char*>(Q_malloc(2048));
			for (; *p != '\0'; ++p, ++writtenChars) {
				if (*p == '%') {
					++p;
					switch (*p) {
					case 'u': 
					case 'i':
					case 'd': {
						const auto it = *static_cast<functional_size_t*>(args[thiz]);
						const char* converted = Q_itoa(it);
						Q_strcat(buffer, converted);
						writtenChars += Q_strlen(converted);
						++thiz;
						++p;
					}
							break;
					case 'f': {
						const auto it = *static_cast<float*>(args[thiz]);
						const char* converted = Q_ftoa(it, 6);
						if (it < 1.f) Q_strcat(buffer, "0");
						Q_strcat(buffer, converted);
						if (it < 1.f) writtenChars++;
						writtenChars += Q_strlen(converted);
						++thiz;
						++p;
					}
							break;
					case 'c': {
						buffer[writtenChars] = *static_cast<char*>(args[thiz]);
						++writtenChars;
						++thiz;
						++p;
					}
							break;
					case 'x': {
						if (const auto num = *static_cast<unsigned int*>(args[thiz])) {
							const char* str = Q_itohexa(num);
							writtenChars += Q_strlen(str);
							Q_strcat(buffer, str);
						} else {
							writtenChars += sizeof("0x0") - 1;
							Q_strcat(buffer, "0x0");
						}
						++thiz;
						++p;
					}
							break;
					case 'X': {
						if (const auto num = *static_cast<unsigned int*>(args[thiz])) {
							const char* str = Q_itohexa_upper(num);
							writtenChars += Q_strlen(str);
							Q_strcat(buffer, str);
						}
						else {
							writtenChars += sizeof("0x0") - 1;
							Q_strcat(buffer, "0x0");
						}
						++thiz;
						++p;
					}
							break;
					case 's': {
						const auto str = reinterpret_cast<const char*>(args[thiz]);
						writtenChars += Q_strlen(str);
						Q_strcat(buffer, str);
						++thiz;
						++p;
					}
							break;
					case 'p': {
						if (const auto addr = reinterpret_cast<uintptr_t>(reinterpret_cast<void*>(args[thiz]))) {
							const char* str = Q_itohexa(addr);
							writtenChars += Q_strlen(str);
							Q_strcat(buffer, str);
						}
						else {
							writtenChars += sizeof("(null)") - 1;
							Q_strcat(buffer, "(null)");
						}
						++thiz;
						++p;
					}
							break;
					default: {
						buffer[writtenChars] = *p;
						++writtenChars;
					}
						   break;
					}
				}
				buffer[writtenChars] = *p;
			}

			buffer[writtenChars + 1] = '\0';

			Q_memcpy(_Buffer, buffer, Q_strlen(buffer) + 1);
			Q_free(toBeFreed);
			Q_free(buffer);

			return writtenChars;
		} else {
			if (_Format) {
				const auto toWrite = Q_strlen(_Format);
				*(_Buffer + toWrite + 1) = '\0';
				Q_memcpy(_Buffer, _Format, toWrite);
				return toWrite;
			}
		}

		return -1;
	}
}

#else //__cplusplus
#error C++ compiler required to compile functional.hpp.
#endif //__cplusplus

#endif //FUNCTIONAL_HPP_GUARD

#undef FUNCTIONAL_HPP_RECURSE_GUARD
#endif //FUNCTIONAL_HPP_RECURSE_GUARD