#include "ContentBrowserPanel.hpp"

#include "InspectorPanel.hpp"
#include "Strype/Utils/ImguiUtils.hpp"

#include <stb_image.h>

#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>

namespace Strype {

	namespace Utils {

		static int NumOfFileOrDirs(const std::filesystem::path& path)
		{
			return std::distance(std::filesystem::directory_iterator(path), std::filesystem::directory_iterator{});
		}

		static AGI::Texture LoadTexture(const std::filesystem::path& path)
		{
			int width, height, channels;

        	stbi_set_flip_vertically_on_load(1);
        	stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

			STY_VERIFY(data, "Failed to load texture \"{}\" ", path);

			AGI::TextureSpecification textureSpec;
			textureSpec.Width = width;
			textureSpec.Height = height;
			textureSpec.Format = AGI::Utils::ChannelsToImageFormat(channels);
			textureSpec.LinearFiltering = true;
			
        	AGI::Texture texture = Renderer::GetCurrent()->GetContext()->CreateTexture(textureSpec);
        	texture->SetData(data, width * height * channels);

			stbi_image_free(data);
			return texture;
		}

	}

	ContentBrowserPanel::ContentBrowserPanel()
	{
		Title = "Content Browser";

		m_DirectoryIcon = Utils::LoadTexture("assets/icons/DirectoryIcon.png");
		m_FileIcon = Utils::LoadTexture("assets/icons/FileIcon.png");

		for (const auto& entry : std::filesystem::directory_iterator("assets/icons/"))
		{
			if (!entry.is_regular_file())
				continue;

			std::string filename = entry.path().stem().string();

			const std::string suffix = "Icon";
			if (filename.size() <= suffix.size() || filename.substr(filename.size() - suffix.size()) != suffix)
				continue;

			std::string enumName = filename.substr(0, filename.size() - suffix.size());

			auto typeEnum = magic_enum::enum_cast<AssetType>(enumName);
			if (!typeEnum.has_value())
				continue;

			m_Icons[typeEnum.value()] = Utils::LoadTexture(entry.path());
		}
	
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		float padding = 16.0f;
		float thumbnailSize = 90.0f;
		float cellSize = thumbnailSize + padding;
		bool popupOpen = false;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		if (m_CurrentDirectory->Parent != nullptr)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton("<-", (ImTextureID)m_DirectoryIcon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				m_CurrentDirectory = m_CurrentDirectory->Parent;

			ImGui::TextWrapped("../");

			ImGui::NextColumn();
		}
		
		for (size_t i = 0; i < m_CurrentDirectory->Children.size();)
		{
			TreeNode& node = m_CurrentDirectory->Children[i];

			TexCoords tx = RenderCaps::TextureCoords;
			AGI::Texture icon = GetIcon(node.Handle, &tx);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushID(i);

			// ImGui needs todo this
			tx = Utils::FlipTexCoordsV(tx);
			
			ImGui::ImageButton("", (ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, ImVec2(tx[0].x, tx[0].y), ImVec2(tx[2].x, tx[2].y));
			
			ImGui::PopID();
			ImGui::PopStyleColor();

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &node.Handle, sizeof(AssetHandle));
				ImGui::EndDragDropSource();
			}
			
			if (ImGui::BeginPopupContextItem())
			{
				popupOpen = true;

				if (ImGui::MenuItem("Delete"))
				{
					auto filepath = Project::GetFilePath(node.Handle);
					Project::RemoveAsset(node.Handle);

					std::filesystem::remove(Project::GetProjectDirectory() / filepath);

					ImGui::EndPopup();
					continue;
				}

				if (ImGui::MenuItem("Reload Asset"))
				{
					Project::ReloadAsset(node.Handle);
				}

				ImGui::EndPopup();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (!node.Handle)
				{
					m_CurrentDirectory = &node;
					break;
				}
				else 
				{
					auto asset = Project::GetAsset<Asset>(node.Handle);
					AssetType type = asset->GetType();

					if (m_ItemClicksCallbacks.find(type) != m_ItemClicksCallbacks.end())
					{
						m_ItemClicksCallbacks[type](asset);
					}
				}
			}

			ImGui::TextWrapped(Project::GetName(node.Handle).c_str());

			ImGui::NextColumn(); 
			++i;
		}

		if (m_InputType != AssetType::None)
		{
			AGI::Texture icon = GetIcon(m_InputType);
			ImGui::Image((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			std::filesystem::path path = std::string(m_InputText) + Utils::GetFileExtensionFromAssetType(m_InputType).string();
			bool overwriting = std::filesystem::exists(m_CurrentDirectory->Path / path);
			
			if (overwriting) 
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));

			ImGui::SetNextItemWidth(thumbnailSize);
			ImGui::InputText("##Input", m_InputText, sizeof(m_InputText));

			if (ImGui::GetCurrentContext()->ColorStack.Size > 0)
				ImGui::PopStyleColor();

			if (ImGui::IsItemActivated()) m_InputActivated = true;
			if (ImGui::IsItemDeactivated() && !overwriting)
			{
				if (m_InputActivated && Input::IsKeyDown(KeyCode::Enter))
				{
					Project::CreateAsset(m_CurrentDirectory->Path / path);
					RefreshAssetTree();
				}

				m_InputType = AssetType::None;
				m_InputActivated = false;
			}

			ImGui::NextColumn();
		}
		else
		{
			m_InputActivated = false;
			memset(m_InputText, 0, sizeof(m_InputText));
		}

		if (!popupOpen)
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::BeginMenu("Create"))
				{
					constexpr auto assetTypes = magic_enum::enum_entries<AssetType>();
					for (const auto& entry : assetTypes)
					{
						const AssetType& type = entry.first;
						const std::string_view& name = entry.second;

						if (type == AssetType::None)
							continue;
						
						//INFO: This tells us whetever the asset can be created
						//      completely empty like a Object/Room but a Sprite/AudioFile
						//      must be based off a file.
						bool canCreate = Project::CanCreateAsset(type);

						if (!ImGui::MenuItem(name.data(), ""))
							continue;

						if (canCreate)
						{
							m_InputType = type;
						}
						else
						{
							const std::filesystem::path& path = FileDialogs::OpenFile(GetDialogMessage(type).data());
							if (path.empty())
								continue;

							std::filesystem::copy_file(path, m_CurrentDirectory->Path / path.filename(), std::filesystem::copy_options::overwrite_existing);
							Project::ImportAsset(m_CurrentDirectory->Path / path.filename());
						}
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Create Folder"))
				{
					STY_CORE_VERIFY(false, "Not implemented");
				}

				ImGui::EndPopup();
			}
		}

		ImGui::Columns(1);

		m_Inspector->AddType<Room>(STY_BIND_EVENT_FN(ContentBrowserPanel::OnInspectorRender));
	}

	void ContentBrowserPanel::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<AssetImportedEvent>(STY_BIND_EVENT_FN(ContentBrowserPanel::OnAssetsUpdated));
		dispatcher.Dispatch<AssetRemovedEvent>(STY_BIND_EVENT_FN(ContentBrowserPanel::OnAssetsUpdated));
	}

	void ContentBrowserPanel::OnAssetsUpdated(Event& e)
	{
		if (e.IsInCategory(EventCategoryAsset))
			RefreshAssetTree();
	}

	void ContentBrowserPanel::RefreshAssetTree()
	{
		m_RootDirectory.Path = Project::GetProjectDirectory();
		m_RootDirectory.Children.clear();
		m_CurrentDirectory = &m_RootDirectory;
		
		auto manager = Project::GetAssetManager();
		manager->ForEach([this](AssetHandle handle)
		{
			TreeNode* currentFolder = &m_RootDirectory;
			const auto& filepath = Project::GetFilePath(handle);

			std::filesystem::path currentPath;
			for (auto it = filepath.begin(); it != filepath.end(); ++it)
			{
				currentPath /= *it;

				// Is file?
				if (std::next(it) == filepath.end())
				{
					currentFolder->AddNode(currentPath, handle);
				}
				else
				{
					currentFolder = &currentFolder->AddNode(currentPath);
				}
			}
		});
	}

	void ContentBrowserPanel::OnInspectorRender(Room* select)
	{
		ImVec2 size = ImVec2(128.0f, 128.0f);

		UI::CenterWidget(size);
		ImGui::Image(m_Icons[AssetType::Room]->GetRendererID(), size, { 0, 1 }, { 1, 0 });

		UI::CenterWidget(size);
		ImGui::Button(select->Name.c_str(), ImVec2(size.x, 0));

		if (UI::DropdownMenu("Properties"))
		{
			ImGui::DragScalar("Width", ImGuiDataType_U64, &select->m_Width);
			ImGui::DragScalar("Height", ImGuiDataType_U64, &select->m_Height);

			ImGui::ColorEdit3("Background Colour", glm::value_ptr(select->m_BackgroundColour), ImGuiColorEditFlags_NoInputs);

			ImGui::TreePop();
		}
	}

	AGI::Texture ContentBrowserPanel::GetIcon(AssetHandle handle, TexCoords* tx)
	{
		if (handle == 0)
			return m_DirectoryIcon;

		AssetType type = Project::GetAssetType(handle);

		if (type == AssetType::Sprite)
		{
			auto sprite = Project::GetAsset<Sprite>(handle);
			*tx = sprite->GetTexCoords();

			return Renderer::GetCurrent()->GetTexture(sprite);
		}

		return GetIcon(type);
	}

	AGI::Texture ContentBrowserPanel::GetIcon(AssetType type)
	{
		if (m_Icons.find(type) == m_Icons.end() || type == AssetType::None)
		{
			STY_CORE_WARN("No available icons for AssetType: {}", magic_enum::enum_name(type));
			return m_FileIcon;
		}

		return m_Icons.at(type);
	}

	std::vector<char> ContentBrowserPanel::GetDialogMessage(AssetType type)
	{
		std::vector<std::string> message;
		message.emplace_back(std::string("Strype ") + std::string(magic_enum::enum_name(type)));

		for (const auto& [ext, assetType] : s_AssetExtensionMap)
		{
			if (assetType == type)
				message.push_back("*" + ext.string());
		}

		std::vector<char> buffer;
		for (const auto& ext : message)
		{
			buffer.insert(buffer.end(), ext.begin(), ext.end());
			buffer.push_back('\0');
		}

		buffer.push_back('\0');
		return buffer;
	}

}