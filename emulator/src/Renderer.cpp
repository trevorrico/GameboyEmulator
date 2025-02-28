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
"   ivec2 pixelPos = ivec2(uv * vec2(160, 144));\n"
"   int id = 160 * (144 - pixelPos.y) + pixelPos.x;\n"
"   uint pixelId = pixels[id];\n"
"   if(pixelId == 0) {outColor = vec4(155.0/255.0, 188.0/255.0, 15.0/255.0, 1.0);}\n"
"   else if(pixelId == 1) {outColor = vec4(139.0/255.0, 172.0/255.0, 15.0/255.0, 1.0);}\n"
"   else if(pixelId == 2) {outColor = vec4(48.0/255.0, 98.0/255.0, 48.0/255.0, 1.0);}\n"
"   else if(pixelId == 3) {outColor = vec4(15.0/255.0, 56.0/255.0, 15.0/255.0, 1.0);}\n"
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