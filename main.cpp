#include <iostream>


int yyparse();
extern FILE * yyin;


#include "little_imgui/imgui.h"
#include "little_imgui/imgui_impl_glfw.h"
#include "little_imgui/imgui_impl_opengl3.h"
#include <cstdio>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#include <GLFW/glfw3.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>


static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int ac, char **av) {
	yyin = stdin;
	yyparse();

	bool gui = false;
	if (gui)
	{
		// Setup window
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit())
			return 1;

		// GL 3.2 + GLSL 150
		const char* glsl_version = "#version 150";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac


		// Create window with graphics context
		GLFWwindow* window = glfwCreateWindow(1280, 720, "AllocVisualizer", NULL, NULL);
		if (window == nullptr)
			return 1;
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); // Enable vsync

		// Initialize OpenGL loader

#if defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
		bool err = glewInit() != GLEW_OK;
#endif
		if (err)
		{
			fprintf(stderr, "Failed to initialize OpenGL loader!\n");
			return 1;
		}

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		// Our state
		bool show_memory_window = false;
		bool show_tiny_clusters = false, show_small_clusters = false, show_huge_clusters = false;


		std::vector<ImColor>	colorBoxes;

		ImColor Cluster{0xFF, 0xFF, 0x00};
		ImColor Block{0x33,0x30, 0x86};
		ImColor FreeCell{0xC9, 0xF7, 0x6F};
		ImColor Usedcell{0xFF, 0x45, 0x40};

		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		std::stringstream archive;

		// Main loop
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// service panel window
			{
				static float f = 0.0f;
				static int counter = 0;
				ImGui::Begin("Service Panel");
				static bool HWstart = false;
				if (!HWstart)
				{
					HWstart = true;
					ImGui::SetWindowSize({350, 300});
				}
				// Config boxes
				ImGui::Checkbox("memory Window", &show_memory_window);
				ImGui::Checkbox("Tiny Clusters", &show_tiny_clusters);
				ImGui::Checkbox("Small Clusters", &show_small_clusters);
				ImGui::Checkbox("Huge Clusters", &show_huge_clusters);

				if (ImGui::Button(">>"))
				{
					colorBoxes.clear();

				}
				ImDrawList* draw_list = ImGui::GetWindowDrawList();

				const ImVec2 p = ImGui::GetCursorScreenPos();
				draw_list->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + 20, p.y + 20), Cluster);
				ImGui::Text("   Start of memory zone");

				const ImVec2 p1 = ImGui::GetCursorScreenPos();
				draw_list->AddRectFilled(ImVec2(p1.x, p1.y), ImVec2(p1.x + 20, p1.y + 20), Block);
				ImGui::Text("   Start of memory block");

				const ImVec2 p2 = ImGui::GetCursorScreenPos();
				draw_list->AddRectFilled(ImVec2(p2.x, p2.y), ImVec2(p2.x + 20, p2.y + 20), FreeCell);
				ImGui::Text("   Free memory cells");

				const ImVec2 p3 = ImGui::GetCursorScreenPos();
				draw_list->AddRectFilled(ImVec2(p3.x, p3.y), ImVec2(p3.x + 20, p3.y + 20), Usedcell);
				ImGui::Text("   Used memory cells");

				ImGui::Text("\nApplication average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

				ImGui::End();
			}

			if (show_memory_window)
			{
				ImGui::Begin("Memory");
				static bool SMstart = false;
				if (!SMstart)
				{
					SMstart = true;
					ImGui::SetWindowSize({870, 620});
				}

				static ImVec4 colf = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
				const ImU32 col = ImColor(colf);
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				const ImVec2 p = ImGui::GetCursorScreenPos();

				float i = p.x;
				float j = p.y;
				for (auto & colorBoxe : colorBoxes)
				{
					if (p.x + ImGui::GetWindowSize().x < i + 10)
					{
						i = p.x;
						j += 20;
					}
					draw_list->AddRectFilled(ImVec2(i, j), ImVec2(i + 20, j + 20), colorBoxe);
					i += 20;
				}
				ImGui::End();
			}

			// Rendering
			ImGui::Render();
			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(window);
		}

		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwDestroyWindow(window);
		glfwTerminate();
	}
}