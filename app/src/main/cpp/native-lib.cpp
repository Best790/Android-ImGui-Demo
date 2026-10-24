#include <jni.h>
#include <GLES3/gl3.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
#include "imgui.h"
#include "backends/imgui_impl_android.h"
#include "backends/imgui_impl_opengl3.h"

namespace {
int g_width = 1280, g_height = 720;
bool g_initialized = false;
bool g_menu = true;
bool g_box = true, g_skeleton = true, g_snaplines = false, g_distance = true;
float g_maxDistance = 250.0f;
float g_alpha = 0.92f;
int g_tab = 0;

struct TestObject {
    const char* name;
    float x, y;       // normalized screen position, demo only
    float distance;
    float health;
    bool active;
};
std::vector<TestObject> g_objects = {
    {"Test object A", 0.28f, 0.35f, 42.0f, 100.0f, true},
    {"Test object B", 0.62f, 0.43f, 86.0f, 72.0f, true},
    {"Test object C", 0.77f, 0.26f, 132.0f, 45.0f, true},
};

void DrawDemoESP() {
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    const ImVec2 display = ImGui::GetIO().DisplaySize;
    const ImU32 boxColor = IM_COL32(72, 235, 155, 240);
    const ImU32 boneColor = IM_COL32(255, 210, 90, 240);
    const ImU32 lineColor = IM_COL32(90, 185, 255, 210);
    const ImU32 textColor = IM_COL32(245, 248, 255, 255);

    for (const auto& o : g_objects) {
        if (!o.active || (g_distance && o.distance > g_maxDistance)) continue;
        const ImVec2 c(display.x * o.x, display.y * o.y);
        const float h = display.y * 0.22f * (42.0f / std::max(20.0f, o.distance));
        const float clampedH = std::clamp(h, 42.0f, display.y * 0.28f);
        const float w = clampedH * 0.46f;
        const ImVec2 top(c.x - w*0.5f, c.y - clampedH*0.5f);
        const ImVec2 bottom(c.x + w*0.5f, c.y + clampedH*0.5f);

        if (g_box) dl->AddRect(top, bottom, boxColor, 3.0f, 0, 2.0f);
        if (g_snaplines) dl->AddLine(ImVec2(display.x*0.5f, display.y-8.0f),
                                     ImVec2(c.x, bottom.y), lineColor, 1.5f);
        if (g_skeleton) {
            const ImVec2 head(c.x, top.y + clampedH*0.13f);
            const ImVec2 neck(c.x, top.y + clampedH*0.25f);
            const ImVec2 hip(c.x, top.y + clampedH*0.58f);
            const ImVec2 lShoulder(c.x-w*0.34f, top.y+clampedH*0.30f);
            const ImVec2 rShoulder(c.x+w*0.34f, top.y+clampedH*0.30f);
            const ImVec2 lHand(c.x-w*0.45f, top.y+clampedH*0.52f);
            const ImVec2 rHand(c.x+w*0.45f, top.y+clampedH*0.52f);
            const ImVec2 lKnee(c.x-w*0.20f, top.y+clampedH*0.78f);
            const ImVec2 rKnee(c.x+w*0.20f, top.y+clampedH*0.78f);
            const ImVec2 lFoot(c.x-w*0.22f, bottom.y);
            const ImVec2 rFoot(c.x+w*0.22f, bottom.y);
            dl->AddLine(head, neck, boneColor, 2.0f);
            dl->AddLine(neck, hip, boneColor, 2.0f);
            dl->AddLine(neck, lShoulder, boneColor, 2.0f);
            dl->AddLine(neck, rShoulder, boneColor, 2.0f);
            dl->AddLine(lShoulder, lHand, boneColor, 2.0f);
            dl->AddLine(rShoulder, rHand, boneColor, 2.0f);
            dl->AddLine(hip, lKnee, boneColor, 2.0f);
            dl->AddLine(hip, rKnee, boneColor, 2.0f);
            dl->AddLine(lKnee, lFoot, boneColor, 2.0f);
            dl->AddLine(rKnee, rFoot, boneColor, 2.0f);
            dl->AddCircleFilled(head, 3.5f, boneColor);
        }
        char label[128];
        std::snprintf(label, sizeof(label), "%s  %.0fm  HP %.0f",
                      o.name, o.distance, o.health);
        dl->AddText(ImVec2(top.x, top.y - 19.0f), textColor, label);
    }
}

void DrawMenu() {
    if (!g_menu) {
        ImGui::SetNextWindowPos(ImVec2(14,14), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.70f);
        ImGui::Begin("##showmenu", nullptr, ImGuiWindowFlags_AlwaysAutoResize |
                    ImGuiWindowFlags_NoTitleBar);
        if (ImGui::Button("OPEN MENU")) g_menu = true;
        ImGui::End();
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(360, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(g_alpha);
    ImGui::Begin("DEMO PANEL  /  IMGUI", &g_menu,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::BeginTabBar("MainTabs")) {
        if (ImGui::BeginTabItem("Visuals")) { g_tab = 0;
            ImGui::Checkbox("Box ESP (demo objects)", &g_box);
            ImGui::Checkbox("Skeleton (synthetic)", &g_skeleton);
            ImGui::Checkbox("Snaplines", &g_snaplines);
            ImGui::Checkbox("Distance filter", &g_distance);
            ImGui::SliderFloat("Max distance", &g_maxDistance, 10.0f, 300.0f, "%.0f m");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Objects")) { g_tab = 1;
            for (size_t i=0; i<g_objects.size(); ++i) {
                ImGui::PushID((int)i);
                ImGui::Checkbox(g_objects[i].name, &g_objects[i].active);
                ImGui::SameLine();
                ImGui::TextDisabled("%.0fm", g_objects[i].distance);
                ImGui::PopID();
            }
            ImGui::TextWrapped("These entities are synthetic placeholders inside this demo app.");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Settings")) { g_tab = 2;
            ImGui::SliderFloat("Menu opacity", &g_alpha, 0.45f, 1.0f, "%.2f");
            if (ImGui::Button("Reset settings")) {
                g_box=true; g_skeleton=true; g_snaplines=false; g_distance=true;
                g_maxDistance=250.0f; g_alpha=0.92f;
                for (auto& o:g_objects) o.active=true;
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::Separator();
    ImGui::TextDisabled("Standalone visual test • no game data");
    ImGui::End();
}
}

extern "C" JNIEXPORT void JNICALL
Java_dev_demo_imguioverlay_MainActivity_nativeInit(JNIEnv* env, jobject, jobject surface) {
    if (g_initialized) return;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 5.0f;
    style.GrabRounding = 5.0f;
    style.ScaleAllSizes(1.15f);
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    ImGui_ImplAndroid_Init(window);
    if (window) ANativeWindow_release(window);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    g_initialized = true;
}

extern "C" JNIEXPORT void JNICALL
Java_dev_demo_imguioverlay_MainActivity_nativeResize(JNIEnv*, jobject, jint w, jint h) {
    g_width = std::max(1, (int)w); g_height = std::max(1, (int)h);
    glViewport(0,0,g_width,g_height);
}

extern "C" JNIEXPORT void JNICALL
Java_dev_demo_imguioverlay_MainActivity_nativeRender(JNIEnv*, jobject) {
    glViewport(0,0,g_width,g_height);
    glClearColor(0.035f,0.045f,0.065f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (!g_initialized) return;
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)g_width,(float)g_height);
    io.DeltaTime = 1.0f/60.0f;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame();
    ImGui::NewFrame();
    DrawDemoESP();
    DrawMenu();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

extern "C" JNIEXPORT void JNICALL
Java_dev_demo_imguioverlay_MainActivity_nativeTouch(JNIEnv*, jobject, jint action, jfloat x, jfloat y) {
    if (!g_initialized) return;
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(x,y);
    // Android MotionEvent: DOWN=0, UP=1, MOVE=2, CANCEL=3
    if (action == 0) io.AddMouseButtonEvent(0,true);
    else if (action == 1 || action == 3) io.AddMouseButtonEvent(0,false);
}
