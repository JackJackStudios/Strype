#pragma once

#include "EditorPanel.hpp"

#include <typeindex>

namespace Strype {

	using InspectorRenderFunc = std::function<void(void*)>;

	class InspectorPanel : public EditorPanel
	{
	public:
		InspectorPanel();

		virtual void OnImGuiRender();

		template<typename T>
		void SetSelected(T* asset)
		{
			m_SelectedAsset = asset;
			m_SelectedType = std::type_index(typeid(T));
		}

		template<typename T>
		void SetSelected(AssetHandle handle)
		{
			SetSelected<T>(Project::GetAsset<T>(handle).get());
		}

		void RemoveSelected()
		{
			SetSelected<void>(nullptr);
		}

		template<typename T>
		void AddType(std::function<void(T*)> func)
		{
			m_ItemClicksCallbacks[std::type_index(typeid(T))] = [func](void* obj) {
				func(static_cast<T*>(obj));
			};
		}
	private:
		std::unordered_map<std::type_index, InspectorRenderFunc> m_ItemClicksCallbacks;
		void* m_SelectedAsset = nullptr;
		std::type_index m_SelectedType = typeid(void);
	};
	
}
