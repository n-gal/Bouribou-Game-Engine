
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <random>
#include <Windows.h>

#include "Shader.h"
#include "stb_image.h"
#include "camera.h"
#include "Light.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//-------------------------------------------------------------
// Instantiate objects
//-------------------------------------------------------------

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));


float screenWidth = 1920.0f;
float screenHeight = 1080.0f;

double previousTime = glfwGetTime();
int frameCount = 0;
int lastFps = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void drawTitle();
void setupCMDWindowParams();
void CalculateDeltaTime();

unsigned int loadCubemap(std::vector<std::string> faces);

unsigned int loadTexture(char const* path);
int GetFps();



glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = screenWidth / 2.0;
float lastY = screenHeight / 2.0;
float fov = 45.0f;


bool cursorIsUnfocused = false;
bool cursorIsHoveringUI = false;


int main()
{

    setupCMDWindowParams();
    drawTitle();

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Bouribou Game Engine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);
    //glfwSwapInterval(0); -disables vsync
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    //-------------------------------------------------------------
    // Set up vertex data
    //-------------------------------------------------------------
    float texturedRectangleVertices[] = {
        // Front face
        // positions            //normals             // texture coords
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,     0.0f, 0.0f,   // bottom left
         0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,     1.0f, 0.0f,   // bottom right
         0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,     1.0f, 1.0f,   // top right
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,     0.0f, 1.0f,   // top left

        // Right face         
        // positions                                  // texture coords
         0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,   0.0f, 0.0f,   // bottom left
         0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   // bottom right
         0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,   1.0f, 1.0f,   // top right
         0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   // top left

        // Back face          
        // positions                                  // texture coords
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   0.0f, 0.0f,   // bottom left
         0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   1.0f, 0.0f,   // bottom right
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   1.0f, 1.0f,   // top right
        -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   0.0f, 1.0f,   // top left

        // Left face          
        // positions                                  // texture coords
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,   // bottom left
        -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,   // bottom right
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,   // top right
        -0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,   // top left

        // Top face           
        // positions                                  // texture coords
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,  0.0f,    0.0f, 0.0f,   // bottom left
         0.5f,  0.5f,  0.5f,    0.0f, 1.0f,  0.0f,    1.0f, 0.0f,   // bottom right
         0.5f,  0.5f, -0.5f,    0.0f, 1.0f,  0.0f,    1.0f, 1.0f,   // top right
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,  0.0f,    0.0f, 1.0f,   // top left

        // Bottom face        
        // positions                                  // texture coords
        -0.5f, -0.5f, -0.5f,    0.0f,  -1.0f,  0.0f,  0.0f, 0.0f,   // bottom left
         0.5f, -0.5f, -0.5f,    0.0f,  -1.0f,  0.0f,  1.0f, 0.0f,   // bottom right
         0.5f, -0.5f,  0.5f,    0.0f,  -1.0f,  0.0f,  1.0f, 1.0f,   // top right
        -0.5f, -0.5f,  0.5f,    0.0f,  -1.0f,  0.0f,  0.0f, 1.0f    // top left
    };
    unsigned int indices[] = {
        // Front face
        0, 1, 2,  // first triangle
        0, 2, 3,  // second triangle

        // Right face
        4, 5, 6,  // first triangle
        4, 6, 7,  // second triangle

        // Back face
        8, 9, 10, // first triangle
        8, 10, 11,// second triangle

        // Left face
        12, 13, 14, // first triangle
        12, 14, 15, // second triangle

        // Top face
        16, 17, 18, // first triangle
        16, 18, 19, // second triangle

        // Bottom face
        20, 21, 22, // first triangle
        20, 22, 23  // second triangle
    };


    //-------------------------------------------------------------
    // Set up texture data
    //-------------------------------------------------------------
    stbi_set_flip_vertically_on_load(true);

    unsigned int roadDiffuseMap = loadTexture("Textures/TCom_Pavement_RoadWet_BrokenLineWhite_512_diffuse.png");
    unsigned int roadSpecularMap = loadTexture("Textures/test2.png");

    unsigned int crateDiffuseMap = loadTexture("Textures/container2.png");
    unsigned int crateSpecularMap = loadTexture("Textures/container2_specular.png");

    //-------------------------------------------------------------
    // Set up texture data for skybox cubemap
    //-------------------------------------------------------------


    std::vector<std::string> faces
    {
        "Textures/Skyboxes/right.jpg",
        "Textures/Skyboxes/left.jpg",
        "Textures/Skyboxes/top.jpg",
        "Textures/Skyboxes/bottom.jpg",
        "Textures/Skyboxes/front.jpg",
        "Textures/Skyboxes/back.jpg"
    };

    unsigned int cubemapTexture = loadCubemap(faces);



    //-------------------------------------------------------------
    // Set up cube VAO + VBO & EBO
    //-------------------------------------------------------------
    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);



    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedRectangleVertices), texturedRectangleVertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);



    //-------------------------------------------------------------
    // Set up light VAO
    //-------------------------------------------------------------
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);



    //-------------------------------------------------------------
    // Set up sky VAO
    //-------------------------------------------------------------
    unsigned int SkyCubeVAO;
    glGenVertexArrays(1, &SkyCubeVAO);
    glBindVertexArray(SkyCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);



    //-------------------------------------------------------------
    // Create the shaders
    //-------------------------------------------------------------
    Shader BaseTexturedShader("Shaders/BaseTexturedVertexShader.glsl", "Shaders/BaseTexturedFragmentShader.glsl");
    Shader BaseLitShader("Shaders/BaseLitVertexShader.glsl", "Shaders/BaseLitFragmentShader.glsl");
    Shader BaseUnlitShader("Shaders/BaseUnlitVertexShader.glsl", "Shaders/BaseUnlitFragmentShader.glsl");
    Shader BaseSkyboxShader("Shaders/BaseSkyboxVertexShader.glsl", "Shaders/BaseSkyboxFragmentShader.glsl");
    Shader BaseReflectiveShader("Shaders/BaseReflectiveVertexShader.glsl", "Shaders/BaseReflectiveFragmentShader.glsl");

    /*
    BaseTexturedShader.use();
    BaseTexturedShader.setInt("texture1", 0); 
    BaseTexturedShader.setInt("texture2", 1); 
    */

    BaseSkyboxShader.setInt("skybox", 0);



    //-------------------------------------------------------------
    // Initiate ImGui, manages the UI
    //-------------------------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::SetNextWindowSize(ImVec2(400, 200));

    //UI variables
    bool drawCube = true;
    float cubeSpeed = 0;
    float backgroundColor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
    float textureBlendValue = 0.5;
    float vertexColorBlendValue = 1;

    float diffuse[3] = { 1.0f, 1.0f, 1.0f };
    float specularColor[3] = { 1.0f, 1.0f, 1.0f };
    float smoothness = 32;
    float roughness = 0;

    float reflectivity = 0.5f;

    std::vector<Light> lights(0);


    //-------------------------------------------------------------
    // Frame Loop
    //-------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {

        CalculateDeltaTime();
        processInput(window);

        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        //glClearColor(lightColor[0], lightColor[1], lightColor[2], 1);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, roadDiffuseMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, roadSpecularMap);


        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, crateDiffuseMap);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, crateSpecularMap);



        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(camera.Zoom), screenWidth / screenHeight, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);



        //-------------------------------------------------------------
        // Lights
        //-------------------------------------------------------------
        BaseUnlitShader.use();
        BaseUnlitShader.setMatrix4("view", view);
        BaseUnlitShader.setMatrix4("projection", projection);
        int i = 0;
        for (auto& light : lights)
        {
            i++;
            model = glm::mat4(1.0f);
            model = glm::translate(model, light.pos);
            model = glm::scale(model, glm::vec3(0.3, 0.3, 0.3));

            BaseUnlitShader.use();
            BaseUnlitShader.setMatrix4("model", model);
            BaseUnlitShader.set3Float("lightColor", light.color[0], light.color[1], light.color[2]);


            glBindVertexArray(lightCubeVAO);
            if (drawCube)
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            light.ID = i;
            light.updatePos();
            light.updateUi(cursorIsUnfocused);

            BaseLitShader.use();

            BaseLitShader.set3Float(("inLight[" + std::to_string(i) + "].lightColor").c_str(), light.color[0], light.color[1], light.color[2]);
            BaseLitShader.set3Float(("inLight[" + std::to_string(i) + "].lightPos").c_str(), light.pos.x, light.pos.y, light.pos.z);
            BaseLitShader.set3Float(("inLight[" + std::to_string(i) + "].direction").c_str(), light.dir.x, light.dir.y, light.dir.z);
            BaseLitShader.setFloat(("inLight[" + std::to_string(i) + "].spreadStrength").c_str(), light.spreadStrength);
            BaseLitShader.setFloat(("inLight[" + std::to_string(i) + "].ambientStrength").c_str(), light.ambientStrength);
            BaseLitShader.setFloat(("inLight[" + std::to_string(i) + "].cutOff").c_str(), light.cutOff);
            BaseLitShader.setFloat(("inLight[" + std::to_string(i) + "].intensity").c_str(), light.intensity);
            BaseLitShader.setFloat(("inLight[" + std::to_string(i) + "].outerCutOff").c_str(), light.outerCutOff);
            BaseLitShader.setFloat(("inLight[" + std::to_string(i) + "].lightFalloff").c_str(), light.falloff);
            BaseLitShader.setInt(("inLight[" + std::to_string(i) + "].type").c_str(), light.type);
        }


        //-------------------------------------------------------------
        // Road
        //-------------------------------------------------------------
        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

        BaseLitShader.use();
        BaseLitShader.set3Float("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);

        BaseLitShader.setMatrix4("view", view);
        BaseLitShader.setMatrix4("projection", projection);

        BaseLitShader.setInt("skybox", 0);
        BaseLitShader.setInt("inMaterial.diffuse", 1);
        BaseLitShader.setInt("inMaterial.specular", 2);
        BaseLitShader.setFloat("inMaterial.smoothness", smoothness);
        BaseLitShader.setFloat("inMaterial.roughness", roughness);
        BaseLitShader.setFloat("inMaterial.reflectivity", reflectivity);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, sin((float)glfwGetTime()) * cubeSpeed, glm::vec3(1.0f, 0.3f, 0.5f));
        model = glm::scale(model, glm::vec3(8, 1, 4));

        BaseLitShader.setMatrix4("model", model);



        if (drawCube)
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



        //-------------------------------------------------------------
        // Crate
        //-------------------------------------------------------------

        BaseLitShader.use();

        BaseLitShader.setInt("inMaterial.diffuse", 3);
        BaseLitShader.setInt("inMaterial.specular", 4);
        BaseLitShader.setFloat("inMaterial.smoothness", smoothness);
        BaseLitShader.setFloat("inMaterial.reflectivity", reflectivity);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, sin((float)glfwGetTime()) * cubeSpeed, glm::vec3(1.0f, 0.3f, 0.5f));

        model = glm::scale(model, glm::vec3(1, 1, 1));
        model = glm::translate(model, glm::vec3(1, 1, 1));

        BaseLitShader.setMatrix4("model", model);



        if (drawCube)
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



        //-------------------------------------------------------------
        // Skybox
        //-------------------------------------------------------------
        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(SkyCubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        BaseSkyboxShader.use();
        BaseSkyboxShader.setMatrix4("view", view);
        BaseSkyboxShader.setMatrix4("projection", projection);



        model = glm::mat4(1.0f);
        model = glm::rotate(model, sin((float)glfwGetTime()) * cubeSpeed, glm::vec3(1.0f, 0.3f, 0.5f));
        BaseSkyboxShader.setMatrix4("model", model);



        if (drawCube)
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthFunc(GL_LESS);

        //-------------------------------------------------------------
        // Clear the light data
        //-------------------------------------------------------------
        i = 0;
        for (auto& light : lights)
        {
            i++;

            BaseLitShader.use();

            BaseLitShader.set3Float(("inLight[" + std::to_string(i) + "].lightColor").c_str(), 0, 0, 0);
            BaseLitShader.set3Float(("inLight[" + std::to_string(i) + "].lightPos").c_str(), 0, 0, 0);
            BaseLitShader.setFloat(("inLight[" + std::to_string(i) + "].spreadStrength").c_str(), 0);
            BaseLitShader.setFloat(("inLight[" + std::to_string(i) + "].ambientStrength").c_str(), 0);
            BaseLitShader.setFloat(("inLight[" + std::to_string(i) + "].lightFalloff").c_str(), 0);

            /*
            BaseReflectiveShader.use();

            BaseReflectiveShader.set3Float(("inLight[" + std::to_string(i) + "].lightColor").c_str(), 0, 0, 0);
            BaseReflectiveShader.set3Float(("inLight[" + std::to_string(i) + "].lightPos").c_str(), 0, 0, 0);
            BaseReflectiveShader.setFloat(("inLight[" + std::to_string(i) + "].spreadStrength").c_str(), 0);
            BaseReflectiveShader.setFloat(("inLight[" + std::to_string(i) + "].ambientStrength").c_str(), 0);
            BaseReflectiveShader.setFloat(("inLight[" + std::to_string(i) + "].lightFalloff").c_str(), 0);

            */
        }


        
        //-------------------------------------------------------------
        // UI
        //-------------------------------------------------------------
        if (!io.WantCaptureMouse)
        {
            cursorIsHoveringUI = false;
        }
        else
        {
            cursorIsHoveringUI = true;
        }

        ImGui::Begin("Scene parameters", nullptr, !cursorIsUnfocused ? ImGuiWindowFlags_NoInputs : 0);
        ImGui::Checkbox("draw objects", &drawCube);
        ImGui::ColorPicker4("background color", backgroundColor);
        ImGui::SliderFloat("cube speed", &cubeSpeed, 0, 10);
        ImGui::End();

        ImGui::Begin("Material parameters", nullptr, !cursorIsUnfocused ? ImGuiWindowFlags_NoInputs : 0);
        ImGui::ColorPicker4("diffuse color", diffuse);
        ImGui::ColorPicker4("specular color", specularColor);
        ImGui::SliderFloat("smoothness", &smoothness, 0, 500);
        ImGui::SliderFloat("roughness", &roughness, 0, 8);
        ImGui::SliderFloat("reflectivty", &reflectivity, 0, 1);
        ImGui::End();

        ImGui::Begin("Object manager", nullptr, !cursorIsUnfocused ? ImGuiWindowFlags_NoInputs : 0);
        if (ImGui::Button("add enviro light")) {
            lights.push_back(Light());
            lights.back().type = 1;
        }
        if (ImGui::Button("add spotlight")) {
            lights.push_back(Light());
            lights.back().type = 2;
        }
        if (ImGui::Button("add point light")) {
            lights.push_back(Light());
        }
        if (ImGui::Button("remove light") && lights.size() > 0) {
            lights.pop_back();
        }
        ImGui::End();

        ImGui::Begin("Monitor", nullptr, !cursorIsUnfocused ? ImGuiWindowFlags_NoInputs : 0);
        int fps = GetFps();
        ImGui::Text("FPS: %d", fps);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}



//-------------------------------------------------------------
// Rescales the viewport with the window size
//-------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    screenWidth = width;
    screenHeight = height;
    glViewport(0, 0, width, height);
}



//-------------------------------------------------------------
// Draws the bouribou game engine title in the cmd
//-------------------------------------------------------------
void drawTitle()
{
    std::cout << R"(
 ____                               __                                                                                                                
/\  _`\                          __/\ \                                                                                          __                   
\ \ \L\ \    ___   __  __  _ __ /\_\ \ \____    ___   __  __         __      __      ___ ___      __          __    ___      __ /\_\    ___      __   
 \ \  _ <'  / __`\/\ \/\ \/\`'__\/\ \ \ '__`\  / __`\/\ \/\ \      /'_ `\  /'__`\  /' __` __`\  /'__`\      /'__`\/' _ `\  /'_ `\/\ \ /' _ `\  /'__`\ 
  \ \ \L\ \/\ \L\ \ \ \_\ \ \ \/ \ \ \ \ \L\ \/\ \L\ \ \ \_\ \    /\ \L\ \/\ \L\.\_/\ \/\ \/\ \/\  __/     /\  __//\ \/\ \/\ \L\ \ \ \/\ \/\ \/\  __/ 
   \ \____/\ \____/\ \____/\ \_\  \ \_\ \_,__/\ \____/\ \____/    \ \____ \ \__/.\_\ \_\ \_\ \_\ \____\    \ \____\ \_\ \_\ \____ \ \_\ \_\ \_\ \____\
    \/___/  \/___/  \/___/  \/_/   \/_/\/___/  \/___/  \/___/      \/___L\ \/__/\/_/\/_/\/_/\/_/\/____/     \/____/\/_/\/_/\/___L\ \/_/\/_/\/_/\/____/
                                                                     /\____/                                                 /\____/                  
                                                                     \_/__/                                                  \_/__/  
)" << '\n';
}



//-------------------------------------------------------------
// Sets up the cmd window size to display the bouribou game enigne text properly
//-------------------------------------------------------------
void setupCMDWindowParams()
{
    int width = 160;
    int height = 70;


    HWND hwnd = GetConsoleWindow();

    RECT rect;
    GetWindowRect(hwnd, &rect);


    int posX = rect.left;
    int posY = rect.top;
    int consoleWidth = width * 8;
    int consoleHeight = height * 8;
    int consolePosX = posX;
    int consolePosY = posY;
    int consoleRight = consolePosX + consoleWidth;
    int consoleBottom = consolePosY + consoleHeight;

    SetWindowPos(hwnd, NULL, consolePosX, consolePosY, consoleWidth, consoleHeight, SWP_SHOWWINDOW);
}



//-------------------------------------------------------------
// Process keyobard inputs
//-------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        cursorIsUnfocused = true;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !cursorIsHoveringUI)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        cursorIsUnfocused = false;
    }

    
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.MovementSpeed = 10.0f;
    else
        camera.ResetSpeed();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}



//-------------------------------------------------------------
// Calculates the DeltaTime
//-------------------------------------------------------------
void CalculateDeltaTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}



//-------------------------------------------------------------
// Processes mouse movement inputs
//-------------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (!cursorIsUnfocused)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
    else
    {
        if(!firstMouse)
            firstMouse = true;
    }
}



//-------------------------------------------------------------
// Processes scrollwheel inputs
//-------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (!cursorIsHoveringUI)
    {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}

int GetFps()
{
    frameCount++;
    if (glfwGetTime() - previousTime >= 1)
    {
        int fps = frameCount;
        frameCount = 0;
        previousTime = glfwGetTime();
        lastFps = fps;
        return fps;
    }
    else if (glfwGetTime() < 1)
    {
        return 0;
    }
    return lastFps;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}