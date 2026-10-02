#include <raylib.h>
#include <rlgl.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

namespace {

constexpr float kPi = 3.14159265359f;
constexpr int kSurfaceResolution = 34;

struct Mat2 {
    float xx;
    float xy;
    float yy;
};

struct CriticalPoint {
    Vector2 position;
    float value;
    float eigenvalue1;
    float eigenvalue2;
    const char* classification;
};

struct Landscape {
    const char* name;
    const char* formula;
    const char* gradientFormula;
    const char* hessianFormula;
    float xMin;
    float xMax;
    float yMin;
    float yMax;
    float zScale;
    float eta;
    int maxSteps;
    Vector2 start;
    std::function<float(float, float)> value;
    std::function<Vector2(float, float)> gradient;
    std::function<Mat2(float, float)> hessian;
    std::vector<CriticalPoint> criticalPoints;
};

float clampFloat(float value, float low, float high) {
    return std::max(low, std::min(value, high));
}

float length2(Vector2 v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

Vector2 add(Vector2 a, Vector2 b) {
    return {a.x + b.x, a.y + b.y};
}

Vector2 subtract(Vector2 a, Vector2 b) {
    return {a.x - b.x, a.y - b.y};
}

Vector2 scale(Vector2 value, float factor) {
    return {value.x * factor, value.y * factor};
}

float determinant(Mat2 h) {
    return h.xx * h.yy - h.xy * h.xy;
}

float worldHeight(const Landscape& landscape, float z) {
    return z / landscape.zScale;
}

Vector3 worldPoint(const Landscape& landscape, Vector2 point, float heightOffset = 0.0f) {
    return {point.x, worldHeight(landscape, landscape.value(point.x, point.y)) + heightOffset, point.y};
}

Color surfaceColor(const Landscape& landscape, float z) {
    const float normalized = clampFloat(0.5f + 0.5f * std::tanh(z / landscape.zScale), 0.0f, 1.0f);
    return ColorFromHSV(220.0f - 190.0f * normalized, 0.78f, 0.92f);
}

std::vector<Landscape> makeLandscapes() {
    std::vector<Landscape> landscapes;

    landscapes.push_back({
        "1  Rotated anisotropic quadratic",
        "f = 3x^2 + 2xy + 2y^2",
        "grad f = (6x + 2y, 2x + 4y)",
        "H = [[6,2],[2,4]]",
        -4.0f, 4.0f, -4.0f, 4.0f, 12.0f, 0.055f, 140, {3.0f, -3.0f},
        [](float x, float y) { return 3.0f * x * x + 2.0f * x * y + 2.0f * y * y; },
        [](float x, float y) { return Vector2{6.0f * x + 2.0f * y, 2.0f * x + 4.0f * y}; },
        [](float, float) { return Mat2{6.0f, 2.0f, 4.0f}; },
        {{{0.0f, 0.0f}, 0.0f, 2.7639f, 7.2361f, "local/global minimum"}}
    });

    landscapes.push_back({
        "2  Double well",
        "f = (x^2 - 1)^2 + y^2",
        "grad f = (4x(x^2 - 1), 2y)",
        "H = [[12x^2-4,0],[0,2]]",
        -2.5f, 2.5f, -2.5f, 2.5f, 6.0f, 0.035f, 140, {0.25f, 1.9f},
        [](float x, float y) { return (x * x - 1.0f) * (x * x - 1.0f) + y * y; },
        [](float x, float y) { return Vector2{4.0f * x * (x * x - 1.0f), 2.0f * y}; },
        [](float x, float) { return Mat2{12.0f * x * x - 4.0f, 0.0f, 2.0f}; },
        {
            {{-1.0f, 0.0f}, 0.0f, 2.0f, 8.0f, "local/global minimum"},
            {{1.0f, 0.0f}, 0.0f, 2.0f, 8.0f, "local/global minimum"},
            {{0.0f, 0.0f}, 1.0f, -4.0f, 2.0f, "saddle"}
        }
    });

    landscapes.push_back({
        "3  Gaussian/Lorentzian crater",
        "f = 2 - exp(-(x^4+y^2)) - 1/(x^2+y^4+1)",
        "grad f = (4x^3A+2xs^-2, 2yA+4y^3s^-2)",
        "at (0,0): H = [[2,0],[0,2]]",
        -2.4f, 2.4f, -2.4f, 2.4f, 1.2f, 0.035f, 140, {1.8f, 1.7f},
        [](float x, float y) {
            const float a = std::exp(-(x * x * x * x + y * y));
            const float s = x * x + y * y * y * y + 1.0f;
            return 2.0f - a - 1.0f / s;
        },
        [](float x, float y) {
            const float a = std::exp(-(x * x * x * x + y * y));
            const float s = x * x + y * y * y * y + 1.0f;
            return Vector2{4.0f * x * x * x * a + 2.0f * x / (s * s),
                           2.0f * y * a + 4.0f * y * y * y / (s * s)};
        },
        [](float x, float y) {
            const float x2 = x * x;
            const float x3 = x2 * x;
            const float x4 = x2 * x2;
            const float y2 = y * y;
            const float y3 = y2 * y;
            const float y4 = y2 * y2;
            const float a = std::exp(-(x4 + y2));
            const float s = x2 + y4 + 1.0f;
            const float s2 = s * s;
            const float s3 = s2 * s;
            return Mat2{
                12.0f * x2 * a - 16.0f * x4 * x2 * a + 2.0f / s2 - 8.0f * x2 / s3,
                -8.0f * x3 * y * a - 16.0f * x * y3 / s3,
                2.0f * a - 4.0f * y2 * a + 12.0f * y2 / s2 - 32.0f * y4 * y2 / s3
            };
        },
        {{{0.0f, 0.0f}, 0.0f, 2.0f, 2.0f, "local/global minimum"}}
    });

    landscapes.push_back({
        "4  Rosenbrock valley",
        "f = (1-x)^2 + 100(y-x^2)^2",
        "grad f = (2(x-1)-400x(y-x^2), 200(y-x^2))",
        "H = [[2-400y+1200x^2,-400x],[-400x,200]]",
        // Crop the display window around the valley. The formula grows very
        // quickly in the far corners, which otherwise creates camera-filling
        // walls that hide the characteristic Rosenbrock shape.
        -1.5f, 1.5f, -0.5f, 2.5f, 120.0f, 0.0008f, 20000, {-1.2f, 1.44f},
        [](float x, float y) { return (1.0f - x) * (1.0f - x) + 100.0f * (y - x * x) * (y - x * x); },
        [](float x, float y) { return Vector2{2.0f * (x - 1.0f) - 400.0f * x * (y - x * x), 200.0f * (y - x * x)}; },
        [](float x, float y) { return Mat2{2.0f - 400.0f * y + 1200.0f * x * x, -400.0f * x, 200.0f}; },
        {{{1.0f, 1.0f}, 0.0f, 0.3994f, 1001.6006f, "local/global minimum"}}
    });

    landscapes.push_back({
        "5  Himmelblau's function",
        "f = (x^2+y-11)^2 + (x+y^2-7)^2",
        "grad f = (4xa+2b, 2a+4yb), a=x^2+y-11, b=x+y^2-7",
        "H = [[12x^2+4y-42,4x+4y],[4x+4y,4x+12y^2-26]]",
        -5.0f, 5.0f, -5.0f, 5.0f, 260.0f, 0.0015f, 140, {-4.2f, 4.0f},
        [](float x, float y) {
            const float a = x * x + y - 11.0f;
            const float b = x + y * y - 7.0f;
            return a * a + b * b;
        },
        [](float x, float y) {
            const float a = x * x + y - 11.0f;
            const float b = x + y * y - 7.0f;
            return Vector2{4.0f * x * a + 2.0f * b, 2.0f * a + 4.0f * y * b};
        },
        [](float x, float y) {
            return Mat2{12.0f * x * x + 4.0f * y - 42.0f, 4.0f * x + 4.0f * y,
                        4.0f * x + 12.0f * y * y - 26.0f};
        },
        {
            {{3.000000f, 2.000000f}, 0.0f, 25.7157f, 82.2843f, "local/global minimum"},
            {{-2.805118f, 3.131313f}, 0.0f, 64.8404f, 80.5501f, "local/global minimum"},
            {{-3.779310f, -3.283186f}, 0.0f, 70.7144f, 133.7856f, "local/global minimum"},
            {{3.584428f, -1.848127f}, 0.0f, 28.6907f, 105.4189f, "local/global minimum"},
            {{-0.270845f, -0.923039f}, 181.6165f, -45.6052f, -16.0660f, "local maximum"},
            {{-3.073026f, -0.081353f}, 104.0152f, -39.6515f, 72.4352f, "saddle"},
            {{-0.127961f, -1.953715f}, 178.3372f, -50.6102f, 20.2841f, "saddle"},
            {{0.086678f, 2.884255f}, 67.7192f, -31.7066f, 75.5076f, "saddle"},
            {{3.385154f, 0.073852f}, 13.3119f, -14.1352f, 97.5479f, "saddle"}
        }
    });

    return landscapes;
}

Vector3 basePoint(Vector2 point, float height = -0.22f) {
    return {point.x, height, point.y};
}

void drawDomain(const Landscape& landscape) {
    const float y = -0.20f;
    const Vector3 a{landscape.xMin, y, landscape.yMin};
    const Vector3 b{landscape.xMax, y, landscape.yMin};
    const Vector3 c{landscape.xMax, y, landscape.yMax};
    const Vector3 d{landscape.xMin, y, landscape.yMax};
    const Color axis = Fade(LIGHTGRAY, 0.6f);
    DrawLine3D(a, b, axis);
    DrawLine3D(b, c, axis);
    DrawLine3D(c, d, axis);
    DrawLine3D(d, a, axis);

    for (int i = 1; i < 10; ++i) {
        const float tx = static_cast<float>(i) / 10.0f;
        const float x = landscape.xMin + tx * (landscape.xMax - landscape.xMin);
        DrawLine3D({x, y, landscape.yMin}, {x, y, landscape.yMax}, Fade(GRAY, 0.22f));
        const float ty = static_cast<float>(i) / 10.0f;
        const float z = landscape.yMin + ty * (landscape.yMax - landscape.yMin);
        DrawLine3D({landscape.xMin, y, z}, {landscape.xMax, y, z}, Fade(GRAY, 0.22f));
    }
    DrawLine3D({landscape.xMin, y, 0.0f}, {landscape.xMax, y, 0.0f}, Fade(RED, 0.75f));
    DrawLine3D({0.0f, y, landscape.yMin}, {0.0f, y, landscape.yMax}, Fade(GREEN, 0.75f));
}

void drawSurface(const Landscape& landscape) {
    for (int iy = 0; iy < kSurfaceResolution; ++iy) {
        const float ty0 = static_cast<float>(iy) / kSurfaceResolution;
        const float ty1 = static_cast<float>(iy + 1) / kSurfaceResolution;
        const float y0 = landscape.yMin + ty0 * (landscape.yMax - landscape.yMin);
        const float y1 = landscape.yMin + ty1 * (landscape.yMax - landscape.yMin);
        for (int ix = 0; ix < kSurfaceResolution; ++ix) {
            const float tx0 = static_cast<float>(ix) / kSurfaceResolution;
            const float tx1 = static_cast<float>(ix + 1) / kSurfaceResolution;
            const float x0 = landscape.xMin + tx0 * (landscape.xMax - landscape.xMin);
            const float x1 = landscape.xMin + tx1 * (landscape.xMax - landscape.xMin);
            const float z00 = landscape.value(x0, y0);
            const float z10 = landscape.value(x1, y0);
            const float z11 = landscape.value(x1, y1);
            const float z01 = landscape.value(x0, y1);
            const Color colorA = Fade(surfaceColor(landscape, (z00 + z10 + z11) / 3.0f), 0.73f);
            const Color colorB = Fade(surfaceColor(landscape, (z00 + z11 + z01) / 3.0f), 0.73f);
            DrawTriangle3D(worldPoint(landscape, {x0, y0}), worldPoint(landscape, {x1, y0}),
                           worldPoint(landscape, {x1, y1}), colorA);
            DrawTriangle3D(worldPoint(landscape, {x0, y0}), worldPoint(landscape, {x1, y1}),
                           worldPoint(landscape, {x0, y1}), colorB);
        }
    }
}

void drawContours(const Landscape& landscape) {
    constexpr int contourResolution = 28;
    const float maximum = landscape.value(landscape.xMax, landscape.yMax);
    const float levelMax = std::min(maximum, landscape.zScale * 1.8f);
    for (int levelIndex = 1; levelIndex <= 6; ++levelIndex) {
        const float level = levelMax * static_cast<float>(levelIndex) / 7.0f;
        const Color contourColor = Fade(ColorFromHSV(45.0f + levelIndex * 14.0f, 0.85f, 1.0f), 0.95f);
        for (int iy = 0; iy < contourResolution; ++iy) {
            const float y0 = landscape.yMin + (landscape.yMax - landscape.yMin) * iy / contourResolution;
            const float y1 = landscape.yMin + (landscape.yMax - landscape.yMin) * (iy + 1) / contourResolution;
            for (int ix = 0; ix < contourResolution; ++ix) {
                const float x0 = landscape.xMin + (landscape.xMax - landscape.xMin) * ix / contourResolution;
                const float x1 = landscape.xMin + (landscape.xMax - landscape.xMin) * (ix + 1) / contourResolution;
                const Vector2 points[4] = {{x0, y0}, {x1, y0}, {x1, y1}, {x0, y1}};
                const float values[4] = {landscape.value(x0, y0), landscape.value(x1, y0),
                                         landscape.value(x1, y1), landscape.value(x0, y1)};
                std::vector<Vector2> hits;
                for (int edge = 0; edge < 4; ++edge) {
                    const int next = (edge + 1) % 4;
                    if ((values[edge] < level && values[next] >= level) ||
                        (values[edge] >= level && values[next] < level)) {
                        const float denominator = values[next] - values[edge];
                        const float t = denominator == 0.0f ? 0.5f : (level - values[edge]) / denominator;
                        hits.push_back(add(points[edge], scale(subtract(points[next], points[edge]), t)));
                    }
                }
                for (size_t hit = 1; hit < hits.size(); hit += 2) {
                    DrawLine3D(basePoint(hits[hit - 1], -0.17f), basePoint(hits[hit], -0.17f), contourColor);
                }
            }
        }
    }
}

void drawVectorField(const Landscape& landscape) {
    constexpr int fieldResolution = 10;
    for (int iy = 1; iy < fieldResolution; ++iy) {
        const float y = landscape.yMin + (landscape.yMax - landscape.yMin) * iy / fieldResolution;
        for (int ix = 1; ix < fieldResolution; ++ix) {
            const float x = landscape.xMin + (landscape.xMax - landscape.xMin) * ix / fieldResolution;
            const Vector2 gradient = landscape.gradient(x, y);
            const float magnitude = length2(gradient);
            if (magnitude < 0.0001f || !std::isfinite(magnitude)) {
                continue;
            }
            const Vector2 direction = scale(gradient, 1.0f / magnitude);
            const float arrowLength = 0.28f * std::min(1.0f, 0.35f + 0.03f * std::log1p(magnitude));
            const Vector2 end = add(Vector2{x, y}, scale(direction, arrowLength));
            const Vector3 start3 = basePoint({x, y}, -0.12f);
            const Vector3 end3 = basePoint(end, -0.12f);
            const Color color = Fade(SKYBLUE, 0.88f);
            DrawLine3D(start3, end3, color);
            const Vector2 perpendicular{-direction.y, direction.x};
            const Vector2 headA = add(end, scale(add(scale(direction, -1.0f), perpendicular), 0.10f));
            const Vector2 headB = add(end, scale(add(scale(direction, -1.0f), scale(perpendicular, -1.0f)), 0.10f));
            DrawLine3D(end3, basePoint(headA, -0.12f), color);
            DrawLine3D(end3, basePoint(headB, -0.12f), color);
        }
    }
}

void drawCriticalPoints(const Landscape& landscape) {
    for (const CriticalPoint& point : landscape.criticalPoints) {
        Color color = ORANGE;
        if (std::string(point.classification).find("minimum") != std::string::npos) {
            color = LIME;
        } else if (std::string(point.classification).find("saddle") != std::string::npos) {
            color = MAGENTA;
        } else if (std::string(point.classification).find("maximum") != std::string::npos) {
            color = RED;
        }
        DrawSphere(worldPoint(landscape, point.position, 0.08f), 0.13f, color);
        DrawSphere(basePoint(point.position, -0.05f), 0.09f, Fade(color, 0.8f));
    }
}

void drawTrajectory(const Landscape& landscape, const std::vector<Vector2>& trajectory) {
    for (size_t i = 1; i < trajectory.size(); ++i) {
        DrawLine3D(worldPoint(landscape, trajectory[i - 1], 0.10f),
                   worldPoint(landscape, trajectory[i], 0.10f), GOLD);
        DrawLine3D(basePoint(trajectory[i - 1], -0.04f), basePoint(trajectory[i], -0.04f), GOLD);
    }
    if (!trajectory.empty()) {
        DrawSphere(worldPoint(landscape, trajectory.front(), 0.13f), 0.16f, YELLOW);
        DrawSphere(worldPoint(landscape, trajectory.back(), 0.15f), 0.14f, WHITE);
    }
}

void drawPanel(const Landscape& landscape, int index, bool paused, size_t iteration, float eta,
               float lastStepLength,
               int screenWidth, int screenHeight) {
    const int panelX = screenWidth - 350;
    DrawRectangle(panelX, 0, 350, screenHeight, Color{20, 24, 35, 255});
    DrawRectangle(panelX, 0, 5, screenHeight, SKYBLUE);
    DrawText("PHYS 500 / HOMEWORK 1-2", panelX + 18, 18, 18, RAYWHITE);
    DrawText(TextFormat("Landscape %d of 5", index + 1), panelX + 18, 48, 22, SKYBLUE);
    DrawText(landscape.name + 3, panelX + 18, 78, 17, GOLD);

    DrawText("FUNCTION", panelX + 18, 112, 12, GRAY);
    DrawText(landscape.formula, panelX + 18, 130, 14, RAYWHITE);
    DrawText("GRADIENT", panelX + 18, 158, 12, GRAY);
    DrawText(landscape.gradientFormula, panelX + 18, 176, 13, RAYWHITE);
    DrawText("HESSIAN", panelX + 18, 206, 12, GRAY);
    DrawText(landscape.hessianFormula, panelX + 18, 224, 12, RAYWHITE);

    DrawText("VISUAL LAYERS", panelX + 18, 264, 12, GRAY);
    DrawText("surface  /  contours  /  gradient field", panelX + 18, 282, 13, LIGHTGRAY);
    DrawText("critical points  /  descent trajectory", panelX + 18, 301, 13, LIGHTGRAY);

    DrawText("CRITICAL POINTS + SOC", panelX + 18, 335, 12, GRAY);
    int y = 356;
    for (size_t i = 0; i < landscape.criticalPoints.size(); ++i) {
        const CriticalPoint& point = landscape.criticalPoints[i];
        const Color color = std::string(point.classification).find("saddle") != std::string::npos
                                ? MAGENTA
                                : (std::string(point.classification).find("maximum") != std::string::npos ? RED : LIME);
        DrawText(TextFormat("%zu  (%+.2f,%+.2f) z=%+.2f", i + 1, point.position.x, point.position.y, point.value),
                 panelX + 18, y, 12, color);
        DrawText(TextFormat("eig: %+.2f, %+.2f  %s", point.eigenvalue1, point.eigenvalue2, point.classification),
                 panelX + 28, y + 14, 11, LIGHTGRAY);
        y += 35;
    }

    const int footerY = screenHeight - 116;
    DrawLine(panelX + 18, footerY - 12, screenWidth - 18, footerY - 12, DARKGRAY);
    DrawText(TextFormat("gradient descent: %zu / %d steps", iteration, landscape.maxSteps), panelX + 18, footerY, 13, GOLD);
    DrawText(TextFormat("eta = %.5f   [%s]", eta, paused ? "paused" : "running"), panelX + 18, footerY + 19, 13, RAYWHITE);
    DrawText(TextFormat("actual |delta| = %.5f", lastStepLength), panelX + 18, footerY + 35, 12, LIGHTGRAY);
    DrawText("1-5 / arrows: landscape    R: reset", panelX + 18, footerY + 47, 11, LIGHTGRAY);
    DrawText("Up/Down: eta    Space: pause", panelX + 18, footerY + 64, 11, LIGHTGRAY);
    DrawText("WASD: orbit    wheel: zoom    Esc: quit", panelX + 18, footerY + 81, 11, LIGHTGRAY);
}

void resetTrajectory(const Landscape& landscape, std::vector<Vector2>& trajectory) {
    trajectory.clear();
    trajectory.push_back(landscape.start);
}

void advanceTrajectory(const Landscape& landscape, std::vector<Vector2>& trajectory, float& stepLength) {
    if (trajectory.empty() || trajectory.size() >= static_cast<size_t>(landscape.maxSteps) + 1) {
        return;
    }
    const Vector2 current = trajectory.back();
    const Vector2 rawNext = subtract(current, scale(landscape.gradient(current.x, current.y), landscape.eta));
    if (!std::isfinite(rawNext.x) || !std::isfinite(rawNext.y)) {
        return;
    }
    const Vector2 next{clampFloat(rawNext.x, landscape.xMin, landscape.xMax),
                      clampFloat(rawNext.y, landscape.yMin, landscape.yMax)};
    stepLength = length2(subtract(next, current));
    trajectory.push_back(next);
}

}  // namespace

int main() {
    const int screenWidth = 1440;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "PHYS 500 - Gradient Descent Landscapes");
    SetTargetFPS(60);

    const std::vector<Landscape> landscapes = makeLandscapes();
    int landscapeIndex = 0;
    bool paused = false;
    float etaOffset = 0.0f;
    float lastStepLength = 0.0f;
    std::vector<Vector2> trajectory;
    resetTrajectory(landscapes[landscapeIndex], trajectory);

    float cameraYaw = 0.78f;
    float cameraPitch = 0.60f;
    float cameraDistance = 15.0f;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ONE)) landscapeIndex = 0;
        if (IsKeyPressed(KEY_TWO)) landscapeIndex = 1;
        if (IsKeyPressed(KEY_THREE)) landscapeIndex = 2;
        if (IsKeyPressed(KEY_FOUR)) landscapeIndex = 3;
        if (IsKeyPressed(KEY_FIVE)) landscapeIndex = 4;
        if (IsKeyPressed(KEY_RIGHT)) landscapeIndex = (landscapeIndex + 1) % static_cast<int>(landscapes.size());
        if (IsKeyPressed(KEY_LEFT)) landscapeIndex = (landscapeIndex + landscapes.size() - 1) % landscapes.size();
        if (IsKeyPressed(KEY_SPACE)) paused = !paused;
        if (IsKeyPressed(KEY_R)) {
            resetTrajectory(landscapes[landscapeIndex], trajectory);
            lastStepLength = 0.0f;
        }
        if (IsKeyPressed(KEY_UP)) etaOffset += landscapes[landscapeIndex].eta * 0.10f;
        if (IsKeyPressed(KEY_DOWN)) etaOffset -= landscapes[landscapeIndex].eta * 0.10f;
        etaOffset = std::max(-0.8f * landscapes[landscapeIndex].eta,
                             std::min(2.0f * landscapes[landscapeIndex].eta, etaOffset));

        if (IsKeyPressed(KEY_W)) cameraPitch = clampFloat(cameraPitch + 0.06f, 0.18f, 1.25f);
        if (IsKeyPressed(KEY_S)) cameraPitch = clampFloat(cameraPitch - 0.06f, 0.18f, 1.25f);
        if (IsKeyPressed(KEY_A)) cameraYaw -= 0.08f;
        if (IsKeyPressed(KEY_D)) cameraYaw += 0.08f;
        cameraDistance = clampFloat(cameraDistance - GetMouseWheelMove() * 0.8f, 7.0f, 28.0f);

        static int previousLandscape = 0;
        if (previousLandscape != landscapeIndex) {
            resetTrajectory(landscapes[landscapeIndex], trajectory);
            lastStepLength = 0.0f;
            etaOffset = 0.0f;
            previousLandscape = landscapeIndex;
        }

        const Landscape& landscape = landscapes[landscapeIndex];
        const float eta = landscape.eta + etaOffset;
        Landscape activeLandscape = landscape;
        activeLandscape.eta = eta;
        if (!paused) {
            // Rosenbrock needs many iterations. Advance a few mathematical
            // steps per rendered frame so the long run remains practical,
            // while still drawing every intermediate point in the path.
            const int stepsThisFrame = activeLandscape.maxSteps > 1000 ? 4 : 1;
            for (int step = 0; step < stepsThisFrame; ++step) {
                advanceTrajectory(activeLandscape, trajectory, lastStepLength);
            }
        }

        const Vector3 target{0.0f, 0.35f, 0.0f};
        const Vector3 cameraPosition{
            std::cos(cameraYaw) * std::cos(cameraPitch) * cameraDistance,
            std::sin(cameraPitch) * cameraDistance,
            std::sin(cameraYaw) * std::cos(cameraPitch) * cameraDistance};
        Camera3D camera{};
        camera.position = cameraPosition;
        camera.target = target;
        camera.up = {0.0f, 1.0f, 0.0f};
        camera.fovy = 45.0f;
        camera.projection = CAMERA_PERSPECTIVE;

        BeginDrawing();
        ClearBackground(Color{10, 13, 21, 255});
        BeginMode3D(camera);
        rlDisableBackfaceCulling();
        drawDomain(landscape);
        drawSurface(landscape);
        drawContours(landscape);
        drawVectorField(landscape);
        drawCriticalPoints(landscape);
        drawTrajectory(activeLandscape, trajectory);
        //rlEnableBackfaceCulling();
        EndMode3D();

        DrawRectangle(0, 0, screenWidth - 350, 44, Fade(Color{8, 10, 18, 255}, 0.88f));
        DrawText("3D surface", 18, 14, 14, SKYBLUE);
        DrawText("contours", 130, 14, 14, GOLD);
        DrawText("gradient field", 235, 14, 14, LIGHTGRAY);
        DrawText("critical points", 375, 14, 14, LIME);
        DrawText("descent path", 515, 14, 14, YELLOW);
        drawPanel(landscape, landscapeIndex, paused, trajectory.size() - 1, eta, lastStepLength,
                  screenWidth, screenHeight);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
