#include "functions.h"
#include <imgui.h>
#include "tinyfiledialogs.h"
#include <iostream>
#include <cmath>

extern ImageAnalysisResult currentAnalysis;
extern GLuint programID;
extern GLuint vao;
extern GLuint imageTexture;
extern cv::Mat currentImage;
extern std::vector<float> responseFunction;
extern std::string outputMessage;

void GenerateTestImage() {
    std::cout << "Generating test image..." << std::endl;
    currentImage = cv::Mat(500, 500, CV_8UC1, cv::Scalar(0));
    
    // Рисуем белый круг в центре
    cv::Point center(250, 250);
    int radius = 100;
    cv::circle(currentImage, center, radius, cv::Scalar(255), -1);
    
    // Применяем размытие по Гауссу для создания плавного перехода
    cv::GaussianBlur(currentImage, currentImage, cv::Size(5, 5), 2.0);
    
    UpdateImageTexture();
    outputMessage = "Test image with circle generated successfully";
}

void GenerateCustomCircle(int width, int height, int radius) {
    currentImage = cv::Mat(height, width, CV_8UC1, cv::Scalar(0));
    
    // Рисуем круг в центре изображения
    cv::Point center(width/2, height/2);
    cv::circle(currentImage, center, radius, cv::Scalar(255), -1);
    
    // Применяем размытие
    cv::GaussianBlur(currentImage, currentImage, cv::Size(5, 5), 2.0);
    
    UpdateImageTexture();
    outputMessage = "Custom circle generated";
}

void LoadImage() {
    const char* filepath = tinyfd_openFileDialog(
        "Choose an image",
        "",
        0,
        nullptr,
        nullptr,
        0
    );
    
    if (filepath) {
        currentImage = cv::imread(filepath, cv::IMREAD_GRAYSCALE);
        if (!currentImage.empty()) {
            UpdateImageTexture();
            outputMessage = "Image loaded successfully";
            CalculateResponseFunction();
        } else {
            outputMessage = "Failed to load image";
        }
    }
}

ImageAnalysisResult AnalyzeImage(const cv::Mat& image, const cv::Point& center, int radius) {
    ImageAnalysisResult result;
    result.centerX = center.x;
    result.centerY = center.y;
    result.radius = radius;
    
    // Анализ профиля края
    for (int r = -radius; r <= radius; ++r) {
        double sum = 0;
        int count = 0;
        
        for (int theta = 0; theta < 360; theta += 1) {
            double rad = theta * CV_PI / 180.0;
            int x = static_cast<int>(center.x + r * std::cos(rad));
            int y = static_cast<int>(center.y + r * std::sin(rad));
            
            if (x >= 0 && x < image.cols && y >= 0 && y < image.rows) {
                sum += static_cast<double>(image.at<uchar>(y, x));
                count++;
            }
        }
        
        if (count > 0) {
            result.edgeProfile.push_back(sum / static_cast<double>(count));
        }
    }
    
    // Анализ шума
    cv::Rect roiRect(
        std::max(0, center.x - radius/2),
        std::max(0, center.y - radius/2),
        std::min(radius, image.cols - center.x + radius/2),
        std::min(radius, image.rows - center.y + radius/2)
    );
    cv::Mat roi = image(roiRect);
    
    cv::Mat meanFiltered;
    cv::blur(roi, meanFiltered, cv::Size(3, 3));
    
    for (int y = 0; y < roi.rows; ++y) {
        double rowNoise = 0;
        for (int x = 0; x < roi.cols; ++x) {
            double diff = static_cast<double>(roi.at<uchar>(y, x)) - static_cast<double>(meanFiltered.at<uchar>(y, x));
            rowNoise += diff * diff;
        }
        result.noiseProfile.push_back(std::sqrt(rowNoise / static_cast<double>(roi.cols)));
    }
    
    // Расчет статистик
    cv::Scalar mean, stddev;
    cv::meanStdDev(roi, mean, stddev);
    
    result.signalMean = mean[0];
    result.noiseStd = stddev[0];
    result.cnr = (stddev[0] > 0) ? (mean[0] / stddev[0]) : 0;
    
    return result;
}

void CalculateResponseFunction() {
    if (currentImage.empty()) {
        outputMessage = "No image loaded";
        return;
    }
    
    cv::Mat edges;
    cv::Canny(currentImage, edges, 100, 200);
    
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(edges, circles, cv::HOUGH_GRADIENT, 1, edges.rows/8, 200, 100, 0, 0);
    
    if (circles.empty()) {
        outputMessage = "No circles detected";
        return;
    }
    
    cv::Vec3f circle = circles[0];
    cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
    int radius = cvRound(circle[2]);
    
    currentAnalysis = AnalyzeImage(currentImage, center, radius);
    
    responseFunction.clear();
    for (size_t i = 1; i < currentAnalysis.edgeProfile.size(); ++i) {
        responseFunction.push_back(static_cast<float>(
            currentAnalysis.edgeProfile[i] - currentAnalysis.edgeProfile[i-1]
        ));
    }
    
    outputMessage = "Analysis completed successfully";
}

void UpdateImageTexture() {
    if (currentImage.empty()) return;
    
    if (imageTexture != 0) {
        glDeleteTextures(1, &imageTexture);
    }
    
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, currentImage.cols, currentImage.rows,
                 0, GL_RED, GL_UNSIGNED_BYTE, currentImage.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void RenderImage() {
    if (currentImage.empty()) return;
    
    glUseProgram(programID);
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void RenderAnalysisWindows() {
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin("Analysis Results", nullptr, ImGuiWindowFlags_NoCollapse);
    
    if (ImGui::BeginTabBar("AnalysisTabs")) {
        if (ImGui::BeginTabItem("Edge Response")) {
            RenderEdgeProfile();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Noise Analysis")) {
            RenderNoiseProfile();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Statistics")) {
            RenderStatistics();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    
    ImGui::End();
}

void RenderEdgeProfile() {
    if (!responseFunction.empty()) {
        ImVec2 plotSize = ImVec2(700, 400);
        
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
        
        ImGui::PlotLines("##EdgeResponse",
                         responseFunction.data(),
                         static_cast<int>(responseFunction.size()),
                         0,
                         "Edge Response Function",
                         FLT_MAX,
                         FLT_MAX,
                         plotSize);
        
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        
        ImGui::Text("Distance from Edge (pixels)");
        ImGui::SameLine(plotSize.x - 100);
        ImGui::Text("Response");
    }
}

void RenderNoiseProfile() {
    if (!currentAnalysis.noiseProfile.empty()) {
        std::vector<float> noiseData;
        noiseData.reserve(currentAnalysis.noiseProfile.size());
        for (const auto& val : currentAnalysis.noiseProfile) {
            noiseData.push_back(static_cast<float>(val));
        }
        
        ImVec2 plotSize = ImVec2(700, 400);
        
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
        
        ImGui::PlotLines("##NoiseProfile",
                         noiseData.data(),
                         static_cast<int>(noiseData.size()),
                         0,
                         "Noise Profile",
                         FLT_MAX,
                         FLT_MAX,
                         plotSize);
        
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        
        ImGui::Text("Position (pixels)");
        ImGui::SameLine(plotSize.x - 100);
        ImGui::Text("Noise Level");
    }
}

void RenderStatistics() {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 20));
    
    ImGui::BeginChild("Statistics", ImVec2(0, 0), true);
    
    ImGui::Text("Signal Statistics:");
    ImGui::Separator();
    
    ImGui::Text("Mean Signal: %.2f", currentAnalysis.signalMean);
    ImGui::Text("Noise StdDev: %.2f", currentAnalysis.noiseStd);
    ImGui::Text("Contrast-to-Noise Ratio: %.2f", currentAnalysis.cnr);
    
    ImGui::Spacing();
    ImGui::Text("Region Information:");
    ImGui::Separator();
    
    ImGui::Text("Center Position: (%d, %d)", currentAnalysis.centerX, currentAnalysis.centerY);
    ImGui::Text("Circle Radius: %d pixels", currentAnalysis.radius);
    
    ImGui::EndChild();
    
    ImGui::PopStyleVar(2);
}

json GetAnalysisData() {
    json data;
    
    if (!currentAnalysis.edgeProfile.empty()) {
        data["edgeProfile"] = currentAnalysis.edgeProfile;
        data["noiseProfile"] = currentAnalysis.noiseProfile;
        data["signalMean"] = currentAnalysis.signalMean;
        data["noiseStd"] = currentAnalysis.noiseStd;
        data["cnr"] = currentAnalysis.cnr;
        data["centerX"] = currentAnalysis.centerX;
        data["centerY"] = currentAnalysis.centerY;
        data["radius"] = currentAnalysis.radius;
    }
    
    return data;
}
