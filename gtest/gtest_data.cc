#include "gtest_data.h"
#include "data.h"

//#define DEBUG_CONFIG

constexpr auto CONFIG{ "GeneticOptimizationModule/gtest/test.json" };
constexpr auto TEST_DATA{ "TestData" };

using ::testing::AtLeast;


namespace gtest_data {

    TEST_F(GTest_data, test_data_from_video)
    {
        Logger->set_level(static_cast<spdlog::level::level_enum>(2));
	    Logger->set_pattern("[%Y-%m-%d] [%H:%M:%S.%e] [%t] [%^%l%$] %v");
        const QJsonObject _config{{"DatasetWin32",QJsonObject{{"PathToDataset","gtest_files\\test_dataset1\\"},{"ConfigName","test1.json"}}}};

        DataMemory* dm = new DataMemory();
        dm->configure(_config);
        
        #ifdef DEBUG_CONFIG
        qDebug() << "obj:" << obj;
        #endif
        
        //EXPECT_EQ ();
    }

    TEST_F(GTest_data, test_from_files_error)
    {
        Logger->set_level(static_cast<spdlog::level::level_enum>(2));
	    Logger->set_pattern("[%Y-%m-%d] [%H:%M:%S.%e] [%t] [%^%l%$] %v");
        const QJsonObject _config{{"DatasetWin32",QJsonObject{{"PathToDataset","gtest_files\\test_dataset2\\"},{"ConfigName","test2.json"}}}};

        DataMemory* dm = new DataMemory();
        bool ret = dm->configure(_config);
        
        #ifdef DEBUG_CONFIG
        qDebug() << "obj:" << obj;
        #endif
        
        EXPECT_EQ (ret, false);
    }
}  // namespace gtest_data
