#include "gtest/gtest.h"
#include "gmock/gmock.h" 

#include <QDebug>

#include "data.h"



using ::testing::AtLeast;

namespace gtest_data
{
	class GTest_data : public ::testing::Test
	{
		protected:
			GTest_data(){}
			~GTest_data() override {}
			void SetUp() override{}
			void TearDown() override {}
	};
}  // namespace gtest_data
