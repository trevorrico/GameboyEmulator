#include "Application.h"

Application::Application()
{

}

Application::~Application()
{
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

	this->ConfigureImGui();

	this->gameboy = new GameBoy();
	return 0;
}

void Application::Run()
{
	float dt = 0.0;
	float lastTime = glfwGetTime();
	while(glfwWindowShouldClose(this->window) == false)
	{
		dt = glfwGetTime() - lastTime;
		lastTime = glfwGetTime();

		if(paused == false)
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

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

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
			ImGui::MenuItem("Toggle Breakpoints", nullptr, &show_breakpoints);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}	

	if(show_cpu_debug)
	{
		ImGui::Begin("CPU Debug Viewer", &show_cpu_debug);
		ImGui::Text("Registers");
		ImGui::Text("AF: 0x%04X", gameboy->cpu->registers.AF);
		ImGui::Text("BC: 0x%04X", gameboy->cpu->registers.BC);
		ImGui::Text("DE: 0x%04X", gameboy->cpu->registers.DE);
		ImGui::Text("HL: 0x%04X", gameboy->cpu->registers.HL);
		ImGui::Text("SP: 0x%04X", gameboy->cpu->registers.SP);
		ImGui::Text("PC: 0x%04X", gameboy->cpu->registers.PC);
		ImGui::Text("OP: 0x%02X", gameboy->mmu->Read(gameboy->cpu->registers.PC));
		ImGui::Text("Z: %u | N: %u | H: %u | C: %u", 
			gameboy->cpu->get_zero_flag(), 
			gameboy->cpu->get_subtraction_flag(), 
			gameboy->cpu->get_half_carry_flag(), 
			gameboy->cpu->get_carry_flag());
		
		ImGui::Text("Cycle: %u", gameboy->cpu->cycles);

		if(ImGui::Button("Cycle"))
		{
			this->gameboy->Update(0.0);
		}
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
			if(this->current_breakpoint_item < 0)
			{
				this->current_breakpoint_item = this->breakpoints.size()-1;
			}
		}

		ImGui::SameLine();
		
		if(ImGui::Button("Next"))
		{
			this->current_breakpoint_item += 1;
			this->current_breakpoint_item % this->breakpoints.size();
		}

		ImGui::SliderInt("Address", (int*)&this->hex_val, 0, 0xFFFF, "0x%04x");

		if(ImGui::Button("Add"))
		{
			this->AddBreakpoint(this->hex_val);
		}
		
		ImGui::SameLine();

		if(ImGui::Button("Remove"))
		{
			this->RemoveBreakpoint(this->current_breakpoint_item);
			this->current_breakpoint_item % this->breakpoints.size();
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