#include "ArmVis.h"

#include <iostream>

int main()
{
	ArmVis viz;
	viz.Init("Arm Visualization", 1280, 720);

	ArmVis::vec3 end_pos;

	while (!viz.WindowShouldClose())
	{
		viz.OnUpdate();

		ArmVis::vec3 tmp = viz.getEndPos();

		if (tmp != end_pos)
		{
			end_pos = tmp;
			std::cout << end_pos.x << ", " << end_pos.y << ", " << end_pos.z << '\n';
		}
	}

	return 0;
}