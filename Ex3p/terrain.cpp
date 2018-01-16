#include "Vec3.h"
#include <math.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <stdio.h>
#include "terrain.h"
#include <iostream>

Terrain::Terrain(int size, float disp) : _size(size), _displacement(disp)
    {
        Terrain::_max_height = 200.0f;
        Terrain::_threshold = 100;
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

    void Terrain::generate(int mode, bool smooth)
    {
        float min = 10.0f;
        float max = -10.0f;
        
        if(mode == 0)
        {
            int count;
            int it = 0;

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
                {
                    for(int x = 0; x < Terrain::_size; ++x)
                    {
                        if(a*x + b*z - c > 0)
                            Terrain::_height[z][x] += Terrain::_displacement;
                        else
                            Terrain::_height[z][x] -= Terrain::_displacement;

                        if (abs(Terrain::_height[z][x]) >= Terrain::_max_height)
                            ++count;
                        if (Terrain::_height[z][x] > max)
                            max = Terrain::_height[z][x];
                        if (Terrain::_height[z][x] < min)
                            min = Terrain::_height[z][x];
                    }
                }
            } while((count < Terrain::_threshold) && (it < 1000));
        }

        if(smooth)
        {
            int ksize = 3;
            int hsize = ksize / 2;
            float kernel [] = {1.0f, 1.0f, 1.0f,
                               1.0f, 2.0f, 1.0f,
                               1.0f, 1.0f, 1.0f};

            for(int z = 0; z < Terrain::_size; ++z)
            {
                for(int x = 0; x < Terrain::_size; ++x)
                {
                    float sum = 0.0f;

                    for(int n = 0; n < ksize; ++n)
                    {
                        for(int m = 0; m < ksize; ++m)
                        {
                            int curz = z + (n - hsize);
                            int curx = x + (m - hsize);

                            if(curz < 0 || curz > (Terrain::_size - 1) || curx < 0 || curx > (Terrain::_size - 1))
                                sum += 0.0f;
                            else
                                sum += Terrain::_height[z][x] * (n * ksize + m);
                        }
                    }

                    if (sum > max) max = sum;
                    if (sum < min) min = sum;

                    Terrain::_height[z][x] = sum;
                }
            }
        }

        // normalize
        for(int z = 0; z < Terrain::_size; ++z)
            for(int x = 0; x < Terrain::_size; ++x)
            {
                Terrain::_height[z][x] = (Terrain::_height[z][x] - min) / (max - min);

                if(Terrain::_height[z][x] >= 1.0f) {
                    Terrain::_height[z][x] = 1.0f;
                }
                if(Terrain::_height[z][x] <= 0.0f) {
                    Terrain::_height[z][x] = 0.0f;
                }
            }
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

    std::Vec3f Terrain::coords(int idx, int idz, float bbox_height, float bbox_plane)
    {
        float step = bbox_plane / Terrain::_size;
        float x = idx * step - 0.5 * bbox_plane;
        float z = idz * step - 0.5 * bbox_plane;
        float y = Terrain::_height[idz][idx] * bbox_height;
        std::Vec3f coord(x, y, z);
        return coord;
    }

    int Terrain::getSize()
    {
        return Terrain::_size;
    }
