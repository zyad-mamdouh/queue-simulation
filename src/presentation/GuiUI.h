#pragma once
#include "../application/SimulationController.h"
#include "imgui.h"
#include <vector>

class GuiUI {
public:
    void run();

private:
    SimulationController controller;

    // UI state
    int  stepsToRun = 10;
    bool autoRun    = false;
    int  autoDelay  = 300; // ms between auto steps

    void initServers();

    // Panels
    void renderControlPanel(float width, float height);
    void renderMetricsPanel(float width, float height);
    void renderVisualScene(float width, float height);

    // Visual drawing helpers
    void drawPerson(ImDrawList* dl, ImVec2 center, ImU32 bodyColor, ImU32 headColor, int label, float scale);
    void drawServer(ImDrawList* dl, ImVec2 topLeft, float w, float h,
                    bool busy, int serverIdx, int custNum, int timeLeft);
    void drawArrow(ImDrawList* dl, ImVec2 from, ImVec2 to, ImU32 color, float thickness);

    // Log
    void addLog(const char* msg);
    static constexpr int LOG_CAPACITY = 200;
    char logLines[LOG_CAPACITY][128];
    int  logHead  = 0;
    int  logCount = 0;
};