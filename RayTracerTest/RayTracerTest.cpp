#include "pch.h"
#include "CppUnitTest.h"
#include "../RayTracer/Raytracer.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RayTracerTest
{
	TEST_CLASS(RayTracerTest)
	{
	public:

		vec3 a{ 0., 0., 0. };
		vec3 b{ 0., 0., 0. };
		vec3 c{ 5., -1., 32. };
		vec3 d{ -20., 51., 0. };
		vec3 f{ 1., 0., 0. };
		vec3 g{ 3., 4., 5. };
		vec3 h{ 5., 4., 3. };


		TEST_METHOD(TestVec3OperatorEqual)
		{
			Assert::IsTrue(a == a);
			Assert::IsTrue(c == c);
			Assert::IsTrue(d == d);
			Assert::IsFalse(c == d);
			Assert::IsTrue(a == b);
		}

		TEST_METHOD(TestVec3OperatorMultFloat)
		{
			Assert::IsTrue(a * 56. == a);
			Assert::IsTrue(c * 10. == vec3{50., -10., 320.});
			Assert::IsTrue(d * 0. == a);
		}

		TEST_METHOD(TestVec3OperatorDivVec3)
		{
			Assert::IsTrue(a / 56. == a);
			Assert::IsTrue(d / 10. == vec3{ -2., 5.1, 0. });
		}

		TEST_METHOD(TestVec3OperatorMultVec3)
		{
			vec3 prod_c_d{ -100., -51., 0. };

			Assert::IsTrue(a == a * b);		// Two zero vectors
			Assert::IsTrue(b == c * b);			// One zero vector with a non one
			Assert::IsTrue(prod_c_d == (c * d));	// Two non zero vectors
			Assert::IsTrue(prod_c_d == (d * c));	// Test commutativity
		}

		TEST_METHOD(TestVec3OperatorAddVec3)
		{
			vec3 sum_c_d{ -15., 50., 32. };

			Assert::IsTrue(a == a + a);
			Assert::IsTrue(b == a + b);
			Assert::IsTrue(c == c + b);
			Assert::IsTrue(sum_c_d == (c + d));
			Assert::IsTrue(sum_c_d == (d + c));
		}

		TEST_METHOD(TestVec3OperatorSubVec3)
		{
			vec3 sub_c_d{ 25., -52., 32. };

			Assert::IsTrue(a == a - a);
			Assert::IsTrue(b == a - b);
			Assert::IsTrue(c == c - b);
			Assert::IsTrue(sub_c_d == (c - d));
			Assert::IsFalse(sub_c_d == (d - c));
		}

		TEST_METHOD(TestVec3NormSquared) 
		{
			Assert::IsTrue(a.normSquared() == 0.);
			Assert::IsTrue(f.normSquared() == 1.);
			Assert::IsTrue(g.normSquared() == h.normSquared());
			Assert::IsTrue(d.normSquared() == 3001.);
		}

		TEST_METHOD(TestVec3UnitVector)
		{
			Assert::IsTrue(a.unitVector() == a);
			Assert::IsTrue(f.unitVector() == f);
			vec3 d_u = d.unitVector();

			Assert::IsTrue(d_u.x <= 1 && d_u.y <= 1 && d_u.z <= 1);
			Assert::IsTrue(d_u.x >= -1 && d_u.y >= -1 && d_u.z >= -1);
		}

		TEST_METHOD(TestVec3Dot)
		{
			Assert::IsTrue(a.dot(b) == 0.);
			Assert::IsTrue(f.dot(a) == 0.);
			Assert::IsTrue(c.dot(d) == -151.);
			Assert::IsTrue(c.dot(d) == d.dot(c));
		}

	};
}
