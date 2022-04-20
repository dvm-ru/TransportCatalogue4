#pragma once

namespace geo {

    const int EARTH_RADIUS = 6371000;

    struct Coordinates {
        double lat; // ������
        double lng; // �������
        bool operator==(const Coordinates& other) const;
        bool operator!=(const Coordinates& other) const;
    };

    double ComputeDistance(Coordinates from, Coordinates to);

}  // namespace geo