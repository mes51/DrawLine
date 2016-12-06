#include "DrawLine_Geometry.h"

using std::min;
using std::max;

inline VertexInfo VertexInfo::Interpolate(const VertexInfo v1, const VertexInfo v2, const PF_FpLong t)
{
    AEGP_ColorVal resultColor;
    resultColor.alphaF = Util::Interpolate(v1.color.alpha, v2.color.alpha, t);
    resultColor.redF = Util::Interpolate(v1.color.red, v2.color.red, t);
    resultColor.greenF = Util::Interpolate(v1.color.green, v2.color.green, t);
    resultColor.blueF = Util::Interpolate(v1.color.blue, v2.color.blue, t);
    return VertexInfo(
        Util::Interpolate(v1.width, v2.width, t),
        resultColor
    );
}

HitInfo HitInfo::NoHit(VertexInfo(), 0.0, HUGE_VAL);

PF_FpLong Line::Length() const
{
    return this->b.DistanceTo(this->a);
}

HitInfo Line::Nearest(const Vector2 & v) const
{
    if (!this->rect.inPoint(v)) {
        return HitInfo::NoHit;
    }

    __m128d point = _mm_load_pd((double*)&v);
    __m128d lineA = _mm_load_pd((double*)&this->a);
    __m128d lineB = _mm_load_pd((double*)&this->b);
    __m128d b = _mm_sub_pd(point, lineA);
    __m128d a = _mm_sub_pd(lineB, lineA);
    __m128d denom = _mm_dp_pd(a, a, 0xff);
    __m128d r = _mm_dp_pd(a, b, 0xff);

    __m128d lez = _mm_cmpnle_pd(r, _mm_setzero_pd());
    __m128d ged = _mm_cmpnge_pd(r, denom);

    if (_mm_testz_pd(lez, lez) != 0) {
        PF_FpLong distance = 0;
        _mm_store1_pd(&distance, _mm_sqrt_pd(_mm_dp_pd(b, b, 0xff)));
        return HitInfo(this->aInfo, this->aTime, distance);
    } else if (_mm_testz_pd(ged, ged) != 0) {
        PF_FpLong distance = 0;
        __m128d dp = _mm_sub_pd(point, lineB);
        _mm_store1_pd(&distance, _mm_sqrt_pd(_mm_dp_pd(dp, dp, 0xff)));
        return HitInfo(this->bInfo, this->bTime, distance);
    } else {
        Vector2 p;
        PF_FpLong distance = 0;
        PF_FpLong t = 0;
        __m128d crossPoint = _mm_add_pd(lineA, _mm_mul_pd(a, _mm_div_pd(r, denom)));
        __m128d distCA = _mm_sub_pd(crossPoint, lineA);
        __m128d distCP = _mm_sub_pd(crossPoint, point);
        _mm_store1_pd(
            &t,
            _mm_div_pd(
                _mm_sqrt_pd(_mm_dp_pd(distCA, distCA, 0xff)),
                _mm_sqrt_pd(_mm_dp_pd(a, a, 0xff))
            )
        );
        _mm_store_pd(&distance, _mm_sqrt_pd(_mm_dp_pd(distCP, distCP, 0xff)));
        _mm_store_pd((double*)&p, crossPoint);

        return HitInfo(
            VertexInfo::Interpolate(this->aInfo, this->bInfo, t),
            Util::Interpolate(this->aTime, this->bTime, t),
            distance
        );
    }
    //*/
    
    /*
    Vector2 a = this->b.Sub(this->a);
    Vector2 b = v.Sub(this->a);
    PF_FpLong r = a.Dot(b);
    PF_FpLong denom = a.Dot(a);

    if (r <= 0.0) {
        return HitInfo(this->aInfo, this->aTime, this->a.DistanceTo(v));
    } else if (r >= denom) {
        return HitInfo(this->bInfo, this->bTime, this->b.DistanceTo(v));
    } else {
        Vector2 p = this->a.Add(a.Multiple(r / denom));
        PF_FpLong t = p.DistanceTo(this->a) / this->Length();
        return HitInfo(
            VertexInfo::Interpolate(this->aInfo, this->bInfo, t),
            Util::Interpolate(this->aTime, this->bTime, t),
            v.DistanceTo(p)
        );
    }
    //*/
}

void Path::GetNearests(Vector2 & p, std::vector<HitInfo>& hits)
{
    for (auto itr = this->lines.begin(), end = this->lines.end(); itr != end; ++itr) {
        HitInfo info = (*itr)->Nearest(p);
        if (info.distance < info.info.width) {
            hits.push_back(info);
        }
    }
}

std::vector<std::unique_ptr<Line>> Path::CreateLines(const std::vector<ParameterizedVertex> &points)
{
    std::vector<ParameterizedVertex> bs;
    std::copy(points.begin() + 1, points.end(), std::back_inserter(bs));
    std::vector<std::unique_ptr<Line>> result;

    for (auto t : zip(points, bs)) {
        result.push_back(std::make_unique<Line>(t[0], t[1]));
    }

    return result;
}

Rect Path::CalcBoundingBox(const std::vector<ParameterizedVertex> &points)
{
    std::vector<PF_FpLong> xs;
    std::vector<PF_FpLong> ys;
    std::vector<PF_FpLong> ws;
    std::transform(points.begin(), points.end(), std::back_inserter(xs), ([](ParameterizedVertex p) -> PF_FpLong { return p.point.x; }));
    std::transform(points.begin(), points.end(), std::back_inserter(ys), ([](ParameterizedVertex p) -> PF_FpLong { return p.point.y; }));
    std::transform(points.begin(), points.end(), std::back_inserter(ws), ([](ParameterizedVertex p) -> PF_FpLong { return p.info.width; }));
    Rect rect(
        *std::min_element(xs.begin(), xs.end()),
        *std::min_element(ys.begin(), ys.end()),
        *std::max_element(xs.begin(), xs.end()),
        *std::max_element(ys.begin(), ys.end())
    );
    return rect.Expand(*std::max_element(ws.begin(), ws.end()) + 1.0);
}
