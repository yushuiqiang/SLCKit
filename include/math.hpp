
#ifndef MATH_HPP
#define MATH_HPP

#include "slckit_global.h"
#include <cmath>

// constants
// cmath of msvc seems have no NaN
/**
 * @brief 非数 (Not a Number) 常量, 若编译器支持, 则使用编译器内部版本
 */
#ifndef NAN
// MSDN reference
static const unsigned long nan[2]={0xffffffff, 0x7fffffff};
static const double NAN = *(double *)nan;
#endif

// we have wider range of precision
/**
 * @brief 软件精度常量, 本系统为 1e-4
 */
static const double PREC = 1e-4;
static const double PREC_RANGE = 1e4;

// common constants
// cmath of msvc seems have no PI
/**
 * @brief 圆周率常量
 */
#ifndef M_PI
static const double PI = 3.14159265358979323846;
#else
static const double PI = M_PI;
#endif

//functions

// not using qFuzzyIsNull because it doesn't support int :)
/**
 * @brief 比较指定值是否接近于0 (与 PREC 参数比较)
 * @param value 指定值
 * @param precision 比较精度, 默认为 PREC. @see PREC
 * @return 若指定值小于比较精度的绝对值，返回true，否则返回 false
 */
static inline bool fuzzyIsNull (const double value, const double precision = PREC)
{
    return  (std::abs (value) < precision);
}

static inline bool fuzzyIsEqual (const double value, const double other, const double precision = PREC)
{
    return (std::abs (value - other) < precision);
}

#endif // MATH_HPP
