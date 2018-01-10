#include "Vec3.h"
#include <math.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <stdio.h>

class Terrain {
    private:
        float** _height;
        const int _size = 1;
        float _disp = 0.12345678f;
        float _max_height = 10.0f;
        int _threshold = 20;

    public:
    Terrain(int size);
    Terrain(int size, float disp);

    float getHeight(int x, int y);
    void generate();
    void visualize();
    std::Vec3f coords(int idx, int idz, float size);
};
