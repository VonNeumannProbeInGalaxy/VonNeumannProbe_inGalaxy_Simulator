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

int         kWindowWidth  = 1280;
int         kWindowHeight = 960;
const char* kWindowTitle  = "Von-Neumann Probe in Galaxy Simulator FPS:";
float       kWindowAspect = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight);
int         kMultiSamples = 4;

static void CursorPosCallback(GLFWwindow* Window, double PosX, double PosY);
static void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);
static void MessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, const GLchar* Message, const void* UserParam);
static void ProcessInput(GLFWwindow* Window, double DeltaTime);
static void ScrollCallback(GLFWwindow* Window, GLdouble OffsetX, GLdouble OffsetY);
static void Terminate(GLFWwindow* Window);

using namespace Npgs;
using namespace Npgs::Asset;

Camera* kFreeCamera  = nullptr;
bool    kbFirstMouse = true;
double  kLastX       = 0.0;
double  kLastY       = 0.0;

int main()
{
	if (!glfwInit())
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

	std::vector<std::string> TriangleShaderFiles{ "Lighting.vert", "Lighting.frag" };
	std::vector<std::string> LampShaderMacros{ "__FRAG_LAMP_CUBE" };
	AssetManager::AddAsset<Shader>("Triangle", Shader(TriangleShaderFiles));
	AssetManager::AddAsset<Shader>("Lamp", Shader(TriangleShaderFiles, "", LampShaderMacros));
	AssetManager::AddAsset<Texture>("TexNpgs", Texture(Texture::TextureType::k2D, "Wood.png"));
	AssetManager::AddAsset<Texture>("TexFace", Texture(Texture::TextureType::k2D, "AwesomeFace.png"));

	GLuint VertexArray  = 0;
	GLuint VertexBuffer = 0;
	glCreateBuffers(1, &VertexBuffer);
	glNamedBufferData(VertexBuffer, ContainerVertices.size() * sizeof(GLfloat), ContainerVertices.data(), GL_STATIC_DRAW);
	glCreateVertexArrays(1, &VertexArray);
	glVertexArrayVertexBuffer(VertexArray, 0, VertexBuffer, 0, 8 * sizeof(GLfloat));
	glEnableVertexArrayAttrib(VertexArray, 0);
	glEnableVertexArrayAttrib(VertexArray, 1);
	glEnableVertexArrayAttrib(VertexArray, 2);
	glVertexArrayAttribFormat(VertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(VertexArray, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));
	glVertexArrayAttribFormat(VertexArray, 2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat));
	glVertexArrayAttribBinding(VertexArray, 0, 0);
	glVertexArrayAttribBinding(VertexArray, 1, 0);
	glVertexArrayAttribBinding(VertexArray, 2, 0);

	GLuint LampVertexArray  = 0;
	glCreateVertexArrays(1, &LampVertexArray);
	glVertexArrayVertexBuffer(LampVertexArray, 0, VertexBuffer, 0, 8 * sizeof(GLfloat));
	glEnableVertexArrayAttrib(LampVertexArray, 0);
	glVertexArrayAttribFormat(LampVertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(LampVertexArray, 0, 0);

	glEnable(GL_DEPTH_TEST);

	double CurrentTime   = 0.0;
	double PreviousTime  = glfwGetTime();
	double LastFrameTime = 0.0;
	double DeltaTime     = 0.0;
	int    FrameCount    = 0;

	glm::mat4x4 Model(1.0f);
	glm::mat4x4 View(1.0f);
	glm::mat4x4 Projection(1.0f);

	auto* TriangleShader = AssetManager::GetAsset<Shader>("Triangle");
	auto* LampShader = AssetManager::GetAsset<Shader>("Lamp");
	auto* TexNpgs = AssetManager::GetAsset<Texture>("TexNpgs");
	auto* TexFace = AssetManager::GetAsset<Texture>("TexFace");

	TriangleShader->UseProgram();
	TexNpgs->BindTextureUnit(*TriangleShader, "iNpgs", 1);
	TexFace->BindTextureUnit(*TriangleShader, "iFace", 0);

	glm::vec3 LightColor(1.0f);
	glm::vec3 ObjectColor(1.0f, 0.5f, 0.31f);
	glm::vec3 LightPos(1.2f, 1.0f, 2.0f);

	glm::mat3x3 NormalMatrix(1.0f);

	while (!glfwWindowShouldClose(Window))
	{
		ProcessInput(Window, DeltaTime);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		View = kFreeCamera->GetViewMatrix();
		Projection = glm::perspective(glm::radians(45.0f), kWindowAspect, 0.1f, 100.0f);

		TriangleShader->UseProgram();
		TriangleShader->SetUniform3fv("iLightColor", LightColor);
		TriangleShader->SetUniform3fv("iObjectColor", ObjectColor);
		TriangleShader->SetUniform3fv("iLightPos", LightPos);
		TriangleShader->SetUniformMatrix4fv("iModel", Model);
		TriangleShader->SetUniformMatrix4fv("iView", View);
		TriangleShader->SetUniformMatrix4fv("iProjection", Projection);

		glBindVertexArray(VertexArray);

		for (int i = 0; i != 10; ++i)
		{
			Model = glm::mat4x4(1.0f);
			Model = glm::translate(Model, CubePositions[i]);

			float Angle = 20.0f * i;
			Model = glm::rotate(Model, glm::radians(Angle), glm::vec3(1.0f, 0.3f, 0.5f));
			NormalMatrix = glm::transpose(glm::inverse(Model));

			TriangleShader->SetUniformMatrix4fv("iModel", Model);
			TriangleShader->SetUniformMatrix3fv("iNormalMatrix", NormalMatrix);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(VertexArray);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		Model = glm::mat4x4(1.0f);
		Model = glm::translate(Model, LightPos);
		Model = glm::scale(Model, glm::vec3(0.2f));
		LampShader->UseProgram();
		LampShader->SetUniformMatrix4fv("iModel", Model);
		LampShader->SetUniformMatrix4fv("iView", View);
		LampShader->SetUniformMatrix4fv("iProjection", Projection);
		glBindVertexArray(LampVertexArray);
		glDrawArrays(GL_TRIANGLES, 0, 36);

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
		kWindowWidth  = Width;
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
		kFreeCamera->ProcessKeyboard(Movement::kForward, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
		kFreeCamera->ProcessKeyboard(Movement::kBack, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
		kFreeCamera->ProcessKeyboard(Movement::kLeft, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
		kFreeCamera->ProcessKeyboard(Movement::kRight, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_R) == GLFW_PRESS)
		kFreeCamera->ProcessKeyboard(Movement::kUp, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_F) == GLFW_PRESS)
		kFreeCamera->ProcessKeyboard(Movement::kDown, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
		kFreeCamera->ProcessKeyboard(Movement::kRollLeft, DeltaTime);
	if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
		kFreeCamera->ProcessKeyboard(Movement::kRollRight, DeltaTime);
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
