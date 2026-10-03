
#define STB_IMAGE_IMPLEMENTATION



#include "stb_image.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "stdio.h"

#include "GLFW/glfw3.h"




struct Texture{
    GLuint id = 0;
    int width = 0;
    int height = 0;
};

Texture loadTexture(const char* path){
    Texture tex;
    unsigned char* pixels = stbi_load(path, &tex.width, &tex.height, nullptr, 4);
    if (!pixels){
        printf("failed to load! %s: %s", path, stbi_failure_reason());
        return tex;
    }

    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    stbi_image_free(pixels);
    return tex;
}



int main(){
    if (!glfwInit()){
        printf("glfw failed!");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 450, "app", nullptr, nullptr);
    if (!window){
        printf("could not create window");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

   



    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");



    
    Texture image = loadTexture("ss.png");

    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        

        float sidebar_width = 60.0f;
        ImVec2 display_size = ImGui::GetIO().DisplaySize;

        ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)(intptr_t)image.id, ImVec2(sidebar_width,0),display_size);


        ImGui::SetNextWindowPos(ImVec2(0,0));
        ImGui::SetNextWindowSize(ImVec2(sidebar_width, display_size.y));
        ImGui::Begin("test", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

        if (ImGui::Button("A", ImVec2(-1,0))){
            sidebar_width = 100.0f;
        }
        if (ImGui::Button("B", ImVec2(-1,0))){
            sidebar_width = 60.0f;
        }


        ImGui::End();
        ImGui::Render();

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, image.id);


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);


    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteTextures(1, &image.id);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;

}