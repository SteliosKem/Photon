#pragma once
#include "general.h"

class Interval {
    public:
    double min, max;
    Interval() : min(+infinity), max(-infinity) {}

    Interval(double _min, double _max) : min(_min), max(_max) {}

    double size() const {return max - min;}
    bool contains(double x) {return x >= min && x <= max;}
    bool surrounds(double x) {return x > min && x < max;}
    double clamp(double x) const {
        if(x < min) return min;
        if(x > max) return max;
        return x;
    }

    static const Interval empty, universe;
};

