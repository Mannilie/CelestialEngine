#include "Window.h"

// Utilities
#include "GLFW_Header.h"

// Other
#include <cstdio>

Window* Window::instance = nullptr;

GLFWwindow* Window::window = nullptr;
int Window::width = 400;
int Window::height = 600;
string Window::title = "GameEngine_EV - v(5.0)";
float Window::xScroll = 0.0f;
float Window::yScroll = 0.0f;

void __stdcall OpenGLDebugCallback(GLenum _source, GLenum _type, GLuint _id,
	GLenum _severity, GLsizei _length, const GLchar* _message, const void* _userParam) {
	if (_type != GL_DEBUG_TYPE_OTHER) {
		printf("%s\n", _message);
	}
}

bool Window::Create(int _width, int _height, string _title) {
	if (instance == nullptr) {
		instance = new Window();
	}

	// Initialized GLFW and checks if it failed
	if (glfwInit() == false) {
		// Exits if GLFW failed to initialize
		return false;
	}

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	// Creates a GLFW window 
	window = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);

	// Checks if the window was actually created
	if (window == nullptr) {
		// Exits if the window failed to get created
		return false;
	}

	// Sets the current context to the window that was just created
	glfwMakeContextCurrent(window);

	// Evaluates the graphics card driver for the OpenGL versions
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		// Unloads all of GLFW's created contents if the load failed
		glfwDestroyWindow(window);
		glfwTerminate();
		return false;
	}

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	int major_version = ogl_GetMajorVersion();
	int minor_version = ogl_GetMinorVersion();

	printf("successfully loaded OpenGL version %d.%d\n", major_version, minor_version);

	width = _width;
	height = _height;
	title = _title;

	glDebugMessageCallback((GLDEBUGPROC)OpenGLDebugCallback, NULL);

	return true;
}
bool Window::IsCloseRequested() {
	if (glfwWindowShouldClose(window)) {
		return true;
	}
	return false;
}
void Window::Shutdown() {
	glfwDestroyWindow(window);
	glfwTerminate();
}
bool Window::Update() {
	// Refreshes the frame buffer and the viewport when the user resizes the window
	glfwGetWindowSize(window, &width, &height);
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	return true;
}
void Window::Draw() {
	glfwSwapInterval(0);
	glfwSwapBuffers(window);
	glfwPollEvents();
}
void Window::BindAsRenderTarget() {
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
}