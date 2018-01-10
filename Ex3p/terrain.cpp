#include "Vec3.h"
#include <math.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <stdio.h>
#include "terrain.h"
#include <iostream>

Terrain::Terrain(int size) : _size(size)
    {
        Terrain::_height = new float*[Terrain::_size];
        for(int i = 0; i < Terrain::_size; ++i)
        {
            Terrain::_height[i] = new float[Terrain::_size];
            for(int j = 0; j < Terrain::_size; ++j)
                Terrain::_height[i][j] = 0;
        }
    }

Terrain::Terrain(int size, float disp) : _size(size)
    {
        Terrain::_disp = disp;
        Terrain::_height = new float*[Terrain::_size];
        for(int i = 0; i < Terrain::_size; ++i)
        {
            Terrain::_height[i] = new float[Terrain::_size];
            for(int j = 0; j < Terrain::_size; ++j)
                Terrain::_height[i][j] = 0;
        }
    }

    float Terrain::getHeight(int x, int y)
    {
        return Terrain::_height[x][y];
    }

    void Terrain::generate()
    {
        int count;
        int it = 0;
        float min = 10.0f;
        float max = -10.0f;
        do
        {
            count = 0;
            float v = rand();
            float a = sin(v);
            float b = cos(v);
            float d = sqrt(2) * Terrain::_size;
            float c = ((float) rand() / (float) RAND_MAX) * d - d/2;
            
            std::cout << ++it << " - " << round(a * 1000.0f) / 1000.0f << "*x + " << round(b * 1000.0f) / 1000.0f << "*z = " << round(c * 1000.0f) / 1000.0f << std::endl;
            for(int z = 0; z < Terrain::_size; ++z)
                for(int x = 0; x < Terrain::_size; ++x)
                {
                    if(a*x + b*z - c > 0)
                        Terrain::_height[z][x] += Terrain::_disp;
                    else
                        Terrain::_height[z][x] -= Terrain::_disp;
                    if (abs(Terrain::_height[z][x]) >= Terrain::_max_height)
                        ++count;
                    if (Terrain::_height[z][x] > max)
                        max = Terrain::_height[z][x];
                    if (Terrain::_height[z][x] < min)
                        min = Terrain::_height[z][x];
                }
        } while((count < Terrain::_threshold) && (it < 1000));
        for(int z = 0; z < Terrain::_size; ++z)
            for(int x = 0; x < Terrain::_size; ++x)
                Terrain::_height[z][x] = (Terrain::_height[z][x] - min) / (max - min);
    }

    void Terrain::visualize()
    {
        std::ofstream out("heightmap.pgm");
        out << "P2\n" << Terrain::_size << " " << Terrain::_size << "\n255\n"; 
        for(int y = 0; y < Terrain::_size; ++y)
        {
            for(int x = 0; x < Terrain::_size; ++x)
                out << (int) floor(Terrain::_height[y][x] * 255.0f) << " ";
            out << "\n";
        }
        out.close();
    }

    std::Vec3f Terrain::coords(int idx, int idz, float size)
    {
        float step = size / Terrain::_size;
        float x = idx * step - 0.5 * size;
        float z = idz * step - 0.5 * size;
        float y = Terrain::_height[idz][idx] * size - 0.5 * size;
        std::Vec3f coord(x, y, z);
        return coord;
    }
