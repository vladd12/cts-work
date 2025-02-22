#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Структура для результатов анализа
struct ImageAnalysisResult {
    std::vector<double> edgeProfile;
    std::vector<double> noiseProfile;
    double signalMean = 0.0;
    double noiseStd = 0.0;
    double cnr = 0.0;
    int centerX = 0;
    int centerY = 0;
    int radius = 0;
};

// Глобальные переменные
extern GLuint programID;
extern GLuint vao;
extern GLuint imageTexture;
extern cv::Mat currentImage;
extern std::vector<float> responseFunction;
extern std::string outputMessage;
extern ImageAnalysisResult currentAnalysis;

// Функции
void GenerateTestImage();
void GenerateCustomCircle(int width, int height, int radius);
void LoadImage();
void CalculateResponseFunction();
void ApplyEdgeEnhancement();
void UpdateImageTexture();
void RenderImage();
double CalculateNoiseLevel(const cv::Mat& image);
double CalculateCNR(const cv::Mat& image, const cv::Rect& roi);
ImageAnalysisResult AnalyzeImage(const cv::Mat& image, const cv::Point& center, int radius);
void RenderAnalysisWindows();
void RenderEdgeProfile();
void RenderNoiseProfile();
void RenderStatistics();
json GetAnalysisData();