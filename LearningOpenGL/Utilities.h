#pragma once
#include<iostream>
#include<string>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

namespace Utils {
	/*
	* Attempts to load a text file specified by the first argument into the string specified by the second argument.
	* @param filepath - the path to the text file to load.
	* @param strRef - the string container to which the file's contents will be loaded
	* @return true if file was successfully loaded and move to string, false otherwise
	*/
	bool convertFileToString(const std::string filePath, std::string& strRef);

	/*
	* Set up open gl, returns a pointer pre-configured window if set up is correct. returns nullptr if failure. 
	* pre configured means a call back for resizing has been set and the window has a call back for re-sizing. 
	*
	* @param width - the width of the window to create.
	* @param height - the height of the window to create
	* @return a pointer to a valid window, or nullptr if creation of a valid window failed.
	*/
	GLFWwindow* initOpenGl(const int width, const int height);

	/*
	* Checks if escape is pressed, if so then sets the state of the window to should close.
	* @param An GLFWwindow pointer to close if escape is pressed.
	*/
	void setWindowCloseOnEscape(GLFWwindow* window);

	/*
	* Triangle coordinates for testing.
	*/
	const float basicTriangleVertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};
	const unsigned int indices[] = { 0, 1, 2 };

	/*
	* Creates a single triangle, but uses element array objects instead of just using array
	*/
	bool createSingleElementTriangle(GLuint& EAO, GLuint& VAO, GLuint& VBO);
}