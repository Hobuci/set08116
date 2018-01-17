#define GLM_ENABLE_EXPERIMENTAL
#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtx\projection.hpp>
#include <iostream>

using namespace std;
using namespace glm;

int main()
{

	//
	//VECTORS
	//
	//2D vectors
	vec2 u(-1.0f, 1.0f);
	vec2 u2(-2.0f, 2.0f);
	//3D vectors
	vec3 v(1.0f, 1.0f, 1.0f);
	vec3 v2(1.0f, -1.0f, -1.0f);
	//4D vectors - XYZW or RGBA
	vec4 w(0.0f, 1.0f, 0.0f, 1.0f);
	vec4 w2(1.0f, 0.0f, 0.0f, 1.0f);

	//Vector conversion
	//2D to 3D and 4D
	vec3 c_u(vec2(-1.0f, 1.0f), 1.0f);
	vec4 c_u2(vec2(-1.0f, 1.0f), 1.0f, 1.0f);
	//3D to 2D and 4D
	vec2 c_v(vec3(1.0f, 1.0f, 1.0f)); //Z coordinate gets dropped
	vec4 c_v2(vec3(1.0f, 1.0f, 1.0f), 1.0f);
	//4D to 2D and 3D
	vec2 c_w(vec4(0.0f, 1.0f, 0.0f, 1.0f));
	vec3 c_w2(vec4(0.0f, 1.0f, 0.0f, 1.0f));

	//access vector coordinates by NAME.COORDINATE, get or set

	//Vector additions and subtractions
	vec2 u3 = u + u2;
	vec3 v3 = v - v2;
	vec4 w3 = w + w2 - vec4(1.0f, 0.0f, 1.0f, 0.0f);

	//Vector scaling
	u * 3.0f;
	u2 / 2.0f;

	//Length of a vector
	float l = length(u);
	float l2 = length(v);
	float l3 = length(w);

	//Normalization
	vec2 normalized_u = normalize(u2);
	vec3 normalized_v = normalize(v2);
	vec4 normalized_w = normalize(w2);

	//Dot and Cross
	float d = dot(u, u2);
	vec3 c = cross(v, v2);

	//Projection of a vector
	vec4 p = proj(w, w2);


	//
	//MATRICES
	//

	mat4 m(1.0f);

	//Conversion
	mat3 n(mat4(1.0f)); //last column and row aredropped

	//Addition, Scaling and Multiplication work the same way as for vectors
	//Matrix-Vector Multiplication

	vec4 v4 = m * vec4(v, 1.0f);

	//Translation matrix
		//create translation matrix, provide identity matrix for this
	mat4 T = translate(mat4(1.0f), vec3(1.0, 2.0, 1.0));
	//transform vector with the transformation matrix
	vec3 T_v = T * vec4(v, 1.0f);

	//Rotation matrix
	float angle = 90.0f;
	//creating rotation matrices
	mat4 Rx = rotate(mat4(1.0f), angle, vec3(1.0f, 0.0f, 0.0f));
	mat4 Ry = rotate(mat4(1.0f), angle, vec3(0.0f, 1.0f, 0.0f));
	mat4 Rz = rotate(mat4(1.0f), angle, vec3(0.0f, 0.0f, 1.0f));
	//combine them
	mat4 Rall = Rx * Ry * Rz;
	//perform a transformation
	vec3 R_v = Rall * vec4(v, 1.0f);


	//Euler Angle - uses all three axes in one operation
		//eulerAngleYXZ(YAW, PITCH, ROLL)
	mat4 R = eulerAngleYXZ(90.0f, 0.0f, 0.0f);

	//Scale Matrix
	mat4 S = scale(mat4(1.0f), vec3(5.0f, 5.0f, 5.0f));

	//Combining matrices
		//form matters
	mat4 trans = T * (R * S);

	//
	//QUATERNIONS
	//

	quat q;
		//vec3 is the axis to rotate around
	q = rotate(quat(), angle, vec3(1.0f, 0.0f, 0.0f));

	quat qRx = rotate(quat(), angle, vec3(1.0f, 0.0f, 0.0f));
	quat qRy = rotate(quat(), angle, vec3(0.0f, 1.0f, 0.0f));
	quat qRz = rotate(quat(), angle, vec3(0.0f, 0.0f, 1.0f));
	//combine them
	quat qR = qRx * qRy * qRz;

	//Convert to Matrix
	mat4 mR = mat4_cast(q);

}