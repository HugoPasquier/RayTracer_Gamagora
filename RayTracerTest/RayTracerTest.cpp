#include "pch.h"
#include "CppUnitTest.h"
#include <cmath>
#include <algorithm>
#include <optional>
#include <functional>
#include "../RayTracer/RayTracer.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RayTracerTest
{

	TEST_CLASS(TestVec3)
	{
	public:

		TEST_METHOD(normSquaredNegativ)
		{
			vec3 p1 = { -5, -66, -68 };
			vec3 p2 = { 4, -8, 9 };
			vec3 p3 = { 0, -8, 9 };

			Assert::IsTrue(p1.normSquared() >= 0);
			Assert::IsTrue(p2.normSquared() >= 0);
			Assert::IsTrue(p3.normSquared() >= 0);
		}
	};


	TEST_CLASS(treeNode)
	{
	public:
		
		TEST_METHOD(addObjectsNode)
		{
		}
	};
}
