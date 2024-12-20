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
#include "Engine/Utilities/UniformBlockManager.h"

using namespace Npgs;
using namespace Npgs::Asset;
using namespace Npgs::Util;

namespace
{
	int   kWindowWidth       = 1280;
	int   kWindowHeight      = 960;
	float kWindowAspect      = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight);
	const char* kWindowTitle = "Von-Neumann Probe in Galaxy Simulator FPS:";

	int    kMultiSamples            = 32;
	GLuint kMultiSampleFramebuffer  = 0;
	GLuint kIntermediateFramebuffer = 0;
	GLuint kRenderbuffer            = 0;
	Texture* kTexColorBuffer        = nullptr;
	Texture* kTexMultiSampleBuffer  = nullptr;

	Camera* kFreeCamera = nullptr;
	bool   kbFirstMouse = true;
	double kLastX       = 0.0;
	double kLastY       = 0.0;

	void CursorPosCallback(GLFWwindow* Window, double PosX, double PosY);
	void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);
	void MessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, const GLchar* Message, const void* UserParam);
	void ProcessInput(GLFWwindow* Window, double DeltaTime);
	void ScrollCallback(GLFWwindow* Window, GLdouble OffsetX, GLdouble OffsetY);
	void Terminate(GLFWwindow* Window);
}

struct VertMatrices
{
	glm::mat4x4 Model{ glm::mat4x4(1.0f) };
	glm::mat4x4 View{ glm::mat4x4(1.0f) };
	glm::mat4x4 Projection{ glm::mat4x4(1.0f) };
};

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

	auto* AssetManagerInstance = AssetManager::GetInstance();
	std::vector<std::string> FramebufferShaderFiles{ "Framebuffer.vert", "Framebuffer.frag" };
	std::vector<std::string> LightingShaderFiles{ "Lighting.vert", "Lighting.frag" };
	std::vector<std::string> AdvancedShaderFiles{ "Advanced.vert", "Advanced.frag" };
	std::vector<std::string> PointShaderFiles{ "Point.vert", "Point.geom", "Point.frag" };
	std::vector<std::string> LampShaderMacros{ "__FRAG_LAMP_CUBE" };
	std::vector<std::string> BorderShaderMacros{ "__FRAG_BORDER" };
	AssetManagerInstance->AddAsset<Shader>("Framebuffer", Shader(FramebufferShaderFiles));
	AssetManagerInstance->AddAsset<Shader>("Lighting", Shader(LightingShaderFiles));
	AssetManagerInstance->AddAsset<Shader>("Advanced", Shader(AdvancedShaderFiles));
	AssetManagerInstance->AddAsset<Shader>("Lamp", Shader(LightingShaderFiles, "", LampShaderMacros));
	AssetManagerInstance->AddAsset<Shader>("Border", Shader(AdvancedShaderFiles, "", BorderShaderMacros));
	AssetManagerInstance->AddAsset<Shader>("Point", Shader(PointShaderFiles));
	AssetManagerInstance->AddAsset<Texture>("Metal", Texture(Texture::TextureType::k2D, "Metal.png"));
	AssetManagerInstance->AddAsset<Texture>("Marble", Texture(Texture::TextureType::k2D, "Marble.jpg"));
	AssetManagerInstance->AddAsset<Texture>("RedWindow", Texture(Texture::TextureType::k2D, "RedWindow.png"));
	AssetManagerInstance->AddAsset<Texture>("Grass", Texture(Texture::TextureType::k2D, "Grass.png", false, false));
	//AssetManagerInstance->AddAsset<Model>("Backpack", Model("Backpack/backpack.obj", "Lighting"));
	//AssetManagerInstance->AddAsset<Model>("Nanosuit", Model("Nanosuit/nanosuit.obj", "Lighting"));

	GLuint CubeVertexBuffer = 0;
	glCreateBuffers(1, &CubeVertexBuffer);
	glNamedBufferData(CubeVertexBuffer, CubeVertices.size() * sizeof(GLfloat), CubeVertices.data(), GL_STATIC_DRAW);

	GLuint PlaneVertexBuffer = 0;
	glCreateBuffers(1, &PlaneVertexBuffer);
	glNamedBufferData(PlaneVertexBuffer, PlaneVertices.size() * sizeof(GLfloat), PlaneVertices.data(), GL_STATIC_DRAW);

	GLuint TransparentVertexBuffer = 0;
	glCreateBuffers(1, &TransparentVertexBuffer);
	glNamedBufferData(TransparentVertexBuffer, TransparentVertices.size() * sizeof(GLfloat), TransparentVertices.data(), GL_STATIC_DRAW);

	GLuint QuadVertexBuffer = 0;
	glCreateBuffers(1, &QuadVertexBuffer);
	glNamedBufferData(QuadVertexBuffer, QuadVertices.size() * sizeof(GLfloat), QuadVertices.data(), GL_STATIC_DRAW);

	GLuint PointVertexBuffer = 0;
	glCreateBuffers(1, &PointVertexBuffer);
	glNamedBufferData(PointVertexBuffer, Points.size() * sizeof(GLfloat), Points.data(), GL_STATIC_DRAW);

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

	GLuint QuadVertexArray = 0;
	glCreateVertexArrays(1, &QuadVertexArray);
	glVertexArrayVertexBuffer(QuadVertexArray, 0, QuadVertexBuffer, 0, 4 * sizeof(GLfloat));
	glEnableVertexArrayAttrib(QuadVertexArray, 0);
	glEnableVertexArrayAttrib(QuadVertexArray, 1);
	glVertexArrayAttribFormat(QuadVertexArray, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(QuadVertexArray, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));
	glVertexArrayAttribBinding(QuadVertexArray, 0, 0);
	glVertexArrayAttribBinding(QuadVertexArray, 1, 0);

	GLuint PointVertexArray = 0;
	glCreateVertexArrays(1, &PointVertexArray);
	glVertexArrayVertexBuffer(PointVertexArray, 0, PointVertexBuffer, 0, 5 * sizeof(GLfloat));
	glEnableVertexArrayAttrib(PointVertexArray, 0);
	glEnableVertexArrayAttrib(PointVertexArray, 1);
	glVertexArrayAttribFormat(PointVertexArray, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(PointVertexArray, 1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));
	glVertexArrayAttribBinding(PointVertexArray, 0, 0);
	glVertexArrayAttribBinding(PointVertexArray, 1, 0);

	glCreateFramebuffers(1, &kMultiSampleFramebuffer);
	glCreateFramebuffers(1, &kIntermediateFramebuffer);
	glCreateRenderbuffers(1, &kRenderbuffer);

	kTexMultiSampleBuffer = new Texture(kWindowWidth, kWindowHeight, GL_RGBA16, GL_COLOR_ATTACHMENT0, kMultiSamples, GL_TRUE, kMultiSampleFramebuffer);
	kTexColorBuffer = new Texture(kWindowWidth, kWindowHeight, GL_RGBA16, GL_COLOR_ATTACHMENT0, kIntermediateFramebuffer);

	glNamedRenderbufferStorageMultisample(kRenderbuffer, kMultiSamples, GL_DEPTH24_STENCIL8, kWindowWidth, kWindowHeight);
	glNamedFramebufferRenderbuffer(kMultiSampleFramebuffer, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, kRenderbuffer);

	if (glCheckNamedFramebufferStatus(kMultiSampleFramebuffer,  GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ||
		glCheckNamedFramebufferStatus(kIntermediateFramebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		GLenum Error = glGetError();
		std::println("Error: Framebuffer is not complete. Error: {}", Error);
		Terminate(Window);
		std::system("pause");
		return EXIT_FAILURE;
	}

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_PROGRAM_POINT_SIZE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	double CurrentTime   = 0.0;
	double PreviousTime  = glfwGetTime();
	double LastFrameTime = 0.0;
	double DeltaTime     = 0.0;
	int    FrameCount    = 0;

	auto* FramebufferShader = AssetManagerInstance->GetAsset<Shader>("Framebuffer");
	auto* LightingShader    = AssetManagerInstance->GetAsset<Shader>("Lighting");
	auto* AdvancedShader    = AssetManagerInstance->GetAsset<Shader>("Advanced");
	auto* LampShader        = AssetManagerInstance->GetAsset<Shader>("Lamp");
	auto* BorderShader      = AssetManagerInstance->GetAsset<Shader>("Border");
	auto* PointShader       = AssetManagerInstance->GetAsset<Shader>("Point");
	auto* Metal             = AssetManagerInstance->GetAsset<Texture>("Metal");
	auto* Marble            = AssetManagerInstance->GetAsset<Texture>("Marble");
	auto* RedWindow         = AssetManagerInstance->GetAsset<Texture>("RedWindow");
	auto* Grass             = AssetManagerInstance->GetAsset<Texture>("Grass");
	auto* Backpack          = AssetManagerInstance->GetAsset<Model>("Backpack");
	auto* Nanosuit          = AssetManagerInstance->GetAsset<Model>("Nanosuit");

	Metal->BindTextureUnit(0);
	Marble->BindTextureUnit(1);
	Grass->BindTextureUnit(2);
	RedWindow->BindTextureUnit(3);
	kTexColorBuffer->BindTextureUnit(4);

	glm::mat4x4 Model(1.0f);
	glm::mat4x4 View(1.0f);
	glm::mat4x4 Projection(1.0f);
	glm::mat3x3 NormalMatrix(1.0f);
	glm::vec3   LightPos(1.2f, 1.0f, 2.0f);

	std::vector<std::string> MatrixMembers = { "iModel", "iView", "iProjection" };

	auto* UboManagerInstance = UniformBlockManager::GetInstance();
	UboManagerInstance->CreateSharedBlock(AdvancedShader->GetProgram(), "Matrices", 0, MatrixMembers);
	UboManagerInstance->VerifyBlockLayout(AdvancedShader->GetProgram(), "Matrices");

	auto ModelUpdater      = UboManagerInstance->GetBlockUpdater<glm::mat4x4>("Matrices", "iModel");
	auto ViewUpdater       = UboManagerInstance->GetBlockUpdater<glm::mat4x4>("Matrices", "iView");
	auto ProjectionUpdater = UboManagerInstance->GetBlockUpdater<glm::mat4x4>("Matrices", "iProjection");

	VertMatrices MvpMatrices;

	while (!glfwWindowShouldClose(Window))
	{
		ProcessInput(Window, DeltaTime);

		glBindFramebuffer(GL_FRAMEBUFFER, kMultiSampleFramebuffer);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//PointShader->UseProgram();
		//glBindVertexArray(PointVertexArray);
		//glDrawArrays(GL_POINTS, 0, 4);

		Model = glm::mat4x4(1.0f);
		View  = kFreeCamera->GetViewMatrix();
		Projection = glm::perspective(glm::radians(kFreeCamera->GetCameraZoom()), kWindowAspect, 0.1f, 10000.0f);

		MvpMatrices = { Model, View, Projection };

		Model = glm::mat4x4(1.0f);
		AdvancedShader->UseProgram();
		AdvancedShader->SetUniform1i("iTex", 0);
		UboManagerInstance->UpdateEntrieBlock("Matrices", MvpMatrices);
		glBindVertexArray(PlaneVertexArray);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		AdvancedShader->SetUniform1i("iTex", 1);
		glBindVertexArray(CubeVertexArray);
		Model = glm::translate(Model, glm::vec3(-1.0f, 0.0f, -1.0f));
		ModelUpdater << Model;
		glDrawArrays(GL_TRIANGLES, 0, 36);
		Model = glm::mat4x4(1.0f);
		Model = glm::translate(Model, glm::vec3(2.0f, 0.0f, 0.0f));
		ModelUpdater << Model;
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Model = glm::mat4x4(1.0f);
		//Model = glm::translate(Model, glm::vec3(0.0f));
		//Model = glm::scale(Model, glm::vec3(1.0f));
		//NormalMatrix = glm::transpose(glm::inverse(Model));
		//LightingShader->UseProgram();
		////LightingShader->SetUniformMatrix4fv("iModel", Model);
		////LightingShader->SetUniformMatrix4fv("iView", View);
		////LightingShader->SetUniformMatrix4fv("iProjection", Projection);
		//ModelUpdater = Model;
		//ViewUpdater = View;
		//ProjectionUpdater = Projection;
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
		////LightingShader->SetUniformMatrix4fv("iModel", Model);
		//ModelUpdater = Model;
		//Nanosuit->DynamicDraw(*LightingShader);

		//Model = glm::mat4x4(1.0f);
		//Model = glm::translate(Model, LightPos);
		//Model = glm::scale(Model, glm::vec3(0.2f));
		//LampShader->UseProgram();
		////LampShader->SetUniformMatrix4fv("iModel", Model);
		////LampShader->SetUniformMatrix4fv("iView", View);
		////LampShader->SetUniformMatrix4fv("iProjection", Projection);
		//ModelUpdater = Model;
		//ViewUpdater = View;
		//ProjectionUpdater = Projection;
		//glBindVertexArray(LampVertexArray);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		glBlitNamedFramebuffer(kMultiSampleFramebuffer, kIntermediateFramebuffer, 0, 0, kWindowWidth, kWindowHeight,
							   0, 0, kWindowWidth, kWindowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		FramebufferShader->UseProgram();
		FramebufferShader->SetUniform1i("iTexColorBuffer", 4);
		glBindVertexArray(QuadVertexArray);
		glDrawArrays(GL_TRIANGLES, 0, 6);

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
			kWindowWidth  = Width;
			kWindowHeight = Height;
			kWindowAspect = static_cast<float>(Width) / static_cast<float>(Height);

			glNamedRenderbufferStorageMultisample(kRenderbuffer, kMultiSamples, GL_DEPTH24_STENCIL8, kWindowWidth, kWindowHeight);
			glNamedFramebufferRenderbuffer(kMultiSampleFramebuffer, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, kRenderbuffer);

			delete kTexMultiSampleBuffer;
			kTexMultiSampleBuffer = new Texture(kWindowWidth, kWindowHeight, GL_RGBA16, GL_COLOR_ATTACHMENT0, kMultiSamples, GL_TRUE, kMultiSampleFramebuffer);
			delete kTexColorBuffer;
			kTexColorBuffer = new Texture(kWindowWidth, kWindowHeight, GL_RGBA16, GL_COLOR_ATTACHMENT0, kIntermediateFramebuffer);
			kTexColorBuffer->BindTextureUnit(4);
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
