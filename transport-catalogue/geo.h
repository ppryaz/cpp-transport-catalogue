#pragma once

#include <cmath>

namespace transport_catalogue {

struct Coordinates {
    double lat;
    double lng;
};

inline bool operator==(const Coordinates &lhs, const Coordinates &rhs) {
    return (std::abs(lhs.lat - rhs.lat)<1e-6 && std::abs(lhs.lng - rhs.lng)<1e-6);
}

inline double ComputeDistance(Coordinates from, Coordinates to) {
    using namespace std;
    static const double dr = 3.1415926535 / 180.;
    return acos(sin(from.lat * dr) * sin(to.lat * dr)
                + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
        * 6371000;
}
}