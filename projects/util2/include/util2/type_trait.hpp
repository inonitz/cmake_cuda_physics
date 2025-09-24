#ifndef __UTIL2_BASE_TYPE_TRAIT_DEFINITIONS__
#define __UTIL2_BASE_TYPE_TRAIT_DEFINITIONS__
#include "util2/C/base_type.h"


namespace util2::type_trait {


template<bool condition_t, typename TypeA, typename TypeB> 
struct conditional_operator{};
template<typename TypeA, typename TypeB> 
struct conditional_operator<false, TypeA, TypeB>{ using type = TypeB; };
template<typename TypeA, typename TypeB> 
struct conditional_operator<true,  TypeA, TypeB>{ using type = TypeA; };

template<typename T> struct is_integral_type {};
template<> struct is_integral_type<u64> { static constexpr bool value = true; };
template<> struct is_integral_type<u32> { static constexpr bool value = true; };
template<> struct is_integral_type<u16> { static constexpr bool value = true; };
template<> struct is_integral_type<u8>  { static constexpr bool value = true; };
template<> struct is_integral_type<i64> { static constexpr bool value = true; };
template<> struct is_integral_type<i32> { static constexpr bool value = true; };
template<> struct is_integral_type<i16> { static constexpr bool value = true; };
template<> struct is_integral_type<i8>  { static constexpr bool value = true; };
template<> struct is_integral_type<f32> { static constexpr bool value = false; };
template<> struct is_integral_type<f64> { static constexpr bool value = false; };

template<typename T, typename U> struct is_same_type       { static constexpr bool value = false; };
template<typename T>             struct is_same_type<T, T> { static constexpr bool value = true;  };


template<class T> struct remove_const_qualifier          { typedef T type; };
template<class T> struct remove_const_qualifier<const T> { typedef T type; };

} /* namespace util2::type_trait */


template<typename T> using ref 		 = typename util2::type_trait::conditional_operator<sizeof(T) <= 8, T, T&		>::type;
template<typename T> using const_ref = typename util2::type_trait::conditional_operator<sizeof(T) <= 8, T, T const&>::type;
template<typename T> using value_ptr = typename util2::type_trait::conditional_operator<sizeof(T) <= 8, T, T*>::type;
template<typename T> using imut_type_handle = T const*;
template<typename T> using mut_type_handle  = T*;
template<typename T> using remove_const = typename util2::type_trait::remove_const_qualifier<T>::type;
template<typename T> using immutable_readonly_ptr = const T* const;


typedef imut_type_handle<char_t> k_char;
typedef imut_type_handle<byte>   k_byte;


#endif /* __UTIL2_BASE_TYPE_TRAIT_DEFINITIONS__ */