#include <glm/gtc/quaternion.hpp>

namespace MyEngine {
	/// <summary>
	/// Converts quaternions to Euler angles, handling singularities
	/// </summary>
	/// <param name="q"></param>
	/// <returns></returns>
	glm::vec3 ToEulerSafe(glm::quat q) {
		double test = q.x * q.y + q.z * q.w;
		if (test > 0.499) { // singularity at north pole
			return {
				glm::pi<float>() / 2,
				2 * atan2(q.x, q.w),
				0
			};
		}
		if (test < -0.499) { // singularity at south pole
			return {
				-glm::pi<float>() / 2,
				-2 * atan2(q.x, q.w),
				0
			};
		}
		double sqx = q.x * q.x;
		double sqy = q.y * q.y;
		double sqz = q.z * q.z;

		return {
			asin(2 * test),
			atan2(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * sqy - 2 * sqz),
			atan2(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * sqx - 2 * sqz)
		};
	}


	glm::quat ToQuaternionSafe(glm::vec3 eulerAngles) {
		float c1 = glm::cos(eulerAngles.y / 2);
		float s1 = glm::sin(eulerAngles.y / 2);
		float c2 = glm::cos(eulerAngles.x / 2);
		float s2 = glm::sin(eulerAngles.x / 2);
		float c3 = glm::cos(eulerAngles.z / 2);
		float s3 = glm::sin(eulerAngles.z / 2);
		float c1c2 = c1 * c2;
		float s1s2 = s1 * s2;

		return {
			c1c2 * c3 - s1s2 * s3,
			c1c2 * s3 + s1s2 * c3,
			s1 * c2 * c3 + c1 * s2 * s3,
			c1 * s2 * c3 - s1 * c2 * s3
		};
	}

}