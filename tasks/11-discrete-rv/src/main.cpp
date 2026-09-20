#define IMGUI_APP_IMPL
#include "imgui_app.h"

#define DISCRETE_RANDOM_VARIABLE_IMPL
#include "discrete_random_variable.h"

#include <implot.h>

#include <algorithm>

#include <vector>

int main()
{
  DiscreteRandomVariable rv({
      {1, 0.2},
      {2, 0.5},
      {3, 0.3}
  });

  ImGuiApp::init("Discrete RV Viewer");

  ImGuiApp::run([&]()
  {
      auto pmf = rv.pmf();
      auto cdf = rv.cdf();

      std::vector<double> x;
      std::vector<double> y_pmf;
      std::vector<double> y_cdf;

      x.reserve(pmf.size());
      y_pmf.reserve(pmf.size());
      y_cdf.reserve(cdf.size());

      for (const auto& [v, p] : pmf)
      {
          x.push_back(v);
          y_pmf.push_back(p);
      }

      for (const auto& f : cdf | std::views::values)
      {
          y_cdf.push_back(f);
      }

      ImGui::Text("E[X] = %.6f", rv.expectation());
      ImGui::Text("D[X] = %.6f", rv.variance());
      ImGui::Text("Skewness = %.6f", rv.skewness());
      ImGui::Text("Kurtosis = %.6f", rv.kurtosis());

      ImGui::Separator();

      if (ImPlot::BeginPlot("PMF"))
      {
        ImPlot::PlotBars("P(X)", x.data(), y_pmf.data(), static_cast<int>(x.size()), 0.6);
          ImPlot::EndPlot();
      }

      if (ImPlot::BeginPlot("CDF"))
      {
          const int n = static_cast<int>(y_cdf.size());
          const double left =
              x.front() - 1.0 < 0.0 ? x.front() - 1.0 : -1.0;
          const double right = x.back() + 0.75;

          ImPlot::SetupAxesLimits(left, right, -0.15, 1.15, ImGuiCond_Always);

          ImDrawList* dl = ImPlot::GetPlotDrawList();
          const ImU32 color = ImGui::GetColorU32(ImGuiCol_PlotLines);

          const auto dashed_vertical = [dl, color](const ImVec2& a,
                                                   const ImVec2& b) {
              const float dash = 6.0f;
              const float gap = 4.0f;
              const float step = dash + gap;
              const float x_pix = a.x;
              const float y_lo = std::min(a.y, b.y);
              const float y_hi = std::max(a.y, b.y);
              for (float y = y_lo; y < y_hi; y += step) {
                  dl->AddLine(ImVec2(x_pix, y),
                              ImVec2(x_pix, std::min(y + dash, y_hi)),
                              color, 1.0f);
              }
          };

          const auto solid_horizontal = [dl, color](double x0, double y,
                                                    double x1) {
              dl->AddLine(ImPlot::PlotToPixels(x0, y),
                          ImPlot::PlotToPixels(x1, y), color, 2.0f);
          };

          const auto hollow_point = [dl, color](double x0, double y) {
              dl->AddCircle(ImPlot::PlotToPixels(x0, y), 5.0f, color, 32, 1.5f);
          };

          solid_horizontal(left, 0.0, x.front());

          for (int i = 0; i < n; i++) {
              const double prev = i == 0 ? 0.0 : y_cdf[i - 1];
              const double cur = y_cdf[i];
              const double next_x = i + 1 < n ? x[i + 1] : right;

              dashed_vertical(ImPlot::PlotToPixels(x[i], prev),
                              ImPlot::PlotToPixels(x[i], cur));
              hollow_point(x[i], cur);
              solid_horizontal(x[i], cur, next_x);
          }

          ImPlot::EndPlot();
      }

      if (ImPlot::BeginPlot("Polyline"))
      {
          ImPlot::PlotLine("distribution", x.data(), y_pmf.data(), static_cast<int>(x.size()));
          ImPlot::EndPlot();
      }
  });

  ImGuiApp::shutdown();
}