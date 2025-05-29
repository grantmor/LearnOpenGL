#include <SDL3/SDL.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_video.h>

#include "sl_type.h"

#include "../include/glad.c"

typedef struct {
    u32 width;
    u32 height;
} Window;

static SDL_Window* g_window = NULL;
static SDL_GLContext g_ogl_ctx = NULL;

static Window window = {1280, 720};
static u8 g_quit = false;

void get_gl_version_info()
{
    SDL_Log("Vendor: %s", glGetString(GL_VENDOR));
    SDL_Log("Renderer: %s", glGetString(GL_RENDERER));
    SDL_Log("Version: %s", glGetString(GL_VERSION));
    SDL_Log("Shading Language: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

int main(int argc, char** argv)
{
    // Initialize Memory

    // Init Video
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL Initialization failed: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    if (!(g_window = SDL_CreateWindow("Game", window.width, window.height, SDL_WINDOW_OPENGL)))
    {
        SDL_Log("Window creation failed: %s\n", SDL_GetError());
        exit(1);
    }

    if(!(g_ogl_ctx = SDL_GL_CreateContext(g_window)))
    {
        SDL_Log("OpenGL context creation failed: %s\n", SDL_GetError());
        exit(1);
    }

    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        SDL_Log("Glad loading failed: %s\n", SDL_GetError());
        exit(1);
    }

    get_gl_version_info();

    SDL_Event e;
    SDL_zero(e);

    glViewport(0, 0, window.width, window.height);

    /*** OpenGL Tutorial ***/

    // Load Shaders
    usize vs_code_size = 0;
    const char* vertex_shader_source = SDL_LoadFile("res/vertex.glsl", &vs_code_size);
    //SDL_Log("vs code size: %ld\n", vs_code_size);

    usize fs_code_size = 0;
    const char* fragment_shader_source = SDL_LoadFile("res/fragment.glsl", &fs_code_size);
    //SDL_Log("fs code size: %ld\n", fs_code_size);
    
    // Create Vertex Shader
    u32 vertex_shader = glad_glCreateShader(GL_VERTEX_SHADER);
    glad_glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glad_glCompileShader(vertex_shader);

    // Check for compilation success
    i32 success;
    char info_log[1024] = {0};
    memset(info_log, 0, sizeof(info_log));
    glad_glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glad_glGetShaderInfoLog(vertex_shader, 1024, NULL, info_log);
        SDL_Log("Vertex shader compilation failed: %s", info_log);
    }

    memset(info_log, 0, sizeof(info_log));

    // Create Fragment Shader
    u32 fragment_shader = glad_glCreateShader(GL_FRAGMENT_SHADER);
    glad_glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glad_glCompileShader(fragment_shader);

    char info_log_2[1024] = {0};
    // Check for compilation success
    glad_glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glad_glGetShaderInfoLog(fragment_shader, 1024, NULL, info_log_2);
        SDL_Log("Fragment shader compilation failed: %s", info_log_2);
    }

    u32 shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    //TODO: Log shader compilation failure

    //glUseProgram(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);


    f32 vertices1[] =
    {
        -1.5f, -0.5f, 0.0f,
         0.0f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
	};

	f32 vertices2[] =
	{
        1.5f, -0.5f, 0.0f,
         0.0f, -0.5f, 0.0f,
        1.0f,  0.5f, 0.0f
    };

	/*
	f32 vertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
	};

	u32 indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};  
	*/
    // Create the Vertex Array Object
    u32 VAO1;
    glGenVertexArrays(1, &VAO1);
    // Bind the VAO
    glad_glBindVertexArray(VAO1);

    // Create VBO 
    u32 VBO1;
    glGenBuffers(1, &VBO1);
    // Only one buffer of each type can be bound at once
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    // Copy buffer to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    // Describe the VAO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*) 0);
    // Enable the VAO
    glad_glEnableVertexAttribArray(0);

    // Create the Vertex Array Object
    u32 VAO2;
    glGenVertexArrays(1, &VAO2);
    // Bind the VAO
    glad_glBindVertexArray(VAO2);

    // Create VBO 
    u32 VBO2;
    glGenBuffers(1, &VBO2);
    // Only one buffer of each type can be bound at once
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    // Copy buffer to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    // Describe the VAO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*) 0);
    // Enable the VAO
    glad_glEnableVertexAttribArray(0);

	// Create Element Buffer Object
	//u32 EBO;
	//glGenBuffers(1, &EBO);

	// Bind EBO
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    while(!g_quit)
    {
        glClearColor(0,0,1.0,1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        while(SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
                g_quit = true;
        }

        //Render
        glUseProgram(shader_program);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(g_window);
    }

    SDL_DestroyWindow(g_window);
    SDL_Quit();

    return 0;
}
