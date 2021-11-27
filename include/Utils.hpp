#pragma once

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <array>
#include <functional>
#include <numeric>


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

namespace sp{ // BEGINING OF NAMESPACE //////////////////////////////////////////////////////////////////

template<class T>
using numl = std::numeric_limits<T>;


// CONDITIONAL ARITMETIC
template<uint32_t size> struct IntOfGivenSize;
template<> struct IntOfGivenSize<1>{ typedef int8_t type; };
template<> struct IntOfGivenSize<2>{ typedef int16_t type; };
template<> struct IntOfGivenSize<4>{ typedef int32_t type; };
template<> struct IntOfGivenSize<8>{ typedef int64_t type; };

template<uint32_t size> struct UIntOfGivenSize;
template<> struct UIntOfGivenSize<1>{ typedef uint8_t type; };
template<> struct UIntOfGivenSize<2>{ typedef uint16_t type; };
template<> struct UIntOfGivenSize<4>{ typedef uint32_t type; };
template<> struct UIntOfGivenSize<8>{ typedef uint64_t type; };

template<class T>
inline constexpr T enable(const bool condition, const T value);

template<class T>
inline constexpr T choose(const bool condition, const T valueOnTrue, const T valueOnFalse);


// CONVERTING UNIONS
union ConvF{
	uint32_t i;
	float f;
};
union ConvUninonD{
	uint64_t i;
	double d;
};
template<class T>	// CONVERT TYPE TO AN UNSIGNED INTEGER
union ConvUnion{
	T t;
	typename UIntOfGivenSize<sizeof(T)>::type u;
};



template<class T>
inline constexpr void swap(T *const x, T *const y) noexcept;

template<class T>
inline constexpr T sign(const T x);

template<class T>
inline constexpr int signInt(const T x);

inline constexpr int factorial(int n);

template<class T>
inline constexpr void clamp(const T &min, T *const x, const T &max);

template<class T>
inline constexpr T clamp(const T &min, const T &x, const T &max);

template<class T>
inline constexpr void clamp(const T &min, T *const x, const T &max, bool (*compare)(T, T));

template<class T>
inline constexpr T clamp(const T &min, T const &x, const T &max, bool (*compare)(T, T));


// FLOATING POINTS
inline constexpr int32_t getMantissa(const float x);
inline constexpr int32_t getExponent(const float x);

inline constexpr int64_t getMantissa(const double x);
inline constexpr int64_t getExponent(const double x);


// TYPE ERASURES
template<class Signature>
struct FunctionRef;

template<class Res, class... Args>
struct FunctionRef<Res(Args...)>{
	inline constexpr FunctionRef() noexcept : functionPtr{nullptr}, objectPtr{nullptr} {}
	FunctionRef(const FunctionRef &) = default;
	FunctionRef(FunctionRef &&) = default;
	FunctionRef &operator =(const FunctionRef &) = default;
	FunctionRef &operator =(FunctionRef &&) = default;
	~FunctionRef() = default;

	template<class Func>
	inline constexpr FunctionRef(Func *const func) noexcept;

	template<class Func>
	inline constexpr FunctionRef &operator =(Func *const func) noexcept;

	inline constexpr bool isStateless() const noexcept{ return !objectPtr; }

	inline constexpr Res operator ()(Args... args) noexcept;

private:
	void *functionPtr;
	void *objectPtr;
};


// FAST MATH
struct rand32{
	inline constexpr rand32() noexcept;
	inline constexpr rand32(const uint32_t seed) noexcept;
	inline constexpr uint32_t min() const;
	inline constexpr uint32_t max() const;
	uint32_t operator ()() noexcept;

	typedef uint32_t result_type;
	uint32_t seed;
};

inline float qLog(const float x);

inline float qExp(const float x);

inline float qSqrt(const float x);

inline float qInvSqrt(const float x);

// MATH
inline constexpr uint32_t roundUpTo2Power(uint32_t x);

inline constexpr uint32_t roundDownTo2Power(uint32_t x);


// HEAP
template<class It>
inline void repairHeap(It begin, const It end, const int startingIndex = 0);

template<class It, class Compare>
inline void repairHeap(It begin, const It end, Compare comapre, const int startingIndex = 0);

template<class It>
inline void makeHeap(It begin, const It end);

template<class It, class Compare>
inline void makeHeap(It begin, const It end, Compare compare);


// RANGE
namespace priv__{
	template<class T>
	class RangeClass;

	template<class T>
	class RangeRClass;
} // END OF NAMESPACE PRIV

template<class T>
priv__::RangeClass<T> Range(const T first, const T last, const T step = (T)1);

template<class T>
priv__::RangeRClass<T> RangeR(const T first, const T last, const T step = (T)1);


// TYPE TRAITS
namespace priv__{
template<bool condition, class T1, class T2> struct TypeTernary;
template<class T1, class T2> struct TypeTernary<true, T1, T2> { typedef T1 type; };
template<class T1, class T2> struct TypeTernary<false, T1, T2> { typedef T2 type; };
}
template<bool condition, class on_true_t, class on_false_t>
using TypeTernary = priv__::TypeTernary<condition, on_true_t, on_false_t>;

template<bool condition, auto V1, auto V2> struct StaticTernary;
template<auto V1, auto V2> struct StaticTernary<true, V1, V2>{ static constexpr auto value = V1; };
template<auto V1, auto V2> struct StaticTernary<false, V1, V2>{ static constexpr auto value = V2; };

template<class T>
constexpr bool isTriviallyPassabe = sizeof(T)<=16 && std::is_trivially_constructible_v<T>;

template<class T>
using Optimal = sp::TypeTernary<sizeof(T) <= 16, const T, const T &>;



// GETTING SINGLE ARGUMENT FROM ARGUMENT PACK
template<uint32_t index, class Arg0_t, class... Args_t>
constexpr const auto &getArg(const Arg0_t &arg0, const Args_t &...args){
	if constexpr (index)
		return getArg<index-1>(args...);
	else
		return arg0;
}
template<uint32_t index, class Arg0_t>
constexpr const auto &getArg(const Arg0_t &arg0){ return arg0; }

template<uint32_t index, class Arg0_t, class... Args_t>
constexpr auto &getArg(Arg0_t &arg0, Args_t &...args){
	if constexpr (index)
		return getArg<index-1>(args...);
	else
		return arg0;
}
template<uint32_t index, class Arg0_t>
constexpr auto &getArg(Arg0_t &arg0){ return arg0; }





// CONDITIONAL ARITMETIC
template<class T>
inline constexpr T enable(const bool condition, const T value){
	static_assert(std::is_arithmetic<T>());
	return -(typename IntOfGivenSize<roundUpTo2Power(sizeof(T))>::type)condition & value;
}

template<class T>
inline constexpr T choose(const bool condition, const T valueOnTrue, const T valueOnFalse){
	static_assert(std::is_arithmetic<T>());
	return (-(typename IntOfGivenSize<roundUpTo2Power(sizeof(T))>::type)condition & valueOnTrue) |
		(((typename IntOfGivenSize<roundUpTo2Power(sizeof(T))>::type)condition-1) & valueOnFalse);
}


// PARTITION
template<class T>
constexpr T *partition(T *const first, T *last, T *const pivot){
	if (first >= last) return first;
	sp::swap(first, pivot);
	
	T lastVal = std::move(*--last);
	*last = *first;
	
	T *It = first;
	for (;;){
		do ++It; while (*It < *first);
		*last = std::move(*It);
		do --last; while (*first < *last);
		if (It >= last) break;
		*It = std::move(*last);
	}
	if (It == last+2){
		*It = std::move(*(last+1));
		--It;
	}
	{
		T *const partitionPoint = It - (*first < lastVal);
		*It = std::move(lastVal);
		sp::swap(first, partitionPoint);
		return partitionPoint;
	}
}





// RADIX SORT FOR UNSIGNED TYPES
template<class T, size_t baseBits = 8>
void radixLSD(T *const first, T *const last){
	static_assert(baseBits);
	constexpr size_t base = 1 << baseBits;
	constexpr size_t bitMask = base - 1;
	
	uint32_t counts[base];
	std::vector<T> buffer(last-first);

	using IT = typename sp::UIntOfGivenSize<sp::roundUpTo2Power(sizeof(T))>::type;

	for (IT step=0; step!=sizeof(T)*8/baseBits; ++step){
		memset(counts, 0, sizeof(counts));
		if (step & 1){
			for (T *It=&*std::begin(buffer); It!=&*std::end(buffer); ++It)
				++counts[(*(IT *)It >> step*baseBits) & bitMask];

			for (IT *C=std::begin(counts)+1; C!=std::end(counts); ++C) *C += *(C-1);
			
			for (T *It=&*std::end(buffer)-1; It!=&*std::begin(buffer)-1; --It)
				first[--counts[(*(IT *)It >> step*baseBits) & bitMask]] = *It;

		} else{
			for (T *It=first; It!=last; ++It)
				++counts[(*(IT *)It >> step*baseBits) & bitMask];

			for (IT *C=std::begin(counts)+1; C!=std::end(counts); ++C) *C += *(C-1);
			
			for (T *It=last-1; It!=first-1; --It)
				buffer[--counts[(*(IT *)It >> step*baseBits) & bitMask]] = *It;
		}
	}
}



template<class T>
inline constexpr void swap(T *const x1, T *const x2) noexcept{
	const T temp = std::move(*x1);
	*x1 = std::move(*x2);
	*x2 = std::move(temp);
}

template<class T>
inline constexpr T sign(const T x){
	return (1-(signbit(x)<<1))*(x!=(T)0);
}

template<class T>
inline constexpr int signInt(const T x){
	return (1-(signbit(x)<<1))*(x!=(T)0);
}

inline constexpr int factorial(int n){
	for (int i=n-1; i>1; --i)
		n *= i;
	return n;
}

template<class T>
inline constexpr void clamp(const T &min, T *const x, const T &max){
	*x = *x<max ? (*x>min ? *x : min) : max;
}

template<class T>
inline constexpr T clamp(const T &min, const T &x, const T &max){
	return x<max ? (x>min ? x : min) : max;
}

template<class T>
inline constexpr void clamp(const T &min, T *const x, const T &max, bool (*compare)(T, T)){
	*x = compare(*x, max) ? (compare(*x, min) ? *x : min) : max;
}

template<class T>
inline constexpr T clamp(const T &min, T const &x, const T &max, bool (*compare)(T, T)){
	return compare(x, max) ? (compare(x, min) ? x : min) : max;	
}

// FLOATING POINTS
inline constexpr int32_t getMantissa(const float x){
	if (signbit(x))
		return -((1<<23) | (*(int32_t *)&x & 0x007fffff));
	return (1<<23) | (*(int32_t *)&x & 0x007fffff);
}

inline constexpr int32_t getExponent(const float x){
	return ((*(int32_t *)&x & 0x7fffffff) >> 23) - 0x7f;
}

inline constexpr int64_t getMantissa(const double x){
	if (signbit(x))
		return -((1ll<<52) | (*(int64_t *)&x & 0x000fffffffffffff));
	return (1ll<<52) | (*(int64_t *)&x & 0x000fffffffffffff);
}

inline constexpr int64_t getExponent(const double x){
	return ((*(int64_t *)&x & 0x7fffffffffffffff) >> 52) - 0x3ff;
}


// TYPE ERASURES
#define TPL template<class Res, class... Args>
#define CLS FunctionRef<Res(Args...)>

TPL template<class Func>
inline constexpr CLS::FunctionRef(Func *const func) noexcept{
	static_assert(std::is_invocable<Func, Args...>(), "Function's arguments don't match with thoose declared in template parameters.");
	if constexpr (std::is_function<Func>()){
		functionPtr = (void *)func;
		objectPtr = nullptr;
	
	} else{

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"

		functionPtr = (void *)(Res (Func::*)(Args...))&Func::operator();
		
#pragma GCC diagnostic pop

		objectPtr = func;
	}
}

TPL template<class Func>
inline constexpr CLS &CLS::operator =(Func *const func) noexcept{
	if constexpr (std::is_function<Func>()){
		objectPtr = nullptr;
		functionPtr = (void *)func;
	}
	if constexpr (std::is_class<Func>()){

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
		functionPtr = (void *)(Res (Func::*)(Args...))&Func::operator();
#pragma GCC diagnostic pop

		objectPtr = func;
	}
}

TPL inline constexpr Res CLS::operator ()(Args... args) noexcept{
	if (objectPtr) 
		return ((Res (*)(void *, Args...))functionPtr)(objectPtr, args...);
	else
		return ((Res (*)(Args...))functionPtr)(args...);
}

#undef CLS
#undef TPL


// FAST MATH
inline constexpr rand32::rand32() noexcept : seed{0} {}

inline constexpr rand32::rand32(const uint32_t firstSeed) noexcept : seed{firstSeed} {}

inline constexpr uint32_t rand32::min() const{
	return 0;
}

inline constexpr uint32_t rand32::max() const{
	return std::numeric_limits<uint32_t>::max();
}

uint32_t rand32::operator ()() noexcept{
	seed += 0xe120fc15;
	uint64_t temp = (uint64_t)seed * 0x4a39b70d;
	temp = (uint64_t)((temp >> 32) ^ temp) * 0x12fad5c9;
	return (temp >> 32) ^ temp;
}


inline float qLog(const float x){
	constexpr float scaleDown = 1.f/(float)0x00800000;
	constexpr ConvF one{.f = 1.f};
	return (float)(*(uint32_t *)&x - one.i)*scaleDown;
}

inline float qExp(const float x){
	constexpr float scaleUp = 0x00800000;
	constexpr ConvF one{.f = 1.f};
	const ConvF res{.i = (uint32_t)(x*scaleUp) + one.i};
	return res.f;
}

inline float qPow(const float x, const float y){
	constexpr ConvF one{.f = 1.f};
	ConvF res{.i = (uint32_t)((float)(*(uint32_t *)&x - one.i)*y) + one.i};
	return res.f;
}

inline float qSqrt(const float x){
	ConvF res{.f = x};
	res.i -= 1<<23;
	res.i >>= 1;
	res.i += 1<<29;
	return (res.f + x/res.f)*0.5f;
}

inline float qInvSqrt(const float x){
	ConvF res{.i = (uint32_t)0x5f3759df - (*(uint32_t *)&x>>1)};
	return res.f * (1.5f - 0.5f*x*res.f*res.f);
}


// MATH

template<class T>
inline constexpr T intSqrt(T x) noexcept{
	static_assert(std::is_integral<T>(), "Argument must be a integer.");
	T res = 0;
	T one = 1 << 30;

	while (one > x)
		one >>= 2;

	while (one){
		const T cond = -(x >= res + one);
		x -= (res + one) & cond;
		res += (one << 1) & cond;

		res >>= 1;
		one >>= 2;
	}
	return res;
}

inline constexpr uint32_t roundUpTo2Power(uint32_t x){
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

inline constexpr uint32_t roundDownTo2Power(uint32_t x) {
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x - (x >> 1);
}

inline constexpr uint32_t countOneBits(uint32_t x) {
	x -= (x >> 1) & 0x55555555;
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	x = (x + (x >> 4)) & 0x0F0F0F0F;
	x += x >> 8;
	x += x >> 16;
	return x & 0x0000003F;
}

// HEAP
template<class It>
inline void repairHeap(It begin, const It end, const int startingIndex){
	It child;
	for(int i=startingIndex; (child=begin+((i<<1)|1)) < end; i=child-begin){
		child += child[0] < child[child+1 != end];
		
		if (child[0] < begin[i])
			return;

		sp::swap(begin+i, child);
	}
}

template<class It, class Compare>
inline void repairHeap(It begin, const It end, Compare comapre, const int startingIndex){
	It child;
	for(int i=startingIndex; (child=begin+((i<<1)|1)) < end; i=child-begin){
		child += child[0] < child[child+1 != end];
		
		if (child[0] < begin[i])
			return;

		sp::swap(begin+i, child);
	}
}

template<class It>
inline void makeHeap(It begin, const It end){
	for (int i=(end-begin-1)>>1; i>=0; --i)
		repairHeap(begin, end, i);
}

template<class It, class Compare>
inline void makeHeap(It begin, const It end, Compare compare){
	for (int i=(end-begin-1)>>1; i>=0; --i)
		repairHeap(begin, end, compare, i);
}


// RANGES
namespace priv__{

template<class T>
struct RangeClass{
	struct It : public std::iterator<std::forward_iterator_tag, T>{
		inline constexpr T operator *() noexcept{ return i; }
		inline constexpr const T &operator ++() { i += step; return i; }
		inline constexpr bool operator !=(const T rhs){ return i <= rhs; }

		T i;
		T step;
	};

	inline constexpr It begin(){
		return It{{}, firstElement, stepSize};
	}
	inline constexpr T end(){ 
		return lastElement;
	}

	T firstElement;
	T lastElement;
	T stepSize;
};

template<class T>
struct RangeRClass{
	struct It : public std::iterator<std::forward_iterator_tag, T>{
		inline constexpr T operator *() noexcept{ return i; }
		inline constexpr const T &operator ++() { i -= step; return i; }
		inline constexpr bool operator !=(const T rhs){ return i >= rhs; }

		T i;
		T step;
	};

	inline constexpr It begin(){
		return It{{}, firstElement, stepSize};
	}
	inline constexpr T end(){ 
		return lastElement;
	}

	T firstElement;
	T lastElement;
	T stepSize;
};

} // END OF NAMESPACE PRIV

template<class T>
priv__::RangeClass<T> Range(const T first, const T last, const T step){
	static_assert(std::is_integral<T>() || std::is_floating_point<T>(), "Range must be made of numbers.");
	return priv__::RangeClass<T>{first, last, step};
};

template<class T>
priv__::RangeRClass<T> RangeR(const T first, const T last, const T step){
	static_assert(std::is_integral<T>() || std::is_floating_point<T>(), "Range must be made of numbers.");
	return priv__::RangeRClass<T>{first, last, step};
};

}	// END OF NAMESPACE	///////////////////////////////////////////////////////////////////