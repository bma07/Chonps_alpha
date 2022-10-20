#include "cepch.h"
#include "Renderer.h"


namespace Chonps
{
	// Static Renderer API - Determines the graphics platform API for draw functions and uniforms
	static RendererAPI* s_RendererAPI;

	// Sets and init specific render function calls before drawing vertices
	void Renderer::RenderInit()
	{
		s_RendererAPI->Init();
	}

	// Draw vertices with the count number
	// Number of vertices will be drawn with the size of count
	void Renderer::Draw(const size_t& count)
	{
		s_RendererAPI->Draw(count);
	}

	// Draw vertices through the VAO
	void Renderer::Draw(VAO* VAO)
	{
		s_RendererAPI->Draw(VAO->GetIndexCount());
	}

	// Call before renderering or drawing
	void Renderer::BeginScene(Camera camera, Shader* shader, const char* uniform /*= "camMatrix"*/)
	{
		shader->Bind();
		glm::vec3 camPos = camera.GetPosition();
		Chonps::uploadUniform3f(shader->GetID(), "camPos", camPos.x, camPos.y, camPos.z);
		camera.UploadMatrix(shader, "camMatrix");
	}

	// Call after scene has finished
	void Renderer::EndScene()
	{

	}

	// Clear function clears the window viewport with all of the vertices drawn on it
	void Renderer::Clear()
	{
		s_RendererAPI->Clear();
	}

	// Clears the window and set the color of the viewport/background
	void Renderer::ClearColor(const float r, const float g, const float b, const float w /*= 0.0f*/)
	{
		s_RendererAPI->SetClearColor(r, g, b, w);
	}

	RendererAPI* Renderer::GetRendererAPI()
	{
		return s_RendererAPI;
	}

	// Sets and updates the RenderingAPI to the graphics API currently selected
	void Renderer::SetRendererAPI()
	{
		s_RendererAPI = createRendererAPI();
	}


	// Render Functions

	// Sets and init specific render function calls before drawing vertices
	void renderInit()
	{
		s_RendererAPI->Init();
	}

	// Draw vertices with the count number
	// Number of vertices will be drawn with the size of count
	void renderDraw(const size_t& count)
	{
		s_RendererAPI->Draw(count);
	}

	// Draw vertices through the VAO
	void renderDraw(VAO* VAO)
	{
		s_RendererAPI->Draw(VAO->GetIndexCount());
	}

	// Call before renderering or drawing
	void renderBeginScene(Camera camera, Shader* shader, const char* uniform /*= "camMatrix"*/)
	{
		shader->Bind();
		glm::vec3 camPos = camera.GetPosition();
		Chonps::uploadUniform3f(shader->GetID(), "camPos", camPos.x, camPos.y, camPos.z);
		camera.UploadMatrix(shader, "camMatrix");
	}

	// Call after scene has finished
	void renderEndScene()
	{

	}

	// Clear function clears the window viewport with all of the vertices drawn on it
	void renderClear()
	{
		s_RendererAPI->Clear();
	}

	// Clears the window and set the color of the viewport/background
	void renderClearColor(const float r, const float g, const float b, const float w /*= 0.0f*/)
	{
		s_RendererAPI->SetClearColor(r, g, b, w);
	}

	void setRendererAPI()
	{
		s_RendererAPI = createRendererAPI();
	}

	// Sets and updates the RenderingAPI to the graphics API currently selected
	RendererAPI* getRendererAPI()
	{
		return s_RendererAPI;
	}
}