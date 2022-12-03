#include "cepch.h"
#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp>
#include <cmath>

namespace Chonps
{

	RendererData Renderer::s_Data;

	// Sets and init specific render function calls before drawing vertices
	void Renderer::Init()
	{
		
	}

	// Draw vertices through the VertexArray. Draw type is in Triangles
	void Renderer::Draw(VertexArray* VertexArray)
	{
		VertexArray->Bind();
		getRendererAPI()->Draw(VertexArray->GetIndexCount());
		VertexArray->Unbind();

		s_Data.Stats.vertices += VertexArray->GetVertexCount();
		s_Data.Stats.triangles += VertexArray->GetIndexCount() / 3;
		s_Data.Stats.indices += VertexArray->GetIndexCount();
		s_Data.Stats.drawCalls += 1;
	}

	// Call before renderering or drawing
	void Renderer::BeginScene(Camera& camera, Shader* shader, const char* uniform /*= "camMatrix"*/)
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

	RendererStatistics& Renderer::GetStats()
	{
		return s_Data.Stats;
	}

	void Renderer::ResetStats()
	{
		std::memset(&s_Data.Stats, 0, sizeof(RendererStatistics));
	}


	// Render Functions

	// Draw vertices through the VertexArray. Draw type is in Triangles
	void renderDraw(VertexArray* VertexArray)
	{
		VertexArray->Bind();
		getRendererAPI()->Draw(VertexArray->GetIndexCount());
		VertexArray->Unbind();

		Renderer::GetStats().vertices += VertexArray->GetVertexCount();
		Renderer::GetStats().triangles += VertexArray->GetIndexCount() / 3;
		Renderer::GetStats().indices += VertexArray->GetIndexCount();
		Renderer::GetStats().drawCalls += 1;
	}

	// Call before renderering or drawing
	void renderBeginScene(Camera& camera, Shader* shader, const char* uniform /*= "camMatrix"*/)
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
}