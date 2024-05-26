#include <utils.h>
#include <shader.h>
#include <camera.h>
#include <dataloader.h>
#include <transfunc.h>
#include <cube.h>
#include <sstream>
#include <fstream>
#include <string>
#include <memory>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Windows.h"

const int WIDTH = 1080;
const int HEIGHT = 1080;
const float stepsize = 0.001f;
GLuint VAO, VBO, EBO;
float fps = 0.0f;


void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void gen_bbox();
void init_bf_texture(int width, int height, GLuint &bf_texture);
void init_frame_buffer(int width, int height, GLuint &bf_texture, GLuint &frame_buffer);
Camera camera(glm::vec3(0.5f, 0.5f, 5.0f));
bool if_first = true;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;

GLFWwindow *window;

int main() {
    WindowGuard windowGuard(window, WIDTH, HEIGHT, "CS171 final project");

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(window);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 400");
    glEnable(GL_DEPTH_TEST);

    // init shaders
    Shader bfShader("shader/bfshader.vs", "shader/bfshader.fs");
    Shader glShader("shader/VShader.vs", "shader/FShader.fs");

    Cube cube;
    cube.create(VAO, VBO, EBO);

    TransferFunction tf(32);
    tf.setColorMapTexture();
    GLuint tff_texture = tf.getColorMapTexture();

    GLuint bf_texture, frame_buffer;
    init_bf_texture(WIDTH, HEIGHT, bf_texture);
    init_frame_buffer(WIDTH, HEIGHT, bf_texture, frame_buffer);

    GLuint raw_texture, hash_texture, offset_texture;
    DataLoader rawloader("D:\\hongyu\\academic\\CS171-project\\assets\\bonsai_256x256x256_uint8.raw", 256, 256, 256, 1, true);
    // DataLoader rawloader("../../assets/vertebra_512x512x512_uint16.raw", 512, 512, 512, 2);
    // DataLoader rawloader("../../assets/miranda_1024x1024x1024_float32.raw", 1024, 1024, 1024, 3);
    raw_texture = rawloader.texture;
    hash_texture = rawloader.hash_texture;
    // offset_texture = rawloader.offset_texture;

    long long frames = 0;
    while (!glfwWindowShouldClose(window)) {
        frames++;
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glShader.use();
        glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH/HEIGHT, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // set backface shader
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
        glEnable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        bfShader.use();
        bfShader.setMat4("proj", proj);
        bfShader.setMat4("model", model);
        bfShader.setMat4("view", view);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)NULL);
        glBindVertexArray(0);
        glDisable(GL_CULL_FACE);
        glUseProgram(0);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glShader.use();
        glShader.setMat4("proj", proj);
        glShader.setMat4("model", model);
        glShader.setMat4("view", view);
        glShader.setFloat("StepSize", 0.01f);
        glShader.setVec2("ScreenSize", vec2(WIDTH, HEIGHT));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, tff_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, bf_texture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, raw_texture);
        // glBindTexture(GL_TEXTURE_3D, hash_texture);
        // PSH
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_3D, hash_texture);

        glShader.setInt("TfFunc", 0);
        glShader.setInt("Depth", 1);
        glShader.setInt("RawData", 2);
        glShader.setInt("hashTable", 3);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)NULL);
        glBindVertexArray(0);
        glDisable(GL_CULL_FACE);
        glUseProgram(0);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static float framesPerSecond = 0.0f;  
        static float lastTime = 0.0f;    
        float currentTime = GetTickCount() * 0.001f;    
        ++framesPerSecond;
        if( currentTime - lastTime > 1.0f )
        {
            lastTime = currentTime;
            fps = framesPerSecond;
            framesPerSecond = 0;
        }

        ImGui::Begin("Frame Per Second");
        ImGui::Text("%f FPS", fps);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        processInput(window);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    return 0;
}


void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.ProcessKeyboard(UP);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.ProcessKeyboard(DOWN);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.ProcessKeyboard(LEFT);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.ProcessKeyboard(RIGHT);
  }
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    camera.ProcessKeyboard(FORWARD);
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    camera.ProcessKeyboard(BACKWARD);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    camera.ProcessKeyboard(CLOCKWISE);
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    camera.ProcessKeyboard(ANTICLOCKWISE);
  }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  float xposf = static_cast<float>(xpos);
  float yposf = static_cast<float>(ypos);
  if (if_first) {
    lastX = xposf;
    lastY = yposf;
    if_first = false;
  }
  float xoffset = xposf - lastX;
  float yoffset = lastY - yposf; // reversed since y-coordinates go from bottom to top
  lastX = xpos;
  lastY = ypos;
  camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void init_bf_texture(int width, int height, GLuint &bf_texture) {
  glGenTextures(1, &bf_texture);
  glBindTexture(GL_TEXTURE_2D, bf_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
}

void init_frame_buffer(int width, int height, GLuint &bf_texture, GLuint &frame_buffer) {
  // create depth buffer
  GLuint depth_buffer;
  glGenRenderbuffers(1, &depth_buffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

  // attach texture and depth buffer to frame buffer
  glGenFramebuffers(1, &frame_buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bf_texture, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);
}