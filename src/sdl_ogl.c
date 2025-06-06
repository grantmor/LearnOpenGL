#include <SDL3/SDL.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

#include <math.h>

#include "sl_type.h"

#include "../include/glad.c"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#include "../include/cglm-master/include/cglm/cglm.h"

typedef struct {
    u32 width;
    u32 height;
} Window;

typedef struct {
	i32 width;
	i32 height;
	i32 num_channels;
} Texture;

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

	// Load Texture Image 1
	Texture container;
	//i32 width, height, num_channels;
	unsigned char* container_image = stbi_load("res/container.jpg", &container.width, &container.height, &container.num_channels, 0);

	u32 texture1;
	glGenTextures(1, &texture1);

	//Activate the texture unit (NOT NECESSARY IF ONLY ONE TEXTURE - it will auto map to texture unit 0 (probably))
	glActiveTexture(GL_TEXTURE0);
	// Bind Texture
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Generate Texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, container.width, container.height, 0, GL_RGB, GL_UNSIGNED_BYTE, container_image);
	glGenerateMipmap(GL_TEXTURE_2D);
	free(container_image);

	// Load Texture Image 2
	Texture smile;
	//i32 width, height, num_channels;
	stbi_set_flip_vertically_on_load(true); 
	unsigned char* smile_image = stbi_load("res/awesomeface.png", &smile.width, &smile.height, &smile.num_channels, 0);

	u32 texture2;
	glGenTextures(1, &texture2);

	//Activate the texture unit (NOT NECESSARY IF ONLY ONE TEXTURE - it will auto map to texture unit 0 (probably))
	glActiveTexture(GL_TEXTURE1);
	// Bind Texture
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Generate Texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, smile.width, smile.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, smile_image);
	glGenerateMipmap(GL_TEXTURE_2D);
	free(smile_image);

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

    glUseProgram(shader_program);
	glUniform1i(glGetUniformLocation(shader_program, "container"), 0);
	glUniform1i(glGetUniformLocation(shader_program, "smile"), 1);

    /*****?*/

	// Clean up shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);


/*
    f32 verticies[] =
    {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
*/
/*
	f32 vertices[] = {
		// Positions		 Colors				UVs
    	 0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,	1.0, 1.0, // top right
    	 0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 0.0f, 	1.0, 0.0, // bottom right
    	-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 	0.0, 0.0, // bottom left
    	-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 0.0f, 	0.0, 1.0 // top left 
	};
		*/
float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

	vec3 cubePositions[] = {
		{0.1f,  0.1f,  0.1f}, 
		{-2.0f,  5.0f, -15.0f}, 
		{-1.5f, -2.2f, -2.5f},  
		{2.8f, -2.0f, -12.3f},  
		{-2.4f, -0.4f, -3.5f},  
		{1.7f,  3.0f, -7.5f},  
		{-0.3f, -2.0f, -2.5f},  
		{3.5f,  2.0f, -2.5f}, 
		{0.5f,  1.2f, -1.5f}, 
		{1.3f,  1.0f, -1.5f}  
	};


	u32 indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};  

    // Create the Vertex Array Object
    u32 VAO;
    glGenVertexArrays(1, &VAO);
    // Bind the VAO - For some reason, the VAO should be bound before the VBO
	// even though in a trivial example with one of each, it doesn't matter
    glad_glBindVertexArray(VAO);

    // Create VBO 
    u32 VBO;
    glGenBuffers(1, &VBO);
    // Only one buffer of each type can be bound at once
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Copy buffer to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	// Calculate stride for VAO
	//u32 va_stride = 8 * sizeof(f32);
	u32 va_stride = 5 * sizeof(f32);
    // Describe the VAO position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, va_stride, (void*) 0);
    // Enable the VAO position attribute
    glad_glEnableVertexAttribArray(0);

    // Describe the VAO color attribute
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, va_stride, (void*) (3 * sizeof(f32)));
    // Enable the VAO color attribute
    //glad_glEnableVertexAttribArray(1);

    // Describe the VAO Texture Coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, va_stride, (void*) (3 * sizeof(f32)));
    // Enable the VAO color attribute
    glad_glEnableVertexAttribArray(1);

	// Create Element Buffer Object
	u32 EBO;
	glGenBuffers(1, &EBO);

	// Bind EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	// Enable Depth Buffer
	glEnable(GL_DEPTH_TEST);

    while(!g_quit)
    {
        glClearColor(0,0,1.0,1.0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        while(SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
                g_quit = true;
        }

        //Render
		f32 time_value = SDL_GetTicks() / 1000.0;
		f32 red_value = sin(time_value) / 2.0f + 0.5f;
		usize vertex_color_location = glad_glGetUniformLocation(shader_program, "uniColor");
		usize model_location = glGetUniformLocation(shader_program, "model");
		usize view_location = glGetUniformLocation(shader_program, "view");
		usize projection_location = glGetUniformLocation(shader_program, "projection");
        glUseProgram(shader_program);
		glUniform4f(vertex_color_location, red_value, 0.0, 0.0, 1.0);

        // Model Transformation Matrix
        vec4 vec = {1.0f, 0.0f, 0.0f, 1.0f};
        vec4 result;

        mat4 model_txfrm;
        f32 xFormFactor = SDL_GetTicks() / 500.0; 
		//f32 xFormFactor = glm_rad(-55.0);
        // f32 xFormFactor = 1.0; 
        glm_mat4_identity(model_txfrm);

        // Translate
        glm_translate(model_txfrm, (vec3){0.0f, 0.0f, 0.0f});

		// Rotate
        //vec4 z_axis = {0.0, 0.0, 1.0, 0.0};
        //glm_rotate_x(model_txfrm, glm_rad(-55.0), model_txfrm); // GLM_PI_2 is π/2 (90 degrees)
        //glm_rotate_x(model_txfrm, glm_rad(-55.0), model_txfrm); // GLM_PI_2 is π/2 (90 degrees)
        glm_rotate_x(model_txfrm, xFormFactor, model_txfrm); // GLM_PI_2 is π/2 (90 degrees)

		// Scale
        //glm_scale(model_txfrm, (vec3){sin(xFormFactor) +1.0, sin(xFormFactor)+1.0, 1.0f});
        //glm_scale(model_txfrm, (vec3){sin(xFormFactor) +1.0, sin(xFormFactor)+1.0, 1.0f});

        // Apply Transformation to vertices
        glm_mat4_mulv(model_txfrm, vec, result);
		glUniformMatrix4fv(model_location, 1, GL_FALSE, (GLfloat*)model_txfrm);

		// View Transformation Matrix
		mat4 view_txfrm;
		glm_mat4_identity(view_txfrm);
		glm_translate(view_txfrm, (vec3){0.0, 0.0, -3.0});
		glUniformMatrix4fv(view_location, 1, GL_FALSE, (GLfloat*)view_txfrm);

		// Projection Matrix
		mat4 proj_txfrm;
		glm_perspective(glm_rad(45.0), (f32)window.width / (f32)window.height, 0.01, 100.0, proj_txfrm);
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, (GLfloat*)proj_txfrm);

        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		for (u32 i=0; i < 10; i++)
		{
        	//glm_translate(model_txfrm, (vec3){i* 0.1f, 0.0f, 0.0f});
			glm_mat4_identity(model_txfrm);
        	glm_translate(model_txfrm, cubePositions[i]);
        	glm_rotate_x(model_txfrm, glm_rad(27.0 * i), model_txfrm); // GLM_PI_2 is π/2 (90 degrees)
        	glm_rotate_y(model_txfrm, glm_rad(33.0 * i), model_txfrm); // GLM_PI_2 is π/2 (90 degrees)
        	glm_rotate_z(model_txfrm, glm_rad(18.0 * i), model_txfrm); // GLM_PI_2 is π/2 (90 degrees)
			glUniformMatrix4fv(model_location, 1, GL_FALSE, (GLfloat*)model_txfrm);


        	glDrawArrays(GL_TRIANGLES, 0, 36);
		}

        SDL_GL_SwapWindow(g_window);
    }

    SDL_DestroyWindow(g_window);
    SDL_Quit();

    return 0;
}
