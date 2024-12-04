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

static void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);
static GLvoid MessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, const GLchar* Message, const GLvoid* UserParam);
static void ProcessInput(GLFWwindow* Window, double DeltaTime);
static void Terminate(GLFWwindow* Window);

using namespace Npgs;
using namespace Npgs::Asset;

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
	glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
	// glfwSwapInterval(0);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::println("Error: Failed to initialize Glad.");
		glfwTerminate();
		std::system("pause");
		return EXIT_FAILURE;
	}

#include "Vertices.inc"

	std::vector<std::string> TriangleShaderFiles{ "Triangle.vert", "Triangle.frag" };
	AssetManager::AddAsset<Shader>("Triangle", std::make_shared<Shader>(TriangleShaderFiles));
	AssetManager::AddAsset<Texture>("TexNpgs", std::make_shared<Texture>(Texture::Type::k2D, "NPGS.png"));
	AssetManager::AddAsset<Texture>("TexFace", std::make_shared<Texture>(Texture::Type::k2D, "AwesomeFace.png"));

	GLuint VertexArray   = 0;
	GLuint VertexBuffer  = 0;
	GLuint ElementBuffer = 1;
	glCreateBuffers(1, &VertexBuffer);
	glNamedBufferData(VertexBuffer, ColoredTriangle.size() * sizeof(GLfloat), ColoredTriangle.data(), GL_STATIC_DRAW);
	glCreateBuffers(1, &ElementBuffer);
	glNamedBufferData(ElementBuffer, TriangleIndices.size() * sizeof(GLuint), TriangleIndices.data(), GL_STATIC_DRAW);
	glCreateVertexArrays(1, &VertexArray);
	glVertexArrayVertexBuffer(VertexArray, 0, VertexBuffer, 0, 8 * sizeof(GLfloat));
	glVertexArrayElementBuffer(VertexArray, ElementBuffer);
	glEnableVertexArrayAttrib(VertexArray, 0);
	glEnableVertexArrayAttrib(VertexArray, 1);
	glEnableVertexArrayAttrib(VertexArray, 2);
	glVertexArrayAttribFormat(VertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(VertexArray, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));
	glVertexArrayAttribFormat(VertexArray, 2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat));
	glVertexArrayAttribBinding(VertexArray, 0, 0);
	glVertexArrayAttribBinding(VertexArray, 1, 0);
	glVertexArrayAttribBinding(VertexArray, 2, 0);

	double        CurrentTime   = 0.0;
	double        PreviousTime  = glfwGetTime();
	double        LastFrameTime = 0.0;
	double        DeltaTime     = 0.0;
	std::uint32_t FrameCount    = 0;

	auto TriangleShader = AssetManager::GetAsset<Shader>("Triangle");
	auto TexNpgs = AssetManager::GetAsset<Texture>("TexNpgs");
	auto TexFace = AssetManager::GetAsset<Texture>("TexFace");

	TriangleShader->UseProgram();
	TexNpgs->BindTextureUnit(*TriangleShader, "iNpgs", 1);
	TexFace->BindTextureUnit(*TriangleShader, "iFace", 0);

	while (!glfwWindowShouldClose(Window))
	{
		ProcessInput(Window, DeltaTime);

		TriangleShader->UseProgram();
		TriangleShader->SetUniform1f("iTime", static_cast<GLfloat>(glfwGetTime()));
		glBindVertexArray(VertexArray);
		// glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

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

GLvoid MessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, const GLchar* Message, const GLvoid* UserParam)
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
}

void Terminate(GLFWwindow* Window)
{
	glfwDestroyWindow(Window);
	glfwTerminate();
}
