#pragma once
#include <cmath>

class Vector3 {
public:
    float x, y, z;

    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
        : x(x), y(y), z(z) {
    }

    // Vektoraddition
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    // Vektorsubtraktion
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    // Skalare Multiplikation
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    // Skalare Division
    Vector3 operator/(float scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    // Kreuzprodukt
    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Vektorlänge (Betrag)
    float Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Normalisierung des Vektors (in einen Einheitsvektor umwandeln)
    Vector3 normalize() const {
        float len = Length();
        if (len == 0.0f) {
            return Vector3(0.0f, 0.0f, 0.0f);
        }
        return Vector3(x / len, y / len, z / len);
    }

    // Operatorüberladung für "+="
    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    // Operatorüberladung für "-="
    Vector3& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
};
