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
#include <optional>
#include <functional>



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

optional<float> intersect(const Ray& r, Sphere s)
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
    scene.push_back({ {300, 320, 0}, 50, {0, 0, 1}, 1. });  // Sphere Droite              
    scene.push_back({ {300, 180, 0}, 50, {0, 0, 1}, 1. });  // Sphere Gauche
    scene.push_back({ {250, 1850, 1000}, 1500, {1, 0, 0}, 1 });     
    scene.push_back({ {250, -1350, 1000}, 1500, {0, 1, 0}, 1 });
    scene.push_back({ {1850, 250, 1000}, 1500, {1, 1, 1}, 1 });       // Sol
    scene.push_back({ {-1350, 250, 1000}, 1500, {1, 1, 1}, 1 });    // Plafond
    scene.push_back({ {250, 250, 2700}, 1500, {1, 1, 1}, 1 });      // Arrière plan
    //scene.push_back({ {220, 180, 10}, 30, {0, 0, 1}, 1. });



    PointLight lumiere{ {200, 250, 200} , {100, 100, 100}, 15000 };

    float dist_coef = 0.01;
    
    // Shoot ray for each pixel
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
             
            Ray r = { {(float)i, (float)j, 0}, {0, 0, 1} };

            Vec3b displayColor;
            bool found_intersect = false;
            float min_t;
            Sphere min_sphere;
            
            for (const Sphere& sphere : scene) {
                optional<float> t = intersect({ {(float)i, (float)j, 0}, {0, 0, 1} }, sphere);

                if (t) {
                    float t_val = t.value();
                    if (!found_intersect) {
                        min_t = t_val;
                        min_sphere = sphere;
                        found_intersect = true;
                    }
                    else {
                        if (t_val < min_t) {
                            min_t = t_val;
                            min_sphere = sphere;
                        }
                    }
                }
            }

            if (found_intersect) {
                bool ombre = false;
                Vec3b color;
                
                vec3 x = r.origin + (r.direction * min_t);
                vec3 normal = (x - min_sphere.center).unitVector();
                vec3 w_o = (lumiere.position - x).unitVector();

               //// Ombres
               //float ligthDistance = (lumiere.position - x).normSquared();

               //float epsilon = pow(10, -4);
               //for (const Sphere& sphere_ombre : scene) {
               //    vec3 new_x = x + w_o * epsilon;
               //    optional<float> t_ombre = intersect({ new_x, w_o }, sphere_ombre);

               //    if (t_ombre && t_ombre.value() > 0) {
               //    vec3 x_ombre = x + (w_o * *t_ombre);
               //    float ombreDistance = (x_ombre - x).normSquared();
               //    float normEps = 0.999999f;

               //        if (ombreDistance * normEps < ligthDistance * normEps) {
               //             ombre = true;
               //             break;
               //        }
               //    }
               //}

               if (!ombre) {
                   // Calcul de l'éclairage s'il n'y a pas d'ombres
                   vec3 L_o = min_sphere.color * (lumiere.color * lumiere.intensity * (normal.dot(w_o) / PI) * min_sphere.albedo) / (lumiere.position - x).normSquared();

                   L_o.x = clamp(L_o.x, 0, 1);
                   L_o.y = clamp(L_o.y, 0, 1);
                   L_o.z = clamp(L_o.z, 0, 1);

                   //cout << "b : " << L_o.x << ",g : " << L_o.y << ",r :" << L_o.z << endl;

                   L_o = floatToRgb(L_o);
                   color = Vec3b(L_o.x, L_o.y, L_o.z);
                   displayColor = color;
               }       
            }
            else {
                displayColor = background_color;
            }

            image.at<Vec3b>(i, j) = displayColor;
        }
    }

    cv::imshow("Display Window", image);
    cv::waitKey(0);
    


    
}
