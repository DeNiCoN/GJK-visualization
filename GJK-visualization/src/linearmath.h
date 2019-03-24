#pragma once
#include <xmmintrin.h>
#include <pmmintrin.h>
#include <stdio.h>
#include <math.h>
#include <smmintrin.h>

#define S_INLINE static inline

#define SHUFFLE_PARAM(x, y, z, w) \
((x) | ((y) << 2) | ((z) << 4) | ((w) << 6))

// vec(0, 1, 2, 3) -> (vec[x], vec[y], vec[z], vec[w])
#define VecSwizzle(vec, x,y,z,w)           _mm_shuffle_ps(vec, vec, SHUFFLE_PARAM(x,y,z,w))
#define VecSwizzle1(vec, x)                _mm_shuffle_ps(vec, vec, SHUFFLE_PARAM(x,x,x,x))
// special swizzle
#define VecSwizzle_0101(vec)               _mm_movelh_ps(vec, vec)
#define VecSwizzle_2323(vec)               _mm_movehl_ps(vec, vec)
#define VecSwizzle_0022(vec)               _mm_moveldup_ps(vec)
#define VecSwizzle_1133(vec)               _mm_movehdup_ps(vec)

// return (vec1[x], vec1[y], vec2[z], vec2[w])
#define VecShuffle(vec1, vec2, x,y,z,w)    _mm_shuffle_ps(vec1, vec2, SHUFFLE_PARAM(x,y,z,w))
// special shuffle
#define VecShuffle_0101(vec1, vec2)        _mm_movelh_ps(vec1, vec2)
#define VecShuffle_2323(vec1, vec2)        _mm_movehl_ps(vec2, vec1)

#ifdef __cplusplus
extern "C" {
#endif

	typedef __declspec(align(8)) union
	{
		struct {
			float x;
			float y;
		};

		float value[2];
	}  vec2;

	typedef union
	{
		struct
		{
			float x;
			float y;
			float z;
		};

		float value[3];
	}  vec3;

	typedef __declspec(align(16)) union
	{

		struct
		{
			union
			{
				vec3 xyz;
				struct
				{
					float x;
					float y;
					float z;
				};
			};
			float w;
		};


		float value[4];

		__m128 ssevalue;

	}  vec4;

	typedef vec4 quat;

	typedef __declspec(align(16))
		union
		{
			vec4 value[4];
			float m[4][4];
			__m128 mVec[4];
		} mat44;

	S_INLINE vec2 vec2Create(float x, float y) {
		return { x, y };
	}

	S_INLINE vec3 vec3Create(float x, float y, float z) {
		return { x, y , z };
	}

	S_INLINE vec4 vec4CreateSse(float x, float y, float z, float w) {
		vec4 c;
		c.ssevalue = _mm_set_ps(w, z, y, x);
		return c;
	}

	S_INLINE vec4 vec4Create(float x, float y, float z, float w) {
		return { x, y, z, w };
	}

	S_INLINE vec2 vec2Neg(vec2 a)
	{
		vec2 c;
		c.x = -a.x;
		c.y = -a.y;
		return c;
	}

	S_INLINE vec3 vec3Neg(vec3 a)
	{
		vec3 c;
		c.x = -a.x;
		c.y = -a.y;
		c.z = -a.z;
		return c;
	}

	S_INLINE vec4 vec4Neg(vec4 a)
	{
		vec4 c;
		c.ssevalue = _mm_mul_ps(a.ssevalue, _mm_set1_ps(-1));
		return c;
	}

	S_INLINE vec2 vec2Add(vec2 a, vec2 b)
	{
		vec2 c;

		c.x = a.x + b.x;
		c.y = a.y + b.y;
		return c;
	}

	S_INLINE vec3 vec3Add(vec3 a, vec3 b)
	{
		vec3 c;

		c.x = a.x + b.x;
		c.y = a.y + b.y;
		c.z = a.z + b.z;
		return c;
	}

	S_INLINE vec4 vec4Add(vec4 a, vec4 b)
	{
		vec4 c;
		c.ssevalue = _mm_add_ps(a.ssevalue, b.ssevalue);
		return c;
	}

	S_INLINE vec2 vec2AddScalar(vec2 a, float scalar)
	{
		vec2 c;

		c.x = a.x + scalar;
		c.y = a.y + scalar;
		return c;
	}

	S_INLINE vec3 vec3AddScalar(vec3 a, float scalar)
	{
		vec3 c;

		c.x = a.x + scalar;
		c.y = a.y + scalar;
		c.z = a.z + scalar;
		return c;
	}

	S_INLINE vec4 vec4AddScalar(vec4 a, float scalar)
	{
		vec4 c;

		c.ssevalue = _mm_add_ps(a.ssevalue, _mm_set_ps1(scalar));
		return c;
	}

	S_INLINE vec2 vec2Sub(vec2 a, vec2 b)
	{
		vec2 c;

		c.x = a.x - b.x;
		c.y = a.y - b.y;
		return c;
	}

	S_INLINE vec3 vec3Sub(vec3 a, vec3 b)
	{
		vec3 c;

		c.x = a.x - b.x;
		c.y = a.y - b.y;
		c.z = a.z - b.z;
		return c;
	}

	S_INLINE vec4 vec4Sub(vec4 a, vec4 b)
	{
		vec4 c;
		c.ssevalue = _mm_sub_ps(a.ssevalue, b.ssevalue);
		return c;
	}

	S_INLINE vec2 vec2Mul(vec2 a, vec2 b)
	{
		vec2 c;

		c.x = a.x * b.x;
		c.y = a.y * b.y;
		return c;
	}

	S_INLINE vec3 vec3Mul(vec3 a, vec3 b)
	{
		vec3 c;

		c.x = a.x * b.x;
		c.y = a.y * b.y;
		c.z = a.z * b.z;
		return c;
	}

	S_INLINE vec4 vec4Mul(vec4 a, vec4 b)
	{
		vec4 c;
		c.ssevalue = _mm_mul_ps(a.ssevalue, b.ssevalue);
		return c;
	}

	S_INLINE vec2 vec2Div(vec2 a, vec2 b)
	{
		vec2 c;

		c.x = a.x / b.x;
		c.y = a.y / b.y;
		return c;
	}

	S_INLINE vec3 vec3Div(vec3 a, vec3 b)
	{
		vec3 c;

		c.x = a.x / b.x;
		c.y = a.y / b.y;
		c.z = a.z / b.z;
		return c;
	}

	S_INLINE vec4 vec4Div(vec4 a, vec4 b)
	{
		vec4 c;
		c.ssevalue = _mm_div_ps(a.ssevalue, b.ssevalue);
		return c;
	}

	S_INLINE float vec2Dot(vec2 a, vec2 b)
	{
		return a.x * b.x + a.y * b.y;
	}

	S_INLINE float vec3Dot(vec3 a, vec3 b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	S_INLINE float vec4Dot(vec4 a, vec4 b)
	{
		float result;
		//multiply, sum and store in result
		__m128 tmp = _mm_mul_ps(a.ssevalue, b.ssevalue);
		tmp = _mm_hadd_ps(tmp, tmp);
		tmp = _mm_hadd_ps(tmp, tmp);
		_mm_store_ss(&result, tmp);
		return result;
	}

	S_INLINE float vec2LengthSquared(vec2 a)
	{
		return vec2Dot(a, a);
	}

	S_INLINE float vec3LengthSquared(vec3 a)
	{
		return vec3Dot(a, a);
	}

	S_INLINE float vec4LengthSquared(vec4 a)
	{
		return vec4Dot(a, a);
	}

	S_INLINE float vec2Length(vec2 a)
	{
		return sqrtf(a.x * a.x + a.y * a.y);
	}

	S_INLINE float vec3Length(vec3 a)
	{
		return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
	}

	S_INLINE float vec4Length(vec4 a)
	{
		return sqrtf(vec4LengthSquared(a));
	}
	//return z coordinate of 3d vector perpendicular to 2d vectors a and b 
	S_INLINE float vec2Cross(vec2 a, vec2 b)
	{
		return a.x*b.y - a.y*b.x;
	}

	S_INLINE vec3 vec3Cross(vec3 a, vec3 b)
	{
		vec3 c;
		c.x = a.y*b.z - a.z*b.y;
		c.y = a.z*b.x - a.x*b.z;
		c.z = a.x*b.y - a.y*b.x;
		return c;
	}

	S_INLINE vec2 vec2Scale(vec2 a, float scalar)
	{
		vec2 c;
		c.x = a.x * scalar;
		c.y = a.y * scalar;
		return c;
	}

	S_INLINE vec3 vec3Scale(vec3 a, float scalar)
	{
		vec3 c;
		c.x = a.x * scalar;
		c.y = a.y * scalar;
		c.z = a.z * scalar;
		return c;
	}

	S_INLINE vec4 vec4Scale(vec4 a, float scalar)
	{
		vec4 c;
		__m128 multiplier = _mm_set1_ps(scalar);

		c.ssevalue = _mm_mul_ps(a.ssevalue, multiplier);
		return c;
	}

	S_INLINE vec2 vec2Decreace(vec2 a, float scalar)
	{
		vec2 c;
		c.x = a.x / scalar;
		c.y = a.y / scalar;
		return c;
	}

	S_INLINE vec3 vec3Decreace(vec3 a, float scalar)
	{
		vec3 c;
		c.x = a.x / scalar;
		c.y = a.y / scalar;
		c.z = a.z / scalar;
		return c;
	}

	S_INLINE vec4 vec4Decreace(vec4 a, float scalar)
	{
		vec4 c;
		__m128 multiplier = _mm_set1_ps(scalar);

		c.ssevalue = _mm_div_ps(a.ssevalue, multiplier);
		return c;
	}

	S_INLINE vec2 vec2Normalize(vec2 a)
	{
		return vec2Decreace(a, vec2Length(a));
	}

	S_INLINE vec3 vec3Normalize(vec3 a)
	{
		return vec3Decreace(a, vec3Length(a));
	}

	S_INLINE vec4 vec4Normalize(vec4 a)
	{
		return vec4Decreace(a, vec4Length(a));
	}

	S_INLINE mat44 mat44Identity(float diagonal)
	{
		mat44 c;
		c.value[0] = vec4Create(diagonal, 0, 0, 0);
		c.value[1] = vec4Create(0, diagonal, 0, 0);
		c.value[2] = vec4Create(0, 0, diagonal, 0);
		c.value[3] = vec4Create(0, 0, 0, diagonal);
		return c;
	}

	S_INLINE mat44 mat44Transpose(mat44 a)
	{
		_MM_TRANSPOSE4_PS(a.value[0].ssevalue, a.value[1].ssevalue, a.value[2].ssevalue, a.value[3].ssevalue);
		return a;
	}

	S_INLINE mat44 mat44ScaleUniform(float scalar)
	{
		mat44 c;
		c.value[0] = vec4Create(scalar, 0, 0, 0);
		c.value[1] = vec4Create(0, scalar, 0, 0);
		c.value[2] = vec4Create(0, 0, scalar, 0);
		c.value[3] = vec4Create(0, 0, 0, 1);
		return c;
	}

	S_INLINE mat44 mat44Scale(float x, float y, float z)
	{
		mat44 c;
		c.value[0] = vec4Create(x, 0, 0, 0);
		c.value[1] = vec4Create(0, y, 0, 0);
		c.value[2] = vec4Create(0, 0, z, 0);
		c.value[3] = vec4Create(0, 0, 0, 1);
		return c;
	}

	S_INLINE mat44 mat44Translate(vec3 a)
	{
		mat44 c = mat44Identity(1.0f);
		c.value[3].xyz = a;
		return c;
	}
	S_INLINE mat44 mat44TranslateVec2(vec2 a)
	{
		mat44 c = mat44Identity(1.0f);
		c.value[3].value[0] = a.x;
		c.value[3].value[1] = a.y;
		return c;
	}

	S_INLINE mat44 mat44RotateByX(float radians)
	{
		mat44 c = mat44Identity(1.0f);
		c.value[1].value[1] = cosf(radians);
		c.value[2].value[2] = cosf(radians);
		c.value[1].value[2] = -sinf(radians);
		c.value[2].value[1] = sinf(radians);
		return c;
	}

	S_INLINE mat44 mat44RotateByY(float radians)
	{
		mat44 c = mat44Identity(1.0f);
		c.value[0].value[0] = cosf(radians);
		c.value[2].value[2] = cosf(radians);
		c.value[0].value[2] = sinf(radians);
		c.value[2].value[0] = -sinf(radians);
		return c;
	}

	S_INLINE mat44 mat44RotateByZ(float radians)
	{
		mat44 c = mat44Identity(1.0f);
		c.value[0].value[0] = cosf(radians);
		c.value[1].value[1] = cosf(radians);
		c.value[0].value[1] = -sinf(radians);
		c.value[1].value[0] = sinf(radians);
		return mat44Transpose(c);
	}

	S_INLINE mat44 mat44Orto(float right, float left, float top, float bottom, float Near, float Far)
	{
		mat44 c = mat44Identity(1.0f);
		c.value[0].value[0] = 2 / (right - left);
		c.value[3].value[0] = -((right + left) / (right - left));
		c.value[1].value[1] = 2 / (top - bottom);
		c.value[3].value[1] = -((top + bottom) / (top - bottom));
		c.value[2].value[2] = 2 / (Far - Near);
		c.value[3].value[2] = -((Far + Near) / (Far - Near));
		c.value[3].value[3] = 1;
		return c;
	}

	S_INLINE mat44 mat44Add(const mat44 * a, const mat44 * b)
	{
		mat44 c;
		c.value[0].ssevalue = _mm_add_ps(a->mVec[0], b->mVec[0]);
		c.value[1].ssevalue = _mm_add_ps(a->mVec[1], b->mVec[1]);
		c.value[2].ssevalue = _mm_add_ps(a->mVec[2], b->mVec[2]);
		c.value[3].ssevalue = _mm_add_ps(a->mVec[3], b->mVec[3]);
	}

	S_INLINE __m128 sseVecMat44Multiply(__m128 a, const mat44 * b)
	{
		__m128 c = _mm_mul_ps(_mm_shuffle_ps(a, a, SHUFFLE_PARAM(0, 0, 0, 0)), b->mVec[0]);
		c = _mm_add_ps(c, _mm_mul_ps(_mm_shuffle_ps(a, a, SHUFFLE_PARAM(1, 1, 1, 1)), b->mVec[1]));
		c = _mm_add_ps(c, _mm_mul_ps(_mm_shuffle_ps(a, a, SHUFFLE_PARAM(2, 2, 2, 2)), b->mVec[2]));
		c = _mm_add_ps(c, _mm_mul_ps(_mm_shuffle_ps(a, a, SHUFFLE_PARAM(3, 3, 3, 3)), b->mVec[3]));
		return c;
	}

	S_INLINE mat44 mat44Multiply(const mat44 * a,const mat44 * b)
	{
		mat44 c;
		c.mVec[0] = sseVecMat44Multiply(b->mVec[0], a);
		c.mVec[1] = sseVecMat44Multiply(b->mVec[1], a);
		c.mVec[2] = sseVecMat44Multiply(b->mVec[2], a);
		c.mVec[3] = sseVecMat44Multiply(b->mVec[3], a);
		return c;
	}

	S_INLINE quat quatFromVec3(vec3 a, float radians)
	{
		quat c;
		float tmp = radians / 2;
		c.xyz = vec3Scale(a, sinf(tmp));
		c.w = cosf(tmp);
	}

	S_INLINE quat quatFromVec4V2(vec4 a, float radians)
	{
		quat c;
		float tmp = radians / 2;
		float sin = sinf(tmp);
		__m128 mul = _mm_set_ps(0, sin, sin, sin);
		__m128 add = _mm_set_ps(cosf(tmp), 0, 0, 0);
		c.ssevalue = _mm_add_ps(_mm_mul_ps(a.ssevalue, mul), add);
		return c;
	}

	S_INLINE quat quatNormalize(quat a)
	{
		return vec4Normalize(a);
	}

	S_INLINE quat quatConjugate(quat a)
	{
		a.xyz = vec3Neg(a.xyz);
		return a;
	}

	S_INLINE quat quatConjugateSse(quat a)
	{
		__m128 tmp = _mm_set_ps(1, -1, -1, -1);
		a.ssevalue = _mm_mul_ps(a.ssevalue, tmp);
		return a;
	}

	S_INLINE quat quatMultiply(quat a, quat b)
	{
		quat c;

		__m128 wzyx = _mm_shuffle_ps(a.ssevalue, a.ssevalue, _MM_SHUFFLE(0, 1, 2, 3));
		__m128 baba = _mm_shuffle_ps(b.ssevalue, b.ssevalue, _MM_SHUFFLE(0, 1, 0, 1));
		__m128 dcdc = _mm_shuffle_ps(b.ssevalue, b.ssevalue, _MM_SHUFFLE(2, 3, 2, 3));

		__m128 ZnXWY = _mm_hsub_ps(_mm_mul_ps(a.ssevalue, baba), _mm_mul_ps(wzyx, dcdc));
		__m128 XZYnW = _mm_hadd_ps(_mm_mul_ps(a.ssevalue, dcdc), _mm_mul_ps(wzyx, baba));
		__m128 XZWY = _mm_addsub_ps(_mm_shuffle_ps(XZYnW, ZnXWY, _MM_SHUFFLE(3, 2, 1, 0)), _mm_shuffle_ps(ZnXWY, XZYnW, _MM_SHUFFLE(2, 3, 0, 1)));

		c.ssevalue = _mm_shuffle_ps(XZWY, XZWY, _MM_SHUFFLE(2, 1, 3, 0));

		return c;
	}

	S_INLINE vec3 quatVec2Rotate(quat a, vec2 b)
	{
		quat tmp;
		tmp.ssevalue = _mm_set_ps(0, 0, b.y, b.x);
		tmp = quatMultiply(a, tmp);
		quat conjugate = quatConjugateSse(a);
		tmp = quatMultiply(tmp, conjugate);
		vec3 c = { tmp.x, tmp.y, tmp.z };
		return c;
	}

	S_INLINE vec3 quat_vec3_rotate(quat a, vec3 b)
	{
		quat tmp;
		tmp.ssevalue = _mm_set_ps(0, b.z, b.x, b.y);
		tmp = quatMultiply(a, tmp);
		quat conjugate = quatConjugateSse(a);
		tmp = quatMultiply(tmp, conjugate);
		vec3 c = { tmp.x, tmp.y, tmp.z };
		return c;
	}

	__forceinline __m128 Mat2Mul(__m128 vec1, __m128 vec2)
	{
		return
			_mm_add_ps(_mm_mul_ps(vec1, VecSwizzle(vec2, 0, 3, 0, 3)),
				_mm_mul_ps(VecSwizzle(vec1, 1, 0, 3, 2), VecSwizzle(vec2, 2, 1, 2, 1)));
	}
	// 2x2 row major Matrix adjugate multiply (A#)*B
	__forceinline __m128 Mat2AdjMul(__m128 vec1, __m128 vec2)
	{
		return
			_mm_sub_ps(_mm_mul_ps(VecSwizzle(vec1, 3, 3, 0, 0), vec2),
				_mm_mul_ps(VecSwizzle(vec1, 1, 1, 2, 2), VecSwizzle(vec2, 2, 3, 0, 1)));

	}
	// 2x2 row major Matrix multiply adjugate A*(B#)
	__forceinline __m128 Mat2MulAdj(__m128 vec1, __m128 vec2)
	{
		return
			_mm_sub_ps(_mm_mul_ps(vec1, VecSwizzle(vec2, 3, 0, 3, 0)),
				_mm_mul_ps(VecSwizzle(vec1, 1, 0, 3, 2), VecSwizzle(vec2, 2, 1, 2, 1)));
	}

#define SMALL_NUMBER		(1.e-8f)

	inline mat44 mat44TransformInverse(const mat44 * inM)
	{
		mat44 r;

		// transpose 3x3, we know m03 = m13 = m23 = 0
		__m128 t0 = VecShuffle_0101(inM->mVec[0], inM->mVec[1]); // 00, 01, 10, 11
		__m128 t1 = VecShuffle_2323(inM->mVec[0], inM->mVec[1]); // 02, 03, 12, 13
		r.mVec[0] = VecShuffle(t0, inM->mVec[2], 0, 2, 0, 3); // 00, 10, 20, 23(=0)
		r.mVec[1] = VecShuffle(t0, inM->mVec[2], 1, 3, 1, 3); // 01, 11, 21, 23(=0)
		r.mVec[2] = VecShuffle(t1, inM->mVec[2], 0, 2, 2, 3); // 02, 12, 22, 23(=0)

		// (SizeSqr(mVec[0]), SizeSqr(mVec[1]), SizeSqr(mVec[2]), 0)
		__m128 sizeSqr;
		sizeSqr = _mm_mul_ps(r.mVec[0], r.mVec[0]);
		sizeSqr = _mm_add_ps(sizeSqr, _mm_mul_ps(r.mVec[1], r.mVec[1]));
		sizeSqr = _mm_add_ps(sizeSqr, _mm_mul_ps(r.mVec[2], r.mVec[2]));

		__m128 one = _mm_set1_ps(1.f);
		__m128 rSizeSqr = _mm_div_ps(one, sizeSqr);

		r.mVec[0] = _mm_mul_ps(r.mVec[0], rSizeSqr);
		r.mVec[1] = _mm_mul_ps(r.mVec[1], rSizeSqr);
		r.mVec[2] = _mm_mul_ps(r.mVec[2], rSizeSqr);

		// last line
		r.mVec[3] = _mm_mul_ps(r.mVec[0], VecSwizzle1(inM->mVec[3], 0));
		r.mVec[3] = _mm_add_ps(r.mVec[3], _mm_mul_ps(r.mVec[1], VecSwizzle1(inM->mVec[3], 1)));
		r.mVec[3] = _mm_add_ps(r.mVec[3], _mm_mul_ps(r.mVec[2], VecSwizzle1(inM->mVec[3], 2)));
		r.mVec[3] = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), r.mVec[3]);

		return r;
	}

	inline mat44 mat44GeneralInverse(const mat44 * inM)
	{
		// use block matrix method
		// A is a matrix, then i(A) or iA means inverse of A, A# (or A_ in code) means adjugate of A, |A| (or detA in code) is determinant, tr(A) is trace

		// sub matrices
		__m128 A = VecShuffle_0101(inM->mVec[0], inM->mVec[1]);
		__m128 B = VecShuffle_2323(inM->mVec[0], inM->mVec[1]);
		__m128 C = VecShuffle_0101(inM->mVec[2], inM->mVec[3]);
		__m128 D = VecShuffle_2323(inM->mVec[2], inM->mVec[3]);

		__m128 detA = _mm_set1_ps(inM->m[0][0] * inM->m[1][1] - inM->m[0][1] * inM->m[1][0]);
		__m128 detB = _mm_set1_ps(inM->m[0][2] * inM->m[1][3] - inM->m[0][3] * inM->m[1][2]);
		__m128 detC = _mm_set1_ps(inM->m[2][0] * inM->m[3][1] - inM->m[2][1] * inM->m[3][0]);
		__m128 detD = _mm_set1_ps(inM->m[2][2] * inM->m[3][3] - inM->m[2][3] * inM->m[3][2]);

		// let iM = 1/|M| * | X  Y |
		//                  | Z  W |

		// D#C
		__m128 D_C = Mat2AdjMul(D, C);
		// A#B
		__m128 A_B = Mat2AdjMul(A, B);
		// X# = |D|A - B(D#C)
		__m128 X_ = _mm_sub_ps(_mm_mul_ps(detD, A), Mat2Mul(B, D_C));
		// W# = |A|D - C(A#B)
		__m128 W_ = _mm_sub_ps(_mm_mul_ps(detA, D), Mat2Mul(C, A_B));

		// |M| = |A|*|D| + ... (continue later)
		__m128 detM = _mm_mul_ps(detA, detD);

		// Y# = |B|C - D(A#B)#
		__m128 Y_ = _mm_sub_ps(_mm_mul_ps(detB, C), Mat2MulAdj(D, A_B));
		// Z# = |C|B - A(D#C)#
		__m128 Z_ = _mm_sub_ps(_mm_mul_ps(detC, B), Mat2MulAdj(A, D_C));

		// |M| = |A|*|D| + |B|*|C| ... (continue later)
		detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));

		// tr((A#B)(D#C))
		__m128 tr = _mm_mul_ps(A_B, VecSwizzle(D_C, 0, 2, 1, 3));
		tr = _mm_hadd_ps(tr, tr);
		tr = _mm_hadd_ps(tr, tr);
		// |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C)
		detM = _mm_sub_ps(detM, tr);

		const __m128 adjSignMask = _mm_setr_ps(1.f, -1.f, -1.f, 1.f);
		// (1/|M|, -1/|M|, -1/|M|, 1/|M|)
		__m128 rDetM = _mm_div_ps(adjSignMask, detM);

		X_ = _mm_mul_ps(X_, rDetM);
		Y_ = _mm_mul_ps(Y_, rDetM);
		Z_ = _mm_mul_ps(Z_, rDetM);
		W_ = _mm_mul_ps(W_, rDetM);

		mat44 r;

		// apply adjugate and store, here we combine adjugate shuffle and store shuffle
		r.mVec[0] = VecShuffle(X_, Y_, 3, 1, 3, 1);
		r.mVec[1] = VecShuffle(X_, Y_, 2, 0, 2, 0);
		r.mVec[2] = VecShuffle(Z_, W_, 3, 1, 3, 1);
		r.mVec[3] = VecShuffle(Z_, W_, 2, 0, 2, 0);

		return r;
	}

#ifdef __cplusplus
}
#endif
