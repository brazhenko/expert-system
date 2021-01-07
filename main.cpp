#include "Interpreter.h"


#include <iostream>
#include "ImNodes.h"
#include "ImNodesEz.h"

#include "little_imgui/imgui.h"
#include "little_imgui/imgui_impl_glfw.h"
#include "little_imgui/imgui_impl_opengl3.h"
#include <cstdio>
#include <cstring>
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
#include <unistd.h>

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

Interpreter interpreter;

char *addString(char*name, int scoreString, std::vector<char*> &v) {

	ImGui::Text("Put Rule %d", scoreString + 1);
	ImGui::SameLine(120);
	ImGui::InputText(name, v[scoreString], 100);
}

const char ColorMarkerStart = '{';
const char ColorMarkerEnd = '}';

bool ProcessInlineHexColor( const char* start, const char* end, ImVec4& color )
{
	const int hexCount = ( int )( end - start );
	if( hexCount == 6 || hexCount == 8 )
	{
		char hex[9];
		strncpy( hex, start, hexCount );
		hex[hexCount] = 0;

		unsigned int hexColor = 0;
		if( sscanf( hex, "%x", &hexColor ) > 0 )
		{
			color.x = static_cast< float >( ( hexColor & 0x00FF0000 ) >> 16 ) / 255.0f;
			color.y = static_cast< float >( ( hexColor & 0x0000FF00 ) >> 8  ) / 255.0f;
			color.z = static_cast< float >( ( hexColor & 0x000000FF )       ) / 255.0f;
			color.w = 1.0f;

			if( hexCount == 8 )
			{
				color.w = static_cast< float >( ( hexColor & 0xFF000000 ) >> 24 ) / 255.0f;
			}

			return true;
		}
	}

	return false;
}

void TextWithColors( const char* fmt, ... )
{
	char tempStr[4096];

	va_list argPtr;
	va_start( argPtr, fmt );
	vsnprintf( tempStr, sizeof( tempStr ), fmt, argPtr );
	va_end( argPtr );
	tempStr[sizeof( tempStr ) - 1] = '\0';

	bool pushedColorStyle = false;
	const char* textStart = tempStr;
	const char* textCur = tempStr;
	while( textCur < ( tempStr + sizeof( tempStr ) ) && *textCur != '\0' )
	{
		if( *textCur == ColorMarkerStart )
		{
			// Print accumulated text
			if( textCur != textStart )
			{
				ImGui::TextUnformatted( textStart, textCur );
				ImGui::SameLine( 0.0f, 0.0f );
			}

			// Process color code
			const char* colorStart = textCur + 1;
			do
			{
				++textCur;
			}
			while( *textCur != '\0' && *textCur != ColorMarkerEnd );

			// Change color
			if( pushedColorStyle )
			{
				ImGui::PopStyleColor();
				pushedColorStyle = false;
			}

			ImVec4 textColor;
			if( ProcessInlineHexColor( colorStart, textCur, textColor ) )
			{
				ImGui::PushStyleColor( ImGuiCol_Text, textColor );
				pushedColorStyle = true;
			}

			textStart = textCur + 1;
		}
		else if( *textCur == '\n' )
		{
			// Print accumulated text an go to next line
			ImGui::TextUnformatted( textStart, textCur );
			textStart = textCur + 1;
		}

		++textCur;
	}

	if( textCur != textStart )
	{
		ImGui::TextUnformatted( textStart, textCur );
	}
	else
	{
		ImGui::NewLine();
	}

	if( pushedColorStyle )
	{
		ImGui::PopStyleColor();
	}
}

enum Mode
{
	True,
	False,
	Undefined
};

enum class WorkMode
{
	Interactive = 0,
	File,
	Gui
};

struct
{
	WorkMode wm = WorkMode::Interactive;
	std::string filename;
} globalArgs;

static void argParse(int ac, char **av)
{
	if (ac == 1) return;

	for (int i = 1; i < ac; i++)
	{
		if (strcmp(av[i], "-h") == 0)
		{
			std::cerr << "Usage: " << av[0] << " [-h] (help) | [-g] (GUI) | [-f file] (filemode) " << std::endl;
			exit(0);
		}

		if (strcmp(av[i], "-g") == 0)
		{
			globalArgs.wm = WorkMode::Gui;
			return;
		}

		if (strcmp(av[i], "-f") == 0)
		{
			if (!av[i + 1])
			{
				std::cerr << "Usage: " << av[0] << " [-h] (help) | [-g] (GUI) | [-f file] (filemode) " << std::endl;
				exit(0);
			}
			globalArgs.wm = WorkMode::File;
			globalArgs.filename = av[i + 1];
			return;
		}

	}
}


int main(int ac, char **av) {
	argParse(ac, av);

	if (globalArgs.wm == WorkMode::Interactive)
	{
		try {
			interpreter.startInteractive();
		} catch (const std::exception &e) {
			std::cerr << e.what() << std::endl;
			return 0;
		}
	}
	else if (globalArgs.wm == WorkMode::File)
	{
		try {
			interpreter.startFile(globalArgs.filename);
		} catch (const std::exception &e) {
			std::cerr << e.what() << std::endl;
			return 0;
		}
	}
	else if (globalArgs.wm == WorkMode::Gui)
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
		GLFWwindow* window = glfwCreateWindow(900, 1000, "expert-system", NULL, NULL);
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
		ImNodes::CanvasState canvas;
//		imnodes::Initialize();
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
		auto true_vars = std::shared_ptr<char>(new char[101]);
		bool changeGraph = false;
		true_vars.get()[0] = '=';
		std::string lastError;

		int rows_count = 0;
		const int start_rows_count = 3;
		int value_count = 0;
		std::vector<char*> inputExpressions;
		std::vector<char*> Facts;
		std::vector<int> staticValueForFacts;
		ImVec2 windowSizeFormulas;



		ImVec4 clear_color = ImVec4(1.f, 0.55f, 0.60f, 1.00f);
		std::stringstream archive;



		// Main loop

		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			{
				if (ImGui::Begin("Facts", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
				{
					for (char c = 'A'; c <= 'Z'; c++)
					{
						const float colPos = 30.0f;
						ImGui::Text( "%c", c);
						ImGui::SameLine( colPos );
						if (interpreter.getValueByVarName(c) == expert_system::Value::True)
						{
							TextWithColors("{19a119}True");
						}
						else
						{
							TextWithColors("{ff3232}False");
						}

					}
				}
				ImGui::End();

			}


			{
				// Graph

				if (ImGui::Begin("Graph", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
				{
					ImNodes::BeginCanvas(&canvas);

					struct Node
					{
						ImVec2 pos{};
						bool selected{};
						ImNodes::Ez::SlotInfo inputs[1];
						ImNodes::Ez::SlotInfo outputs[1];
						std::string nodeTytle;
						expert_system::iExpertNode *id;
						std::string reducedVal;
					};


					static std::vector<Node> nnodes;
					if (changeGraph)
					{
						changeGraph = false;
						nnodes.clear();

						float ii = 50, jj = 50;
						for (const auto &el : interpreter.repeatDestroyer_)
						{
							nnodes.push_back({{ii, jj}, false, {{"Out", 1}}, {{"In", 1}}, el.first, el.second, el.second->to_reduced_string()});
							ii+=40;
							jj+=40;
						}

					}
					for (Node& node : nnodes)
					{
						if (ImNodes::Ez::BeginNode(node.id, node.nodeTytle.c_str(), &node.pos, &node.selected))
						{
							ImNodes::Ez::InputSlots(node.outputs, 1);
							ImNodes::Ez::OutputSlots(node.inputs, 1);
							ImGui::Text("%s", node.reducedVal.c_str());

							if (node.id->eval() == expert_system::Value::True)
							{
								TextWithColors("{19a119}True");
							}
							else
							{
								TextWithColors("{ff3232}False");
							}


							ImNodes::Ez::EndNode();
						}

						for (const auto &child: node.id->get_children())
						{
							ImNodes::Connection(child, "In", node.id, "Out");
							canvas.colors[4] = clear_color;
						}

					}

					ImNodes::EndCanvas();
				}
				ImGui::End();


			}

			// service panel window
			{
				static float f = 0.0f;
				static int counter = 0;
				ImGui::Begin("Rules");
				static bool HWstart = false;

				for (; rows_count < start_rows_count; rows_count++)
					inputExpressions.emplace_back(new char[100]);


				if (!HWstart || rows_count)
				{
					HWstart = true;
					windowSizeFormulas.x = 450;
					windowSizeFormulas.y = (float)(90 + 23 * rows_count);
					ImGui::SetWindowSize(windowSizeFormulas);
				}

				for (int i = 0; i < rows_count; i++){
					std::stringstream ss;
					ss << i + 1;
					std::string num = ss.str();
					std::string str = "##Rule " + num;
					addString((char*)str.c_str() ,i, inputExpressions);
				}

				ImGui::Text("Put True Facts");
				ImGui::SameLine(120);
				ImGui::InputText("##True Facts", true_vars.get() + 1, 100);

				if(ImGui::Button ("PRESS TO ADD"))
				{
					rows_count++;
					inputExpressions.emplace_back(new char[100]);
				}


				ImGui::SameLine(125);
				if(ImGui::Button ("PRESS TO DELETE"))
				{
					if (rows_count > 0)
					{
						rows_count--;
						delete[] inputExpressions.back();
						inputExpressions.pop_back();
					}
				}

				ImGui::SameLine(270);
				if(ImGui::Button ("SET RULES"))
				{
					std::stringstream  ss;

					for (auto &el : inputExpressions)
					{
						std::cout << el <<std::endl;
						ss << el << std::endl;
					}
					std::cout << true_vars.get() << std::endl;

					ss <<  true_vars.get() << std::endl;
					ss << "show" << std::endl;

					std::string rulesFileName = "/tmp/expert_system";
					std::ofstream myfile;
					myfile.open (rulesFileName);
					myfile << ss.str();
					myfile.close();

					try {
						extern FILE* yyin;
						fclose(yyin);
						interpreter.reset();
						interpreter.startFile(rulesFileName);
						changeGraph = true;
						lastError = "";
					} catch (const std::exception &e) {
						interpreter.reset();
						changeGraph = true;
						lastError = e.what();
					}
				}


				ImGui::Text("%s", lastError.c_str());

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
	else
		std::cerr << "Unknown error" << std::endl;
}

