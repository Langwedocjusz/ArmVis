#include "ArmVis.h"

#include <iostream>

void generateMockData(std::vector <float>& transform_data);

int main()
{
	//Initialize visualization
	ArmVis viz;
	//Arguments are: window title, window width, window height, number of vertices per circle, radius of cylinder
	viz.Init("Arm Visualization", 1280, 720, 15, 0.1f);

	//Prepare transofrmation data
	std::vector<float> transform_data;
	generateMockData(transform_data);

	//Set data pointer
	viz.setUserPointer(&transform_data);

	//End position to by set by visualization gui
	ArmVis::vec3 end_pos;

	//Rendering loop
	while (!viz.WindowShouldClose())
	{
		//Render scene and process gui
		viz.OnUpdate();

		//Retrieve end pos from gui
		ArmVis::vec3 tmp = viz.getEndPos();

		//Do something if position changed
		if (tmp != end_pos)
		{
			end_pos = tmp;
			std::cout << end_pos.x << ", " << end_pos.y << ", " << end_pos.z << '\n';
		}
	}

	return 0;
}

void generateMockData(std::vector <float>& transform_data)
{
	const float angle_offset = 0.25f * 3.1415926535f;
	float translation[3] = { 0.0f, 0.0f, 0.0f };
	float height = 1.0f;

	for (int i = 0; i < 5; i++)
	{
		float s = sin(angle_offset * float(i));
		float c = cos(angle_offset * float(i));

		float matrix[16] = {             c,             -s,           0.0f, 0.0f,
							    height * s,     height * c,           0.0f, 0.0f,
									  0.0f,           0.0f,           1.0f, 0.0f,
							translation[0], translation[1], translation[2], 1.0f };

		translation[0] += height * s;
		translation[1] += height * c;
		translation[2] += 0.0f;

		height *= 0.5f;

		for (int j = 0; j < 16; j++)
		{
			transform_data.push_back(matrix[j]);
		}
	}
}