// RayTracer.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <algorithm>



using namespace cv;
using namespace std;

struct vec3
{
    float x, y, z;

    vec3 operator*(const float f) const
    {
        return vec3{ x * f, y * f, z * f };
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

float intersect(const Ray& r, Sphere s)
{                // returns distance, 0 if nohit
    vec3 op = s.center - r.origin;        // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
    float t;
    float eps = 1e-4; 
    float b = op.dot(r.direction);
    float det = b * b - op.dot(op) + s.radius * s.radius;
    if (det < 0)
        return 0;
    else
        det = sqrt(det);
    return (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);
}

float clamp(float v, float min, float max) {
    if (v > max) return max;
    if (v < min) return min;
    return v;
}

vec3 floatToRgb(vec3 color) {
    vec3 rgbColor;
    rgbColor.x = color.x * 255;
    rgbColor.y = color.y * 255;
    rgbColor.z = color.z * 255;

    return rgbColor;
}


int main()
{
    const float PI = 3.14159265358979323846;

    
    Mat image = Mat::zeros(500, 500, CV_8UC3);
    
    Vec3b background_color = Vec3b(0, 0, 0);    // B, G, R
    Vec3b sphere_color = Vec3b(0, 0, 1.);

    vector<Sphere> scene;
    scene.push_back({ {300, 320, 0}, 50, {0, 0, 1}, 1. });
    scene.push_back({ {300, 180, 0}, 50, {0, 0, 1}, 1. });
    scene.push_back({ {250, 1850, 1000}, 1500, {1, 0, 0}, 1 });
    scene.push_back({ {250, -1350, 1000}, 1500, {0, 1, 0}, 1 });
    scene.push_back({ {1850, 250, 1000}, 1500, {1, 1, 1}, 1 });
    scene.push_back({ {-1350, 250, 1000}, 1500, {1, 1, 1}, 1 });
    scene.push_back({ {250, 250, 2000}, 1500, {1, 1, 1}, 1 });



    PointLight lumiere{ {200, 250, 200} , {100, 100, 100}, 2000 };

    float dist_coef = 0.01;
    
    // Shoot ray for each pixel
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
             
            Ray r = { {(float)i, (float)j, 0}, {0, 0, 1} };
            float min_t = 100000000000000.f; // A CHANGER !!!!!!!!!!!!
            Vec3b displayColor = background_color;
            for (const Sphere &sphere : scene) {

                float t = intersect({ {(float)i, (float)j, 0}, {0, 0, 1} }, sphere);


                Vec3b color;
                if (t > 0 && t < min_t) {
                    min_t = t;

                    vec3 x = r.origin + (r.direction * t);
                    vec3 normal = (x - sphere.center).unitVector();
                    vec3 w_o = (lumiere.position - x).unitVector();
                    vec3 L_o = sphere.color * (lumiere.color * lumiere.intensity * (normal.dot(w_o) / PI) * sphere.albedo) / (lumiere.position - x).normSquared();

                    L_o.x = clamp(L_o.x, 0, 1);
                    L_o.y = clamp(L_o.y, 0, 1);
                    L_o.z = clamp(L_o.z, 0, 1);
                 
                    //cout << "b : " << L_o.x << ",g : " << L_o.y << ",r :" << L_o.z << endl;

                    L_o = floatToRgb(L_o);
                    color = Vec3b(L_o.x, L_o.y, L_o.z);
                    displayColor = color;
                }
            }

            image.at<Vec3b>(i, j) = displayColor;
        }
    }

    cv::imshow("Display Window", image);
    cv::waitKey(0);
    


    
}
