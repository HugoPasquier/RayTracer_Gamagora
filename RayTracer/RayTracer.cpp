// RayTracer.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
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
    int maxRecursion = 2;
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

void generateSphere(vector<Sphere> &scene, float spaceBtw, float radius, vec3 centerPosition, int nbSpheres) {
    vec3 minPoint = { centerPosition.x - ((nbSpheres / 2.) * (spaceBtw)),
                    centerPosition.y - ((nbSpheres / 2.) * (spaceBtw)),
                    centerPosition.z - ((nbSpheres / 2.) * (spaceBtw)) };
    
    for (int i = 0; i < nbSpheres; i++) {
        for (int j = 0; j < nbSpheres; j++) {
            for (int k = 0; k < nbSpheres; k++) {
                scene.push_back({ {minPoint.x + i * spaceBtw, minPoint.y + j * spaceBtw, minPoint.z + k * spaceBtw}, radius, {0, 0, 0.8}, 1. });
                //cout << "Debug Position : " << i * spaceBtw << ", " << j * spaceBtw << ", " << k * spaceBtw << endl;
            }
        }
    }
}

// --- Octree ---
struct treeNode
{
    vec3 minPoint;
    vec3 maxPoint;
    vector<int> objectsId;
    treeNode * leftSon;
    treeNode * rightSon;

    bool isInBox(vec3 p) {
        return p.x <= maxPoint.x && p.y <= maxPoint.y && p.z <= maxPoint.z && p.x >= minPoint.x && p.y >= minPoint.y && p.z >= minPoint.z;
    }

    treeNode initTree(vector<Sphere> &scene) {
        
        bool firstObject = true;
        vec3 minP;
        vec3 maxP;

        for (const Sphere& sphere : scene) {
            if (firstObject) {
                minP = sphere.center;
                maxP = sphere.center;
                firstObject = false;
            }
            else{
                if (sphere.center.x < minP.x) {
                    minP.x = sphere.center.x;
                }
                if (sphere.center.y < minP.y) {
                    minP.y = sphere.center.y;
                }
                if (sphere.center.z < minP.z) {
                    minP.z = sphere.center.z;
                }
                if (sphere.center.x > maxP.x) {
                    maxP.x = sphere.center.x;
                }
                if (sphere.center.y > maxP.y) {
                    maxP.y = sphere.center.y;
                }
                if (sphere.center.z > maxP.z) {
                    maxP.z = sphere.center.z;
                }
            }
        }

        objectsId = vector<int>();

        int id_sphere = 0;
        for (const Sphere& sphere : scene) {
            if (isInBox(sphere.center)) {
                objectsId.push_back(id_sphere);
            }
            id_sphere++;
        }

        treeNode t = { minP, maxP, objectsId, NULL, NULL };
        return t;
    }

    void addObjectsNode(vector<Sphere> scene) {
        int id_sphere = 0;
        for (const Sphere& sphere : scene) {
            if (isInBox(sphere.center)) {
                objectsId.push_back(id_sphere);
            }
            id_sphere++;
        }
    }

    void splitNode(vector<Sphere> scene) {
        // On calcul la plus grande dimensions
        float xLenght = maxPoint.x - minPoint.x;
        float yLenght = maxPoint.y - maxPoint.y;
        float zLenght = maxPoint.z - maxPoint.z;

        vec3 leftSonMaxPoint = maxPoint;
        vec3 rightSonMinPoint = minPoint;

        if (xLenght > yLenght && xLenght > zLenght) { // X
            float cutValue = xLenght / 2.;
            leftSonMaxPoint.x = minPoint.x + cutValue;
            rightSonMinPoint.x = minPoint.x + cutValue;
        }
        else if (yLenght > xLenght && yLenght > zLenght) {  // Y
            float cutValue = yLenght / 2.;
            leftSonMaxPoint.y = minPoint.y + cutValue;
            rightSonMinPoint.y = minPoint.y + cutValue;
        }
        else {  // Z
            float cutValue = zLenght / 2.;
            leftSonMaxPoint.z = minPoint.z + cutValue;
            rightSonMinPoint.z = minPoint.z + cutValue;
        }
       
        // On initialise les enfants
        treeNode *leftSon = new treeNode{ minPoint, leftSonMaxPoint, vector<int>(), NULL, NULL };
        treeNode *rightSon = new treeNode{ rightSonMinPoint, maxPoint, vector<int>(), NULL, NULL };

        leftSon->addObjectsNode(scene);
        rightSon->addObjectsNode(scene);

        this->leftSon = leftSon;
        this->rightSon = rightSon;
    }

    bool intersect(const Ray& r) const
    {
        float tmin = (minPoint.x - r.origin.x) / r.direction.x;
        float tmax = (maxPoint.x - r.origin.x) / r.direction.x;

        if (tmin > tmax) swap(tmin, tmax);

        float tymin = (minPoint.y - r.origin.y) / r.direction.y;
        float tymax = (maxPoint.y - r.origin.y) / r.direction.y;

        if (tymin > tymax) swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;

        float tzmin = (minPoint.z - r.origin.z) / r.direction.z;
        float tzmax = (maxPoint.z - r.origin.z) / r.direction.z;

        if (tzmin > tzmax) swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;

        if (tzmax < tmax)
            tmax = tzmax;

        return true;
    }
};



struct RayTracer {

    const float PI = 3.14159265358979323846;
    vector<Sphere> scene;
    PointLight lumiere;

    Vec3b processIntersection(Ray r, Vec3b background_color) {

        Vec3b displayColor;
        bool found_intersect = false;
        float min_t;
        Sphere min_sphere;
        float epsilon = pow(10, -4);


        for (const Sphere& sphere : scene) {
            optional<float> t = intersect({ r.origin , {0, 0, 1} }, sphere);

            if (t.value() > 0) {
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

            if (min_sphere.albedo == 1. && r.maxRecursion > 0) {   // Cas Sphere miroir
                //cout << " Ray max recursion = " << r.maxRecursion << endl;
                Ray reflectRay;
                vec3 d = (x - r.origin).unitVector();
                float reflect_epsilon = pow(10, -6);

                reflectRay.maxRecursion = r.maxRecursion;
                reflectRay.direction = d + (normal * (-2) * normal.dot(d));
                reflectRay.origin = x + reflectRay.direction * reflect_epsilon;
                reflectRay.maxRecursion--;

                displayColor = processIntersection(reflectRay, background_color);

            }
            else {  // Cas général
                // Ombres
                float ligthDistance = (lumiere.position - x).normSquared();

                for (const Sphere& sphere_ombre : scene) {
                    vec3 new_x = x + w_o * epsilon;
                    optional<float> t_ombre = intersect({ new_x, w_o }, sphere_ombre);

                    if (t_ombre && t_ombre.value() > 0) {
                        vec3 x_ombre = x + (w_o * *t_ombre);
                        float ombreDistance = (x_ombre - x).normSquared();
                        float normEps = 0.999999f;

                        if (ombreDistance * normEps < ligthDistance * normEps) {
                            ombre = true;
                            break;
                        }
                    }
                }

                if (!ombre) {
                    // Calcul de l'éclairage s'il n'y a pas d'ombres
                    vec3 L_o = min_sphere.color * (lumiere.color * lumiere.intensity * (normal.dot(w_o) / PI * min_sphere.albedo) / (lumiere.position - x).normSquared());

                    L_o.x = clamp(L_o.x, 0, 1);
                    L_o.y = clamp(L_o.y, 0, 1);
                    L_o.z = clamp(L_o.z, 0, 1);

                    L_o = floatToRgb(L_o);
                    color = Vec3b(L_o.x, L_o.y, L_o.z);
                    displayColor = color;
                }
            }
        }
        else{
            displayColor = background_color;
        }

        return displayColor;
    }

};

int main()
{
    RayTracer rayTracer;
    


    Mat image = Mat::zeros(500, 500, CV_8UC3);

    Vec3b background_color = Vec3b(0, 0, 0);    // B, G, R
    Vec3b sphere_color = Vec3b(0, 0, 1.);

    // --- Scene ---
    
    rayTracer.scene.push_back({ {300, 320, 100}, 50, {0, 0, 1}, 0.8 });  // Sphere Droite              
    rayTracer.scene.push_back({ {300, 180, 100}, 50, {0, 0, 1}, 1. });  // Sphere Gauche
    rayTracer.scene.push_back({ {250, 1850, 1000}, 1500, {1, 0, 0}, 0.8 });
    rayTracer.scene.push_back({ {250, -1350, 1000}, 1500, {0, 1, 0}, 0.8 });
    rayTracer.scene.push_back({ {1620, 250, 1000}, 1500, {0.5, 0.5, 0.5}, 0.8 });       // Sol
    rayTracer.scene.push_back({ {-1350, 250, 1000}, 1500, {0.5, 0.5, 0.5}, 0.8 });    // Plafond
    rayTracer.scene.push_back({ {250, 250, 2000}, 1500, {0.5, 0.5, 0.5}, 0.8 });      // Arrière plan
    rayTracer.scene.push_back({ {210, 180, 150}, 30, {0, 0, 1}, 0.6 });  // Sphere miroir

    // Generation de sphere
    //generateSphere(scene, 100, 25, { -10, 250, 1000 }, 10);
    cout << "Nombre d'elements dans la scene : " << rayTracer.scene.size() << endl;

    // Tree
    treeNode tree;
    tree.initTree(rayTracer.scene);
    tree.splitNode(rayTracer.scene);

    rayTracer.lumiere = { {200, 250, 10} , {1, 1, 1}, 300000 };

    float dist_coef = 0.01;

    // Shoot ray for each pixel
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {

            Ray r = { {(float)i, (float)j, 0}, {0, 0, 1} };

            Vec3b displayColor = rayTracer.processIntersection(r, background_color);

            image.at<Vec3b>(i, j) = displayColor;
        }
    }

    cv::imshow("Display Window", image);
    cv::waitKey(0);
}