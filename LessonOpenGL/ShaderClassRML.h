#pragma once

#ifndef SHADER_RML
#define SHADER_RML

#include <glad\glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	void Activate();
	unsigned int GetID();


	// Установка uniforms обьектов
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setMat4(const std::string &name, const GLfloat* value) const;
private:
	unsigned int ID;

};


#endif // !SHADER_RML

