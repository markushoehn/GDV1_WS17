#include "Vec3.h"
#include <math.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <stdio.h>

class Terrain {
    private:
        float** _height;
        const int _size;
        float _displacement;
        float _max_height;
        int _threshold;

    public:
    Terrain(int size = 256, float disp = 0.123456f);

    float getHeight(int x, int y);
    void generate(int mode, bool smooth);
    void visualize();
    std::Vec3f coords(int idx, int idz, float bbox_height, float bbox_plane);
    int getSize();
};
