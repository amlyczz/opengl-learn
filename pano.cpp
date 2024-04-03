#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const GLchar *fragmentShaderSource3D = R"glsl(
    #version 410 core
    precision highp float;
    uniform vec2 iResolution;
    uniform vec2 iMouse;
    uniform float iFov;
    uniform sampler2D iChannel1;

    vec3 rotateX(vec3 p, float a)
    {
        float sa = sin(a);
        float ca = cos(a);
        return vec3(p.x, ca*p.y - sa*p.z, sa*p.y + ca*p.z);
    }

    vec3 rotateY(vec3 p, float a)
    {
        float sa = sin(a);
        float ca = cos(a);
        return vec3(ca*p.x + sa*p.z, p.y, -sa*p.x + ca*p.z);
    }

    vec3 envSampleLOD(vec3 dir, float lod)
    {
        float theta = acos(dir.y);
        float phi = atan(dir.z, dir.x);
        vec2 uv = vec2(phi / (2.0 * 3.14159265) + 0.5, theta / 3.14159265);
        return texture(iChannel1, uv, lod).rgb;
    }

    vec3 background(vec3 rd)
    {
        return envSampleLOD(rd, 0.0);
    }

    void main()
    {
        vec2 pixel = (gl_FragCoord.xy / iResolution.xy) * 2.0 - 1.0;
        float asp = iResolution.x / iResolution.y;
        float fov = radians(iFov);
        float fx = tan(fov / 2.0);
        float fy = fx / asp;
        vec3 rd = normalize(vec3(pixel.x * fx, pixel.y * fy, -1.0));

        vec2 mouse = iMouse.xy / iResolution.xy;
        float roty = (mouse.x - 0.5) * 6.0;
        float rotx = (mouse.y - 0.5) * 3.0;

        rd = rotateX(rd, rotx);
        rd = rotateY(rd, roty);

        vec3 rgb = background(rd);
        gl_FragColor = vec4(rgb, 1.0);
    }
)glsl";

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}



int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Panoramic Rendering", NULL, NULL);
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

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char *data = stbi_load("/opt/CProject/opengl-learn/pano.jpg", &width, &height, &nrChannels, 0);
    if (data == nullptr)
    {
        std::cout << "Failed to load texture" << std::endl;
        return -1;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        return -1;
    }
    stbi_image_free(data);

    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource3D, NULL);
    glCompileShader(fragmentShader);
    GLint success;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(fragmentShader);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform2f(glGetUniformLocation(shaderProgram, "iResolution"), SCR_WIDTH, SCR_HEIGHT);
        glUniform2f(glGetUniformLocation(shaderProgram, "iMouse"), 0.0f, 0.0f); // No mouse interaction
        glUniform1f(glGetUniformLocation(shaderProgram, "iFov"), 90.0f); // Set field of view to 90 degrees
        glUniform1i(glGetUniformLocation(shaderProgram, "iChannel1"), 0); // Use texture unit 0

        glBindTexture(GL_TEXTURE_2D, texture);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
