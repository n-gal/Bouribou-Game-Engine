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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//--------------------------------------------------------------------------------------------------------------------
//
// Light class
// 
// Requirements:                                                                            DONE!
// - a modifyable light position                                                            DONE!
// - light shader parameters (lightColor, ambientStrength, spreadStrength, lightFalloff)    DONE!
// 
// Nice to haves/todo later:
// - integrated light mesh
// - integrated unlit shader assigning
// - automatic GUI                                                                          DONE - BUT NEED AN ID
// - separation between .h and .cpp
// 
//---------------------------------------------------------------------------------------------------------------------

class OmniLight
{
public:
    float color[3];
    float ambientStrength;
    float spreadStrength;
    float falloff;
    glm::vec3 pos;
    float posFl[3];
    unsigned int ID;
    const char* windowName;


    OmniLight() : color{1,1,1}, ambientStrength(0.0f), spreadStrength(0.0f), falloff(1.0f), pos(0.0f), posFl{ 1.0f, 1.0f, 1.0f }
    {
    }

    void updatePos()
    {
        pos = glm::vec3(posFl[0], posFl[1], posFl[2]);
    }

    void updateUi(bool cursorIsUnfocused)
    {
        std::string temp = "Light parameters" + std::to_string(ID);
        windowName = temp.c_str();
        ImGui::Begin(windowName, nullptr, !cursorIsUnfocused ? ImGuiWindowFlags_NoInputs : 0);
        ImGui::ColorPicker4("light color", color);
        ImGui::SliderFloat3("light move", posFl, -2, 2);
        ImGui::SliderFloat("ambient strength", &ambientStrength, 0, 2);
        ImGui::SliderFloat("spread strength", &spreadStrength, 0, 2);
        ImGui::SliderFloat("light falloff", &falloff, 0, 5);
        ImGui::End();
        updatePos();
    }
};