#include "Application.h"

void on_window_resized(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

Application::Application()
{

}

Application::~Application()
{
	delete this->renderer;
	delete this->gameboy;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(this->window);
}

int Application::Initialize()
{
	this->window = glfwCreateWindow(1280, 720, "Gameboy Emulator", nullptr, nullptr);
	if(this->window == nullptr)
	{
		glfwTerminate();
		std::cout << "Failed to initialize GLFW window" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(this->window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetWindowUserPointer(this->window, this);

	glfwSetFramebufferSizeCallback(this->window, &on_window_resized);

	this->ConfigureImGui();

	this->gameboy = new GameBoy();

	this->renderer = new Renderer();
	return 0;
}

void Application::Run()
{
	dt = 0.0;
	float lastTime = glfwGetTime();
	while(glfwWindowShouldClose(this->window) == false)
	{
		dt = glfwGetTime() - lastTime;
		lastTime = glfwGetTime();

		if (glfwGetKey(this->window, GLFW_KEY_Z) == GLFW_RELEASE)
		{
			this->gameboy->OnInputReleased(BUTTON_A);
		}
		else
		{
			this->gameboy->OnInputPressed(BUTTON_A);
		}

		if (glfwGetKey(this->window, GLFW_KEY_X) == GLFW_RELEASE)
		{
			this->gameboy->OnInputReleased(BUTTON_B);
		}
		else
		{
			this->gameboy->OnInputPressed(BUTTON_B);
		}

		if (glfwGetKey(this->window, GLFW_KEY_C) == GLFW_RELEASE)
		{
			this->gameboy->OnInputReleased(BUTTON_SELECT);
		}
		else
		{
			this->gameboy->OnInputPressed(BUTTON_SELECT);
		}

		if (glfwGetKey(this->window, GLFW_KEY_V) == GLFW_RELEASE)
		{
			this->gameboy->OnInputReleased(BUTTON_START);
		}
		else
		{
			this->gameboy->OnInputPressed(BUTTON_START);
		}

		if (glfwGetKey(this->window, GLFW_KEY_UP) == GLFW_RELEASE)
		{
			this->gameboy->OnInputReleased(DPAD_UP);
		}
		else
		{
			this->gameboy->OnInputPressed(DPAD_UP);
		}

		if (glfwGetKey(this->window, GLFW_KEY_DOWN) == GLFW_RELEASE)
		{
			this->gameboy->OnInputReleased(DPAD_DOWN);
		}
		else
		{
			this->gameboy->OnInputPressed(DPAD_DOWN);
		}

		if (glfwGetKey(this->window, GLFW_KEY_LEFT) == GLFW_RELEASE)
		{
			this->gameboy->OnInputReleased(DPAD_LEFT);
		}
		else
		{
			this->gameboy->OnInputPressed(DPAD_LEFT);
		}

		if (glfwGetKey(this->window, GLFW_KEY_RIGHT) == GLFW_RELEASE)
		{
			this->gameboy->OnInputReleased(DPAD_RIGHT);
		}
		else
		{
			this->gameboy->OnInputPressed(DPAD_RIGHT);
		}

		if(paused == false)
		{
			uint32_t cycle_count = 4194304 / 4;
			if (dt < 1.0)
			{
				cycle_count *= dt;
			}
			for(int i = 0; i < cycle_count; i++)
			{
				this->gameboy->Update(dt);
				
				for(int i = 0; i < this->breakpoints.size(); i++)
				{
					if(this->gameboy->cpu->registers.PC == this->breakpoints[i])
					{
						paused = true;
						break;
					}
				}
			}
		}

		if (this->gameboy->ppu->requested_vram_debug_update)
		{
			this->gameboy->ppu->requested_vram_debug_update = false;
			this->renderer->RenderVRAMDebug(this->gameboy, this->vram_render_info);
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		this->renderer->Render(this->gameboy);

		this->RenderGUI();

		glfwSwapBuffers(this->window);
		glfwPollEvents();
	}
}

void Application::ConfigureImGui()
{
	// Setup DearImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad controls

	// Setup style
	ImGui::StyleColorsDark();

	// Configure backends
	ImGui_ImplGlfw_InitForOpenGL(this->window, true);
	ImGui_ImplOpenGL3_Init("#version 430 core");
}

inline bool DrawTabButton(const char* label, bool selected)
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 itemSpacing = style.ItemSpacing;
	ImVec4 color = style.Colors[ImGuiCol_Button];
	ImVec4 colorActive = style.Colors[ImGuiCol_ButtonActive];
	ImVec4 colorHover = style.Colors[ImGuiCol_ButtonHovered];
	style.ItemSpacing.x = 0;

	// push the style
	if (selected)
	{
		style.Colors[ImGuiCol_Button] = colorActive;
		style.Colors[ImGuiCol_ButtonActive] = colorActive;
		style.Colors[ImGuiCol_ButtonHovered] = colorActive;
	}
	else
	{
		style.Colors[ImGuiCol_Button] = color;
		style.Colors[ImGuiCol_ButtonActive] = colorActive;
		style.Colors[ImGuiCol_ButtonHovered] = colorHover;
	}

	// Draw the button
	bool result = ImGui::Button(label);

	// Restore the style
	style.Colors[ImGuiCol_Button] = color;
	style.Colors[ImGuiCol_ButtonActive] = colorActive;
	style.Colors[ImGuiCol_ButtonHovered] = colorHover;
	style.ItemSpacing = itemSpacing;

	return result;
}

void Application::RenderGUI()
{
	// Begin imgui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// ImGui commands here
	if(ImGui::BeginMainMenuBar()) 
	{
		if(ImGui::BeginMenu("File")) 
		{
			if(ImGui::MenuItem("Open ROM")) 
			{
				// Open file dialog
				auto f = pfd::open_file("Choose rom", pfd::path::home(),
				{ "GB Files (.gb)", "*.gb",
				  "All Files", "*" },
				pfd::opt::none);
				
				std::vector<std::string> files = f.result();
				if(!files.empty())
				{
					std::string rom_path = files[0];
					std::cout << "Opening ROM: " << rom_path << std::endl;
					this->gameboy->LoadROM(rom_path);
				}
			}

			if(ImGui::MenuItem("Save State"))
			{
				// TODO
			}

			if(ImGui::MenuItem("Load State"))
			{
				// TODO
			}

			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("Debug"))
		{
			ImGui::MenuItem("Toggle CPU debug", nullptr, &show_cpu_debug);
			ImGui::MenuItem("Toggle Disassembly", nullptr, &show_disassembly);
			ImGui::MenuItem("Toggle Breakpoints", nullptr, &show_breakpoints);
			ImGui::MenuItem("Toggle VRAM view", nullptr, &show_vram_view);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if(show_vram_view)
	{
		if (ImGui::Begin("VRAM", &show_vram_view, ImGuiWindowFlags_HorizontalScrollbar))
		{
			ImGui::SliderFloat("Scale", &vram_debug_image_scale, 0.1f, 10.0f, "%.1f");

			ImGuiStyle& style = ImGui::GetStyle();
			ImVec2 itemSpacing = style.ItemSpacing;
			ImVec4 color = style.Colors[ImGuiCol_Button];
			ImVec4 colorActive = style.Colors[ImGuiCol_ButtonActive];
			ImVec4 colorHover = style.Colors[ImGuiCol_ButtonHovered];

			if (DrawTabButton("Background", this->vram_render_info.render_bg))
			{
				this->vram_render_info.render_bg = true;
				this->vram_render_info.render_oam = false;
				this->vram_render_info.render_tiles = false;
			}

			ImGui::SameLine();

			if (DrawTabButton("Tiles", this->vram_render_info.render_tiles))
			{
				this->vram_render_info.render_bg = false;
				this->vram_render_info.render_oam = false;
				this->vram_render_info.render_tiles = true;
			}

			ImGui::SameLine();

			if (DrawTabButton("OAM", this->vram_render_info.render_oam))
			{
				this->vram_render_info.render_bg = false;
				this->vram_render_info.render_oam = true;
				this->vram_render_info.render_tiles = false;
			}

			ImGui::Separator();

			static int vram_debug_tab = 0;

			ImVec2 win_size = ImGui::GetWindowSize();
			win_size.x -= 32;

			if (this->vram_render_info.render_bg)
			{
				ImGui::Text("Map address:");

				if (ImGui::RadioButton("0x9800", this->vram_render_info.map_use_window_address == false))
				{
					this->vram_render_info.map_use_window_address = false;
				}

				ImGui::SameLine();

				if (ImGui::RadioButton("0x9C00", this->vram_render_info.map_use_window_address))
				{
					this->vram_render_info.map_use_window_address = true;
				}

				ImGui::Text("Tile address:");
				
				if (ImGui::RadioButton("0x8000", this->vram_render_info.map_use_8000_tile_address))
				{
					this->vram_render_info.map_use_8000_tile_address = true;
				}

				ImGui::SameLine();

				if (ImGui::RadioButton("0x8800", this->vram_render_info.map_use_8000_tile_address == false))
				{
					this->vram_render_info.map_use_8000_tile_address = false;
				}

				ImGui::Checkbox("Show scroll view", &this->vram_render_info.show_bg_scroll_view);

				ImGui::Image((ImTextureID)(intptr_t)this->renderer->background_vram_texture, ImVec2(256 * vram_debug_image_scale, 256 * vram_debug_image_scale));
			}
			else if (this->vram_render_info.render_tiles)
			{
				ImGui::Image((ImTextureID)(intptr_t)this->renderer->tiles_vram_texture, ImVec2(128 * vram_debug_image_scale, 192 * vram_debug_image_scale));
			}
			else if (this->vram_render_info.render_oam)
			{
				for (int y = 0; y < 5; y++)
				{
					for (int x = 0; x < 8; x++)
					{
						int i = x + y * 8;
						ImGui::Image((ImTextureID)(intptr_t)this->renderer->oam_textures[i], ImVec2(16 * vram_debug_image_scale, 32 * vram_debug_image_scale));
						ImGui::SameLine();
						ImGui::Text("%d", i);

						if (x != 7)
						{
							ImGui::SameLine(0.0, 16.0);
						}
					}
				}
			}

			ImGui::End();
		}
	}

	if(show_cpu_debug)
	{
		ImGui::Begin("CPU Debug Viewer", &show_cpu_debug);
		ImGui::Text("FPS: %.1f", 1.0f / this->dt);
		ImGui::Text("Registers");
		ImGui::Text("AF: 0x%04X", gameboy->cpu->registers.AF);
		ImGui::Text("BC: 0x%04X", gameboy->cpu->registers.BC);
		ImGui::Text("DE: 0x%04X", gameboy->cpu->registers.DE);
		ImGui::Text("HL: 0x%04X", gameboy->cpu->registers.HL);
		ImGui::Text("SP: 0x%04X", gameboy->cpu->registers.SP);
		ImGui::Text("PC: 0x%04X", gameboy->cpu->registers.PC);
		ImGui::Text("Z: %u | N: %u | H: %u | C: %u", 
			gameboy->cpu->get_zero_flag(), 
			gameboy->cpu->get_subtraction_flag(), 
			gameboy->cpu->get_half_carry_flag(), 
			gameboy->cpu->get_carry_flag());

		uint8_t IE = this->gameboy->mmu->Read(0xFFFF);
		uint8_t IF = this->gameboy->mmu->Read(0xFF0F);

		ImGui::Text("IE: 0x%02X", IE);
		ImGui::Text("IF: 0x%02X", IF);
		if (this->gameboy->cpu->halted)
		{
			ImGui::Text("HALTED: true");
		}
		else
		{
			ImGui::Text("HALTED: false");
		}
		
		ImGui::Text("TIMA: %u", gameboy->mmu->Read(0xFF05));
		ImGui::Text("DIV: %u", gameboy->mmu->Read(0xFF04));

		ImGui::End();
	}
	
	if(show_disassembly)
	{
		ImGui::Begin("Disassembly");

		if(ImGui::Button("Cycle"))
		{
			int c = this->gameboy->cpu->internal_clock + 1;
			for(int i = 0; i < c; i++)
			{
				this->gameboy->Update(0.0);
			}
		}

		ImGui::SameLine();

		if(paused)
		{
			if(ImGui::Button("Unpause"))
			{
				paused = !paused;
			}
		}
		else
		{
			if(ImGui::Button("Pause"))
			{
				paused = !paused;
			}
		}

		ImGui::Separator();
		ImGui::ListBoxHeader("Opcodes");

		uint32_t i = 0;
		while(i <= 0xFFFF)
		{
			uint8_t op = this->gameboy->mmu->Read(i);
			std::string& str = opcode_names[op];
			uint8_t len = opcode_lengths[op];
			if(op == 0xCB)
			{
				op = this->gameboy->mmu->Read(i + 1);
				str = opcode_cb_names[op];
			}

			std::stringstream stream;
			stream << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << i << " #" << (uint32_t)len << " " << str;

			ImGui::Selectable(stream.str().c_str(), i == this->gameboy->cpu->registers.PC);
			
			i += len;
		}

		ImGui::ListBoxFooter();

		ImGui::End();
	}

	if(show_breakpoints)
	{
		ImGui::Begin("Breakpoints");

		ImGui::ListBoxHeader("Addresses");
		
		for(int i = 0; i < this->breakpoints.size(); i++)
		{
			if(i == this->current_breakpoint_item)
			{
				ImGui::Text("*0x%04X", this->breakpoints[i]);
			}
			else
			{
				ImGui::Text("0x%04X", this->breakpoints[i]);
			}
		}
		
		ImGui::EndListBox();

		if(ImGui::Button("Previous"))
		{
			this->current_breakpoint_item -= 1;
			if(this->current_breakpoint_item > this->breakpoints.size()-1)
			{
				this->current_breakpoint_item = 0;
			}
		}

		ImGui::SameLine();
		
		if(ImGui::Button("Next"))
		{
			this->current_breakpoint_item += 1;
			if(this->current_breakpoint_item > this->breakpoints.size()-1)
			{
				this->current_breakpoint_item = this->breakpoints.size() - 1;
			}
		}

		ImGui::InputText("Address", this->hex_val.data(), 5 * sizeof(char));

		if(ImGui::Button("Add"))
		{
			uint16_t val = 0;
			int mult = 1;

			for(int i = this->hex_val.size() - 1; i >= 0; i--)
			{
				int d = 0;
				if(this->hex_val[i] >= '0' && this->hex_val[i] <= '9')
				{
					d = std::clamp(this->hex_val[i] - '0', 0, 10);
				}
				else if(this->hex_val[i] >= 'A' && this->hex_val[i] <= 'F')
				{
					d = std::clamp(this->hex_val[i] - 'A', 0, 10) + 10;
				}
				
				val += d * mult;
				mult *= 0x10;
			}

			this->AddBreakpoint(val);
		}
		
		ImGui::SameLine();

		if(ImGui::Button("Remove") && this->breakpoints.size() > 0)
		{
			this->RemoveBreakpoint(this->current_breakpoint_item);
			this->current_breakpoint_item %= (this->breakpoints.size() - 1);
		}

		ImGui::End();
	}

	// Render GUI
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::AddBreakpoint(uint16_t address)
{
	this->breakpoints.push_back(address);
}

void Application::RemoveBreakpoint(uint16_t index)
{
	this->breakpoints.erase(this->breakpoints.begin() + index);
}