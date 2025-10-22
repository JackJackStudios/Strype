#pragma once

#include "stypch.hpp"

#include <yaml-cpp/yaml.h>
#include <magic_enum/magic_enum.hpp>
#include <glm/glm.hpp>

#include "Strype/Core/UUID.hpp"
#include "Strype/Room/Object.hpp"

namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
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
		static bool decode(const Node& node, Strype::UUID32& rhs)
		{
			if (!node.IsScalar())
				return false;

			rhs = Strype::UUID32(node.as<uint32_t>());
			return true;
		}
	};

	template<>
	struct convert<Strype::CollisionShape>
	{
		static bool decode(const Node& node, Strype::CollisionShape& rhs)
		{
			if (!node.IsSequence())
				return false;

			rhs = magic_enum::enum_cast<Strype::CollisionShape>(node.as<std::string_view>()).value_or(Strype::CollisionShape::None);
		}
	};

#define DECODE_BINDING_TYPE(type, codetype) case Strype::BindingType::type: rhs.Code = (int)magic_enum::enum_cast<Strype::codetype>(enumValue).value(); break

	template<>
	struct convert<Strype::InputBinding>
	{
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

			default: STY_VERIFY(false, "Undefined BindingType");
			}

			return true;
		}
	};

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

	inline YAML::Emitter& operator<<(YAML::Emitter& out, const std::filesystem::path& path)
	{
		out << path.string();
		return out;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& out, const Strype::InputBinding& binding)
	{
		out << fmt::to_string(binding);
		return out;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& out, const Strype::CollisionShape& collision)
	{
		out << magic_enum::enum_name(collision);
		return out;
	}

};