#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "functions.h"
#include "tinyfiledialogs.h"

// Также измените CMakeLists.txt, добавив:
/*
target_include_directories(EdgeResponseAnalyzer PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)
*/

// Глобальные переменные
GLFWwindow* window;
cv::Mat currentImage;
std::vector<float> responseFunction;
GLuint imageTexture = 0;
GLuint programID = 0;
GLuint vao = 0, vbo = 0;
std::string outputMessage;
ImageAnalysisResult currentAnalysis;

// Функция загрузки шейдеров
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Чтение вершинного шейдера
    std::string vertexShaderCode;
    std::ifstream vertexShaderStream(vertex_file_path, std::ios::in);
    if(vertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << vertexShaderStream.rdbuf();
        vertexShaderCode = sstr.str();
        vertexShaderStream.close();
    } else {
        std::cerr << "Could not open " << vertex_file_path << std::endl;
        return 0;
    }

    // Чтение фрагментного шейдера
    std::string fragmentShaderCode;
    std::ifstream fragmentShaderStream(fragment_file_path, std::ios::in);
    if(fragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << fragmentShaderStream.rdbuf();
        fragmentShaderCode = sstr.str();
        fragmentShaderStream.close();
    } else {
        std::cerr << "Could not open " << fragment_file_path << std::endl;
        return 0;
    }

    GLint result = GL_FALSE;
    int infoLogLength;

    // Компиляция вершинного шейдера
    const char* vertexSourcePointer = vertexShaderCode.c_str();
    glShaderSource(vertexShaderID, 1, &vertexSourcePointer, NULL);
    glCompileShader(vertexShaderID);

    // Проверка вершинного шейдера
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0) {
        std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
        std::cerr << &vertexShaderErrorMessage[0] << std::endl;
    }

    // Компиляция фрагментного шейдера
    const char* fragmentSourcePointer = fragmentShaderCode.c_str();
    glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer, NULL);
    glCompileShader(fragmentShaderID);

    // Проверка фрагментного шейдера
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0) {
        std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
        std::cerr << &fragmentShaderErrorMessage[0] << std::endl;
    }

    // Линковка программы
    GLuint progID = glCreateProgram();
    glAttachShader(progID, vertexShaderID);
    glAttachShader(progID, fragmentShaderID);
    glLinkProgram(progID);

    // Проверка программы
    glGetProgramiv(progID, GL_LINK_STATUS, &result);
    glGetProgramiv(progID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0) {
        std::vector<char> programErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(progID, infoLogLength, NULL, &programErrorMessage[0]);
        std::cerr << &programErrorMessage[0] << std::endl;
    }

    glDetachShader(progID, vertexShaderID);
    glDetachShader(progID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return progID;
}

int main() {
    // Инициализация GLFW
    if(!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Настройка GLFW
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создание окна
    window = glfwCreateWindow(1280, 720, "Edge Response Analyzer", NULL, NULL);
    if(window == NULL) {
        std::cerr << "Failed to open GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Инициализация GLEW
    if(glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Настройка Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Установка стиля Dark
    ImGui::StyleColorsDark();

    // Инициализация бэкендов ImGui
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        std::cerr << "Failed to initialize ImGui GLFW backend" << std::endl;
        return -1;
    }

    const char* glsl_version = "#version 130";
    if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
        std::cerr << "Failed to initialize ImGui OpenGL3 backend" << std::endl;
        return -1;
    }

    // Загрузка шейдеров
    programID = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");
    if(programID == 0) {
        std::cerr << "Failed to load shaders" << std::endl;
        return -1;
    }

    // Создание вершинных данных
    float vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Параметры круга
    static int imageWidth = 500;
    static int imageHeight = 500;
    static int circleRadius = 100;

    // Главный цикл
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Новый кадр ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Окно управления
        ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        if(ImGui::Button("Generate Test Circle", ImVec2(200, 30))) {
            GenerateTestImage();
        }

        ImGui::Text("Custom Circle Parameters:");
        ImGui::SliderInt("Width", &imageWidth, 100, 1000);
        ImGui::SliderInt("Height", &imageHeight, 100, 1000);
        ImGui::SliderInt("Radius", &circleRadius, 10, 400);

        if(ImGui::Button("Generate Custom Circle", ImVec2(200, 30))) {
            GenerateCustomCircle(imageWidth, imageHeight, circleRadius);
        }

        if(ImGui::Button("Load Image", ImVec2(200, 30))) {
            LoadImage();
        }

        if(ImGui::Button("Calculate Response", ImVec2(200, 30))) {
            CalculateResponseFunction();
        }

        ImGui::Separator();
        ImGui::TextWrapped("%s", outputMessage.c_str());
        ImGui::End();

        // Окна анализа
        RenderAnalysisWindows();

        // Очистка экрана
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Рендеринг изображения
        if(!currentImage.empty()) {
            RenderImage();
        }

        // Рендеринг ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Очистка
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(programID);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
