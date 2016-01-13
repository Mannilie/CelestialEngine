#include "GUI.h"
#include "Window.h"
#include "GameObject.h"

bool GUI::mousePressed[3] = { false, false, false };
GLuint GUI::fontTexture = 0;
int GUI::shaderHandle = 0;
int	GUI::vertHandle = 0;
int	GUI::fragHandle = 0;
int GUI::attribLocationTex = 0;
int	GUI::attribLocationProjMtx = 0;
int GUI::attribLocationPosition = 0;
int	GUI::attribLocationUV = 0;
int	GUI::attribLocationColor = 0;
size_t  GUI::vboSize = 0;
unsigned int GUI::vboHandle = 0;
unsigned int GUI::vaoHandle = 0;

static void	renderDrawListsImGui(ImDrawList** const _cmdLists, int _cmdListsCount) {
	if (_cmdListsCount == 0) {
		return;
	}

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	GLint lastProgram, lastTexture;
	glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	// Setup orthographic projection matrix
	const float width = ImGui::GetIO().DisplaySize.x;
	const float height = ImGui::GetIO().DisplaySize.y;
	const float ortho_projection[4][4] = {
		{ 2.0f / width, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 2.0f / -height, 0.0f, 0.0f },
		{ 0.0f, 0.0f, -1.0f, 0.0f },
		{ -1.0f, 1.0f, 0.0f, 1.0f },
	};
	glUseProgram(GUI::shaderHandle);
	glUniform1i(GUI::attribLocationTex, 0);
	glUniformMatrix4fv(GUI::attribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);

	// Grow our buffer according to what we need
	size_t totalVertexCount = 0;
	for (int n = 0; n < _cmdListsCount; n++) {
		totalVertexCount += _cmdLists[n]->vtx_buffer.size();
	}

	glBindBuffer(GL_ARRAY_BUFFER, GUI::vboHandle);
	size_t neededVertexSize = totalVertexCount * sizeof(ImDrawVert);
	if (GUI::vboSize < neededVertexSize) {
		GUI::vboSize = neededVertexSize + 5000 * sizeof(ImDrawVert);  // Grow buffer
		glBufferData(GL_ARRAY_BUFFER, GUI::vboSize, NULL, GL_STREAM_DRAW);
	}

	// Copy and convert all vertices into a single contiguous buffer
	unsigned char* buffer_data = (unsigned char*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (!buffer_data) {
		return;
	}
	for (int n = 0; n < _cmdListsCount; n++) {
		const ImDrawList* cmdList = _cmdLists[n];
		memcpy(buffer_data, &cmdList->vtx_buffer[0], cmdList->vtx_buffer.size() * sizeof(ImDrawVert));
		buffer_data += cmdList->vtx_buffer.size() * sizeof(ImDrawVert);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(GUI::vaoHandle);

	int cmdOffset = 0;
	for (int n = 0; n < _cmdListsCount; n++) {
		const ImDrawList* cmdList = _cmdLists[n];
		int vtxOffset = cmdOffset;
		const ImDrawCmd* pcmdEnd = cmdList->commands.end();
		for (const ImDrawCmd* pcmd = cmdList->commands.begin(); pcmd != pcmdEnd; pcmd++) {
			if (pcmd->user_callback) {
				pcmd->user_callback(cmdList, pcmd);
			} else {
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->texture_id);
				glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
				glDrawArrays(GL_TRIANGLES, vtxOffset, pcmd->vtx_count);
			}
			vtxOffset += pcmd->vtx_count;
		}
		cmdOffset = vtxOffset;
	}

	// Restore modified state
	glBindVertexArray(0);
	glUseProgram(lastProgram);
	glDisable(GL_SCISSOR_TEST);
	glBindTexture(GL_TEXTURE_2D, lastTexture);
}
static const char* getClipboardTextImGui() {
	return glfwGetClipboardString(Window::window);
}
static void	setClipboardTextImGui(const char* text) {
	glfwSetClipboardString(Window::window, text);
}
void GUI::mouseButtonCallback(GLFWwindow*, int _button, int _action, int _mods) {
	if (_action == GLFW_PRESS && _button >= 0 && _button < 3) {
		GUI::mousePressed[_button] = true;
	}
}
void GUI::keyCallback(GLFWwindow*, int _key, int, int _action, int _mods) {
	ImGuiIO& io = ImGui::GetIO();
	if (_action == GLFW_PRESS) {
		io.KeysDown[_key] = true;
	}
	if (_action == GLFW_RELEASE) {
		io.KeysDown[_key] = false;
	}
	(void)_mods; // Modifiers are not reliable across systems
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
}
void GUI::charCallback(GLFWwindow*, unsigned int _c) {
	ImGuiIO& io = ImGui::GetIO();
	if (_c > 0 && _c < 0x10000) {
		io.AddInputCharacter((unsigned short)_c);
	}
}

GUI::GUI(){}
GUI::~GUI(){}
void GUI::Create() {
	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                 // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	io.RenderDrawListsFn = renderDrawListsImGui;
	io.SetClipboardTextFn = setClipboardTextImGui;
	io.GetClipboardTextFn = getClipboardTextImGui;
#ifdef _MSC_VER
	io.ImeWindowHandle = glfwGetWin32Window(Window::window);
#endif

	glfwSetMouseButtonCallback(Window::window, GUI::mouseButtonCallback);
	glfwSetKeyCallback(Window::window, GUI::keyCallback);
	glfwSetCharCallback(Window::window, GUI::charCallback);
}
void GUI::Shutdown() {
	if (vaoHandle) glDeleteVertexArrays(1, &vaoHandle);
	if (vboHandle) glDeleteBuffers(1, &vboHandle);
	vaoHandle = 0;
	vboHandle = 0;

	glDetachShader(shaderHandle, vertHandle);
	glDeleteShader(vertHandle);
	vertHandle = 0;

	glDetachShader(shaderHandle, fragHandle);
	glDeleteShader(fragHandle);
	fragHandle = 0;

	glDeleteProgram(shaderHandle);
	shaderHandle = 0;

	if (fontTexture) {
		glDeleteTextures(1, &fontTexture);
		ImGui::GetIO().Fonts->TexID = 0;
		fontTexture = 0;
	}
	ImGui::Shutdown();
}
void GUI::Update() {
	if (!fontTexture) {
		CreateDeviceObjects();
	}

	ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize = ImVec2((float)Window::width, (float)Window::height);

	// Setup inputs
	// (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
	if (glfwGetWindowAttrib(Window::window, GLFW_FOCUSED)) {
		double mouse_x, mouse_y;
		glfwGetCursorPos(Window::window, &mouse_x, &mouse_y);
		//mouse_x *= (float)display_w / w;                        // Convert mouse coordinates to pixels
		//mouse_y *= (float)display_h / h;
		io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);   // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
	} else {
		io.MousePos = ImVec2(-1, -1);
	}
	
	for (int i = 0; i < 3; i++) {
		io.MouseDown[i] = GUI::mousePressed[i] || glfwGetMouseButton(Window::window, i) != 0;    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
		GUI::mousePressed[i] = false;
	}

	io.MouseWheel = Window::yScroll;
	
	// Start the frame
	ImGui::NewFrame();

	//TransformTool::Update();
}
void GUI::Draw(){}
void GUI::CreateFontsTexture() {
	ImGuiIO& io = ImGui::GetIO();

	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

	glGenTextures(1, &fontTexture);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)fontTexture;

	// Cleanup (don't clear the input data if you want to append new fonts later)
	io.Fonts->ClearInputData();
	io.Fonts->ClearTexData();
}
bool GUI::CreateDeviceObjects() {
	const GLchar *vertex_shader =
		"#version 330\n"
		"uniform mat4 ProjMtx;\n"
		"in vec2 Position;\n"
		"in vec2 UV;\n"
		"in vec4 Color;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"	Frag_UV = UV;\n"
		"	Frag_Color = Color;\n"
		"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";

	const GLchar* fragment_shader =
		"#version 330\n"
		"uniform sampler2D Texture;\n"
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
		"}\n";

	shaderHandle = glCreateProgram();
	vertHandle = glCreateShader(GL_VERTEX_SHADER);
	fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(	vertHandle, 1, &vertex_shader, 0);
	glShaderSource(	fragHandle, 1, &fragment_shader, 0);
	glCompileShader(vertHandle);
	glCompileShader(fragHandle);
	glAttachShader(	shaderHandle, vertHandle);
	glAttachShader(	shaderHandle, fragHandle);
	glLinkProgram(	shaderHandle);

	attribLocationTex = glGetUniformLocation(		shaderHandle, "Texture");
	attribLocationProjMtx = glGetUniformLocation(	shaderHandle, "ProjMtx");
	attribLocationPosition = glGetAttribLocation(	shaderHandle, "Position");
	attribLocationUV = glGetAttribLocation(		shaderHandle, "UV");
	attribLocationColor = glGetAttribLocation(	shaderHandle, "Color");

	glGenBuffers(1, &vboHandle);

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);
	glBindBuffer(GL_ARRAY_BUFFER,	vboHandle);
	glEnableVertexAttribArray(		attribLocationPosition);
	glEnableVertexAttribArray(		attribLocationUV);
	glEnableVertexAttribArray(		attribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
	glVertexAttribPointer(attribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(attribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(attribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	CreateFontsTexture();

	return true;
}