﻿#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <random>
#include <Windows.h>
#include "Shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void drawTitle();
void setupCMDWindowParams();

int main()
{
    setupCMDWindowParams();
    drawTitle();

    bool isOtherColour = true;

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

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    //-------------------------------------------------------------
    // Set up vertex data
    //-------------------------------------------------------------
    float rectangle[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
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
    


    //-------------------------------------------------------------
    // Set up texture coordinate data
    //-------------------------------------------------------------
    float texCoords[] = {
    0.0f, 0.0f,  // lower-left corner  
    1.0f, 0.0f,  // lower-right corner
    0.5f, 1.0f   // top-center corner
    };



    //-------------------------------------------------------------
    // Set up VAO, VBO & EBO 
    //-------------------------------------------------------------
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    
    /*
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);*/
    


    //-------------------------------------------------------------
    // Set up how vertex data is read
    //-------------------------------------------------------------

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /*
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    */



    //-------------------------------------------------------------
    // Create the shaders
    //-------------------------------------------------------------
    Shader VertexColorShader("VertexColorVertexShader.glsl", "VertexColorFragmentShader.glsl");
    Shader BaseShader("BaseVertexShader.glsl", "BaseFragmentShader.glsl");
    Shader MovingShader("MovingVertexShader.glsl", "MovingFragmentShader.glsl");
    Shader DefinedColorShader("DefinedColorVertexShader.glsl", "DefinedColorFragmentShader.glsl");



    //-------------------------------------------------------------
    // Frame Loop
    //-------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        BaseShader.use();
  
        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        float vertValue = (sin(timeValue * 20));

        MovingShader.set4Float("VerticeTimeMultiplier", 0.0f, vertValue, 0.0f, 1.0f);
        MovingShader.set4Float("NewColor", 0.0f, greenValue, 0.0f, 1.0f);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
        glfwSetWindowShouldClose(window, true);
}