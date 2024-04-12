#include "imgui.h"
#include <iostream>
#include <random>
#include "math.h"
#include <vector>

using namespace Math;

GLuint image(int width, int height, std::vector<Color3>& data) {
    
    GLuint opengl_texture;
    glGenTextures(1, &opengl_texture);
    glBindTexture(GL_TEXTURE_2D, opengl_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    /*std::vector<Color3> pixels;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            pixels.push_back(Color3((float)y/height, (float)y/height, (float)y/height));
        }
    }*/

    float pixels2[width*height*3];
    int i2 = 0;
    for (Color3 i : data) {
        pixels2[i2++] = i.x();
        pixels2[i2++] = i.y();
        pixels2[i2++] = i.z();
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, pixels2);
    return opengl_texture;
}