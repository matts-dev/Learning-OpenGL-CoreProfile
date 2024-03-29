#pragma once

#include<glad/glad.h> //include opengl headers, so should be before anything that uses those headers (such as GLFW)
#include<GLFW/glfw3.h>
#include <string>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

/**
 * Why this class is flawed:
	If you use this class, at first it will seem to work fine. However, try making little circles with the mouse and
	observe that a roll starts to drift into the camera system.
 */
class CameraQuaternionExperiment1Flawed
{
public:
	CameraQuaternionExperiment1Flawed(float inFOV);
	~CameraQuaternionExperiment1Flawed();

	glm::mat4 getView();

	//callbacks
	void mouseMoved(double xpos, double ypos);
	void windowFocusedChanged(int focusEntered);
	void mouseWheelUpdate(double xOffset, double yOffset);
	void handleInput(GLFWwindow* window, float deltaTime);
	void exclusiveGLFWCallbackRegister(GLFWwindow* window);

	//setters and getters
	void setPosition(float x, float y, float z);

private: //helper fields
	double lastX;
	double lastY;
	bool refocused = true;

private:
	const glm::vec3 DEFAULT_AXIS_U = glm::vec3(1.f, 0, 0);
	const glm::vec3 DEFAULT_AXIS_V = glm::vec3(0, 1.f, 0);
	const glm::vec3 DEFAULT_AXIS_W = glm::vec3(0, 0, 1.f);

	glm::vec3 previousUp;
	glm::vec3 cameraPosition;
	glm::vec3 u_axis;
	glm::vec3 v_axis;
	glm::vec3 w_axis;

	glm::quat quaternion;

	float FOV = 45.0f;
	float mouseSensitivity = 0.005f;
	float cameraSpeed = 2.5f;


	void updateAxes();

private:
	void mouseMovedBroken(double xpos, double ypos); 
};