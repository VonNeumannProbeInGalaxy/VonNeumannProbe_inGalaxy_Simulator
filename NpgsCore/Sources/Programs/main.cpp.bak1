//#pragma warning(disable : 4715)
//
//#include <cstdint>
//#include <cstdlib>
//#include <memory>
//#include <print>
//#include <string>
//#include <vector>
//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//#define ENABLE_ENUM_BIT_OPERATOR
//#include "Npgs.h"
//
//using namespace Npgs;
//using namespace Npgs::Runtime::Asset;
//using namespace Npgs::Util;
//
//namespace
//{
//	int   kWindowWidth       = 1280;
//	int   kWindowHeight      = 960;
//	float kWindowAspect      = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight);
//	const char* kWindowTitle = "Von-Neumann Probe in Galaxy Simulator FPS:";
//
//	int kMultiSamples          = 4;
//	FFramebuffer* kFramebuffer = nullptr;
//
//	FCamera* kFreeCamera = nullptr;
//	bool   kbFirstMouse  = true;
//	double kLastX        = 0.0;
//	double kLastY        = 0.0;
//
//	void CursorPosCallback(GLFWwindow* Window, double PosX, double PosY);
//	void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);
//	void MessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, const GLchar* Message, const void* UserParam);
//	void ProcessInput(GLFWwindow* Window, double DeltaTime);
//	void ScrollCallback(GLFWwindow* Window, GLdouble OffsetX, GLdouble OffsetY);
//	void Terminate(GLFWwindow* Window);
//}
//
//struct VertMatrices
//{
//	glm::mat4x4 Model{ glm::mat4x4(1.0f) };
//	glm::mat4x4 View{ glm::mat4x4(1.0f) };
//	glm::mat4x4 Projection{ glm::mat4x4(1.0f) };
//};
//
//int main()
//{
//	FLogger::Init();
//
//	FApplication App({ 1280, 960 }, "Von-Neumann Probe in Galaxy Simulator FPS:", true, false);
//	App.ExecuteMainRender();
//	return 0;
//
//	if (glfwInit() == GLFW_FALSE)
//	{
//		std::println("Error: Failed to initialize GLFW.");
//		return EXIT_FAILURE;
//	}
//
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//	glfwWindowHint(GLFW_SAMPLES, kMultiSamples);
//
//	GLFWwindow* Window = glfwCreateWindow(kWindowWidth, kWindowHeight, kWindowTitle, nullptr, nullptr);
//	if (Window == nullptr)
//	{
//		std::println("Error: Failed to create GLFW window.");
//		glfwTerminate();
//		std::system("pause");
//		return EXIT_FAILURE;
//	}
//
//	glfwMakeContextCurrent(Window);
//	glfwSetCursorPosCallback(Window, nullptr);
//	glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
//	glfwSetScrollCallback(Window, ScrollCallback);
//	glfwSwapInterval(0);
//
//	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
//	{
//		std::println("Error: Failed to initialize Glad.");
//		glfwTerminate();
//		std::system("pause");
//		return EXIT_FAILURE;
//	}
//
//	glEnable(GL_DEBUG_OUTPUT);
//	glDebugMessageCallback(MessageCallback, nullptr);
//
//	kFreeCamera = new FCamera(glm::vec3(0.0f, 0.0f, 3.0f));
//
//#include "Vertices.inc"
//
//	auto* AssetManager = FAssetManager::GetInstance();
//	std::vector<std::string> TriangleShaderFiles{ "Triangle.vert", "Triangle.frag" };
//	std::vector<std::string> FramebufferShaderFiles{ "Framebuffer.vert", "Framebuffer.frag" };
//	std::vector<std::string> LightingShaderFiles{ "Lighting.vert", "Lighting.frag" };
//	std::vector<std::string> AdvancedShaderFiles{ "Advanced.vert", "Advanced.frag" };
//	std::vector<std::string> PointShaderFiles{ "Point.vert", "Point.geom", "Point.frag" };
//	std::vector<std::string> LampShaderMacros{ "__FRAG_LAMP_CUBE" };
//	std::vector<std::string> BorderShaderMacros{ "__FRAG_BORDER" };
//	AssetManager->AddAsset<FShader>("Triangle", FShader(TriangleShaderFiles));
//	AssetManager->AddAsset<FShader>("Framebuffer", FShader(FramebufferShaderFiles));
//	AssetManager->AddAsset<FShader>("Lighting", FShader(LightingShaderFiles));
//	AssetManager->AddAsset<FShader>("Advanced", FShader(AdvancedShaderFiles));
//	AssetManager->AddAsset<FShader>("Lamp", FShader(LightingShaderFiles, "", LampShaderMacros));
//	AssetManager->AddAsset<FShader>("Border", FShader(AdvancedShaderFiles, "", BorderShaderMacros));
//	AssetManager->AddAsset<FShader>("Point", FShader(PointShaderFiles));
//	AssetManager->AddAsset<FTexture>("Metal", FTexture(FTexture::ETextureType::k2D, "Metal.png", false));
//	AssetManager->AddAsset<FTexture>("Marble", FTexture(FTexture::ETextureType::k2D, "Marble.jpg", false));
//	AssetManager->AddAsset<FTexture>("RedWindow", FTexture(FTexture::ETextureType::k2D, "RedWindow.png", true));
//	AssetManager->AddAsset<FTexture>("Grass", FTexture(FTexture::ETextureType::k2D, "Grass.png", true, false));
//	AssetManager->AddAsset<FTexture>("Wood", FTexture(FTexture::ETextureType::k2D, "Wood.png"));
//	//AssetManager->AddAsset<FModel>("Backpack", FModel("Backpack/backpack.obj", "Lighting"));
//	//AssetManager->AddAsset<FModel>("Nanosuit", FModel("Nanosuit/nanosuit.obj", "Lighting"));
//
//	GLuint TriangleVertexBuffer = 0;
//	glCreateBuffers(1, &TriangleVertexBuffer);
//	glNamedBufferData(TriangleVertexBuffer, SingleTriangle.size() * sizeof(GLfloat), SingleTriangle.data(), GL_STATIC_DRAW);
//
//	GLuint CubeVertexBuffer = 0;
//	glCreateBuffers(1, &CubeVertexBuffer);
//	glNamedBufferData(CubeVertexBuffer, ContainerVertices.size() * sizeof(GLfloat), ContainerVertices.data(), GL_STATIC_DRAW);
//
//	GLuint PlaneVertexBuffer = 0;
//	glCreateBuffers(1, &PlaneVertexBuffer);
//	glNamedBufferData(PlaneVertexBuffer, PlaneVertices.size() * sizeof(GLfloat), PlaneVertices.data(), GL_STATIC_DRAW);
//
//	GLuint TransparentVertexBuffer = 0;
//	glCreateBuffers(1, &TransparentVertexBuffer);
//	glNamedBufferData(TransparentVertexBuffer, TransparentVertices.size() * sizeof(GLfloat), TransparentVertices.data(), GL_STATIC_DRAW);
//
//	GLuint QuadVertexBuffer = 0;
//	glCreateBuffers(1, &QuadVertexBuffer);
//	glNamedBufferData(QuadVertexBuffer, QuadVertices.size() * sizeof(GLfloat), QuadVertices.data(), GL_STATIC_DRAW);
//
//	GLuint PointVertexBuffer = 0;
//	glCreateBuffers(1, &PointVertexBuffer);
//	glNamedBufferData(PointVertexBuffer, Points.size() * sizeof(GLfloat), Points.data(), GL_STATIC_DRAW);
//
//	GLuint TriangleVertexArray = 0;
//	glCreateVertexArrays(1, &TriangleVertexArray);
//	glVertexArrayVertexBuffer(TriangleVertexArray, 0, TriangleVertexBuffer, 0, 3 * sizeof(GLfloat));
//	glEnableVertexArrayAttrib(TriangleVertexArray, 0);
//	glVertexArrayAttribFormat(TriangleVertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
//	glVertexArrayAttribBinding(TriangleVertexArray, 0, 0);
//
//	GLuint LampVertexArray = 0;
//	glCreateVertexArrays(1, &LampVertexArray);
//	glVertexArrayVertexBuffer(LampVertexArray, 0, CubeVertexBuffer, 0, 5 * sizeof(GLfloat));
//	glEnableVertexArrayAttrib(LampVertexArray, 0);
//	glVertexArrayAttribFormat(LampVertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
//	glVertexArrayAttribBinding(LampVertexArray, 0, 0);
//
//	GLuint CubeVertexArray = 0;
//	glCreateVertexArrays(1, &CubeVertexArray);
//	glVertexArrayVertexBuffer(CubeVertexArray, 0, CubeVertexBuffer, 0, 8 * sizeof(GLfloat));
//	glEnableVertexArrayAttrib(CubeVertexArray, 0);
//	glEnableVertexArrayAttrib(CubeVertexArray, 1);
//	glEnableVertexArrayAttrib(CubeVertexArray, 2);
//	glVertexArrayAttribFormat(CubeVertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
//	glVertexArrayAttribFormat(CubeVertexArray, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));
//	glVertexArrayAttribFormat(CubeVertexArray, 2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat));
//	glVertexArrayAttribBinding(CubeVertexArray, 0, 0);
//	glVertexArrayAttribBinding(CubeVertexArray, 1, 0);
//	glVertexArrayAttribBinding(CubeVertexArray, 2, 0);
//
//	GLuint PlaneVertexArray = 0;
//	glCreateVertexArrays(1, &PlaneVertexArray);
//	glVertexArrayVertexBuffer(PlaneVertexArray, 0, PlaneVertexBuffer, 0, 8 * sizeof(GLfloat));
//	glEnableVertexArrayAttrib(PlaneVertexArray, 0);
//	glEnableVertexArrayAttrib(PlaneVertexArray, 1);
//	glVertexArrayAttribFormat(PlaneVertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
//	glVertexArrayAttribFormat(PlaneVertexArray, 1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat));
//	glVertexArrayAttribBinding(PlaneVertexArray, 0, 0);
//	glVertexArrayAttribBinding(PlaneVertexArray, 1, 0);
//
//	GLuint TransparentVertexArray = 0;
//	glCreateVertexArrays(1, &TransparentVertexArray);
//	glVertexArrayVertexBuffer(TransparentVertexArray, 0, TransparentVertexBuffer, 0, 5 * sizeof(GLfloat));
//	glEnableVertexArrayAttrib(TransparentVertexArray, 0);
//	glEnableVertexArrayAttrib(TransparentVertexArray, 1);
//	glVertexArrayAttribFormat(TransparentVertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
//	glVertexArrayAttribFormat(TransparentVertexArray, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));
//	glVertexArrayAttribBinding(TransparentVertexArray, 0, 0);
//	glVertexArrayAttribBinding(TransparentVertexArray, 1, 0);
//
//	GLuint QuadVertexArray = 0;
//	glCreateVertexArrays(1, &QuadVertexArray);
//	glVertexArrayVertexBuffer(QuadVertexArray, 0, QuadVertexBuffer, 0, 4 * sizeof(GLfloat));
//	glEnableVertexArrayAttrib(QuadVertexArray, 0);
//	glEnableVertexArrayAttrib(QuadVertexArray, 1);
//	glVertexArrayAttribFormat(QuadVertexArray, 0, 2, GL_FLOAT, GL_FALSE, 0);
//	glVertexArrayAttribFormat(QuadVertexArray, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));
//	glVertexArrayAttribBinding(QuadVertexArray, 0, 0);
//	glVertexArrayAttribBinding(QuadVertexArray, 1, 0);
//
//	GLuint PointVertexArray = 0;
//	glCreateVertexArrays(1, &PointVertexArray);
//	glVertexArrayVertexBuffer(PointVertexArray, 0, PointVertexBuffer, 0, 5 * sizeof(GLfloat));
//	glEnableVertexArrayAttrib(PointVertexArray, 0);
//	glEnableVertexArrayAttrib(PointVertexArray, 1);
//	glVertexArrayAttribFormat(PointVertexArray, 0, 2, GL_FLOAT, GL_FALSE, 0);
//	glVertexArrayAttribFormat(PointVertexArray, 1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));
//	glVertexArrayAttribBinding(PointVertexArray, 0, 0);
//	glVertexArrayAttribBinding(PointVertexArray, 1, 0);
//
//	kFramebuffer = new FFramebuffer(FFramebuffer::EAttachmentType::kColor | FFramebuffer::EAttachmentType::kDepthStencil,
//									kWindowWidth, kWindowHeight, GL_RGBA16F, kMultiSamples, 1);
//
//	//FScreenQuadRenderer QuadRenderer;
//	//QuadRenderer.SetTexturesFromFramebuffer(*kFramebuffer, { 0 });
//
//	//GLuint DepthMap = 0;
//	//glCreateTextures(GL_TEXTURE_2D, 1, &DepthMap);
//	//glTextureStorage2D(DepthMap, 1, GL_DEPTH_COMPONENT, 1024, 1024);
//	//glTextureParameteri(DepthMap, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	//glTextureParameteri(DepthMap, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	//glTextureParameteri(DepthMap, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	//glTextureParameteri(DepthMap, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	//GLuint DepthMapFramebuffer = 0;
//	//glCreateFramebuffers(1, &DepthMapFramebuffer);
//	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthMap, 0);
//	//glDrawBuffer(GL_NONE);
//	//glReadBuffer(GL_NONE);
//
//	//glEnable(GL_BLEND);
//	//glEnable(GL_DEPTH_TEST);
//	//glEnable(GL_MULTISAMPLE);
//	//glEnable(GL_PROGRAM_POINT_SIZE);
//
//	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//	double CurrentTime   = 0.0;
//	double PreviousTime  = glfwGetTime();
//	double LastFrameTime = 0.0;
//	double DeltaTime     = 0.0;
//	int    FrameCount    = 0;
//
//	auto* TriangleShader    = AssetManager->GetAsset<FShader>("Triangle");
//	auto* FramebufferShader = AssetManager->GetAsset<FShader>("Framebuffer");
//	auto* LightingShader    = AssetManager->GetAsset<FShader>("Lighting");
//	auto* AdvancedShader    = AssetManager->GetAsset<FShader>("Advanced");
//	auto* LampShader        = AssetManager->GetAsset<FShader>("Lamp");
//	auto* BorderShader      = AssetManager->GetAsset<FShader>("Border");
//	auto* PointShader       = AssetManager->GetAsset<FShader>("Point");
//	auto* Metal             = AssetManager->GetAsset<FTexture>("Metal");
//	auto* Marble            = AssetManager->GetAsset<FTexture>("Marble");
//	auto* RedWindow         = AssetManager->GetAsset<FTexture>("RedWindow");
//	auto* Grass             = AssetManager->GetAsset<FTexture>("Grass");
//	auto* Backpack          = AssetManager->GetAsset<FModel>("Backpack");
//	auto* Nanosuit          = AssetManager->GetAsset<FModel>("Nanosuit");
//
//	Metal->BindTextureUnit(0);
//	Marble->BindTextureUnit(1);
//	Grass->BindTextureUnit(2);
//	RedWindow->BindTextureUnit(3);
//
//	glm::mat4x4 Model(1.0f);
//	glm::mat4x4 View(1.0f);
//	glm::mat4x4 Projection(1.0f);
//	glm::mat3x3 NormalMatrix(1.0f);
//	glm::vec3   LightPos(1.2f, 1.0f, 2.0f);
//
//	//std::vector<std::string> MatrixMembers = { "iModel", "iView", "iProjection" };
//
//	////auto* UboManager = FUniformBlockManager::GetInstance();
//	////UboManager->CreateSharedBlock(AdvancedShader->GetProgram(), "Matrices", 0, MatrixMembers);
//	////UboManager->VerifyBlockLayout(AdvancedShader->GetProgram(), "Matrices");
//
//	////auto ModelUpdater      = UboManager->GetBlockUpdater<glm::mat4x4>("Matrices", "iModel");
//	////auto ViewUpdater       = UboManager->GetBlockUpdater<glm::mat4x4>("Matrices", "iView");
//	////auto ProjectionUpdater = UboManager->GetBlockUpdater<glm::mat4x4>("Matrices", "iProjection");
//
//	//auto* SsboManager = FStorageBlockManager::GetInstance();
//	//SsboManager->CreateSharedBlock(AdvancedShader->GetProgram(), "Matrices", 0, MatrixMembers);
//	//SsboManager->VerifyBlockLayout(AdvancedShader->GetProgram(), "Matrices");
//
//	//auto ModelUpdater      = SsboManager->GetBlockUpdater<glm::mat4x4>("Matrices", "iModel");
//	//auto ViewUpdater       = SsboManager->GetBlockUpdater<glm::mat4x4>("Matrices", "iView");
//	//auto ProjectionUpdater = SsboManager->GetBlockUpdater<glm::mat4x4>("Matrices", "iProjection");
//
//	//VertMatrices MvpMatrices;
//
//	while (!glfwWindowShouldClose(Window))
//	{
//		ProcessInput(Window, DeltaTime);
//
//		TriangleShader->UseProgram();
//		glBindVertexArray(TriangleVertexArray);
//		glDrawArrays(GL_TRIANGLES, 0, 3);
//
//		//kFramebuffer->Bind(FFramebuffer::EFramebufferType::kMsaa);
//
//		//glEnable(GL_DEPTH_TEST);
//		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		////PointShader->UseProgram();
//		////glBindVertexArray(PointVertexArray);
//		////glDrawArrays(GL_POINTS, 0, 4);
//
//		////GLfloat NearPlane = 1.0f;
//		////GLfloat FarPlane  = 7.5f;
//
//		////glm::mat4x4 LightProjection  = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, NearPlane, FarPlane);
//		////glm::mat4x4 LightView        = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//		////glm::mat4x4 LightSpaceMatrix = LightProjection * LightView;
//
//		//Model = glm::mat4x4(1.0f);
//		//View  = kFreeCamera->GetViewMatrix();
//		//Projection = glm::perspective(glm::radians(kFreeCamera->GetCameraZoom()), kWindowAspect, 0.1f, 10000.0f);
//
//		//MvpMatrices = { Model, View, Projection };
//
//		//Model = glm::mat4x4(1.0f);
//		//AdvancedShader->UseProgram();
//		//AdvancedShader->SetUniform1i("iTex", 0);
//		//
//		////UboManager->UpdateEntrieBlock("Matrices", MvpMatrices);
//		//SsboManager->UpdateEntrieBlock("Matrices", MvpMatrices);
//		//glBindVertexArray(PlaneVertexArray);
//		//glDrawArrays(GL_TRIANGLES, 0, 6);
//
//		//AdvancedShader->SetUniform1i("iTex", 1);
//		//glBindVertexArray(CubeVertexArray);
//		//Model = glm::translate(Model, glm::vec3(-1.0f, 0.0f, -1.0f));
//		////ModelUpdater << Model;
//		//ModelUpdater.SubmitAt(Model, 0);
//		//glDrawArrays(GL_TRIANGLES, 0, 36);
//		//Model = glm::mat4x4(1.0f);
//		//Model = glm::translate(Model, glm::vec3(2.0f, 0.0f, 0.0f));
//		////ModelUpdater << Model;
//		//ModelUpdater.SubmitAt(Model, 1);
//		//glDrawArrays(GL_TRIANGLES, 0, 36);
//
//		//Model = glm::mat4x4(1.0f);
//		//Model = glm::translate(Model, glm::vec3(0.0f));
//		//Model = glm::scale(Model, glm::vec3(1.0f));
//		//NormalMatrix = glm::transpose(glm::inverse(Model));
//
//		//LightingShader->UseProgram();
//		//ModelUpdater      << Model;
//		//ViewUpdater       << View;
//		//ProjectionUpdater << Projection;
//
//		//LightingShader->SetUniformMatrix3fv("iNormalMatrix", NormalMatrix);
//		//LightingShader->SetUniform1f("iShininess", 64.0f);
//		//LightingShader->SetUniform3fv("iViewPos", kFreeCamera->GetCameraVector(FCamera::EVectorType::kPosition));
//		//LightingShader->SetUniform3fv("iLight.Position", LightPos);
//		//LightingShader->SetUniform3fv("iLight.AmbientColor", glm::vec3(0.2f));
//		//LightingShader->SetUniform3fv("iLight.DiffuseColor", glm::vec3(1.0f));
//		//LightingShader->SetUniform3fv("iLight.SpecularColor", glm::vec3(1.0f));
//		//Backpack->DynamicDraw(*LightingShader);
//
//		//Model = glm::mat4x4(1.0f);
//		//Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));
//		//Model = glm::scale(Model, glm::vec3(0.2f));
//		//ModelUpdater << Model;
//		//Nanosuit->DynamicDraw(*LightingShader);
//
//		//LampShader->UseProgram();
//		//Model = glm::mat4x4(1.0f);
//		//Model = glm::translate(Model, LightPos);
//		//Model = glm::scale(Model, glm::vec3(0.2f));
//		//ModelUpdater      << Model;
//		//ViewUpdater       << View;
//		//ProjectionUpdater << Projection;
//
//		//glBindVertexArray(LampVertexArray);
//		//glDrawArrays(GL_TRIANGLES, 0, 36);
//
//		//kFramebuffer->Blit();
//		//QuadRenderer.Draw(*FramebufferShader, { { "iTexColorBuffer", 4, 0 } });
//
//		glfwSwapBuffers(Window);
//		glfwPollEvents();
//
//		CurrentTime   = glfwGetTime();
//		DeltaTime     = CurrentTime - LastFrameTime;
//		LastFrameTime = CurrentTime;
//		++FrameCount;
//		if (CurrentTime - PreviousTime >= 1.0)
//		{
//			glfwSetWindowTitle(Window, (std::string(kWindowTitle) + " " + std::to_string(FrameCount)).c_str());
//			FrameCount   = 0;
//			PreviousTime = CurrentTime;
//		}
//	}
//
//	Terminate(Window);
//
//	return EXIT_SUCCESS;
//}
//
//namespace
//{
//	void CursorPosCallback(GLFWwindow* Window, double PosX, double PosY)
//	{
//		if (kbFirstMouse)
//		{
//			kLastX = PosX;
//			kLastY = PosY;
//			kbFirstMouse = false;
//		}
//
//		double OffsetX = PosX - kLastX;
//		double OffsetY = kLastY - PosY;
//		kLastX = PosX;
//		kLastY = PosY;
//
//		kFreeCamera->ProcessMouseMovement(OffsetX, OffsetY);
//	}
//
//	void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height)
//	{
//		glViewport(0, 0, Width, Height);
//		if (Width != 0 && Height != 0)
//		{
//			kWindowWidth  = Width;
//			kWindowHeight = Height;
//			kWindowAspect = static_cast<float>(Width) / static_cast<float>(Height);
//
//			kFramebuffer->Resize(Width, Height);
//		}
//	}
//
//	void MessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, const GLchar* Message, const void* UserParam)
//	{
//		auto SourceStr = [Source]() -> std::string
//		{
//			switch (Source)
//			{
//			case GL_DEBUG_SOURCE_API:
//				return "API";
//			case GL_DEBUG_SOURCE_APPLICATION:
//				return "APPLICATION";
//			case GL_DEBUG_SOURCE_OTHER:
//				return "OTHER";
//			case GL_DEBUG_SOURCE_SHADER_COMPILER:
//				return "SHADER";
//			case GL_DEBUG_SOURCE_THIRD_PARTY:
//				return "THIRD_PARTY";
//			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
//				return "WINDOW_SYSTEM";
//			default:
//				assert(false);
//			}
//		}();
//
//		auto TypeStr = [Type]() -> std::string
//		{
//			switch (Type)
//			{
//			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
//				return "DEPRECATED_BEHAVIOR";
//			case GL_DEBUG_TYPE_ERROR:
//				return "ERROR";
//			case GL_DEBUG_TYPE_MARKER:
//				return "MARKER";
//			case GL_DEBUG_TYPE_OTHER:
//				return "OTHER";
//			case GL_DEBUG_TYPE_PERFORMANCE:
//				return "PERFORMANCE";
//			case GL_DEBUG_TYPE_PORTABILITY:
//				return "PORTABILITY";
//			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
//				return "UNDEFINED_BEHAVIOR";
//			default:
//				assert(false);
//			}
//		}();
//
//		auto SeverityStr = [Severity]() -> std::string
//		{
//			switch (Severity)
//			{
//			case GL_DEBUG_SEVERITY_HIGH:
//				return "HIGH";
//			case GL_DEBUG_SEVERITY_LOW:
//				return "LOW";
//			case GL_DEBUG_SEVERITY_MEDIUM:
//				return "MEDIUM";
//			case GL_DEBUG_SEVERITY_NOTIFICATION:
//				return "NOTIFICATION";
//			default:
//				assert(false);
//			}
//		}();
//
//		std::println("Source: {}, Type: {}, Severity: {}\n{}: {}", SourceStr, TypeStr, SeverityStr, Id, Message);
//	}
//
//	void ProcessInput(GLFWwindow* Window, double DeltaTime)
//	{
//		if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		{
//			glfwSetWindowShouldClose(Window, GL_TRUE);
//		}
//
//		if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
//		{
//			glfwSetCursorPosCallback(Window, CursorPosCallback);
//			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//		}
//
//		if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
//		{
//			glfwSetCursorPosCallback(Window, nullptr);
//			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//			kbFirstMouse = true;
//		}
//
//		if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
//			kFreeCamera->ProcessKeyboard(FCamera::EMovement::kForward, DeltaTime);
//		if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
//			kFreeCamera->ProcessKeyboard(FCamera::EMovement::kBack, DeltaTime);
//		if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
//			kFreeCamera->ProcessKeyboard(FCamera::EMovement::kLeft, DeltaTime);
//		if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
//			kFreeCamera->ProcessKeyboard(FCamera::EMovement::kRight, DeltaTime);
//		if (glfwGetKey(Window, GLFW_KEY_R) == GLFW_PRESS)
//			kFreeCamera->ProcessKeyboard(FCamera::EMovement::kUp, DeltaTime);
//		if (glfwGetKey(Window, GLFW_KEY_F) == GLFW_PRESS)
//			kFreeCamera->ProcessKeyboard(FCamera::EMovement::kDown, DeltaTime);
//		if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
//			kFreeCamera->ProcessKeyboard(FCamera::EMovement::kRollLeft, DeltaTime);
//		if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
//			kFreeCamera->ProcessKeyboard(FCamera::EMovement::kRollRight, DeltaTime);
//	}
//
//	void ScrollCallback(GLFWwindow* Window, GLdouble OffsetX, GLdouble OffsetY)
//	{
//		kFreeCamera->ProcessMouseScroll(OffsetY);
//	}
//
//	void Terminate(GLFWwindow* Window)
//	{
//		if (kFreeCamera)
//		{
//			delete kFreeCamera;
//			kFreeCamera = nullptr;
//		}
//
//		glfwDestroyWindow(Window);
//		glfwTerminate();
//	}
//}

int main() {
    return 0;
}
