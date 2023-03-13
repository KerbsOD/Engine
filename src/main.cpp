#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath) 
{
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, 
        VERTEX = 0, 
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    
    while(getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) 
        {
            if (line.find("vertex") != std::string::npos) 
            {
                type = ShaderType::VERTEX;
            } 
            else if (line.find("fragment") != std::string::npos)  
            {
                type = ShaderType::FRAGMENT;
            }
        } 
        else 
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);


static unsigned int CompileShader(unsigned int type, const std::string &source) 
{
    unsigned int id = glCreateShader(type); // parecido a cuando haces un buffer pero es una inconsistencia de opengl
    const char *src = source.c_str(); 
    glShaderSource(id, 1, &src, nullptr); // Atach shader object (by id) to the source code
    glCompileShader(id);

    // error handleing
    GLint success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if(!success) 
    {
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cout << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "_SHADER_COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}


static unsigned int CreateShader(const std::string &vertexShader, const std::string &fragmentShader) 
{    
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


int main() 
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1200, 800, "Engine", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    



    // CODE
    // ----

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    /* un Vertex Array Object (VAO) guarda los siguientes datos:
        - Llamadas a 'glEnableVertexAttribArray'
        - Configuraciones via 'glVertexAttribPointer'
    */
    unsigned int attributeID; // Vertex array object
    glGenVertexArrays(1, &attributeID); // We generate our array and assign it a number ID
    glBindVertexArray(attributeID); // we select our vertex array 

    unsigned int bufferID; // Vertex buffer object
    glGenBuffers(1, &bufferID); // We generate our buffer and assign it a number ID
    glBindBuffer(GL_ARRAY_BUFFER, bufferID); // we select our buffer
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // put the data into the actual buffer that is now selected by opengl

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0); // start position - quantity of vertices - Normalization - byte shift - 

    ShaderProgramSource source = ParseShader("../res/shaders/Basic.shader");
    
    std::cout << "VERTEX: " << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "FRAGMENT: " << std::endl;
    std::cout << source.FragmentSource << std::endl;

    // unsigned int shader = CreateShader(vertexShader, fragmentShader);
    // glUseProgram(shader);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
}
























// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

    