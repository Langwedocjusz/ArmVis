#include "ArmVis.h"

#include "glad/glad.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>

ArmVis::~ArmVis()
{
    glDeleteProgram(m_ShaderID);

    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);

    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void ArmVis::Init(const std::string& title, int width, int height)
{
    //Initialize glfw:
    if (!glfwInit()) {
        std::cerr << "Failed to initialize glfw!" << '\n';
    }

    //Set OpenGL version to 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Create a window
    m_Window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    if (!m_Window)
        std::cerr <<  "Failed to initialize glfw!" << '\n';

    //Set the created window as current context
    glfwMakeContextCurrent(m_Window);

    //initialize GLAD:
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
         std::cerr << "Failed to initialize glad!" <<'\n';

    //Enable vsync
    glfwSwapInterval(1);

    //Initialize ImGui:
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
    ImGui::StyleColorsDark();

    //Prepare geometry
    const int circle_verts = 15;
    const float thickness = 0.1f;
    const float pi = 3.1415926535f;

    //Bottom
    for (int i = 0; i < circle_verts; i++)
    {
        float angle = 2.0*pi*float(i)/float(circle_verts);

        m_VertexData.push_back(thickness*cos(angle));
        m_VertexData.push_back(0.0f);
        m_VertexData.push_back(thickness*sin(angle));

        m_VertexData.push_back(0.0f);
        m_VertexData.push_back(-1.0f);
        m_VertexData.push_back(0.0f);

        if (i < circle_verts - 2)
        {
            m_IndexData.push_back(0);
            m_IndexData.push_back(i+1);
            m_IndexData.push_back(i+2);
        }
    }

    //Top
    for (int i = 0; i < circle_verts; i++)
    {
        float angle = 2.0 * pi * float(i) / float(circle_verts);

        m_VertexData.push_back(thickness*cos(angle));
        m_VertexData.push_back(1.0f);
        m_VertexData.push_back(thickness*sin(angle));

        m_VertexData.push_back(0.0f);
        m_VertexData.push_back(1.0f);
        m_VertexData.push_back(0.0f);

        if (i < circle_verts - 2)
        {
            m_IndexData.push_back(circle_verts + 0);
            m_IndexData.push_back(circle_verts + i + 1);
            m_IndexData.push_back(circle_verts + i + 2);
        }
    }

    //Sides
    for (int i = 0; i < circle_verts; i++)
    {
        float angle = 2.0 * pi * float(i) / float(circle_verts);

        //Positions
        m_VertexData.push_back(thickness*cos(angle));
        m_VertexData.push_back(0.0f);
        m_VertexData.push_back(thickness*sin(angle));

        //Normals
        m_VertexData.push_back(cos(angle));
        m_VertexData.push_back(0.0f);
        m_VertexData.push_back(sin(angle));
    }

    for (int i = 0; i < circle_verts; i++)
    {
        float angle = 2.0 * pi * float(i) / float(circle_verts);

        //Positions
        m_VertexData.push_back(thickness*cos(angle));
        m_VertexData.push_back(1.0f);
        m_VertexData.push_back(thickness*sin(angle));

        //Normals
        m_VertexData.push_back(cos(angle));
        m_VertexData.push_back(0.0f);
        m_VertexData.push_back(sin(angle));
    }

    for (int i = 0; i < circle_verts - 1; i++)
    {
        m_IndexData.push_back(2*circle_verts + i);
        m_IndexData.push_back(2*circle_verts + i + 1);
        m_IndexData.push_back(2*circle_verts + circle_verts + i + 1);

        m_IndexData.push_back(2*circle_verts + i);
        m_IndexData.push_back(2*circle_verts + circle_verts + i + 1);
        m_IndexData.push_back(2*circle_verts + circle_verts + i);
    }

    m_IndexData.push_back(2 * circle_verts + circle_verts - 1);
    m_IndexData.push_back(2 * circle_verts + 0);
    m_IndexData.push_back(2 * circle_verts + circle_verts + 0);

    m_IndexData.push_back(2 * circle_verts + circle_verts - 1);
    m_IndexData.push_back(2 * circle_verts + circle_verts + 0);
    m_IndexData.push_back(2 * circle_verts + circle_verts + circle_verts - 1);

    //Generate GL buffers
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_VertexData.size(),
        &m_VertexData[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_IndexData.size(),
        &m_IndexData[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    //Compile shaders
    const char* vertex_code = R"(
        #version 330 core
        
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNorm;

        out vec3 norm;

        //uniform float uLength;
        //uniform float uThickness;
        //uniform vec3 uOffset;
        uniform mat4 uVP;
        uniform mat4 uM;

        void main()
        {
            norm = mat3(transpose(inverse(uM)))*aNorm;

            vec3 pos = aPos;// vec3(uThickness, uLength, uThickness) * aPos + uOffset;
            gl_Position = uVP * uM * vec4(pos, 1.0);
        }
    )";

    const char* fragment_code = R"(
        #version 330 core

        out vec4 FragColor;

        in vec3 norm;

        float sat(float x) {return clamp(x, 0.0, 1.0);}

        void main()
        {
            const vec3 ldir = normalize(vec3(1.0, 1.0, -1.0));

            float dif = 0.1 + 0.8 * sat(dot(norm, ldir)) + 0.2 * sat(dot(norm, -ldir));
            
            dif = pow(dif, 1.0/2.2);

            FragColor = vec4(vec3(dif), 1.0);
        }
    )";


    unsigned int vert_id = 0, frag_id = 0;
    int success;
    char info_log[512];

    vert_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_id, 1, &vertex_code, NULL);
    glCompileShader(vert_id);

    glGetShaderiv(vert_id, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vert_id, 512, NULL, info_log);
        std::cerr << "Error: Vertex compilation failed: \n" << info_log << '\n';
        glDeleteShader(vert_id);
        return;
    }

    frag_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_id, 1, &fragment_code, NULL);
    glCompileShader(frag_id);

    glGetShaderiv(frag_id, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(frag_id, 512, NULL, info_log);
        std::cerr << "Error: Fragment compilation failed: \n" << info_log << '\n';
        glDeleteShader(vert_id);
        glDeleteShader(frag_id);
        return;
    }

    m_ShaderID = glCreateProgram();
    glAttachShader(m_ShaderID, vert_id);
    glAttachShader(m_ShaderID, frag_id);
    glLinkProgram(m_ShaderID);

    glGetProgramiv(m_ShaderID, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(m_ShaderID, 512, NULL, info_log);
        std::cerr << "Error: Shader program linking failed: \n" << info_log << '\n';
    }

    glDeleteShader(vert_id);
    glDeleteShader(frag_id);

    glEnable(GL_DEPTH_TEST);
    //Wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void ArmVis::OnUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    OnRender();
    OnImGui();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}

void ArmVis::OnRender()
{
    //Construct view-projection matrix
    glm::vec3 front;
    front.x = - sin(m_CamPos.y) * cos(m_CamPos.z);
    front.z = - sin(m_CamPos.y) * sin(m_CamPos.z);
    front.y = - cos(m_CamPos.y);

    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0, 1.0, 0.0)));
    glm::vec3 up = glm::normalize(glm::cross(right, front));

    glm::vec3 pos = -m_CamPos.x * front + glm::vec3(0.0, 0.5, 0.0);

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1280.0f/720.0f, 0.1f, 1000.0f);
    glm::mat4 view = glm::lookAt(pos, pos + front, up);

    glm::mat4 VP = proj * view;

    //Render scene
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_ShaderID);

    unsigned location = glGetUniformLocation(m_ShaderID, "uVP");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(VP));

    for (int i = 0; i < m_TransformData->size()/16; i++)
    {
        location = glGetUniformLocation(m_ShaderID, "uM");
        glUniformMatrix4fv(location, 1, GL_FALSE, &(m_TransformData->at(16 * i)));

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glDrawElements(GL_TRIANGLES, m_IndexData.size(), GL_UNSIGNED_INT, 0);
    }
}

void ArmVis::OnImGui()
{
    ImGui::Begin("Window");
    ImGui::SliderFloat("Camera r", &m_CamPos.x, 0.0, 10.0);
    ImGui::SliderFloat("Camera theta", &m_CamPos.y, 0.01, 3.14);
    ImGui::SliderFloat("Camera phi", &m_CamPos.z, 0.0, 6.28);
    ImGui::Spacing();
    ImGui::SliderFloat("Length", &length, 0.0, 1.0);
    ImGui::SliderFloat("Thickness", &thickness, 0.0, 1.0);
    ImGui::Spacing();
    ImGui::SliderFloat("Position X", &m_EndPos.x, -1.0f, 1.0f);
    ImGui::SliderFloat("Position Y", &m_EndPos.y, -1.0f, 1.0f);
    ImGui::SliderFloat("Position Z", &m_EndPos.z, -1.0f, 1.0f);
    ImGui::End();
}

void ArmVis::setUserPointer(std::vector<float>* transform_data)
{
    m_TransformData = transform_data;
}

bool ArmVis::WindowShouldClose()
{
    return glfwWindowShouldClose(m_Window);
}

bool operator==(const ArmVis::vec3& lhs, const ArmVis::vec3& rhs)
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
}

bool operator!=(const ArmVis::vec3& lhs, const ArmVis::vec3& rhs)
{
    return !(lhs == rhs);
}