#include <cmath>
#include "color.hpp"

namespace Color
{

Vector3 rgbFromHsv(float H, float S, float V)
{
	// Normalize H to 0 -- 2pi
	H = fmodf(H, 2.0 * M_PI);
	if (H < 0.0)
		H += 2.0 * M_PI;

	float C = S * V;
	float H_ = H / (M_PI / 3.0);
	float X = C * (1.0f - fabsf(fmodf(H_, 2.0f) - 1.0f));

	Vector3 rgb;
	switch(int(H_)) {
		case 0:
			rgb = Vector3(C, X, 0);
			break;
		case 1:
			rgb = Vector3(X, C, 0);
			break;
		case 2:
			rgb = Vector3(0, C, X);
			break;
		case 3:
			rgb = Vector3(0, X, C);
			break;
		case 4:
			rgb = Vector3(X, 0, C);
			break;
		default:
			rgb = Vector3(C, 0, X);
			break;
	}

	rgb += V - C;
	return rgb;
}

}
