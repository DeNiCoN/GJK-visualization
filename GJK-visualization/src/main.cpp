#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "linearmath.h"
#include "gjk.h"

GLFWwindow* window;
mat44 fmodel, smodel, projection;


const char * v = "#version 330 core\n" 
"layout(location = 0) in vec2 vertex;\n" 
"uniform vec2 position;\n" 
"uniform mat4 projection;\n" 
"void main()\n" 
"{\n" 
"	gl_Position = projection * vec4(position + vertex, 1.f, 1.f);\n" 
"}";

const char * f = "#version 330 core\n"
"out vec4 fColor;\n"
"uniform vec4 color;\n"
"void main()\n"
"{\n"
"	fColor = color;\n"
"}";

unsigned int VAO1, VAO2, VAO3, VBO1, VBO2, VBO3, vertex, fragment, shader;
vec4 fcolor = { 0.f, 0.f, 1.f, 1.f };
vec4 scolor = { 1.f, 0.f, 0.f, 1.f };
vec4 mcolor = { 0.f, 1.f, 0.f, 1.f };
vec2 position1, position2 = { 200.f, 0.f };

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

vec2 minkowskiVertices[sizeof(vertices1) / sizeof(vertices1[0]) * sizeof(vertices2) / sizeof(vertices2[0])];

int orientation(vec2 p, vec2 q, vec2 r)
{
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear 
	return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

//Jarvis’s Algorithm 
void convexHull(const vec2 points[], int size, vec2 hullPoints[], int* hullSize)
{
	// There must be at least 3 points 
	*hullSize = 0;
	if (size < 3) return;

	// Find the leftmost point 
	int l = 0;
	for (int i = 1; i < size; i++)
		if (points[i].x < points[l].x)
			l = i;

	// Start from leftmost point, keep moving counterclockwise 
	// until reach the start point again.  This loop runs O(h) 
	// times where h is number of points in result or output. 
	int p = l, q;
	do
	{
		// Add current point to result 
		hullPoints[(*hullSize)++] = points[p];

		// Search for a point 'q' such that orientation(p, x, 
		// q) is counterclockwise for all points 'x'. The idea 
		// is to keep track of last visited most counterclock- 
		// wise point in q. If any point 'i' is more counterclock- 
		// wise than q, then update q. 
		q = (p + 1) % size;
		for (int i = 0; i < size; i++)
		{
			// If i is more counterclockwise than current q, then 
			// update q 
			if (orientation(points[p], points[i], points[q]) == 2)
				q = i;
		}

		// Now q is the most counterclockwise with respect to p 
		// Set p as q for next iteration, so that q is added to 
		// result 'hull' 
		p = q;

	} while (p != l);  // While we don't come to first point 
}

vec2 fartestPoint(vec2 direction, vec2* vertices, int size)
{
	float max = vec2Dot(vertices[0], direction);
	vec2 tmp = vertices[0];
	float t = 0;
	for (int i = 1; i < size; i++)
	{
		t = vec2Dot(vertices[i], direction);
		if (t > max)
		{
			max = t;
			tmp = vertices[i];
		}
	}
	return tmp;
}

vec2 firstGJKSupportFunction(vec2 direction)
{
	return vec2Add(fartestPoint(direction, vertices1, size1), position1);
}

vec2 secondGJKSupportFunction(vec2 direction)
{
	return vec2Add(fartestPoint(direction, vertices2, size2), position2);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	projection = mat44Orto(width / 2, -width / 2, height / 2, -height / 2, 0.1f, 1024.f);
	glViewport(0, 0, width, height);
}

void updateBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), &vertices2, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), &vertices1, GL_DYNAMIC_DRAW);
}

void init()
{
	glGenVertexArrays(1, &VAO1);
	glGenVertexArrays(1, &VAO2);
	glGenVertexArrays(1, &VAO3);

	glGenBuffers(1, &VBO1);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &VBO3);

	glBindVertexArray(VAO1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), &vertices1, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), &vertices2, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
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

void drag(vec2** dragged, int* drag, vec2 cursorPos, vec2* position, vec2* vertices, int size)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		if (*drag)
		{
			**dragged = vec2Sub(cursorPos, *position);
			vec2 average = { 0.f, 0.f };
			for (int i = 0; i < size; i++)
			{
				average = vec2Add(average, vertices[i]);
			}
			average = vec2Scale(average, 1.f / 3.f);
			for (int i = 0; i < size; i++)
			{
				vertices[i] = vec2Sub(vertices[i], average);
			}
			*position = vec2Add(*position, average);
			updateBuffers();
		}
		else
		{
			vec2 tmp;
			float minl = 1000.f;
			for (int i = 0; i < size; i++)
			{
				tmp = vec2Sub(vertices[i], vec2Sub(cursorPos, *position));
				float len = vec2LengthSquared(tmp);
				if (len < minl)
				{
					*dragged = &vertices[i];
					minl = len;
				}
			}
			if (minl < 100.f)
				*drag = 1;
		}
	}
	else
	{
		*drag = 0;
	}
}

void processInput()
{
	if (!(ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyWindowFocused()))
	{
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		vec2 cursorPos = { (float)xpos - width / 2, (float)-ypos + height / 2 };

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
		{
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
				position1 = cursorPos;
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
				position2 = cursorPos;
		}
		else
		{
			//doing some weird things
			static int drag1 = 0, drag2 = 0;
			static vec2* pdragged1, *pdragged2;
			static vec2** dragged1; dragged1 = &pdragged1;
			static vec2** dragged2; dragged2 = &pdragged2;

			drag(dragged1, &drag1, cursorPos, &position1, vertices1, size1);
			drag(dragged2, &drag2, cursorPos, &position2, vertices2, size2);
		}
	}
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

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	double xpos, ypos;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		processInput();

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

		vec2 nul = { 0.f, 0.f };
		glUniform4fv(glGetUniformLocation(shader, "color"), 1, (float*) &mcolor);
		glUniform2fv(glGetUniformLocation(shader, "position"), 1, &nul.x);

		ImGui::Begin("Options");
		ImGuiIO io = ImGui::GetIO();
		ImGui::Text("Window: Mouse x: %f | Mouse y: %f", io.MousePos.x, io.MousePos.y);
		glfwGetCursorPos(window, &xpos, &ypos);
		vec2 cursorPos = { (float)xpos - width / 2, (float)-ypos + height / 2 };
		ImGui::Text("World: Mouse x: %f | Mouse y: %f", cursorPos.x, cursorPos.y);
		//ImGui::Text("Intersecting: %s", GJKIntersect((GJKSupportFunction) &firstGJKSupportFunction, (GJKSupportFunction) secondGJKSupportFunction) ? "true" : "false");
		bool checked;
		ImGui::Checkbox("Draw minkowski difference", &checked);
		if (checked)
		{
			//construct minkowski difference polygon
			for (int i = 0; i < size1; i++)
			{
				for (int j = 0; j < size2; j++)
				{
					minkowskiVertices[i * size2 + j] = vec2Sub(vec2Add(vertices1[i], position1), vec2Add(vertices2[j], position2));
				}
			}
			glBindBuffer(GL_ARRAY_BUFFER, VBO3);
			glBufferData(GL_ARRAY_BUFFER, size1 * size2 * sizeof(vec2), NULL, GL_DYNAMIC_DRAW);
			vec2* ptr = (vec2*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

			int hullSize;
			convexHull(minkowskiVertices, size1 * size2, ptr, &hullSize);

			glUnmapBuffer(GL_ARRAY_BUFFER);
			glBindVertexArray(VAO3);
			glDrawArrays(GL_LINE_LOOP, 0, hullSize);
		}
		if (ImGui::CollapsingHeader("Help"))
		{
			ImGui::BulletText("Left click to move first object, right for second");
			ImGui::BulletText("Shift + drag on object points to change it");
		}
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