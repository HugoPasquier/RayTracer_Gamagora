#include "pch.h"
#include "CppUnitTest.h"
#include "../RayTracer/Raytracer.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RayTracerTest
{
	TEST_CLASS(RayTracerTest)
	{
	public:
		
		TEST_METHOD(TestVec3OperatorMult)
		{
			vec3 a{ 0., 0., 0. };
			vec3 b{ 0., 0., 0. };
			vec3 c{ 5., -1.2, 32.02 };
			vec3 d{ -20, 51.4, 0. };

			vec3 prod_c_d{ -100, -61.68, 0 };

			Assert::AreEqual(a, a * b);			// Two zero vectors
			Assert::AreEqual(b, c * b);			// One zero vector with a non one
			Assert::AreEqual(prod_c_d, c * d);	// Two non zero vectors
			Assert::AreEqual(prod_c_d, d * c);	// Test commutativity

		}
	};
}
