#ifndef EMULATOR_RENDERER_H_
#define EMULATOR_RENDERER_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GameBoy.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void Render(GameBoy* gb);
private:
    GLuint vao;
	GLuint vbo;
	GLuint screen_buffer;

    GLuint screen_shader;
};

#endif