#pragma once
#include "float.h"
namespace Tools
{
    inline static bool DoubleEqual(double d1, double d2,double EPSILON=DBL_EPSILON)
    {
        return (abs(d1 - d2) < DBL_EPSILON);
    }
    inline static bool DoubleNotEqual(double d1, double d2,double EPSILON=DBL_EPSILON)
    {
        return !DoubleEqual(d1, d2,EPSILON);
    }
    inline static bool DoubleNotZero(double d1)
    {
        return !DoubleEqual(d1, 0.0);
    }
    inline static bool DoubleEqualZero(double d1)
    {
        return DoubleEqual(d1, 0.0);
    }
    inline static bool DoubleGreater(double d1, double d2)
    {
        if ((d1 - d2) > DBL_EPSILON)
            return true;
        else
            return false;
    }
    inline static bool DoubleGreaterOrEqual(double d1, double d2)
    {
        return (DoubleGreater(d1, d2) || DoubleEqual(d1, d2));
    }
};