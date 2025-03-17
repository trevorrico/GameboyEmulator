#include "Renderer.h"

const char* screenVertexShader = "#version 430 core\n"
"layout (location = 0) in vec3 vPos;\n"
"layout (location = 1) in vec2 vTexCoords;\n"
"out vec2 uv;\n"
"void main() {\n"
"   gl_Position = vec4(vPos, 1.0);\n"
"   uv = vTexCoords;\n"
"}";

const char* screenFragmentShader = "#version 430 core\n"
"layout (location = 0) out vec4 outColor;\n"
"layout (std430, binding = 0) buffer pixelsBuffer { uint pixels[]; };\n"
"in vec2 uv;\n"
"void main() {\n"
"   ivec2 pixelPos = ivec2(floor(uv * vec2(160, 144)));\n"
"   int id = 160 * (143 - pixelPos.y) + pixelPos.x;\n"
"   uint pixelId = pixels[id];\n"
"   if(pixelId == 0) {outColor = vec4(232.0/255.0, 252.0/255.0, 204.0/255.0, 1.0);}\n"
"   else if(pixelId == 1) {outColor = vec4(172.0/255.0, 212.0/255.0, 144.0/255.0, 1.0);}\n"
"   else if(pixelId == 2) {outColor = vec4(84.0/255.0, 140.0/255.0, 112.0/255.0, 1.0);}\n"
"   else if(pixelId == 3) {outColor = vec4(30.0/255.0, 44.0/255.0, 56.0/255.0, 1.0);}\n"
"   else { outColor = vec4(1.0, 0.0, 0.0, 1.0); }\n"
"}";

float quadVertices[] = {
	-1.0, 1.0, 0.5, 0.0, 1.0,
	-1.0,-3.0, 0.5, 0.0,-1.0,
	 3.0, 1.0, 0.5, 2.0, 1.0
};

Renderer::Renderer()
{
    // Setup vbo
    glGenBuffers(1, &this->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    // Setup vao  
    glGenVertexArrays(1, &this->vao);
    
    glBindVertexArray(this->vao);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Setup screen buffer
    glGenBuffers(1, &this->screen_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->screen_buffer);

    glBufferData(GL_SHADER_STORAGE_BUFFER, 160 * 144 * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

    // Setup vram debug textures

    glGenTextures(1, &this->tiles_vram_texture);
    glBindTexture(GL_TEXTURE_2D, this->tiles_vram_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 192, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glGenTextures(1, &this->background_vram_texture);
    glBindTexture(GL_TEXTURE_2D, this->background_vram_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glGenTextures(40, this->oam_textures);

    for (int i = 0; i < 40; i++)
    {
        glBindTexture(GL_TEXTURE_2D, this->oam_textures[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 16, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }

    // Setup shader
    GLuint vertex, fragment;
    
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &screenVertexShader, NULL);
    glCompileShader(vertex);

    int  success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "Failed vertex shader compilation\n" << infoLog << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &screenFragmentShader, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "Failed fragment shader compilation\n" << infoLog << std::endl;
    }
    
    this->screen_shader = glCreateProgram();
    glAttachShader(this->screen_shader, vertex);
    glAttachShader(this->screen_shader, fragment);
    glLinkProgram(this->screen_shader);

    glGetProgramiv(this->screen_shader, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(this->screen_shader, 512, NULL, infoLog);
        std::cout << "Failed shader program linking\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Renderer::~Renderer()
{
    glDeleteTextures(1, &this->tiles_vram_texture);
    glDeleteTextures(1, &this->background_vram_texture);
    glDeleteTextures(40, this->oam_textures);

    glDeleteBuffers(1, &this->screen_buffer);
    glDeleteBuffers(1, &this->vbo);

    glDeleteVertexArrays(1, &this->vao);

    glDeleteProgram(this->screen_shader);
}

void Renderer::Render(GameBoy* gb)
{
    glUseProgram(this->screen_shader);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->screen_buffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 160 * 144 * sizeof(uint32_t), gb->ppu->screen_pixels);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->screen_buffer);

    glBindVertexArray(this->vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

static uint8_t tile_debug_buffer[3 * 128 * 192];
static uint8_t bg_debug_buffer[3 * 256 * 256];
static uint8_t sprite_debug_buffer[3 * 256 * 256];
void Renderer::RenderVRAMDebug(GameBoy* gb, VRAMDebugInfo& info)
{
    if (info.render_tiles)
    {
        for (uint32_t i = 0; i < 384; i++)
        {
            uint32_t address = 0x8000 + i * 0x10;
            uint16_t tile_x = 8 * (i % 16);
            uint16_t tile_y = 8 * (i / 16);

            for (uint8_t y = 0; y < 8; y++)
            {
                uint8_t b0 = gb->mmu->Read(address + y * 2);
                uint8_t b1 = gb->mmu->Read(address + 1 + y * 2);

                for (uint8_t x = 0; x < 8; x++)
                {
                    uint8_t color = (GET_BIT(b1, 7 - x) << 1) | (GET_BIT(b0, 7 - x));

                    uint16_t pixel_pos_x = tile_x + x;
                    uint16_t pixel_pos_y = tile_y + y;
                    uint32_t id = (pixel_pos_x + pixel_pos_y * 128) * 3;

                    color = 3 - color;

                    tile_debug_buffer[id] = (uint8_t)(((float)color / 3.0f) * 255);
                    tile_debug_buffer[id + 1] = (uint8_t)(((float)color / 3.0f) * 255);
                    tile_debug_buffer[id + 2] = (uint8_t)(((float)color / 3.0f) * 255);
                }
            }
        }

        glBindTexture(GL_TEXTURE_2D, this->tiles_vram_texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 128, 192, GL_RGB, GL_UNSIGNED_BYTE, tile_debug_buffer);
    }

    if (info.render_bg)
    {
        uint8_t scy = gb->mmu->Read(0xFF42);
        uint8_t scx = gb->mmu->Read(0xFF43);

        uint8_t scroll_min_corner_x = scx % 256;
        uint8_t scroll_max_corner_x = (scroll_min_corner_x + 159) % 256;

        uint8_t scroll_min_corner_y = scy % 256;
        uint8_t scroll_max_corner_y = (scroll_min_corner_y + 143) % 256;

        for (uint32_t i = 0; i < 1024; i++)
        {
            uint32_t address = (info.map_use_window_address ? 0x9C00 : 0x9800) + i;
            uint8_t tile = gb->mmu->Read(address);
            uint16_t tile_address = 0;

            if (info.map_use_8000_tile_address)
            {
                tile_address = 0x8000 + tile * 0x10;
            }
            else
            {
                if (tile >= 128 && tile < 256)
                {
                    tile_address = (0x8800 + (tile - 128) * 0x10);
                }
                else
                {
                    tile_address = (0x9000 + tile * 0x10);
                }
            }

            uint16_t tile_x = 8 * (i % 32);
            uint16_t tile_y = 8 * (i / 32);

            for (uint8_t y = 0; y < 8; y++)
            {
                uint8_t b0 = gb->mmu->Read(tile_address + y * 2);
                uint8_t b1 = gb->mmu->Read(tile_address + 1 + y * 2);

                for (uint8_t x = 0; x < 8; x++)
                {
                    uint8_t color = (GET_BIT(b1, 7 - x) << 1) | (GET_BIT(b0, 7 - x));

                    uint16_t pixel_pos_x = tile_x + x;
                    uint16_t pixel_pos_y = tile_y + y;
                    uint32_t id = (pixel_pos_x + pixel_pos_y * 256) * 3;

                    

                    bool in_bounds = false;
                    if (info.show_bg_scroll_view)
                    {
                        if ((scroll_min_corner_x < scroll_max_corner_x && pixel_pos_x >= scroll_min_corner_x && pixel_pos_x <= scroll_max_corner_x) ||
                            (scroll_min_corner_x > scroll_max_corner_x && (pixel_pos_x >= scroll_min_corner_x || pixel_pos_x <= scroll_max_corner_x)))
                        {
                            if ((scroll_min_corner_y < scroll_max_corner_y && pixel_pos_y >= scroll_min_corner_y && pixel_pos_y <= scroll_max_corner_y) ||
                                (scroll_min_corner_y > scroll_max_corner_y && (pixel_pos_y >= scroll_min_corner_y || pixel_pos_y <= scroll_max_corner_y)))
                            {
                                if (pixel_pos_x == scroll_min_corner_x || pixel_pos_x == scroll_max_corner_x || pixel_pos_y == scroll_min_corner_y || pixel_pos_y == scroll_max_corner_y)
                                {
                                    color = 5;
                                }
                                
                                in_bounds = true;
                            }
                        }
                    }

                    if (in_bounds)
                    {
                        if (color == 0)
                        {
                            bg_debug_buffer[id] = 232;
                            bg_debug_buffer[id + 1] = 252;
                            bg_debug_buffer[id + 2] = 204;
                        }
                        else if (color == 1)
                        {
                            bg_debug_buffer[id] = 172;
                            bg_debug_buffer[id + 1] = 212;
                            bg_debug_buffer[id + 2] = 144;
                        }
                        else if (color == 2)
                        {
                            bg_debug_buffer[id] = 84;
                            bg_debug_buffer[id + 1] = 140;
                            bg_debug_buffer[id + 2] = 112;
                        }
                        else if (color == 3)
                        {
                            bg_debug_buffer[id] = 30;
                            bg_debug_buffer[id + 1] = 44;
                            bg_debug_buffer[id + 2] = 56;
                        }
                        else
                        {
                            bg_debug_buffer[id] = 0;
                            bg_debug_buffer[id + 1] = 0;
                            bg_debug_buffer[id + 2] = 0;
                        }
                    }
                    else
                    {
                        color = 3 - color;
                        bg_debug_buffer[id] = (uint8_t)(((float)color / 3.0f) * 255);
                        bg_debug_buffer[id + 1] = (uint8_t)(((float)color / 3.0f) * 255);
                        bg_debug_buffer[id + 2] =  (uint8_t)(((float)color / 3.0f) * 255);
                    }
                }
            }
        }

        glBindTexture(GL_TEXTURE_2D, this->background_vram_texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, bg_debug_buffer);
    }

    if (info.render_oam)
    {
        uint8_t lcd_control = gb->mmu->Read(0xFF40);
        uint8_t sprite_height = (lcd_control & 0x04) ? 16 : 8;
        /*
        for (int i = 0; i < 40; i++)
        {
            uint16_t address = 4 * i;

            Sprite sp;

            sp.y_pos = this->[address];
            sp.x_pos = this->oam[address + 1];
            sp.tile = this->oam[address + 2];
            sp.flags = this->oam[address + 3];

            if (sp.x_pos > 0)
            {
                if (ly + 16 >= sp.y_pos)
                {
                    if (ly + 16 < sp.y_pos + sprite_height)
                    {
                        if (sprite_height == 16)
                        {
                            CLEAR_BIT(sp.tile, 0);
                        }

                        this->object_buffer[this->object_count] = sp;
                        this->object_count++;

                        if (this->object_count >= 10)
                        {
                            break;
                        }
                    }
                }
            }
        }
        */

        for (uint32_t i = 0; i < 40; i++)
        {
            uint32_t address = 0xFE00 + 2 + i * 4;
            uint8_t tile = gb->mmu->Read(address);

            if(sprite_height == 16)
                CLEAR_BIT(tile, 0);
            
            uint16_t tile_address = 0x8000 + tile * 0x10;

            for (uint8_t y = 0; y < sprite_height; y++)
            {
                if (y == 8)
                {
                    SET_BIT(tile, 0);
                    tile_address = 0x8000 + tile * 0x10;
                }

                uint8_t b0 = gb->mmu->Read(tile_address + (y % 8) * 2);
                uint8_t b1 = gb->mmu->Read(tile_address + 1 + (y % 8) * 2);

                for (uint8_t x = 0; x < 8; x++)
                {
                    uint8_t color = (GET_BIT(b1, 7 - x) << 1) | (GET_BIT(b0, 7 - x));
                    uint32_t id = (x + y * 8) * 3;

                    color = 3 - color;

                    sprite_debug_buffer[id] = (uint8_t)(((float)color / 3.0f) * 255);
                    sprite_debug_buffer[id + 1] = (uint8_t)(((float)color / 3.0f) * 255);
                    sprite_debug_buffer[id + 2] = (uint8_t)(((float)color / 3.0f) * 255);

                    if (tile == 0)
                    {
                        if (x == y % 8)
                        {
                            sprite_debug_buffer[id] = 255;
                            sprite_debug_buffer[id + 1] = 0;
                            sprite_debug_buffer[id + 2] = 0;
                        }
                    }
                }
            }

            glBindTexture(GL_TEXTURE_2D, oam_textures[i]);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 8, 16, GL_RGB, GL_UNSIGNED_BYTE, sprite_debug_buffer);
        }
    }
}