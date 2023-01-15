#pragma once

#include <GLFW/glfw3.h>

#include <string>
#include <vector>

class ArmVis {
public:
	struct vec3 {
		float x = 0.0f, y = 0.0f, z = 0.0f;
	};

	ArmVis() = default;
	~ArmVis();

	void Init(const std::string& title, int width, int height, int circle_verts, float thickness);
	void OnUpdate();

	bool WindowShouldClose();

	void setUserPointer(std::vector<float>* transform_data);
	vec3 getEndPos() { return m_EndPos; }

private:
	unsigned int m_VAO, m_VBO, m_EBO;
	unsigned int m_ShaderID;

	std::vector<float> m_VertexData;
	std::vector<unsigned int> m_IndexData;

	std::vector<float>* m_TransformData;

	GLFWwindow* m_Window;

	vec3 m_EndPos;
	vec3 m_CamPos = vec3{2.0f, 1.5f, 0.0f};

	void OnRender();
	void OnImGui();
};

bool operator==(const ArmVis::vec3& lhs, const ArmVis::vec3& rhs);
bool operator!=(const ArmVis::vec3& lhs, const ArmVis::vec3& rhs);