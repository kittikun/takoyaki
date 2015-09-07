#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "framework.h"

namespace UnitTests
{
    TEST_CLASS(UnitTest1)
    {
    public:
        TEST_METHOD(TestMethod1)
        {
            Takoyaki::Framework frmk;

            Assert::AreEqual(1, 1);
        }
    };
}