#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define TITLE "DVD Screensaver"
#define START_X 0.0f
#define START_Y 0.0f
#define START_VX 200.0f
#define START_VY 200.0f

extern char _binary_dvd_png_start[];
extern char _binary_dvd_png_end[];
extern char _binary_dvd_png_size[];

extern char _binary_icon_png_start[];
extern char _binary_icon_png_end[];
extern char _binary_icon_png_size[];

typedef struct app_context_t {
	GLFWwindow *window;
	int window_width;
	int window_height;
	GLint shader_program, vao, texture;
	GLfloat mat_proj[16];
	float x, y;
	float vx, vy;
	int width, height;
	float r, g, b;
} app_context_t;

void
fail(char *message)
{
	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}

void
fail_if(bool condition, char *message)
{
	if (condition) {
		fprintf(stderr, "%s\n", message);
		exit(EXIT_FAILURE);
	}
}

void
hsv2rgb(float h, float s, float v, float *r, float *g, float *b)
{
	int i;
	float f, p, q, t;

	i = (int)floorf(h * 6.0f);
	f = h * 6.0f - i;
	p = v * (1.0f - s);
	q = v * (1.0f - f * s);
	t = v * (1.0f - (1.0f - f) * s);

	switch (i % 6) {
	case 0:
		*r = v; *g = t; *b = p;
		break;
	case 1:
		*r = q; *g = v; *b = p;
		break;
	case 2:
		*r = p; *g = v; *b = t;
		break;
	case 3:
		*r = p; *g = q; *b = v;
		break;
	case 4:
		*r = t; *g = p; *b = v;
		break;
	case 5:
		*r = v; *g = p; *b = q;
		break;
	}
}

void
render(app_context_t *context)
{
	GLFWwindow *window = context->window;
	GLint shader_program = context->shader_program;
	GLint vao = context->vao;
	GLint texture = context->texture;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint proj_loc = glGetUniformLocation(context->shader_program, "projection");
	GLuint pos_loc = glGetUniformLocation(context->shader_program, "position");
	GLuint tint_loc = glGetUniformLocation(context->shader_program, "imageTint");
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, context->mat_proj);
	glUniform2f(pos_loc, context->x, context->y);
	glUniform4f(tint_loc, context->r, context->g, context->b, 1.0f);

	glBindTexture(GL_TEXTURE_2D, texture);

	glUseProgram(shader_program);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

GLint
create_shader(const char *vertex_source, const char *fragment_source)
{
	GLuint vertex_shader, fragment_shader, shader_program;
	GLint success;
	GLchar info_log[1024];

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_source, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 1024, NULL, info_log);
		fprintf(stderr, "Failed to compile vertex shader\n%s\n", info_log);
		exit(EXIT_FAILURE);
	}

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_source, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 1024, NULL, info_log);
		fprintf(stderr, "Failed to compile fragment shader\n%s\n", info_log);
		exit(EXIT_FAILURE);
	}

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, 1024, NULL, info_log);
		fprintf(stderr, "Failed to link shader program\n%s\n", info_log);
		exit(EXIT_FAILURE);
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}

void
framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	app_context_t *context = glfwGetWindowUserPointer(window);

	context->window_width = width;
	context->window_height = height;

	glViewport(0, 0, width, height);

	render(context);

	glfwSwapBuffers(context->window);
}

void
window_refresh_callback(GLFWwindow *window)
{
	app_context_t *context = glfwGetWindowUserPointer(window);

	render(context);

	glfwSwapBuffers(context->window);
}

int
main(void)
{
	app_context_t context;

	fail_if(!glfwInit(), "Failed to initialize GLFW");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
	glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);

	context.window = glfwCreateWindow(1, 1, TITLE, NULL, NULL);

	if (context.window == NULL) {
		glfwTerminate();
		fail("Failed to create window");
	}

	glfwGetFramebufferSize(context.window, &context.window_width, &context.window_height);

	glfwSetWindowUserPointer(context.window, &context);

	glfwSetFramebufferSizeCallback(context.window, framebuffer_size_callback);
	glfwSetWindowRefreshCallback(context.window, window_refresh_callback);

	glfwMakeContextCurrent(context.window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		glfwTerminate();
		fail("Failed to load OpenGL");
	}

	glfwSwapInterval(1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Icon
	int icon_width, icon_height, n_channels;
	unsigned char *icon_data = stbi_load_from_memory(_binary_icon_png_start,
		(long long)_binary_icon_png_size, &icon_width, &icon_height, &n_channels, 0);

	GLFWimage icon_image = { icon_width, icon_height, icon_data };
	glfwSetWindowIcon(context.window, 1, &icon_image);

	// Shader
	const char *vertex_source = "#version 330 core\n"
		"layout (location = 0) in vec2 aPos;\n"
		"layout (location = 1) in vec2 aTexCoord;\n"
		"out vec2 texCoord;\n"
		"uniform mat4 projection;\n"
		"uniform vec2 position;\n"
		"void main() {\n"
		"  gl_Position = projection * vec4(aPos + position, 0.0, 1.0);\n"
		"  texCoord = aTexCoord;\n"
		"}";

	const char *fragment_source = "#version 330 core\n"
		"out vec4 outColor;\n"
		"in vec2 texCoord;\n"
		"uniform sampler2D imageTexture;\n"
		"uniform vec4 imageTint;"
		"void main() {\n"
		"  outColor = imageTint * texture(imageTexture, texCoord);\n"
		"}";

	context.shader_program = create_shader(vertex_source, fragment_source);

	// Texture
	glGenTextures(1, &context.texture);
	glBindTexture(GL_TEXTURE_2D, context.texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char *data = stbi_load_from_memory(_binary_dvd_png_start,
		(long long)_binary_dvd_png_size, &context.width, &context.height, &n_channels, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, context.width, context.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		fail("Failed to load texture");
	}

	stbi_image_free(data);

	for (uint32_t i = 0; i < 16; i++) {
		context.mat_proj[i] = 0.0f;
	}

	context.mat_proj[0] = 1.0f / ((float)context.window_width / 2.0f);
	context.mat_proj[5] = -1.0f / ((float)context.window_height / 2.0f);
	context.mat_proj[10] = 1.0f;
	context.mat_proj[15] = 1.0f;
	context.mat_proj[12] = -1.0f;
	context.mat_proj[13] = 1.0f;

	context.x = START_X;
	context.y = START_Y;
	context.vx = START_VX;
	context.vy = START_VY;

	// Buffers
	GLfloat vertices[] = {
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, (float)context.height, 0.0f, 1.0f,
		(float)context.width, 0.0f, 1.0f, 0.0f,
		(float)context.width, (float)context.height, 1.0f, 1.0f
	};

	GLuint indices[] = {
		0, 1, 2,
		1, 2, 3
	};

	GLuint ebo, vbo;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &context.vao);

	glBindVertexArray(context.vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	srand(time(NULL));

	float hue = (float)rand() / (float)RAND_MAX;
	hsv2rgb(hue, 0.5f, 0.95f, &context.r, &context.g, &context.b);

	double last_time = 0.0;

	while (!glfwWindowShouldClose(context.window)) {
		double current_time = glfwGetTime();
		if (last_time == 0.0) last_time = current_time;
		float dt = (float)(current_time - last_time);
		last_time = current_time;

		float next_left = context.x + context.vx * dt;
		float next_right = next_left + (float)context.width;
		float next_top = context.y + context.vy * dt;
		float next_bottom = next_top + (float)context.height;

		if (next_right > context.window_width || next_left < 0.0f) {
			context.vx *= -1.0f;
			hue = (float)rand() / (float)RAND_MAX;
			hsv2rgb(hue, 0.5f, 0.95f, &context.r, &context.g, &context.b);
		}

		if (next_bottom > context.window_height || next_top < 0.0f) {
			context.vy *= -1.0f;
			hue = (float)rand() / (float)RAND_MAX;
			hsv2rgb(hue, 0.5f, 0.95f, &context.r, &context.g, &context.b);
		}

		context.x += context.vx * dt;
		context.y += context.vy * dt;

		render(&context);

		glfwSwapBuffers(context.window);
		glfwPollEvents();
	}

	glfwTerminate();

	return EXIT_SUCCESS;
}