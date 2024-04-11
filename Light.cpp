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
// Requirements:
// - a modifyable light position
// - light shader parameters (lightColor, ambientStrength, spreadStrength, lightFalloff)
// 
// Nice to haves/todo later:
// - integrated light mesh
// - integrated unlit shader assigning
// - automatic GUI
// 
//---------------------------------------------------------------------------------------------------------------------


