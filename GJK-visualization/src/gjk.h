#pragma once
/*
	Implementation of 2D GJK algorithm

*/

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
	}  GJKVec2;

	inline GJKVec2 vec2Sub(GJKVec2 a, GJKVec2 b)
	{
		GJKVec2 c;

		c.x = a.x - b.x;
		c.y = a.y - b.y;
		return c;
	}

	typedef GJKVec2 (*supportFunction)(GJKVec2);

	unsigned int GJKIntersect(supportFunction a, supportFunction b)
	{
		static GJKVec2 simplex[3];

	}




#ifdef __cplusplus
}
#endif