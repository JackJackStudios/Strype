#include "ContentBrowserPanel.hpp"

#include "InspectorPanel.hpp"

#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>

namespace Strype {

	namespace Utils {

		template<typename UIFunction>
		static void DropdownMenu(const std::string& name, UIFunction uiFunction)
		{
			const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
				ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
				ImGuiTreeNodeFlags_FramePadding;

			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
			bool open = ImGui::TreeNodeEx(name.c_str(), treeNodeFlags, "%s", name.c_str());
			ImGui::PopStyleVar();

			if (open)
			{
				uiFunction();
				ImGui::TreePop();
			}
		}

		static int NumOfFileOrDirs(const std::filesystem::path& path)
		{
			return std::distance(std::filesystem::directory_iterator(path), std::filesystem::directory_iterator{});
		}

		static AGI::Texture LoadTexture(const std::filesystem::path& path)
		{
			int width, height, channels;

        	stbi_set_flip_vertically_on_load(1);
        	stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

			STY_VERIFY(data, "Failed to load sprite \"{}\" ", path);

			AGI::TextureSpecification textureSpec;
			textureSpec.Width = width;
			textureSpec.Height = height;
			textureSpec.Format = AGI::ChannelsToImageFormat(channels);
			
        	AGI::Texture texture = Renderer::GetContext()->CreateTexture(textureSpec);
        	texture->SetData(data, width * height * channels);

			return texture;
		}

	}

	ContentBrowserPanel::ContentBrowserPanel()
	{
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
		ImGui::Begin("Content Browser");

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
		
		for (size_t i=0; i < m_CurrentDirectory->Nodes.size();)
		{
			TreeNode& node = m_CurrentDirectory->Nodes[i];

			const std::filesystem::path& path = node.Path;
			AGI::Texture icon = GetIcon(node.Handle);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((std::string("##") + std::to_string(node.Handle)).c_str(), (ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
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
					if (Project::GetAssetType(node.Handle) == AssetType::Object)
					{
						STY_CORE_VERIFY(false, "Not implemented");
					}

					Project::DeleteAsset(node.Handle);

					RefreshAssetTree();
					ImGui::EndPopup();
					continue;
				}

				ImGui::EndPopup();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (std::filesystem::is_directory(path))
				{
					m_CurrentDirectory = &node;
				}
				else if (AssetType type = Utils::GetAssetTypeFromFileExtension(path.extension()); m_ItemClicksCallbacks.find(type) != m_ItemClicksCallbacks.end())
				{
					AssetMetadata metadata;
					metadata.FilePath = path;
					m_ItemClicksCallbacks[type](metadata);
				}
			}

			ImGui::TextWrapped(path.stem().string().c_str());

			ImGui::NextColumn();
			++i;
		}

		if (m_InputActive)
		{
			ImGui::OpenPopup("TextInput");

			if (ImGui::BeginPopup("TextInput"))
			{
				char inputText[256] = ""; 

				ImGui::Text("Name"); ImGui::SameLine();
				ImGui::InputText("##Input", inputText, sizeof(inputText));

				if (ImGui::IsItemDeactivated())
				{
					m_InputActive = false;

					if (m_InputType != AssetType::None)
					{
						std::string file = std::filesystem::path(inputText).stem().string() + Utils::GetFileExtensionFromAssetType(m_InputType).string();
						Project::NewAsset(m_CurrentDirectory->Path / file);
					}
					else
						std::filesystem::create_directory(m_CurrentDirectory->Path / inputText);

					RefreshAssetTree();
				}

				if (Input::IsKeyPressed(KeyCode::Escape))
					m_InputActive = false;

				ImGui::EndPopup();
			}
		}

		if (!popupOpen)
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::BeginMenu("Create"))
				{
					if (ImGui::MenuItem("Prefab", "Alt+P"))
					{
						m_InputActive = true;
						m_InputType = AssetType::Object;
					}

					if (ImGui::MenuItem("Room", "Alt+R"))
					{
						m_InputActive = true;
						m_InputType = AssetType::Room;
					}

					if (ImGui::MenuItem("Sprite", "Alt+S"))
					{
						const std::filesystem::path& path = FileDialogs::OpenFile("Strype Sprite (.png, .jpg, .jpeg)\0*.png\0*.jpg\0*.jpeg\0");
						std::filesystem::copy_file(path, m_CurrentDirectory->Path / path.filename(), std::filesystem::copy_options::overwrite_existing);

						Project::ImportAsset(std::filesystem::relative(m_CurrentDirectory->Path / path.filename(), Project::GetProjectDirectory()));
						RefreshAssetTree();
					}

					if (ImGui::MenuItem("Sound", "Alt+U"))
					{
						const std::filesystem::path& path = FileDialogs::OpenFile("Strype Sound (.wav)\0*.wav\0");
						std::filesystem::copy_file(path, m_CurrentDirectory->Path / path.filename(), std::filesystem::copy_options::overwrite_existing);

						Project::ImportAsset(std::filesystem::relative(m_CurrentDirectory->Path / path.filename(), Project::GetProjectDirectory()));
						RefreshAssetTree();
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Create Folder"))
				{
					m_InputActive = true;
					m_InputType = AssetType::None;
				}

				ImGui::EndPopup();
			}
		}

		ImGui::Columns(1);
		ImGui::End();

		m_Inspector->AddType<Room>(STY_BIND_EVENT_FN(ContentBrowserPanel::OnInspectorRender));
	}

	void ContentBrowserPanel::RefreshAssetTree()
	{
		m_RootDirectory = TreeNode(Project::GetProjectDirectory(), nullptr);
		m_CurrentDirectory = &m_RootDirectory;
		
		RefreshTreeNode(m_RootDirectory);
	}

	void ContentBrowserPanel::RefreshTreeNode(TreeNode& node)
	{
		if (!Utils::NumOfFileOrDirs(Project::GetProjectDirectory() / node.Path))
			return;

		for (const auto& entry : std::filesystem::directory_iterator(Project::GetProjectDirectory() / node.Path))
		{
			std::filesystem::path relativePath = std::filesystem::relative(entry.path(), m_RootDirectory.Path);
			bool isDirectory = entry.is_directory();

			if (!isDirectory && (s_AssetExtensionMap.find(relativePath.extension()) == s_AssetExtensionMap.end()))
				continue; // Not a valid asset

			if (isDirectory)
			{
				if (entry.path().filename() != HIDDEN_FOLDER)
				{
					node.Nodes.emplace_back(entry.path(), &node);
					RefreshTreeNode(node.Nodes.back());
				}
			}
			else if (Project::IsAssetLoaded(relativePath))
			{
				node.Nodes.emplace_back(entry.path(), &node, Project::GetAssetHandle(relativePath));
			}
		}

		std::stable_partition(node.Nodes.begin(), node.Nodes.end(), [this](const TreeNode& node) 
		{
			return std::filesystem::is_directory(Project::GetProjectDirectory() / node.Path);
		});
	}

	void ContentBrowserPanel::OnInspectorRender(Room* select)
	{
		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 128.0f) * 0.5f);
		ImGui::Image(m_Icons[AssetType::Room]->GetRendererID(), ImVec2(128.0f, 128.0f), {0, 1}, {1, 0});

		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 128.0f) * 0.5f);
		ImGui::Button(Project::GetFilePath(select->Handle).filename().string().c_str(), ImVec2(128.0f, 0));

		Utils::DropdownMenu("Properties", [&]() {
			ImGui::Columns(2, nullptr, false);

			ImGui::DragScalar("Width", ImGuiDataType_U64, &select->m_Width);
			ImGui::NextColumn();
			
			ImGui::DragScalar("Height", ImGuiDataType_U64, &select->m_Height);
			ImGui::NextColumn();

			ImGui::Columns(1);
			ImGui::ColorEdit3("Background Colour", glm::value_ptr(select->m_BackgroundColour), ImGuiColorEditFlags_NoInputs);
		});
	}

	AGI::Texture ContentBrowserPanel::GetIcon(AssetHandle handle)
	{
		if (handle == 0)
			return m_DirectoryIcon;

		AssetType type = Project::GetAssetType(handle);
		if (type == AssetType::Sprite)
			return Project::GetAsset<Sprite>(handle)->GetTexture();

		if (m_Icons.find(type) == m_Icons.end() || type == AssetType::None)
		{
			STY_CORE_WARN("No available icons from AssetHandle: {}", (uint64_t)handle);
			return m_FileIcon;
		}

		return m_Icons.at(type);
	}

}