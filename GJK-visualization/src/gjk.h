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

	inline GJKVec2 GJKVec2Sub(GJKVec2 a, GJKVec2 b)
	{
		GJKVec2 c;

		c.x = a.x - b.x;
		c.y = a.y - b.y;
		return c;
	}

	inline float GJKVec2Dot(GJKVec2 a, GJKVec2 b)
	{
		return a.x*b.x;
	}

	inline GJKVec2 GJKVec2Neg(GJKVec2 a)
	{
		GJKVec2 c;

		c.x = a.x;
		c.y = a.y;
		return c;
	}

	// function that returns the farthest point in object in some direction
	typedef GJKVec2 (*GJKSupportFunction)(GJKVec2);

	GJKSupportFunction _GJKSupport1;
	GJKSupportFunction _GJKSupport2;
	GJKVec2 _GJKSupportOneFunction(GJKVec2 direction)
	{
		return GJKVec2Sub(_GJKSupport1(direction), _GJKSupport2(GJKVec2Neg(direction)));
	}



	/*
		TODO Come up with better name
		Supposed to be supplied with function that returns farthest point 
		in minkowski difference in some direction,
		otherwise it will check object intersection with origin
	*/
	unsigned int GJKIntersectOneFunction(GJKSupportFunction c)
	{
		int size = 1;
		GJKVec2 simplex[3];
		GJKVec2 searchDirection = { 0, 1 };
		simplex[0] = c(searchDirection); // find first point
		searchDirection = GJKVec2Neg(simplex[0]); // set searchDirection towards origin

		while (true)
		{
			simplex[size] = c(searchDirection);
			if (GJKVec2Dot(simplex[size++], searchDirection) < 0)
				return 0;

			if (size == 2) // we can build only a line
			{

			}

		}
	}

	/*
		Function with checks intersection between two supplied objects
		given by two GJKSupportFunctions a and b with must return farthest
		point in object in given direction.

		Loads given fuctions into _GJKSupport1 and _GJKSupport2
		and calls GJKIntersectOneFunction supplied with _GJKSupportOneFunction
		that uses these two functions to compute minkowski difference
		(merely substracting output from first function with second, second function
		supplied with negative direction)
	*/
	inline unsigned int GJKIntersect(GJKSupportFunction a, GJKSupportFunction b)
	{
		_GJKSupport1 = a;
		_GJKSupport2 = b;
		return GJKIntersectOneFunction(&_GJKSupportOneFunction);
	}


#ifdef __cplusplus
}
#endif