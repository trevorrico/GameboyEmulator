#include "Application.h"

Application::Application()
{

}

Application::~Application()
{
	cpu.Shutdown();
	MemoryBus::Shutdown();

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

	MemoryBus::Initialize();
	cpu.Initialize();
	return 0;
}

void Application::Run()
{
	while(glfwWindowShouldClose(this->window) == false)
	{
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
					// TODO
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

		if(ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Toggle CPU debug viewer", nullptr, &show_cpu_debug);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}	

	if(show_cpu_debug)
	{
		ImGui::Begin("CPU Debug Viewer", &show_cpu_debug);
		ImGui::Text("Registers");
		ImGui::Text("AF: 0x%04X", cpu.registers.AF);
		ImGui::Text("BC: 0x%04X", cpu.registers.BC);
		ImGui::Text("DE: 0x%04X", cpu.registers.DE);
		ImGui::Text("HL: 0x%04X", cpu.registers.HL);
		ImGui::Text("SP: 0x%04X", cpu.registers.SP);
		ImGui::Text("PC: 0x%04X", cpu.registers.PC);
		ImGui::Text("Z: %u | N: %u | H: %u | C: %u", cpu.get_zero_flag(), cpu.get_subtraction_flag(), cpu.get_half_carry_flag(), cpu.get_carry_flag());
		ImGui::Text("Cycle: %u", cpu.cycles);
		ImGui::End();
	}

	// Render GUI
	ImGui::Render();
	int displayW, displayH;
	glfwGetFramebufferSize(window, &displayW, &displayH);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}