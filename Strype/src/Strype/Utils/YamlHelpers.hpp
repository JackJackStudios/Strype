#pragma once

#include "stypch.hpp"

#include <yaml-cpp/yaml.h>
#include <magic_enum/magic_enum.hpp>
#include <glm/glm.hpp>

#include "Strype/Core/UUID.hpp"

namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::ivec2>
	{
		static Node encode(const glm::ivec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::ivec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<int>();
			rhs.y = node[1].as<int>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<std::filesystem::path> 
	{
		static Node encode(const std::filesystem::path& rhs) 
		{
			Node node;
			node = rhs.string();
			return node;
		}

		static bool decode(const Node& node, std::filesystem::path& rhs) 
		{
			if (!node.IsScalar())
				return false;

			rhs = node.as<std::string>();
			return true;
		}
	};

	template<>
	struct convert<Strype::UUID> 
	{
		static Node encode(const Strype::UUID& rhs) 
		{
			Node node;
			node = static_cast<uint64_t>(rhs);
			return node;
		}

		static bool decode(const Node& node, Strype::UUID& rhs) 
		{
			if (!node.IsScalar())
				return false;

			rhs = Strype::UUID(node.as<uint64_t>());
			return true;
		}
	};

	template<>
	struct convert<Strype::UUID32>
	{
		static Node encode(const Strype::UUID32& rhs)
		{
			Node node;
			node = static_cast<uint32_t>(rhs);
			return node;
		}

		static bool decode(const Node& node, Strype::UUID32& rhs)
		{
			if (!node.IsScalar())
				return false;

			rhs = Strype::UUID32(node.as<uint32_t>());
			return true;
		}
	};

#define ENCODE_BINDING_TYPE(type) case Strype::BindingType::type: str.append(magic_enum::enum_name(binding.Value.type)); break
#define DECODE_BINDING_TYPE(type, codetype) case Strype::BindingType::type: rhs.Value.type = magic_enum::enum_cast<Strype::codetype>(enumValue).value(); break

	std::string BindingToString(const Strype::InputBinding& binding)
	{
		std::string str = magic_enum::enum_name(binding.Type).data();
		str.append("::");

		switch (binding.Type)
		{
		ENCODE_BINDING_TYPE(Keyboard);
		ENCODE_BINDING_TYPE(MouseButton);
		ENCODE_BINDING_TYPE(GamepadButton);
		ENCODE_BINDING_TYPE(GamepadAxis);

		default: STY_CORE_VERIFY(false, "Undefined InputBinding");
		}

		return str;
	}

	template<>
	struct convert<Strype::InputBinding>
	{
		static Node encode(const Strype::InputBinding& rhs)
		{
			return Node(BindingToString(rhs));
		}

		static bool decode(const Node& node, Strype::InputBinding& rhs)
		{
			std::string name = node.as<std::string>();

			auto seper = name.find("::");
			if (seper == std::string::npos) return false;
		
			std::string enumType = name.substr(0, seper);
			std::string enumValue = name.substr(seper + 2);

			rhs.Type = magic_enum::enum_cast<Strype::BindingType>(enumType).value();
			switch (rhs.Type)
			{
			DECODE_BINDING_TYPE(Keyboard, KeyCode);
			DECODE_BINDING_TYPE(MouseButton, MouseCode);
			DECODE_BINDING_TYPE(GamepadButton, ButtonCode);
			DECODE_BINDING_TYPE(GamepadAxis, GamepadAxis);

			default: STY_CORE_VERIFY(false, "Undefined BindingType");
			}
		}
	};

}

namespace Strype {

	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}


	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& out, const std::filesystem::path& path) {
		out << path.string();
		return out;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& out, const Strype::InputBinding& binding) {
		out << YAML::BindingToString(binding);
		return out;
	}

	class ScopedMap 
	{
	public:
		ScopedMap(YAML::Emitter& emitter, const std::string& name = std::string())
			: m_Emitter(emitter) 
		{
			if (!name.empty())
				m_Emitter << YAML::Key << name;

			m_Emitter << YAML::BeginMap;
		}

		~ScopedMap() {
			m_Emitter << YAML::EndMap;
		}

	private:
		YAML::Emitter& m_Emitter;
	};

	class ScopedSeq 
	{
	public:
		ScopedSeq(YAML::Emitter& emitter, const std::string& name = std::string())
			: m_Emitter(emitter) 
		{
			if (!name.empty())
				m_Emitter << YAML::Key << name;

			m_Emitter << YAML::BeginSeq;
		}

		~ScopedSeq() 
		{
			m_Emitter << YAML::EndSeq;
		}

	private:
		YAML::Emitter& m_Emitter;
	};

}