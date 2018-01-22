#include "Vec3.h"
#include <math.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <stdio.h>
#include "terrain.h"
#include <iostream>
#include <time.h>

Terrain::Terrain(int size, float disp) : _size(size), _displacement(disp)
    {
        Terrain::_max_height = 265.0f;
        Terrain::_threshold = 128;
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
        int iterations = 1024;
        
        srand(time(NULL));
        
        if(mode == 0)
        {
            int count;
            int it = 0;

            do
            {
                count = 0;
                min = 10.0f;
                max = -10.0f;
                float v = rand();
                float a = sin(v);
                float b = cos(v);
                float d = sqrt(2) * Terrain::_size;
                float c = ((float) rand() / (float) RAND_MAX) * d - d/2;

                // testing variable displacement
                float disp_start = Terrain::_displacement;
                float disp_max = disp_start * 32.0f;

                ++it;
                //std::cout << it << " - " << round(a * 1000.0f) / 1000.0f << "*x + " << round(b * 1000.0f) / 1000.0f << "*z = " << round(c * 1000.0f) / 1000.0f << std::endl;
                for(int z = 0; z < Terrain::_size; ++z)
                {
                    for(int x = 0; x < Terrain::_size; ++x)
                    {
                        // calculate current displacement
                        float disp_cur = disp_start;
                        if(it < iterations)
                            disp_cur = disp_start + ((float)it / (float)iterations) * (disp_max - disp_start); 

                        if(a*x + b*z - c > 0)
                            Terrain::_height[z][x] += disp_cur; //Terrain::_displacement;
                        else
                            Terrain::_height[z][x] -= disp_cur; //Terrain::_displacement;

                        if (abs(Terrain::_height[z][x]) >= Terrain::_max_height)
                            ++count;
                        if (Terrain::_height[z][x] > max)
                            max = Terrain::_height[z][x];
                        if (Terrain::_height[z][x] < min)
                            min = Terrain::_height[z][x];
                    }
                }
            } while((count < Terrain::_threshold) && (it < iterations));
        }
        else if(mode == 1)
        {
            int count;
            int it = 0;

            do
            {
                count = 0;
                min = 10.0f;
                max = -10.0f;
                float v = rand();
                float a = sin(v);
                float b = cos(v);
                float d = sqrt(2) * Terrain::_size;
                float c = ((float) rand() / (float) RAND_MAX) * d - d/2;

                // testing variable displacement
                float disp_start = Terrain::_displacement;
                float disp_max = disp_start * 32.0f;

                ++it;
                for(int z = 0; z < Terrain::_size; ++z)
                {
                    for(int x = 0; x < Terrain::_size; ++x)
                    {
                        // calculate current displacement
                        float disp_cur = disp_start;
                        if(it < iterations)
                            disp_cur = disp_start + ((float)it / (float)iterations) * (disp_max - disp_start); 

                        float half_wavelength = 25.0f;
                        float dist = abs(a*x + b*z - c);

                        if(dist < half_wavelength)
                            Terrain::_height[z][x] += disp_cur / 2.0f + sin(dist + half_wavelength) * disp_cur;
                       // else
                         //   Terrain::_height[z][x] -= disp_cur; //Terrain::_displacement;

                        if (abs(Terrain::_height[z][x]) >= Terrain::_max_height)
                            ++count;
                        if (Terrain::_height[z][x] > max)
                            max = Terrain::_height[z][x];
                        if (Terrain::_height[z][x] < min)
                            min = Terrain::_height[z][x];
                    }
                }
            } while((count < Terrain::_threshold) && (it < iterations));
        }
        else if(mode == 2)
        {
            int count;
            int it = 0;

            do
            {
                count = 0;
                min = 10.0f;
                max = -10.0f;
                float rz = ((float) rand() / (RAND_MAX)) * Terrain::_size;
                float rx = ((float) rand() / (RAND_MAX)) * Terrain::_size;
                
                // testing variable displacement
                float disp_start = Terrain::_displacement;
                float disp_max = disp_start * 16.0f;

                ++it;

                for(int z = 0; z < Terrain::_size; ++z)
                {
                    for(int x = 0; x < Terrain::_size; ++x)
                    {
                        // calculate current displacement
                        float disp_cur = disp_start;
                        if(it < iterations)
                            disp_cur = disp_start + ((float)it / (float)iterations) * (disp_max - disp_start); 

                        float terrainCircleSize = 32.0f;
                        float dist = sqrt((z-rz)* (z-rz) + (x-rx)* (x-rx)) * 2.0f / terrainCircleSize;
    
                        if (abs(dist) <= 1.0)
                            Terrain::_height[z][x] += disp_cur / 2.0f + cos(dist * 3.14) * disp_cur / 2;

                        if (abs(Terrain::_height[z][x]) >= Terrain::_max_height)
                            ++count;
                        if (Terrain::_height[z][x] > max)
                            max = Terrain::_height[z][x];
                        if (Terrain::_height[z][x] < min)
                            min = Terrain::_height[z][x];
                    }
                }
            } while((count < Terrain::_threshold) && (it < iterations));
        }



        if(smooth)
        {
            // normal smooth filter
            /*            
            int kernel_size = 3;
            float kernel [] = {1.0f, 1.0f, 1.0f,
                               1.0f, 2.0f, 1.0f,
                               1.0f, 1.0f, 1.0f};
            */

            // gaussioan smooth filter
            int kernel_size = 5;
            float d = 273.0f;
            float kernel [] = {1.0f / d,  4.0f / d,  7.0f / d,  4.0f / d, 1.0f / d,
                               4.0f / d, 16.0f / d, 26.0f / d, 16.0f / d, 4.0f / d,
                               7.0f / d, 26.0f / d, 41.0f / d, 26.0f / d, 7.0f / d,
                               4.0f / d, 16.0f / d, 26.0f / d, 16.0f / d, 4.0f / d,
                               1.0f / d,  4.0f / d,  7.0f / d,  4.0f / d, 1.0f / d};

            int half_kernel_size = kernel_size / 2;
            for(int z = 0; z < Terrain::_size; ++z)
                for(int x = 0; x < Terrain::_size; ++x)
                {
                    float sum = 0.0f;
                    for(int n = 0; n < kernel_size; ++n)
                        for(int m = 0; m < kernel_size; ++m)
                        {
                            int curz = z + (n - half_kernel_size);
                            int curx = x + (m - half_kernel_size);

                            if(curz < 0 || curz > (Terrain::_size - 1) || curx < 0 || curx > (Terrain::_size - 1))
                                sum += 0.0f;
                            else
                                sum += Terrain::_height[z][x] * kernel[kernel_size * n + m];
                        }

                    if (sum > max) max = sum;
                    if (sum < min) min = sum;

                    Terrain::_height[z][x] = sum;
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
