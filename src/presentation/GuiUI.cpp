// GuiUI.cpp  –  Visual Queue Simulation (Dear ImGui + SDL2 + OpenGL3)

#include "GuiUI.h"

#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"

#include <SDL2/SDL.h>
#include <GL/gl.h>

#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <chrono>

// ────────────────────────────────────────────────────────────────────────────
//  Color palette
// ────────────────────────────────────────────────────────────────────────────
static const ImU32 COL_BG_SCENE   = IM_COL32( 0,  17,  23, 255);
static const ImU32 COL_FLOOR      = IM_COL32( 30,  35,  45, 255);
static const ImU32 COL_QUEUE_LANE = IM_COL32( 40,  50,  65, 200);
static const ImU32 COL_ARROW      = IM_COL32( 90, 160, 255, 200);
static const ImU32 COL_PERSON_IDLE_BODY = IM_COL32( 70, 130, 220, 255);
static const ImU32 COL_PERSON_IDLE_HEAD = IM_COL32(140, 190, 255, 255);
static const ImU32 COL_PERSON_BUSY_BODY = IM_COL32(220, 150,  50, 255);
static const ImU32 COL_PERSON_BUSY_HEAD = IM_COL32(255, 210, 120, 255);
static const ImU32 COL_SERVER_IDLE = IM_COL32( 30,  55,  90, 255);
static const ImU32 COL_SERVER_BUSY = IM_COL32( 30,  80,  55, 255);
static const ImU32 COL_SERVER_BORDER_IDLE = IM_COL32( 60, 120, 200, 255);
static const ImU32 COL_SERVER_BORDER_BUSY = IM_COL32( 60, 200, 120, 255);
static const ImU32 COL_TEXT_BRIGHT = IM_COL32(230, 235, 245, 255);
static const ImU32 COL_TEXT_DIM    = IM_COL32(120, 130, 150, 255);
static const ImU32 COL_PROGRESS_BG = IM_COL32( 30,  35,  45, 255);
static const ImU32 COL_PROGRESS_FG = IM_COL32( 60, 200, 120, 255);

// ────────────────────────────────────────────────────────────────────────────
//  Helpers
// ────────────────────────────────────────────────────────────────────────────

void GuiUI::addLog(const char* msg)
{
    snprintf(logLines[logHead], 128, "%s", msg);
    logHead = (logHead + 1) % LOG_CAPACITY;
    if (logCount < LOG_CAPACITY) logCount++;
}

void GuiUI::initServers()
{
    controller.addServer(new Server());
    controller.addServer(new Server());
}

// Draw a stick-figure person centered at `center`
// scale=1 → head radius ~10, body height ~28
void GuiUI::drawPerson(ImDrawList* dl, ImVec2 center,
                        ImU32 bodyColor, ImU32 headColor,
                        int label, float scale)
{
    float hr  = 10.0f * scale;   // head radius
    float bh  = 28.0f * scale;   // body height
    float sh  = 14.0f * scale;   // shoulder half-width
    float lh  = 16.0f * scale;   // leg half-width
    float thick = 2.5f * scale;

    ImVec2 headC  = { center.x, center.y - bh * 0.5f - hr };
    ImVec2 neck   = { center.x, center.y - bh * 0.5f };
    ImVec2 waist  = { center.x, center.y + bh * 0.1f };
    ImVec2 feet   = { center.x, center.y + bh * 0.5f };

    // Shadow
    dl->AddCircleFilled({center.x, feet.y + 4.0f * scale}, hr * 0.9f,
                        IM_COL32(0,0,0,60), 20);

    // Head
    dl->AddCircleFilled(headC, hr, headColor, 32);
    dl->AddCircle(headC, hr, IM_COL32(0,0,0,80), 32, 1.5f);

    // Body
    dl->AddLine(neck, waist, bodyColor, thick);

    // Arms
    dl->AddLine({center.x - sh, center.y - bh * 0.3f},
                {center.x + sh, center.y - bh * 0.3f},
                bodyColor, thick);

    // Legs
    dl->AddLine(waist, {center.x - lh * 0.7f, feet.y}, bodyColor, thick);
    dl->AddLine(waist, {center.x + lh * 0.7f, feet.y}, bodyColor, thick);

    // Label inside head
    char buf[8]; snprintf(buf, sizeof(buf), "%d", label);
    ImVec2 ts = ImGui::CalcTextSize(buf);
    dl->AddText({headC.x - ts.x * 0.5f, headC.y - ts.y * 0.5f},
                IM_COL32(10,10,20,255), buf);
}

// Draw a server "desk/booth" rectangle
void GuiUI::drawServer(ImDrawList* dl, ImVec2 tl, float w, float h,
                        bool busy, int serverIdx, int custNum, int timeLeft)
{
    ImVec2 br = { tl.x + w, tl.y + h };
    ImU32 fillColor   = busy ? COL_SERVER_BUSY  : COL_SERVER_IDLE;
    ImU32 borderColor = busy ? COL_SERVER_BORDER_BUSY : COL_SERVER_BORDER_IDLE;

    // Desk body
    dl->AddRectFilled(tl, br, fillColor, 8.0f);
    dl->AddRect(tl, br, borderColor, 8.0f, 0, 2.5f);

    // Desk surface line
    float deskY = tl.y + h * 0.55f;
    dl->AddLine({tl.x + 6, deskY}, {br.x - 6, deskY}, borderColor, 1.5f);

    // Server label
    char sLabel[32];
    snprintf(sLabel, sizeof(sLabel), "Server %d", serverIdx + 1);
    ImVec2 ts = ImGui::CalcTextSize(sLabel);
    dl->AddText({ tl.x + (w - ts.x) * 0.5f, tl.y + 6.0f },
                COL_TEXT_BRIGHT, sLabel);

    if (busy) {
        // Draw person behind the desk (upper half of server box)
        ImVec2 personCenter = { tl.x + w * 0.5f, tl.y + h * 0.30f };
        drawPerson(dl, personCenter,
                   COL_PERSON_BUSY_BODY, COL_PERSON_BUSY_HEAD,
                   custNum, 0.85f);

        // Service time remaining bar
        float barX = tl.x + 8;
        float barW = w - 16;
        float barY = br.y - 18;
        float barH = 8;
        dl->AddRectFilled({barX, barY}, {barX + barW, barY + barH},
                          COL_PROGRESS_BG, 4.0f);
        // We don't know maxTime easily, show as animated shrinking bar
        // Use timeLeft as inverse proportion (cap at 10)
        float frac = 1.0f - (float)(timeLeft - 1) / 9.0f;
        if (frac < 0.05f) frac = 0.05f;
        if (frac > 1.00f) frac = 1.00f;
        dl->AddRectFilled({barX, barY}, {barX + barW * frac, barY + barH},
                          COL_PROGRESS_FG, 4.0f);

        char tBuf[16]; snprintf(tBuf, sizeof(tBuf), "t-%d", timeLeft);
        ImVec2 tts = ImGui::CalcTextSize(tBuf);
        dl->AddText({tl.x + (w - tts.x) * 0.5f, barY - 14},
                    COL_TEXT_DIM, tBuf);
    } else {
        // Idle indicator
        const char* idleTxt = "idle";
        ImVec2 its = ImGui::CalcTextSize(idleTxt);
        dl->AddText({ tl.x + (w - its.x) * 0.5f, tl.y + h * 0.42f },
                    COL_TEXT_DIM, idleTxt);
    }
}

// Draw an arrow with arrowhead
void GuiUI::drawArrow(ImDrawList* dl, ImVec2 from, ImVec2 to,
                       ImU32 color, float thickness)
{
    dl->AddLine(from, to, color, thickness);

    // Arrowhead
    float dx = to.x - from.x, dy = to.y - from.y;
    float len = sqrtf(dx*dx + dy*dy);
    if (len < 1.0f) return;
    dx /= len; dy /= len;
    float hs = 10.0f; // head size
    ImVec2 p1 = { to.x - hs * dx + hs * 0.5f * dy,
                  to.y - hs * dy - hs * 0.5f * dx };
    ImVec2 p2 = { to.x - hs * dx - hs * 0.5f * dy,
                  to.y - hs * dy + hs * 0.5f * dx };
    dl->AddTriangleFilled(to, p1, p2, color);
}

// ────────────────────────────────────────────────────────────────────────────
//  Visual scene  (the big canvas)
// ────────────────────────────────────────────────────────────────────────────

void GuiUI::renderVisualScene(float width, float height)
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.055f, 0.067f, 0.09f, 1.0f));
    ImGui::BeginChild("##scene", ImVec2(width, height), false,
                      ImGuiWindowFlags_NoScrollbar);

    ImVec2 origin = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    // Background
    dl->AddRectFilled(origin, {origin.x + width, origin.y + height},
                      COL_BG_SCENE);

    // ── Layout constants ──────────────────────────────────────────────────
    float margin      = 18.0f;
    float queueW      = width * 0.38f;
    float arrowZoneW  = width * 0.10f;
    float serversW    = width - queueW - arrowZoneW - margin * 2.0f;

    float queueX      = origin.x + margin;
    float arrowX      = queueX + queueW;
    float serversX    = arrowX + arrowZoneW;
    float sceneH      = height - margin * 2.0f;
    float midY        = origin.y + height * 0.5f;

    // ── Queue lane background ─────────────────────────────────────────────
    dl->AddRectFilled({queueX, origin.y + margin},
                      {queueX + queueW, origin.y + margin + sceneH},
                      COL_QUEUE_LANE, 10.0f);

    // Queue label
    const char* qLabel = "QUEUE";
    ImVec2 qls = ImGui::CalcTextSize(qLabel);
    dl->AddText({queueX + (queueW - qls.x) * 0.5f, origin.y + margin + 6},
                IM_COL32(100, 160, 255, 200), qLabel);

    // ── Draw queued people ────────────────────────────────────────────────
    const Queue& queue = controller.getQueue();
    std::vector<Customer*> snap = queue.getSnapshot();

    float personR    = 12.0f;
    float personStep = 52.0f;  // space between people
    float personY    = midY;

    // People stand in a row from right to left inside queue lane
    // First in queue = rightmost (closest to arrow/server)
    int maxVisible = (int)((queueW - 40.0f) / personStep);

    for (int i = 0; i < (int)snap.size() && i < maxVisible; i++) {
        Customer* c = snap[i];
        // i=0 is front of queue → rightmost
        float px = queueX + queueW - 30.0f - i * personStep;
        float py = personY;
        drawPerson(dl, {px, py},
                   COL_PERSON_IDLE_BODY, COL_PERSON_IDLE_HEAD,
                   c->getCustomerNumber(), 1.0f);
    }

    // If more people than visible, show "+N"
    if ((int)snap.size() > maxVisible) {
        char more[16];
        snprintf(more, sizeof(more), "+%d more", (int)snap.size() - maxVisible);
        dl->AddText({queueX + 6, personY - 8}, COL_TEXT_DIM, more);
    }

    // Empty queue text
    if (snap.empty()) {
        const char* empty = "empty";
        ImVec2 es = ImGui::CalcTextSize(empty);
        dl->AddText({queueX + (queueW - es.x) * 0.5f, midY - 8},
                    COL_TEXT_DIM, empty);
    }

    // Queue count badge
    {
        char cnt[16]; snprintf(cnt, sizeof(cnt), "%d waiting", (int)snap.size());
        ImVec2 cs = ImGui::CalcTextSize(cnt);
        float badgeX = queueX + (queueW - cs.x - 12) * 0.5f;
        float badgeY = origin.y + margin + sceneH - 28;
        dl->AddRectFilled({badgeX - 2, badgeY - 2},
                          {badgeX + cs.x + 10, badgeY + cs.y + 2},
                          IM_COL32(40, 60, 100, 200), 6.0f);
        dl->AddText({badgeX + 4, badgeY}, COL_TEXT_BRIGHT, cnt);
    }

    // ── Arrow from queue to servers ───────────────────────────────────────
    float arrowMidX = arrowX + arrowZoneW * 0.5f;
    drawArrow(dl,
              {arrowX + 4,              midY},
              {arrowX + arrowZoneW - 4, midY},
              COL_ARROW, 2.5f);

    // ── Servers ───────────────────────────────────────────────────────────
    const std::vector<Server*>& servers = controller.getServers().getServers();
    int   nServers  = (int)servers.size();
    float srvW      = serversW * 0.80f;
    float srvH      = (nServers == 1) ? sceneH * 0.55f
                                      : (sceneH - 20.0f * (nServers - 1)) / nServers;
    srvH = (srvH > 160.0f) ? 160.0f : srvH;
    float totalSrvH = srvH * nServers + 20.0f * (nServers - 1);
    float srvStartY = origin.y + (height - totalSrvH) * 0.5f;
    float srvX      = serversX + (serversW - srvW) * 0.5f;

    for (int i = 0; i < nServers; i++) {
        const Server* srv  = servers[i];
        Customer*     cust = srv->getCurrentCustomer();
        float sy = srvStartY + i * (srvH + 20.0f);

        // Arrow from queue arrow-end to each server
        drawArrow(dl,
                  {arrowX + arrowZoneW - 4, midY},
                  {srvX,                    sy + srvH * 0.5f},
                  IM_COL32(80, 130, 200, 120), 1.5f);

        drawServer(dl, {srvX, sy}, srvW, srvH,
                   cust != nullptr, i,
                   cust ? cust->getCustomerNumber() : 0,
                   cust ? cust->getTransactionTime() : 0);
    }

    // ── Time stamp ───────────────────────────────────────────────────────
    char timeBuf[32];
    snprintf(timeBuf, sizeof(timeBuf), "T = %d", controller.getCurrentTime());
    ImVec2 tts = ImGui::CalcTextSize(timeBuf);
    dl->AddText({origin.x + width - tts.x - 12, origin.y + margin + 6},
                IM_COL32(180, 190, 210, 180), timeBuf);

    ImGui::Dummy(ImVec2(width, height));
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// ────────────────────────────────────────────────────────────────────────────
//  Control panel  (left sidebar top)
// ────────────────────────────────────────────────────────────────────────────

void GuiUI::renderControlPanel(float width, float height)
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f, 0.12f, 0.16f, 1.0f));
    ImGui::BeginChild("##ctrl", ImVec2(width, height), true);

    ImGui::TextColored(ImVec4(0.4f, 0.75f, 1.0f, 1.0f), "CONTROLS");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Time:"); ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f),
                       "%d", controller.getCurrentTime());

    ImGui::Spacing();
    if (ImGui::Button("Run 1 Step", ImVec2(-1, 34))) {
        controller.runStep();
        char buf[64];
        snprintf(buf, sizeof(buf), "[T=%d] Step done.", controller.getCurrentTime());
        addLog(buf);
    }

    ImGui::Spacing();
    ImGui::SetNextItemWidth(-1);
    ImGui::SliderInt("##s", &stepsToRun, 1, 100, "Steps: %d");
    if (ImGui::Button("Run N Steps", ImVec2(-1, 34))) {
        for (int i = 0; i < stepsToRun; i++) controller.runStep();
        char buf[80];
        snprintf(buf, sizeof(buf), "[T=%d] Ran %d steps.",
                 controller.getCurrentTime(), stepsToRun);
        addLog(buf);
    }

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    if (autoRun) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.65f, 0.18f, 0.18f, 1.0f));
        if (ImGui::Button("Stop Auto-Run", ImVec2(-1, 34))) {
            autoRun = false; addLog("Auto-run stopped.");
        }
        ImGui::PopStyleColor();
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.55f, 0.30f, 1.0f));
        if (ImGui::Button("Start Auto-Run", ImVec2(-1, 34))) {
            autoRun = true; addLog("Auto-run started.");
        }
        ImGui::PopStyleColor();
    }
    ImGui::SetNextItemWidth(-1);
    ImGui::SliderInt("##d", &autoDelay, 100, 2000, "Delay: %d ms");

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// ────────────────────────────────────────────────────────────────────────────
//  Metrics panel  (left sidebar bottom)
// ────────────────────────────────────────────────────────────────────────────

void GuiUI::renderMetricsPanel(float width, float height)
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.11f, 0.15f, 1.0f));
    ImGui::BeginChild("##metrics", ImVec2(width, height), true);

    ImGui::TextColored(ImVec4(0.4f, 0.75f, 1.0f, 1.0f), "METRICS");
    ImGui::Separator();
    ImGui::Spacing();

    auto row = [](const char* label, const char* val) {
        ImGui::TextDisabled("%s", label);
        ImGui::SameLine(150);
        ImGui::TextColored(ImVec4(0.88f, 0.90f, 0.95f, 1.0f), "%s", val);
    };

    char buf[32];
    snprintf(buf,sizeof(buf),"%d",  controller.getTotalCustomersServed()); row("Served",   buf);
    snprintf(buf,sizeof(buf),"%lld",controller.getTotalWaitingTime());      row("Tot.Wait", buf);
    snprintf(buf,sizeof(buf),"%.2f",controller.getAverageWaitingTime());    row("Avg Wait", buf);
    snprintf(buf,sizeof(buf),"%.2f",controller.getAverageQueueLength());    row("Avg Q",    buf);

    double util = controller.getServerUtilization() * 100.0;
    snprintf(buf,sizeof(buf),"%.1f%%", util);
    row("Utilization", buf);

    ImGui::Spacing();
    ImGui::TextDisabled("Utilization");
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.24f, 0.78f, 0.47f, 1.0f));
    ImGui::ProgressBar((float)(util / 100.0), ImVec2(-1, 10), "");
    ImGui::PopStyleColor();

    // Log section inside metrics panel
    ImGui::Spacing(); ImGui::Separator();
    ImGui::TextColored(ImVec4(0.4f, 0.75f, 1.0f, 1.0f), "LOG");
    ImGui::Spacing();

    ImGui::BeginChild("##log_inner", ImVec2(0, 0), false);
    int start = (logCount == LOG_CAPACITY) ? logHead : 0;
    for (int i = 0; i < logCount; i++) {
        int idx = (start + i) % LOG_CAPACITY;
        ImGui::TextDisabled("%s", logLines[idx]);
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

// ────────────────────────────────────────────────────────────────────────────
//  Main run loop
// ────────────────────────────────────────────────────────────────────────────

void GuiUI::run()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window* window = SDL_CreateWindow(
        "Queue Simulation System",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1400, 820,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );
    SDL_GLContext gl_ctx = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_ctx);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0; style.FrameRounding = 4;
    style.WindowBorderSize = 0;

    ImVec4* c = style.Colors;
    c[ImGuiCol_WindowBg]      = ImVec4(0.07f, 0.08f, 0.11f, 1.0f);
    c[ImGuiCol_Button]        = ImVec4(0.18f, 0.40f, 0.75f, 0.80f);
    c[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.90f);
    c[ImGuiCol_ButtonActive]  = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    c[ImGuiCol_FrameBg]       = ImVec4(0.13f, 0.15f, 0.20f, 1.00f);
    c[ImGuiCol_SliderGrab]    = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

    ImGui_ImplSDL2_InitForOpenGL(window, gl_ctx);
    ImGui_ImplOpenGL3_Init("#version 330");

    initServers();
    addLog("Initialized. 2 servers ready.");

    using Clock = std::chrono::steady_clock;
    auto lastAuto = Clock::now();

    bool running = true;
    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            ImGui_ImplSDL2_ProcessEvent(&ev);
            if (ev.type == SDL_QUIT) running = false;
            if (ev.type == SDL_WINDOWEVENT &&
                ev.window.event == SDL_WINDOWEVENT_CLOSE &&
                ev.window.windowID == SDL_GetWindowID(window))
                running = false;
        }

        if (autoRun) {
            auto now = Clock::now();
            int ms = (int)std::chrono::duration_cast<std::chrono::milliseconds>(
                         now - lastAuto).count();
            if (ms >= autoDelay) {
                controller.runStep();
                char buf[64];
                snprintf(buf, sizeof(buf), "[T=%d] auto step.",
                         controller.getCurrentTime());
                addLog(buf);
                lastAuto = Clock::now();
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Full-screen window
        {
            ImGuiViewport* vp = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(vp->Pos);
            ImGui::SetNextWindowSize(vp->Size);
            ImGui::Begin("##root", nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize   | ImGuiWindowFlags_NoMove     |
                ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);

            float totalW  = ImGui::GetContentRegionAvail().x;
            float totalH  = ImGui::GetContentRegionAvail().y;
            float sideW   = 230.0f;
            float sceneW  = totalW - sideW - 8.0f;
            float ctrlH   = 260.0f;
            float metricsH = totalH - ctrlH - 8.0f;

            // Left sidebar
            ImGui::BeginChild("##sidebar", ImVec2(sideW, totalH), false);
            renderControlPanel(sideW - 4, ctrlH);
            ImGui::Spacing();
            renderMetricsPanel(sideW - 4, metricsH);
            ImGui::EndChild();

            ImGui::SameLine();

            // Main visual scene
            renderVisualScene(sceneW, totalH);

            ImGui::End();
        }

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.06f, 0.07f, 0.09f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_ctx);
    SDL_DestroyWindow(window);
    SDL_Quit();
}