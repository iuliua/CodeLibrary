#pragma once
namespace Tools
{
    inline static bool DoubleEqual(double d1, double d2)
    {
        return (abs(d1 - d2) < DBL_EPSILON);
    }
    inline static bool DoubleNotEqual(double d1, double d2)
    {
        return !DoubleEqual(d1, d2);
    }
    inline static bool DoubleNotZero(double d1)
    {
        return !DoubleEqual(d1, 0.0);
    }
    inline static bool DoubleEqualZero(double d1)
    {
        return DoubleEqual(d1, 0.0);
    }
};