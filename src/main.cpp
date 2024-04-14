#include <iostream>

#include "glad.h"
#include <GLFW/glfw3.h>
#include <string>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_spectrum.h"
#include "imgui_impl_glfw.h"
#include "image.h"
#include "shapes.h"

#include "renderer.h"
#include "interval.h"

const Interval Interval::empty = Interval(+infinity, -infinity);
const Interval Interval::universe = Interval(-infinity, +infinity);

int main() {
    
    GLFWwindow* window;

    if (!glfwInit()) {
        return -1;
    }

    window = glfwCreateWindow(1200, 700, "Zeus", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Couldn't load OpenGL" << std::endl;
        glfwTerminate();
        return -1;
    }
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    io.ConfigDockingWithShift = true;
    

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    ImGui::Spectrum::StyleColorsSpectrum();
    ImGui::Spectrum::LoadFont(18);

    ImVec4 clear_color(0.45f, 0.55f, 0.60f, 1.00f);

    std::vector<Color3> pixels;
    bool show = false;

    while (!glfwWindowShouldClose(window)) {
        
        glfwPollEvents();
        
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", nullptr, window_flags);
        ImGui::PopStyleVar();

        ImGui::PopStyleVar(2);

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("GLAppDockspace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        ImGui::End();
        

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Project")) {
                }
                if (ImGui::MenuItem("Open", "Ctrl+O")) { 
                }
                if (ImGui::MenuItem("Save", "Ctrl+S")) {
                }
                if (ImGui::MenuItem("Save as..")) {
                }
                ImGui::EndMenu();
            }
                
            
            if(ImGui::BeginMenu("Edit"))
            ImGui::EndMenu();
            if(ImGui::BeginMenu("Scene"))
            ImGui::EndMenu();
            ImGui::EndMainMenuBar();
        }
        

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport");
        double camz;
        {
            ImGui::Begin("Framerate");
            double min = -10;
            double max = 10;
            
            ImGui::SliderScalar("Camera", ImGuiDataType_Double, &camz, &min, &max, "%.3lf");
            ImGui::Text("%.3f fps", ImGui::GetIO().Framerate);
            if(ImGui::Button("Show")) show = true;
            ImGui::End();
        }
        Renderer renderer(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
        
        renderer.world.add(make_shared<Shapes::Sphere>(Point3(0,-100.5,-1), 100));
        renderer.world.add(make_shared<Shapes::Sphere>(Point3(0,0,-1), 0.5));

        pixels = renderer.Render();
    
        ImGui::Image((void*)(intptr_t)image(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y, pixels), ImGui::GetContentRegionAvail());
        
        ImGui::End();
        ImGui::PopStyleVar();

        
        

        ImGui::EndFrame();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    
    return 0;
}