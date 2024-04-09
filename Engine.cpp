﻿


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <random>
#include <Windows.h>
#include "Shader.h"
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void drawTitle();
void setupCMDWindowParams();
void CalculateDeltaTime();


glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;



int main()
{

    setupCMDWindowParams();
    drawTitle();

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Bouribou Game Engine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);
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
    float rectangleVertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };
    float texturedRectangleVertices[] = {
        // Front face
        // positions          // colors           // texture coords
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,   // bottom right
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,   1.0f, 1.0f,   // top right
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // top left

        // Right face
        // positions          // colors           // texture coords
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // bottom left
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,   0.0f, 1.0f,   // top left

        // Back face
        // positions          // colors           // texture coords
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // bottom left
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // top right
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // top left

        // Left face
        // positions          // colors           // texture coords
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // bottom left
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // top right
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // top left

        // Top face
        // positions          // colors           // texture coords
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // bottom left
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // top right
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // top left

        // Bottom face
        // positions          // colors           // texture coords
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // bottom left
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // top right
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
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
    

    float triangleVertices[] = {
        // positions        
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    
    float colorVertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  0.5f, 0.5f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 1.0f, 1.0f    // top 
    };

    glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    //-------------------------------------------------------------
    // Set up texture data
    //-------------------------------------------------------------

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned int texture1, texture2;

    unsigned char* data = stbi_load("real.png", &width, &height, &nrChannels, 0);

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    data = stbi_load("BouribouGameEngine.png", &width, &height, &nrChannels, 0);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    


    //-------------------------------------------------------------
    // Set up VAO, VBO & EBO 
    //-------------------------------------------------------------
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedRectangleVertices), texturedRectangleVertices, GL_STATIC_DRAW);
    
    
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    


    //-------------------------------------------------------------
    // Set up how vertex data is read
    //-------------------------------------------------------------

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    //-------------------------------------------------------------
    // Create the shaders
    //-------------------------------------------------------------
    Shader VertexColorShader("VertexColorVertexShader.glsl", "VertexColorFragmentShader.glsl");
    Shader BaseShader("BaseVertexShader.glsl", "BaseFragmentShader.glsl");
    Shader MovingShader("MovingVertexShader.glsl", "MovingFragmentShader.glsl");
    Shader DefinedColorShader("DefinedColorVertexShader.glsl", "DefinedColorFragmentShader.glsl");
    Shader BaseTexturedShader("BaseTexturedVertexShader.glsl", "BaseTexturedFragmentShader.glsl");

    BaseTexturedShader.use();
    BaseTexturedShader.setInt("texture1", 0); 
    BaseTexturedShader.setInt("texture2", 1); 


    //-------------------------------------------------------------
    // Frame Loop
    //-------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        CalculateDeltaTime();
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        //glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);  
        //glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        //glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
        //glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        //glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
        //glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);



        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

        //model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 1.0f));

        //model = glm::scale(model, glm::vec3( sin((float)glfwGetTime()), sin((float)glfwGetTime()), sin((float)glfwGetTime())));


        BaseTexturedShader.setMatrix4("view", view);
        BaseTexturedShader.setMatrix4("projection", projection);

        glBindVertexArray(VAO);
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, sin((float)glfwGetTime()), glm::vec3(1.0f, 0.3f, 0.5f));
            model = glm::scale(model, glm::vec3(sin((float)glfwGetTime()), sin((float)glfwGetTime()), sin((float)glfwGetTime())));

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            BaseTexturedShader.setMatrix4("model", model);
        }
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    
    glViewport(0, 0, width, height);
}

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

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraSpeed = 15.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

}

void CalculateDeltaTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}