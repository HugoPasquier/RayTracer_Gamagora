#pragma once
struct vec3
{
    float x, y, z;

    vec3 operator*(const float f) const
    {
        return vec3{ x * f, y * f, z * f };
    }

    bool operator==(const vec3 v) const
    {
        return (x == v.x && y == v.y && z == v.z);
    }

    vec3 operator/(const float f) const
    {
        return vec3{ x / f, y / f, z / f };
    }

    vec3 operator*(const vec3 v) const
    {
        return vec3{ x * v.x, y * v.y, z * v.z };
    }

    vec3 operator+(const vec3 v) const
    {
        return vec3{ x + v.x, y + v.y, z + v.z };
    }

    vec3 operator-(const vec3 v) const
    {
        return vec3{ x - v.x, y - v.y, z - v.z };
    }

    float normSquared() const
    {
        return x * x + y * y + z * z;
    }

    vec3 unitVector() const
    {
        const float norm = std::sqrt(normSquared());
        if (norm == 0) return vec3{ 0., 0., 0. };
        return vec3{ x / norm, y / norm, z / norm };
    }

    float dot(vec3 v) {
        return (x * v.x) + (y * v.y) + (z * v.z);
    }
};

struct Ray
{
    vec3 origin;
    vec3 direction;
    int maxRecursion = 10;
};

struct Sphere
{
    vec3 center;
    float radius;
    vec3 color;
    float albedo;
};

struct PointLight
{
    vec3 position;
    vec3 color;
    float intensity;
};
