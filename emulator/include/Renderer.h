#ifndef EMULATOR_RENDERER_H_
#define EMULATOR_RENDERER_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "BitwiseUtils.h"

#include "GameBoy.h"

struct VRAMDebugInfo
{
    bool render_tiles = false;
    bool render_bg = true;
    bool render_oam = false;

    bool show_bg_scroll_view = true;
    bool map_use_window_address = false;
    bool map_use_8000_tile_address = false;
};

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void Render(GameBoy* gb);
    void RenderVRAMDebug(GameBoy* gb, VRAMDebugInfo& info);

    GLuint tiles_vram_texture;
    GLuint background_vram_texture;
    GLuint oam_textures[40];
private:
    GLuint vao;
	GLuint vbo;
	GLuint screen_buffer;

    GLuint screen_shader;
};

#endif