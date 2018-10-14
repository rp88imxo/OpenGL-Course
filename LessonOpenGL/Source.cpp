#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <glm\glm.hpp> // Либа GLM
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "ShaderClassRML.h"

#define WND_WIDTH 800
#define WND_HEIGHT 600

LPSTR wndTitle = (LPSTR)"Lesson1";

void framebuffer_size_callback(GLFWwindow* window, int width, int height); // Обработчик изменения размера окна
void processInput(GLFWwindow* window); // Обработчик нажатий на клавиши

GLfloat* MakeGrid(float (*y)(float x), float a, float b, unsigned int count = 512, bool isXnorm = true);
void CreateAxis();
float fun1(float x);
float fun2(float x);
float fun3(float x);
float fun4(float x);

// Некоторые глобальные переменные

// Матрицы трансформаций
glm::mat4 trans(1.);
glm::mat4 translationMatrix(1.);
glm::mat4 scaleMatrix(1.);

unsigned int Points = 8192;

float lineWidth = 1.;
float pDiv = 1.05f;
int RotateHard = 1.0f;
float mix = 0.5f;

float Angle = 1.;

int main()
{
	system("chcp 1251");

	if (!glfwInit())
		return -1;
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 16);
	
	GLFWwindow* window = glfwCreateWindow(WND_WIDTH, WND_HEIGHT, wndTitle, NULL,NULL);
	if (!window)
		std::cout << "Can't init the GLFW  window\n" << std::endl;

	glfwMakeContextCurrent(window);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, WND_WIDTH, WND_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	Shader shader("Shaders/vertex.dat","Shaders/fragment.dat");
	

	GLfloat vertices[] = 
	{
		// Position				// Color
		-0.5f, -0.5f, 0.0f,		0.5f, 0.2f, 0.1f,
		0.5f, -0.5f, 0.0f,		0.3f, 1.f, 0.1f,
		0.0f,  0.5f, 0.0f,		0.1f, 0.2f, 0.3f
	};

	GLfloat QuadVertices[] = 
	{
		// positions          // colors           // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};

	GLfloat tCoords[] = 
	{
		0.f, 0.f,
		1.f, 0.f,
		0.5f, 1.f
	};

	GLuint QuadInd[] =
	{
		3, 0, 1,
		1, 2, 3,
	};

	
	
	GLuint VBO[2], EBO;

	GLuint VAO[2];

	GLfloat* Grid =  MakeGrid(fun4, -100, 100, Points, false) ;

	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	// Привязка к данному VAO
	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QuadInd), QuadInd, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * Points * 3, Grid, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);


	glBindVertexArray(0);
	//Отвязка от VAO

	// Текстуры
	stbi_set_flip_vertically_on_load(true);

	int tWidth, tHeight, nrChannels;
	unsigned char *data = stbi_load("Textures/sad.png", &tWidth, &tHeight, &nrChannels, 0);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tWidth, tHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	stbi_image_free(data);



	data = stbi_load("Textures/awesomeface.png", &tWidth, &tHeight, &nrChannels, 0);

	GLuint textureID2;
	glGenTextures(1, &textureID2);
	glBindTexture(GL_TEXTURE_2D, textureID2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tWidth, tHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	shader.Activate();
	shader.setInt("ourTexture1", 0);
	shader.setInt("ourTexture2", 1);

	// Настройки рендеринга
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_MULTISAMPLE);
	glLineWidth(lineWidth);

	//for (int i = 0; i < Points * 3; i += 3)
	//{
	//	std::cout << Grid[i] <<"|" << Grid[i + 1] << "|" << Grid[i + 2] << std::endl;
	//}

	while (!glfwWindowShouldClose(window))
	{
		// Обработка ввода
		processInput(window);

#pragma region RENDERING

		glLineWidth(lineWidth);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// Изменение матриц, векторов, цвета и тд
		
		// ...

		shader.Activate();
		shader.setFloat("pDiv", pDiv);
		shader.setFloat("time", glfwGetTime());
		shader.setFloat("blend", mix);
		shader.setMat4("trans", glm::value_ptr(trans));
		shader.setMat4("translationMatrix", glm::value_ptr(translationMatrix));
		shader.setMat4("scaleMatrix", glm::value_ptr(scaleMatrix));
		
		/*glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureID2);

		glBindVertexArray(VAO[0]);

		glDrawElements(GL_TRIANGLES, sizeof(QuadVertices)/sizeof(GLfloat), GL_UNSIGNED_INT, 0);*/

		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_LINE_STRIP, 0, Points);
		
		glBindVertexArray(0);

#pragma endregion

		
		// Проверка и выполнение событий и свап буферов
		glfwSwapBuffers(window);
		glfwPollEvents(); // Обработка событый, таких как нажатия клавиш, мышь и тд
	}



	glfwTerminate(); // Очищаем занятые ресурсы
	return 1;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, TRUE);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		scaleMatrix = glm::scale(scaleMatrix, glm::vec3(pDiv, pDiv, pDiv));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		scaleMatrix = glm::scale(scaleMatrix, glm::vec3(1 / pDiv, 1 / pDiv,1 / pDiv));

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		if (mix <= 1.f)
			mix += 0.05;
		
	}
		
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		if (mix >= 0.0)
			mix -= 0.05;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		trans = glm::rotate(trans, glm::radians(-Angle), glm::vec3(0., 1., 0.));
	}
	
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		trans = glm::rotate(trans, glm::radians(Angle), glm::vec3(0., 1., 0.));
	}

	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
	{
		if (lineWidth < 3.)
			lineWidth += 0.1;
	}

	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
	{
		if (lineWidth > 0.1f)
			lineWidth -= 0.1;
	}
	
	
}

GLfloat * MakeGrid(float(*y)(float x),float a, float b ,unsigned int count, bool isXnorm)
{
	GLfloat* grid = new GLfloat[count * 3];
	float norm = 2. / fabs(b - a);
	float step = fabs(b - a) / count;
	float Min = a;
	float Max = b;
	float x = 2 * ((a - Min)/ (Max - Min)) - 1.;



	for (int i = 0; i < count * 3; i += 3)
	{
		grid[i] = isXnorm ? x : a;
		grid[i + 1] = y(a);
		grid[i + 2] = 1.f;
		a += step;

		x = 2 * ((a - Min) / (Max - Min)) - 1.;
	}

	return grid;
}

void CreateAxis()
{

}

float fun1(float x)
{
	return x * x;
}

float fun2(float x)
{
	return sin(x + 5) * cos(x * 5) * sqrt(pow(sin(x),2)) + 0.1 / (x + 1.001241242362);
}

float fun3(float x)
{
	return x;
}

float fun4(float x)
{
	return log(fabs(x));
}


