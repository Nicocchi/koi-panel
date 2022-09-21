#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <cstdlib>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <GLFW/glfw3native.h>

int seconds, minutes, hours;
int sec_prev = 0;
std::string timeStr;

// Set window size hints
void SetXWindowSizeHint(Display *dpy, Window window, int minWidth, int minHeight, int maxWidth, int maxHeight, int x, int y)
{
    XSizeHints hints = {};
    if (minWidth > 0 && minHeight > 0)
        hints.flags |= PMinSize | PPosition;
    if (maxWidth > 0 && maxHeight > 0)
        hints.flags |= PMaxSize | PPosition;

    hints.min_width = minWidth;
    hints.min_height = minHeight;
    hints.max_width = maxWidth;
    hints.max_height = maxHeight;
    hints.x = x;
    hints.y = y;

    XSetWMNormalHints(dpy, window, &hints);
}

// Set window properties
void SetXWindowProperties(Display *dpy, Window win)
{
    printf("[Change Property]\n");
    Atom win_type = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);
    long value = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DOCK", False);
    Atom prop_strut = XInternAtom(dpy, "_NET_WM_STRUT", False);
    Atom prop_strut_partial = XInternAtom(dpy, "_NET_WM_STRUT_PARTIAL", False);
    long insets[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    insets[2] = 30;
    insets[8] = 0;
    insets[9] = 1920;
    XChangeProperty(dpy, win, win_type, XA_ATOM, 32, PropModeReplace, (unsigned char *)&value, 1);
    XChangeProperty(dpy, win, prop_strut, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&insets, 4);
    XChangeProperty(dpy, win, prop_strut_partial, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&insets, 12);
}

static void glfwErrorCallback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// System time
void StartClock()
{
    // storing total seconds
    time_t total_seconds = time(0);

    // getting values of seconds, minutes and hours
    struct tm *ct = localtime(&total_seconds);

    seconds = ct->tm_sec;
    minutes = ct->tm_min;
    hours = ct->tm_hour;

    // converting it to 12 hour format
    if (hours >= 12)
    {
        timeStr = "PM";
    }
    else
    {
        timeStr = "AM";
    }

    hours = hours > 12 ? hours - 12 : hours;

    sec_prev = seconds;
}

int main(int argc, char *argv[])
{

    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
        return 1;
    
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  

    // Create window with graphics context
    // GLFWmonitor *primary = glfwGetPrimaryMonitor();
    // int count;
    // GLFWmonitor **monitors = glfwGetMonitors(&count);
    GLFWwindow *window = glfwCreateWindow(1920, 30, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
    glfwSetWindowPos(window, 0, 0);

    // Get X11 WID & set attrs
    Window win = glfwGetX11Window(window);
    Display *dpy = glfwGetX11Display();

    SetXWindowSizeHint(dpy, win, 1920, 30, 1920, 30, 0, 0);
    SetXWindowProperties(dpy, win);

    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

// Font
#include "IconsFontAwesome.h"
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImFont *font = io.Fonts->AddFontDefault();

    ImFontConfig config;
    config.MergeMode = true;
    // config.PixelSnapH = true;
    // config.GlyphMinAdvanceX = 13.0f; // Use ifyou want to make the icon monospaced
    static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImFont *font1 = io.Fonts->AddFontFromFileTTF("./assets/fonts/HackRegularNerdFontComplete.ttf", 16.0f);
    ImFont *font2 = io.Fonts->AddFontFromFileTTF("./assets/fonts/fa-regular-400.ttf", 16.0f, &config, icon_ranges);
    ImFont *font3 = io.Fonts->AddFontFromFileTTF("./assets/fonts/fa-solid-900.ttf", 16.0f, &config, icon_ranges);
    // io.Fonts->Build();

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowBorderSize = 0.0f;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // clock

        StartClock();

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        glfwSetWindowPos(window, 0, 0);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // int windowWidth = ImGui::GetWindowSize().x;
        // int windowHeight = ImGui::GetWindowSize().y;
        // int textWidth = ImGui::CalcTextSize("Sun 05:29").x;

        ImGui::SetNextWindowSize(ImVec2(1920, 30));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        if (ImGui::Begin("Hello", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar))
        {
            ImGui::PushFont(font2);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 0, 0)));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(255, 255, 255, 0)));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(234, 234, 234)));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(255, 255, 255, 0)));
            ImGui::SetCursorPosY(5);
            if (ImGui::Button(ICON_FA_BARS))
            {
            }
            ImGui::PopStyleColor(4);
            ImGui::PopFont();

            ImGui::SameLine();
            ImGui::SetCursorPos(ImVec2(1920 - 80, 5));
            ImGui::Text("%s%d:%s%d %s", hours < 10 ? "0" : "", hours, minutes < 10 ? "0" : "", minutes, timeStr.c_str());
            ImGui::SameLine();
        }
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
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

    return 0;
}
