#include "stypch.h"
#include "RenderPipeline.h"

#include "Strype/Core/Application.h"

#include "Renderer.h"
#include <numeric>

namespace Strype {

	namespace Utils {

		static std::string ReadFile(const std::filesystem::path& filepath)
		{
			std::string result;
			std::ifstream in(filepath, std::ios::in | std::ios::binary);
			if (in)
			{
				in.seekg(0, std::ios::end);
				size_t size = in.tellg();
				if (size != -1)
				{
					result.resize(size);
					in.seekg(0, std::ios::beg);
					in.read(&result[0], size);
					in.close();
				}
				else
				{
					STY_CORE_ERROR("Could not read from file '{0}'", filepath.string());
				}
			}
			else
			{
				STY_CORE_ERROR("Could not open file '{0}'", filepath.string());
			}

			return result;
		}

		static void* ShiftPtr(void* origin, uint32_t shift)
		{
			return (uint8_t*)origin + shift;
		}
	};

	QuadPipeline::QuadPipeline()
	{
		Layout = {
			{ AGI::ShaderDataType::Float3, "a_Position" },
			{ AGI::ShaderDataType::Float4, "a_Colour" },
			{ AGI::ShaderDataType::Float2, "a_TexCoord" },
			{ AGI::ShaderDataType::Float,  "a_TexIndex" },
		};
		TextureSampler = "u_Textures";
		ProjectionUniform = "u_ViewProjection";

		Shader = Renderer::GetContext()->CreateShader(AGI::Shader::ProcessSource(Utils::ReadFile((Application::Get().GetConfig().MasterDir / "shaders" / "QuadShader.glsl").string())));
	}

	void QuadPipeline::DrawPrimitive(const glm::mat4& transform, const glm::vec4& colour, const TexCoords& texcoords, float textureIndex)
	{
		for (size_t i = 0; i < 4; i++)
		{
			SubmitAttribute("a_Position", transform * RenderCaps::VertexPositions[i]);
			SubmitAttribute("a_Colour", colour);
			SubmitAttribute("a_TexCoord", texcoords[i]);
			SubmitAttribute("a_TexIndex", textureIndex);

			for (auto& [name, value] : UserAttributes)
				std::memcpy(Utils::ShiftPtr(VBPtr, AttributeCache[name].Offset), &value, AttributeCache[name].Size);

			VBPtr = Utils::ShiftPtr(VBPtr, VertexBuffer->GetLayout().GetStride());
		}

		IndexCount += 6;
		UserAttributes.clear();
	}

}