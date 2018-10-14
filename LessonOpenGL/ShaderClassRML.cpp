#include "ShaderClassRML.h"

Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath)
{
	std::string
		vertexCode,
		fragmentCode;
	std::ifstream
		vShaderFile,
		fShaderFile;

	// Возможность выбрасывание исключений при работе с файловым потоком

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// Открытие файлов

		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		std::stringstream
			vShaderStream,
			fShaderStream;
		
		// Считываем в поток данные из файлов 
		
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();


		// Закрытие файловых потоков

		vShaderFile.close();
		fShaderFile.close();

		// Конвертим в в строку

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Ошибка чтения файлов шейдера:\n" << e.what() << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// Компилирование шейдеров
	unsigned int
		vertex, fragment;
	int success;
	char infoLog[512];

	// Вершинный шейдер
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "Ошибка компиляции вершинного шейдера!\n" << infoLog << std::endl;
	}

	// Фрагментый шейдер
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "Ошибка компиляции фрагментного шейдера!\n" << infoLog << std::endl;
	}

	// Шейдерная программа
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "Ошибка компиляции шейдерной программы!\n" << infoLog << std::endl;
		ID = INT32_MAX;
	}

	// Удалаем из памяти шейдеры, т.к программа уже скомпилированна
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::Activate()
{
	glUseProgram(ID);
}

unsigned int Shader::GetID()
{
	return ID;
}

void Shader::setBool(const std::string & name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string & name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string & name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string & name, const GLfloat* value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
}
