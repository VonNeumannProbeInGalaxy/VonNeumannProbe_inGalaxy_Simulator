#pragma warning(disable : 4715)

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <print>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Engine/AssetLoader/AssetManager.h"
#include "Engine/Core/Camera.h"

using namespace Npgs;
using namespace Npgs::Asset;

namespace
{

int         kWindowWidth  = 1280;
int         kWindowHeight = 960;
const char* kWindowTitle  = "Von-Neumann Probe in Galaxy Simulator FPS:";
float       kWindowAspect = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight);
int         kMultiSamples = 32;

Camera* kFreeCamera  = nullptr;
bool    kbFirstMouse = true;
double  kLastX       = 0.0;
double  kLastY       = 0.0;

void CursorPosCallback(GLFWwindow* Window, double PosX, double PosY);
void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);
void MessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, const GLchar* Message, const void* UserParam);
void ProcessInput(GLFWwindow* Window, double DeltaTime);
void ScrollCallback(GLFWwindow* Window, GLdouble OffsetX, GLdouble OffsetY);
void Terminate(GLFWwindow* Window);

}

int main()
{
	if (glfwInit() == GLFW_FALSE)
	{
		std::println("Error: Failed to initialize GLFW.");
		return EXIT_FAILURE;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, kMultiSamples);

	GLFWwindow* Window = glfwCreateWindow(kWindowWidth, kWindowHeight, kWindowTitle, nullptr, nullptr);
	if (Window == nullptr)
	{
		std::println("Error: Failed to create GLFW window.");
		glfwTerminate();
		std::system("pause");
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(Window);
	glfwSetCursorPosCallback(Window, nullptr);
	glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
	glfwSetScrollCallback(Window, ScrollCallback);
	// glfwSwapInterval(0);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::println("Error: Failed to initialize Glad.");
		glfwTerminate();
		std::system("pause");
		return EXIT_FAILURE;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, nullptr);

	kFreeCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

#include "Vertices.inc"

	std::vector<std::string> LightingShaderFiles{ "Lighting.vert", "Lighting.frag" };
	std::vector<std::string> AdvancedShaderFiles{ "Advanced.vert", "Advanced.frag" };
	std::vector<std::string> LampShaderMacros{ "__FRAG_LAMP_CUBE" };
	std::vector<std::string> BorderShaderMacros{ "__FRAG_BORDER" };	
	AssetManager::AddAsset<Shader>("Lighting", Shader(LightingShaderFiles));
	AssetManager::AddAsset<Shader>("Advanced", Shader(AdvancedShaderFiles));
	AssetManager::AddAsset<Shader>("Lamp", Shader(LightingShaderFiles, "", LampShaderMacros));
	AssetManager::AddAsset<Shader>("Border", Shader(AdvancedShaderFiles, "", BorderShaderMacros));
	AssetManager::AddAsset<Texture>("Metal", Texture(Texture::TextureType::k2D, "Metal.png"));
	AssetManager::AddAsset<Texture>("Marble", Texture(Texture::TextureType::k2D, "Marble.jpg"));
	AssetManager::AddAsset<Texture>("RedWindow", Texture(Texture::TextureType::k2D, "RedWindow.png"));
	AssetManager::AddAsset<Texture>("Grass", Texture(Texture::TextureType::k2D, "Grass.png", false, false));
	//AssetManager::AddAsset<Model>("Backpack", Model("Backpack/backpack.obj", "Lighting"));
	//AssetManager::AddAsset<Model>("Nanosuit", Model("Nanosuit/nanosuit.obj", "Lighting"));

	GLuint CubeVertexBuffer = 0;
	glCreateBuffers(1, &CubeVertexBuffer);
	glNamedBufferData(CubeVertexBuffer, CubeVertices.size() * sizeof(GLfloat), CubeVertices.data(), GL_STATIC_DRAW);

	GLuint PlaneVertexBuffer = 0;
	glCreateBuffers(1, &PlaneVertexBuffer);
	glNamedBufferData(PlaneVertexBuffer, PlaneVertices.size() * sizeof(GLfloat), PlaneVertices.data(), GL_STATIC_DRAW);

	GLuint TransparentVertexBuffer = 0;
	glCreateBuffers(1, &TransparentVertexBuffer);
	glNamedBufferData(TransparentVertexBuffer, TransparentVertices.size() * sizeof(GLfloat), TransparentVertices.data(), GL_STATIC_DRAW);

	GLuint LampVertexArray = 0;
	glCreateVertexArrays(1, &LampVertexArray);
	glVertexArrayVertexBuffer(LampVertexArray, 0, CubeVertexBuffer, 0, 5 * sizeof(GLfloat));
	glEnableVertexArrayAttrib(LampVertexArray, 0);
	glVertexArrayAttribFormat(LampVertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(LampVertexArray, 0, 0);

	GLuint CubeVertexArray = 0;
	glCreateVertexArrays(1, &CubeVertexArray);
	glVertexArrayVertexBuffer(CubeVertexArray, 0, CubeVertexBuffer, 0, 5 * sizeof(GLfloat));
	glEnableVertexArrayAttrib(CubeVertexArray, 0);
	glEnableVertexArrayAttrib(CubeVertexArray, 1);
	glVertexArrayAttribFormat(CubeVertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(CubeVertexArray, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));
	glVertexArrayAttribBinding(CubeVertexArray, 0, 0);
	glVertexArrayAttribBinding(CubeVertexArray, 1, 0);

	GLuint PlaneVertexArray = 0;
	glCreateVertexArrays(1, &PlaneVertexArray);
	glVertexArrayVertexBuffer(PlaneVertexArray, 0, PlaneVertexBuffer, 0, 8 * sizeof(GLfloat));
	glEnableVertexArrayAttrib(PlaneVertexArray, 0);
	glEnableVertexArrayAttrib(PlaneVertexArray, 1);
	glVertexArrayAttribFormat(PlaneVertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(PlaneVertexArray, 1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat));
	glVertexArrayAttribBinding(PlaneVertexArray, 0, 0);
	glVertexArrayAttribBinding(PlaneVertexArray, 1, 0);

	GLuint TransparentVertexArray = 0;
	glCreateVertexArrays(1, &TransparentVertexArray);
	glVertexArrayVertexBuffer(TransparentVertexArray, 0, TransparentVertexBuffer, 0, 5 * sizeof(GLfloat));
	glEnableVertexArrayAttrib(TransparentVertexArray, 0);
	glEnableVertexArrayAttrib(TransparentVertexArray, 1);
	glVertexArrayAttribFormat(TransparentVertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(TransparentVertexArray, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));
	glVertexArrayAttribBinding(TransparentVertexArray, 0, 0);
	glVertexArrayAttribBinding(TransparentVertexArray, 1, 0);

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_STENCIL_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	double CurrentTime   = 0.0;
	double PreviousTime  = glfwGetTime();
	double LastFrameTime = 0.0;
	double DeltaTime     = 0.0;
	int    FrameCount    = 0;

	auto* LightingShader = AssetManager::GetAsset<Shader>("Lighting");
	auto* AdvancedShader = AssetManager::GetAsset<Shader>("Advanced");
	auto* LampShader     = AssetManager::GetAsset<Shader>("Lamp");
	auto* BorderShader   = AssetManager::GetAsset<Shader>("Border");
	auto* Metal          = AssetManager::GetAsset<Texture>("Metal");
	auto* Marble         = AssetManager::GetAsset<Texture>("Marble");
	auto* RedWindow      = AssetManager::GetAsset<Texture>("RedWindow");
	auto* Grass          = AssetManager::GetAsset<Texture>("Grass");
	auto* Backpack       = AssetManager::GetAsset<Model>("Backpack");
	auto* Nanosuit       = AssetManager::GetAsset<Model>("Nanosuit");

	Metal->BindTextureUnit(0);
	Marble->BindTextureUnit(1);
	Grass->BindTextureUnit(2);
	RedWindow->BindTextureUnit(3);

	glm::mat4x4 Model(1.0f);
	glm::mat4x4 View(1.0f);
	glm::mat4x4 Projection(1.0f);
	glm::mat3x3 NormalMatrix(1.0f);
	glm::vec3   LightPos(1.2f, 1.0f, 2.0f);

	while (!glfwWindowShouldClose(Window))
	{
		ProcessInput(Window, DeltaTime);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT  | GL_STENCIL_BUFFER_BIT);

		Model = glm::mat4x4(1.0f);
		View = kFreeCamera->GetViewMatrix();
		Projection = glm::perspective(kFreeCamera->GetCameraZoom(), kWindowAspect, 0.1f, 100.0f);

		//glStencilMask(0x00);

		AdvancedShader->UseProgram();
		AdvancedShader->SetUniformMatrix4fv("iView", View);
		AdvancedShader->SetUniformMatrix4fv("iProjection", Projection);
		AdvancedShader->SetUniform1i("iTex", 0);
		glBindVertexArray(PlaneVertexArray);
		Model = glm::mat4x4(1.0f);
		AdvancedShader->SetUniformMatrix4fv("iModel", Model);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//glStencilFunc(GL_ALWAYS, 1, 0xFF);
		//glStencilMask(0xFF);
		//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		AdvancedShader->SetUniform1i("iTex", 1);
		glBindVertexArray(CubeVertexArray);
		Model = glm::translate(Model, glm::vec3(-1.0f, 0.0f, -1.0f));
		AdvancedShader->SetUniformMatrix4fv("iModel", Model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		Model = glm::mat4x4(1.0f);
		Model = glm::translate(Model, glm::vec3(2.0f, 0.0f, 0.0f));
		AdvancedShader->SetUniformMatrix4fv("iModel", Model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		AdvancedShader->SetUniform1i("iTex", 3);
		glBindVertexArray(TransparentVertexArray);
		for (std::size_t i = 0; i != TransparentPositions.size(); ++i)
		{
			Model = glm::mat4x4(1.0f);
			Model = glm::translate(Model, TransparentPositions[i]);
			AdvancedShader->SetUniformMatrix4fv("iModel", Model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		//glStencilMask(0x00);
		//glDisable(GL_DEPTH_TEST);

		//BorderShader->UseProgram();
		//BorderShader->SetUniformMatrix4fv("iView", View);
		//BorderShader->SetUniformMatrix4fv("iProjection", Projection);

		//Model = glm::mat4x4(1.0f);
		//Model = glm::translate(Model, glm::vec3(-1.0f, 0.0f, -1.0f));
		//Model = glm::scale(Model, glm::vec3(1.1f));
		//BorderShader->SetUniformMatrix4fv("iModel", Model);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//Model = glm::mat4x4(1.0f);
		//Model = glm::translate(Model, glm::vec3(2.0f, 0.0f, 0.0f));
		//Model = glm::scale(Model, glm::vec3(1.1f));
		//BorderShader->SetUniformMatrix4fv("iModel", Model);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		//glStencilMask(0xFF);
		//glEnable(GL_DEPTH_TEST);

		//Model = glm::mat4x4(1.0f);
		//Model = glm::translate(Model, glm::vec3(0.0f));
		//Model = glm::scale(Model, glm::vec3(1.0f));
		//NormalMatrix = glm::transpose(glm::inverse(Model));
		//LightingShader->UseProgram();
		//LightingShader->SetUniformMatrix4fv("iModel", Model);
		//LightingShader->SetUniformMatrix4fv("iView", View);
		//LightingShader->SetUniformMatrix4fv("iProjection", Projection);
		//LightingShader->SetUniformMatrix3fv("iNormalMatrix", NormalMatrix);
		//LightingShader->SetUniform1f("iShininess", 64.0f);
		//LightingShader->SetUniform3fv("iViewPos", kFreeCamera->GetCameraVector(Camera::VectorType::kPosition));
		//LightingShader->SetUniform3fv("iLight.Position", LightPos);
		//LightingShader->SetUniform3fv("iLight.AmbientColor", glm::vec3(0.2f));
		//LightingShader->SetUniform3fv("iLight.DiffuseColor", glm::vec3(1.0f));
		//LightingShader->SetUniform3fv("iLight.SpecularColor", glm::vec3(1.0f));
		//Backpack->DynamicDraw(*LightingShader);
		//Model = glm::mat4x4(1.0f);
		//Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));
		//Model = glm::scale(Model, glm::vec3(0.2f));
		//LightingShader->SetUniformMatrix4fv("iModel", Model);
		//Nanosuit->DynamicDraw(*LightingShader);

		//Model = glm::mat4x4(1.0f);
		//Model = glm::translate(Model, LightPos);
		//Model = glm::scale(Model, glm::vec3(0.2f));
		//LampShader->UseProgram();
		//LampShader->SetUniformMatrix4fv("iModel", Model);
		//LampShader->SetUniformMatrix4fv("iView", View);
		//LampShader->SetUniformMatrix4fv("iProjection", Projection);
		//glBindVertexArray(LampVertexArray);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(Window);
		glfwPollEvents();

		CurrentTime   = glfwGetTime();
		DeltaTime     = CurrentTime - LastFrameTime;
		LastFrameTime = CurrentTime;
		++FrameCount;
		if (CurrentTime - PreviousTime >= 1.0)
		{
			glfwSetWindowTitle(Window, (std::string(kWindowTitle) + " " + std::to_string(FrameCount)).c_str());
			FrameCount   = 0;
			PreviousTime = CurrentTime;
		}
	}

	Terminate(Window);

	return EXIT_SUCCESS;
}

namespace
{

void CursorPosCallback(GLFWwindow* Window, double PosX, double PosY)
{
	if (kbFirstMouse)
	{
		kLastX = PosX;
		kLastY = PosY;
		kbFirstMouse = false;
	}

	double OffsetX = PosX - kLastX;
	double OffsetY = kLastY - PosY;
	kLastX = PosX;
	kLastY = PosY;

	kFreeCamera->ProcessMouseMovement(OffsetX, OffsetY);
}

void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height)
{
	glViewport(0, 0, Width, Height);
	if (Width != 0 && Height != 0)
	{
		kWindowWidth = Width;
		kWindowHeight = Height;
		kWindowAspect = static_cast<float>(Width) / static_cast<float>(Height);
	}
}

void MessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, const GLchar* Message, const void* UserParam)
{
	auto SourceStr = [Source]() -> std::string
	{
		switch (Source)
		{
		case GL_DEBUG_SOURCE_API:
			return "API";
		case GL_DEBUG_SOURCE_APPLICATION:
			return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER:
			return "OTHER";
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			return "SHADER";
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			return "THIRD_PARTY";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			return "WINDOW_SYSTEM";
		default:
			assert(false);
		}
	}();

	auto TypeStr = [Type]() -> std::string
	{
		switch (Type)
		{
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_ERROR:
			return "ERROR";
		case GL_DEBUG_TYPE_MARKER:
			return "MARKER";
		case GL_DEBUG_TYPE_OTHER:
			return "OTHER";
		case GL_DEBUG_TYPE_PERFORMANCE:
			return "PERFORMANCE";
		case GL_DEBUG_TYPE_PORTABILITY:
			return "PORTABILITY";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			return "UNDEFINED_BEHAVIOR";
		default:
			assert(false);
		}
	}();

	auto SeverityStr = [Severity]() -> std::string
	{
		switch (Severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			return "HIGH";
		case GL_DEBUG_SEVERITY_LOW:
			return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM:
			return "MEDIUM";
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			return "NOTIFICATION";
		default:
			assert(false);
		}
	}();

	std::println("Source: {}, Type: {}, Severity: {}\n{}: {}", SourceStr, TypeStr, SeverityStr, Id, Message);
}

void ProcessInput(GLFWwindow* Window, double DeltaTime)
{
	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(Window, GL_TRUE);
	}

	if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwSetCursorPosCallback(Window, CursorPosCallback);
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetCursorPosCallback(Window, nullptr);
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		kbFirstMouse = true;
	}

	if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
		kFreeCamera->ProcessKeyboard(Camera::Movement::kForward, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
		kFreeCamera->ProcessKeyboard(Camera::Movement::kBack, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
		kFreeCamera->ProcessKeyboard(Camera::Movement::kLeft, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
		kFreeCamera->ProcessKeyboard(Camera::Movement::kRight, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_R) == GLFW_PRESS)
		kFreeCamera->ProcessKeyboard(Camera::Movement::kUp, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_F) == GLFW_PRESS)
		kFreeCamera->ProcessKeyboard(Camera::Movement::kDown, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
		kFreeCamera->ProcessKeyboard(Camera::Movement::kRollLeft, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
		kFreeCamera->ProcessKeyboard(Camera::Movement::kRollRight, DeltaTime);
}

void ScrollCallback(GLFWwindow* Window, GLdouble OffsetX, GLdouble OffsetY)
{
	kFreeCamera->ProcessMouseScroll(OffsetY);
}

void Terminate(GLFWwindow* Window)
{
	if (kFreeCamera)
	{
		delete kFreeCamera;
		kFreeCamera = nullptr;
	}

	glfwDestroyWindow(Window);
	glfwTerminate();
}

}
