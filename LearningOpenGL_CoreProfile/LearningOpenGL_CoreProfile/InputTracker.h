#pragma once

#include<glad/glad.h> //include opengl headers, so should be before anything that uses those headers (such as GLFW)
#include<GLFW/glfw3.h>
#include <set>
#include <stack>

class InputTracker final
{
public:
	InputTracker();
	~InputTracker();

	bool isKeyJustPressed(GLFWwindow* window, int key);
	bool isKeyDown(GLFWwindow* window, int key);
	void updateState(GLFWwindow* window);

private:
	std::set<int> keysCurrentlyPressed;

};
