#pragma once

#define LPOINT_RENAME_COMPONENTS 1

#include <cmath>
#include <algorithm>
#include <vector>
#include <iterator>
#include <memory>
#include <intrin.h>
#include "AE_Effect.h"
#include "AE_GeneralPlug.h"
#include "DrawLine_Pixel.h"
#include "DrawLine_Util.h"

using std::min;
using std::max;
using std::abs;

#ifdef _MSC_VER
#define ALIGN(n) __declspec(align(n))
#else
#define ALIGN(n) __attribute__((aligned(n)))
#endif

ALIGN(16) struct Vector2 {
public:
    PF_FpLong x;
    PF_FpLong y;

    Vector2(const PF_FpLong x, const PF_FpLong y) : x(x), y(y) { }

    Vector2(const AEGP_TwoDVal &p) : x(p.x), y(p.y) { }

    Vector2() : x(0.0), y(0.0) { }

    Vector2(const Vector2 &v) : x(v.x), y(v.y) { }

    inline Vector2 Normalize() const;

    inline Vector2 Invert() const;

    inline Vector2 Add(const Vector2 &v) const;

    inline Vector2 Sub(const Vector2 &v) const;

    inline Vector2 Multiple(const PF_FpLong s) const;

    inline PF_FpLong Dot(const Vector2 &v) const;

    inline PF_FpLong Length() const;

    inline PF_FpLong DistanceTo(const Vector2 &v) const;

    Vector2& operator=(const Vector2 &v) {
        this->x = v.x;
        this->y = v.y;
        return *this;
    }

    inline bool operator == (const Vector2 &v) {
        return this->x == v.x && this->y == v.y;
    }

    inline bool operator != (const Vector2 &v) {
        return this->x != v.x || this->y != v.y;
    }
};

inline Vector2 Vector2::Normalize() const
{
    PF_FpLong l = Length();
    if (!isnan(l)) {
        return Vector2(this->x / l, this->y / l);
    } else {
        return Vector2();
    }
}

inline Vector2 Vector2::Invert() const
{
    return Vector2(-this->x, -this->y);
}

inline Vector2 Vector2::Add(const Vector2 &v) const
{
    return Vector2(this->x + v.x, this->y + v.y);
}

inline Vector2 Vector2::Sub(const Vector2 &v) const
{
    return Vector2(this->x - v.x, this->y - v.y);
}

inline Vector2 Vector2::Multiple(const PF_FpLong s) const
{
    return Vector2(this->x * s, this->y * s);
}

inline PF_FpLong Vector2::Dot(const Vector2 &v) const
{
    return this->x * v.x + this->y * v.y;
}

inline PF_FpLong Vector2::Length() const
{
    return sqrt(Dot(*this));
}

inline PF_FpLong Vector2::DistanceTo(const Vector2 &v) const
{
    PF_FpLong tx = this->x - v.x;
    PF_FpLong ty = this->y - v.y;
    return sqrt(tx * tx + ty * ty);
}

struct VertexInfo {
public:
    PF_FpLong width;
    Pixel color;

    VertexInfo() : width(0.0), color(Pixel()) { }

    VertexInfo(const PF_FpLong width, const Pixel &color) : width(width), color(color) { }

    VertexInfo(const VertexInfo &v) : width(v.width), color(v.color) { }

    static inline VertexInfo Interpolate(const VertexInfo v1, const VertexInfo v2, const PF_FpLong t);

    VertexInfo& operator=(const VertexInfo &v) {
        this->width = v.width;
        this->color = v.color;
        return *this;
    }
};

struct ParameterizedVertex {
public:
    Vector2 point;
    VertexInfo info;
    PF_FpLong time;

    ParameterizedVertex() : point(), info(), time(0.0) { }

    ParameterizedVertex(const Vector2 &point, const VertexInfo info, const PF_FpLong time) :
        point(point), info(info), time(time) { };

    ParameterizedVertex(const ParameterizedVertex &v) :
        point(v.point), info(v.info), time(v.time) { }

    ParameterizedVertex& operator=(const ParameterizedVertex &v) {
        this->point = v.point;
        this->info = v.info;
        this->time = v.time;
        return *this;
    }
};

struct HitInfo {
public:
    VertexInfo info;
    PF_FpLong time;
    PF_FpLong distance;

    static HitInfo NoHit;

    HitInfo() : info(VertexInfo()), time(0.0), distance(0.0) { }

    HitInfo(const VertexInfo &info, const PF_FpLong time, const PF_FpLong distance) :
        info(info), time(time), distance(distance) { }

    HitInfo(const HitInfo &h) : info(h.info), time(h.time), distance(h.distance) { }

    HitInfo& operator=(const HitInfo &h) {
        this->distance = h.distance;
        this->time = h.time;
        this->info = h.info;
        return *this;
    }
};

ALIGN(16) struct Rect {
public:
    const PF_FpLong left;
    const PF_FpLong top;
    const PF_FpLong right;
    const PF_FpLong bottom;

    Rect(const PF_FpLong left, const PF_FpLong top, const PF_FpLong right, const PF_FpLong bottom) :
        left(min(left, right)), top(min(top, bottom)), right(max(left, right)), bottom(max(top, bottom)) { }

    inline bool inPoint(const Vector2 &v) const;

    inline Rect Expand(const PF_FpLong size) const;

    inline Rect Intersect(const Rect &r) const;

    inline Rect Quantize() const;
};

inline bool Rect::inPoint(const Vector2 &v) const
{
    __m128d vec = _mm_load_pd((double*)&v);
    __m128d minPair = _mm_load_pd((double*)this);
    __m128d maxPair = _mm_load_pd(((double*)this) + 2);
    __m128d comp = _mm_or_pd(_mm_cmpge_pd(minPair, vec), _mm_cmple_pd(maxPair, vec));
    return _mm_testz_pd(comp, comp) != 0;
}

inline Rect Rect::Expand(PF_FpLong size) const
{
    return Rect(
        this->left - size,
        this->top - size,
        this->right + size,
        this->bottom + size
    );
}

inline Rect Rect::Intersect(const Rect &r) const
{
    return Rect(
        min(this->left, r.left),
        min(this->top, r.top),
        max(this->right, r.right),
        max(this->bottom, r.bottom)
    );
}

inline Rect Rect::Quantize() const
{
    return Rect(
        floor(this->left),
        floor(this->top),
        ceil(this->right),
        ceil(this->bottom)
    );
}

class Line {
public:
    Line(const ParameterizedVertex &a, const ParameterizedVertex &b) :
        a(a.point), b(b.point), aInfo(a.info), bInfo(b.info), aTime(a.time), bTime(b.time),
        rect(Rect(
                min(a.point.x, b.point.x),
                min(a.point.y, b.point.y),
                max(a.point.x, b.point.x),
                max(a.point.y, b.point.y)
            ).Expand(max(a.info.width, b.info.width))) { }

    PF_FpLong Length() const;

    HitInfo Nearest(const Vector2 &v) const;

private:
    const Vector2 a;
    const Vector2 b;
    const VertexInfo aInfo;
    const VertexInfo bInfo;
    const PF_FpLong aTime;
    const PF_FpLong bTime;
    const Rect rect;
};

class Path {
public:
    const Rect rect;

    Path(const std::vector<ParameterizedVertex> &points) : lines(CreateLines(points)), rect(CalcBoundingBox(points)) { }

    void GetNearests(Vector2 &p, std::vector<HitInfo> &hits);

private:
    const std::vector<std::unique_ptr<Line>> lines;

    static std::vector<std::unique_ptr<Line>> CreateLines(const std::vector<ParameterizedVertex> &points);

    static Rect CalcBoundingBox(const std::vector<ParameterizedVertex> &points);
};


// zip
// see; https://gist.github.com/goldsborough/395faa93cf94ba52ecf9
template<typename T>
class Zip
{

public:

    typedef std::vector<T> container_t;

    template<typename... Args>
    Zip(const T& head, const Args&... args)
        : items_(head.size()),
        min_(head.size())
    {
        zip_(head, args...);
    }

    inline operator container_t() const
    {
        return items_;
    }

    inline container_t operator()() const
    {
        return items_;
    }

private:

    template<typename... Args>
    void zip_(const T& head, const Args&... tail)
    {
        // If the current item's size is less than
        // the one stored in min_, reset the min_
        // variable to the item's size
        if (head.size() < min_) min_ = head.size();

        for (std::size_t i = 0; i < min_; ++i)
        {
            // Use begin iterator and advance it instead
            // of using the subscript operator adds support
            // for lists. std::advance has constant complexity
            // for STL containers whose iterators are
            // RandomAccessIterators (e.g. vector or deque)
            typename T::const_iterator itr = head.begin();

            std::advance(itr, i);

            items_[i].push_back(*itr);
        }

        // Recursive call to zip_(T, Args...)
        // while the expansion of tail... is not empty
        // else calls the overload with no parameters
        return zip_(tail...);
    }

    inline void zip_()
    {
        // If min_ has shrunk since the
        // constructor call
        items_.resize(min_);
    }

    /*! Holds the items for iterating. */
    container_t items_;

    /*! The minimum number of values held by all items */
    std::size_t min_;

};

template<typename T, typename... Args>
typename Zip<T>::container_t zip(const T& head, const Args&... tail)
{
    return Zip<T>(head, tail...);
}