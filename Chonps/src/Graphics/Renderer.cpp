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
	void Renderer::Draw(VertexArray* vertexArray)
	{
		vertexArray->Bind();
		getRendererAPI()->Draw(vertexArray->GetIndexCount());
		vertexArray->Unbind();

		s_Data.Stats.vertices += vertexArray->GetVertexCount();
		s_Data.Stats.triangles += vertexArray->GetIndexCount() / 3;
		s_Data.Stats.indices += vertexArray->GetIndexCount();
		s_Data.Stats.drawCalls += 1;
	}

	void Renderer::DrawLine(VertexArray* vertexArray)
	{

	}

	// Call before renderering or drawing
	void Renderer::BeginScene(Camera& camera, Shader* shader, const char* uniform)
	{
		shader->Bind();
		glm::vec3 camPos = camera.GetPosition();
		Chonps::uploadUniform3f(shader->GetID(), "camPos", camPos.x, camPos.y, camPos.z);
		camera.UploadMatrix(shader, uniform);
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
	void renderDraw(VertexArray* vertexArray)
	{
		vertexArray->Bind();
		getRendererAPI()->Draw(vertexArray->GetIndexCount());
		vertexArray->Unbind();

		Renderer::GetStats().vertices += vertexArray->GetVertexCount();
		Renderer::GetStats().triangles += vertexArray->GetIndexCount() / 3;
		Renderer::GetStats().indices += vertexArray->GetIndexCount();
		Renderer::GetStats().drawCalls += 1;
	}

	void renderDraw(const uint32_t& count)
	{
		getRendererAPI()->Draw(count);
	}

	void renderDrawLine(VertexArray* vertexArray)
	{
		vertexArray->Bind();
		getRendererAPI()->DrawLine();
		vertexArray->Unbind();
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