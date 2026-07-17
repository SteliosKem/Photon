#pragma once
#include "Utility.h"

class Interval {
public:
    Interval() : m_min(INF), m_max(-INF) {}
    Interval(double min, double max) : m_min(min), m_max(max) {}

    double size() const { return m_max - m_min; }
    bool contains(double x) const { return m_min <= x && x <= m_max; }
    bool surrounds(double x) const { return m_min < x && x < m_max; }

    const static Interval empty;
    const static Interval R;
private:
    double m_min;
    double m_max;
};

inline const Interval Interval::empty = Interval();
inline const Interval Interval::R = Interval(-INF, INF);