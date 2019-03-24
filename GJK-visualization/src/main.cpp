#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "linearmath.h"

GLFWwindow* window;
mat44 fmodel, smodel, projection;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	projection = mat44Orto(width / 2, width / 2, height / 2, height / 2, 0.1f, 1024.f);
	glViewport(0, 0, width, height);
}

const char * v = "#version 330 core\n" 
"layout(location = 0) in vec2 vertex;\n" 
"uniform mat4 model;\n" 
"uniform mat4 projection;\n" 
"void main()\n" 
"{\n" 
"	gl_Position = projection * model * vec4(vertex, 1.f, 1.f);\n" 
"}";

const char * f = "#version 430 core\n"
"out vec4 fColor;\n"
"uniform vec4 color;\n"
"void main()\n"
"{\n"
"	fColor = color;\n"
"}";

unsigned int VAO1, VAO2, VBO1, VBO2, EBO1, EBO2, vertex, fragment, shader;
vec4 fcolor = { 0.f, 0.f, 1.f, 1.f };
vec4 scolor = { 1.f, 0.f, 0.f, 1.f };
vec4 mcolor = { 0.f, 1.f, 0.f, 1.f };

float vertices1[] = {
 -100.f, -100.f ,
 -100.f, 100.f ,
 100.f, 100.f 
};

float vertices2[] = {
 -100.f, -100.f ,
 -100.f, 100.f ,
 100.f, 100.f,
 100.f, -100.f
};

unsigned int indices1[] = {
	0, 1,
	1, 2,
	2, 0
};

unsigned int indices2[] = {
	0, 1,
	1, 2,
	2, 3,
	3, 0
};

void init()
{
	glGenVertexArrays(1, &VAO1);

	glGenBuffers(1, &VBO1);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);
	glGenBuffers(1, &EBO1);


	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), &vertices1, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), &vertices2, GL_STATIC_DRAW);

	glBindVertexArray(VAO1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), &indices1, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), &indices2, GL_STATIC_DRAW);

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
	
	init();
	
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, 1);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glUseProgram(shader);
		glUniform4fv(glGetUniformLocation(shader, "color"), 1, (float*) &fcolor);
		glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, (float*) &projection);
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&mat44Identity(1.f));

		glBindVertexArray(VAO1);
		glBindBuffer(GL_ARRAY_BUFFER, VBO1);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
		glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);

		glUniform4fv(glGetUniformLocation(shader, "color"), 1, (float*) &scolor);
		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
		glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);

		ImGui::Begin("Options");
		ImGui::Text("This is some useful text");
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