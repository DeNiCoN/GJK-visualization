#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "linearmath.h"

GLFWwindow* window;
mat44 fmodel, smodel, projection;


const char * v = "#version 330 core\n" 
"layout(location = 0) in vec2 vertex;\n" 
"uniform vec2 position;\n" 
"uniform mat4 projection;\n" 
"void main()\n" 
"{\n" 
"	gl_Position = projection * (vec4(position, 0.f, 0.f) + vec4(vertex, 1.f, 1.f));\n" 
"}";

const char * f = "#version 430 core\n"
"out vec4 fColor;\n"
"uniform vec4 color;\n"
"void main()\n"
"{\n"
"	fColor = color;\n"
"}";

unsigned int VAO1, VAO2, VBO1, VBO2, vertex, fragment, shader;
vec4 fcolor = { 0.f, 0.f, 1.f, 1.f };
vec4 scolor = { 1.f, 0.f, 0.f, 1.f };
vec4 mcolor = { 0.f, 1.f, 0.f, 1.f };
vec2 position1, position2;

int size1 = 3, size2 = 4;
vec2 vertices1[16] = {
{-50.f, -50.f},
{0.f, 50.f},
{50.f, -50.f}
};

vec2 vertices2[16] = {
 {-50.f, -50.f},
 {-50.f, 50.f},
 {50.f, 50.f},
 {50.f, -50.f}
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	projection = mat44Orto(width / 2, -width / 2, height / 2, -height / 2, 0.1f, 1024.f);
	glViewport(0, 0, width, height);
}

float lengthBetweenLineAndPoint(vec2 fp, vec2 sp, vec2 p)
{
	float len = (sp.y - fp.y) * p.x - (sp.x - fp.x) * p.y + sp.x * fp.y - sp.y * fp.x;
	if (len < 0)
		len = -len;
	float l = sqrtf((sp.y - fp.y) * (sp.y - fp.y) + (sp.x - fp.x) * (sp.x - fp.x));
	return len / l;
}

void addPoint(vec2* a, int* size, vec2 point)
{
	if (*size == 16)
		return;
	int index = 0;
	int min = 1000;

	//find nearest pair of points
	for (int i = 0; i < (*size - 1); i++)
	{
		int len = lengthBetweenLineAndPoint(a[i], a[i + 1], point);
		if (min > len)
		{
			index = i;
			min = len;
		}
	}

	for (int i = *size; i > index; i--)
	{
		a[i] = a[i - 1];
	}
	a[index + 1] = point;
}

void init()
{
	glGenVertexArrays(1, &VAO1);
	glGenVertexArrays(1, &VAO2);

	glGenBuffers(1, &VBO1);
	glGenBuffers(1, &VBO2);

	glBindVertexArray(VAO1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), &vertices1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), &vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	int  success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertex, 1, &v, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s \n", infoLog);
	}

	glShaderSource(fragment, 1, &f, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n %s \n", infoLog);
	}

	shader = glCreateProgram();
	glAttachShader(shader, vertex);
	glAttachShader(shader, fragment);
	glLinkProgram(shader);
	// check for linking errors
	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader, 512, NULL, infoLog);
		printf("ERROR::SHADER::LINK_FAILED\n %s \n", infoLog);
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	projection = mat44Orto(800 / 2, -800 / 2, 600 / 2, -600 / 2, 0.1f, 1024.f);
}

int main()
{
	if (!glfwInit())
		return 1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(800, 600, "GJK visualization", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return 3;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return 2;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glViewport(0, 0, 800, 600);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
	
	init();
	
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, 1);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		if (!(ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyWindowFocused()))
		{
			int width, height;
			double xpos, ypos;
			glfwGetWindowSize(window, &width, &height);
			glfwGetCursorPos(window, &xpos, &ypos);
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
				position1 = { (float)xpos - width / 2, (float)-ypos + height / 2 };
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
				position2 = { (float)xpos - width / 2, (float)-ypos + height / 2 };
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glUseProgram(shader);
		glUniform4fv(glGetUniformLocation(shader, "color"), 1, (float*) &fcolor);
		glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, (float*) &projection);
		glUniform2fv(glGetUniformLocation(shader, "position"), 1, &position1.x);

		glBindVertexArray(VAO1);
		glDrawArrays(GL_LINE_LOOP, 0, size1);

		glUniform4fv(glGetUniformLocation(shader, "color"), 1, (float*) &scolor);
		glUniform2fv(glGetUniformLocation(shader, "position"), 1, &position2.x);
		glBindVertexArray(VAO2);
		glDrawArrays(GL_LINE_LOOP, 0, size2);

		ImGui::Begin("Options");
		ImGuiIO io = ImGui::GetIO();
		ImGui::Text("Mouse x: %f | Mouse y: %f", io.MousePos.x, -io.MousePos.y);
		ImGui::ColorPicker4("Object1", &fcolor.x);
		ImGui::ColorPicker4("Object2", &scolor.x);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	return 0;
}