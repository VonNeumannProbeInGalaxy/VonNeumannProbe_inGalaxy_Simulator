#pragma warning(disable : 4715)

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

GLint         kWindowWidth  = 1280;
GLint         kWindowHeight = 960;
const GLchar* kWindowTitle  = "Von-Neumann Probe in Galaxy Simulator FPS:";
GLfloat       kWindowAspect = static_cast<GLfloat>(kWindowWidth) / static_cast<GLfloat>(kWindowHeight);
GLint         kMultiSamples = 4;

static GLvoid FramebufferSizeCallback(GLFWwindow* Window, GLint Width, GLint Height);
static GLvoid MessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, const GLchar* Message, const GLvoid* UserParam);
static GLvoid ProcessInput(GLFWwindow* Window, GLdouble DeltaTime);
static GLvoid Terminate(GLFWwindow* Window);

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
	// glfwSwapInterval(0);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::println("Error: Failed to initialize Glad.");
		glfwTerminate();
		std::system("pause");
		return EXIT_FAILURE;
	}

	std::vector<std::string> TriangleShaderFiles{ "Triangle.vert", "Triangle.frag" };
	AssetManager::AddAsset<Shader>("Triangle", std::make_shared<Shader>(TriangleShaderFiles));
	AssetManager::AddAsset<Texture>("Tex", std::make_shared<Texture>(Texture::Type::k2D, "RedWindow.png"));

#include "Vertices.inc"

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
	glVertexArrayAttribFormat(VertexArray, 3, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat));
	glVertexArrayAttribBinding(VertexArray, 0, 0);
	glVertexArrayAttribBinding(VertexArray, 1, 0);
	glVertexArrayAttribBinding(VertexArray, 2, 0);

	GLdouble CurrentTime   = 0.0;
	GLdouble PreviousTime  = glfwGetTime();
	GLdouble LastFrameTime = 0.0;
	GLdouble DeltaTime     = 0.0;
	GLuint   FrameCount    = 0;

	auto TriangleShader = AssetManager::GetAsset<Shader>("Triangle");
	auto Tex = AssetManager::GetAsset<Texture>("Tex");

	while (!glfwWindowShouldClose(Window))
	{
		ProcessInput(Window, DeltaTime);

		TriangleShader->UseProgram();
		TriangleShader->SetUniform1f("iTime", static_cast<GLfloat>(glfwGetTime()));
		Tex->BindTextureUnit(*TriangleShader, "iTex", GL_TEXTURE0);
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

GLvoid FramebufferSizeCallback(GLFWwindow* Window, GLint Width, GLint Height)
{
	glViewport(0, 0, Width, Height);
	if (Width != 0 && Height != 0)
	{
		kWindowWidth  = Width;
		kWindowHeight = Height;
		kWindowAspect = static_cast<GLfloat>(Width) / static_cast<GLfloat>(Height);
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
			assert(GL_FALSE);
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
			assert(GL_FALSE);
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
			assert(GL_FALSE);
		}
	}();

	std::println("Source: {}, Type: {}, Severity: {}\n{}: {}", SourceStr, TypeStr, SeverityStr, Id, Message);
}

GLvoid ProcessInput(GLFWwindow* Window, GLdouble DeltaTime)
{
	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(Window, GL_TRUE);
	}
}

GLvoid Terminate(GLFWwindow* Window)
{
	glfwDestroyWindow(Window);
	glfwTerminate();
}
