#include "data.h"

#include "scanfiles.h"
#include "includespdlog.h"
#include "configreader.h"

#define DEBUG

constexpr auto NAME{ "Name" };
constexpr auto TYPE{ "Type" };
constexpr auto CONFIG{ "Config" };

constexpr auto WIDTH{ "Width" };
constexpr auto HEIGHT{ "Height" };
constexpr auto FROM_VIDEO{ "FromVideo" };
constexpr auto VIDEO_NAME{ "VideoName" };

constexpr auto DIR_CLEAN{ "DirectoryClean" };
constexpr auto DIR_GT{ "DirectoryGt" };
constexpr auto DIR_ROI{ "DirectoryROI" };
constexpr auto DIR_CLEAN_TRAIN{ "DirectoryCleanTrain" };
constexpr auto DIR_CLEANT_TEST{ "DirectoryCleanTest" };
constexpr auto DIR_GT_TRAIN{ "DirectoryGtTrain" };
constexpr auto DIR_GT_TEST{ "DirectoryGtTest" };
constexpr auto START_TRAIN{"StartTrain"};
constexpr auto STOP_TRAIN{"StopTrain"};
constexpr auto START_TEST{"StartTest"};
constexpr auto STOP_TEST{"StopTest"};
constexpr auto RESIZE{"Resize"};
constexpr auto INPUT_TYPE{ "InputType" };
constexpr auto OUTPUT_TYPE{ "OutputType" };
constexpr auto INPUT_PREFIX{ "InputPrefix" };
constexpr auto INITIALIZATION_FRAMES{ "InitializationFrames" };
constexpr auto ZERO_PADDING{ "ZeroPadding" };

constexpr auto PATH_TO_DATASET{ "PathToDataset" };
constexpr auto CONFIG_NAME{ "ConfigName" };
constexpr auto DATASET_UNIX{ "DatasetLinux" };
constexpr auto DATASET_WIN32{ "DatasetWin32" };
constexpr auto SAVE_PREPROCESSING_DATASET{ "SavePreprocessingDataset" };
constexpr auto ALL_FRAMES{ "AllFrames" };



DataMemory::DataMemory()
{
	#ifdef DEBUG
	Logger->debug("DataMemory::DataMemory()");
	#endif
	DataMemory::createSplit();
}

DataMemory::~DataMemory(){}

void DataMemory::createSplit()
{
	#ifdef DEBUG
	Logger->debug("DataMemory::createSplit()");
	#endif
	#ifdef _WIN32
	m_split = "\\";
	#endif // _WIN32
	#ifdef __linux__
	m_split = "/";
	#endif // _UNIX
}

cv::Mat DataMemory::gt(int id) 
{ 
	Logger->debug("DataMemory::gt(id:{})", id);
	int elements{0};
	for (int i = 0; i < m_gt[id].cols; i++)
	{
		for (int j = 0; j < m_gt[id].rows; j++)
		{
			if ((m_gt[id].at<unsigned char>(j, i) > 0 ) && (m_gt[id].at<unsigned char>(j, i) <=255))
			{
				elements++;
			}
		}
	}
	m_gtElements[id] = elements;
	Logger->debug("DataMemory::m_gtElements[i:{}]={}", id, elements);
	return m_gt[id]; 
}

void DataMemory::countGtElements(int id)
{
	Logger->debug("DataMemory::gt(id:{})", id);
	int elements{0};
	for (int i = 0; i < m_gt[id].cols; i++)
	{
		for (int j = 0; j < m_gt[id].rows; j++)
		{
			if ((m_gt[id].at<unsigned char>(j, i) > 0 ) && (m_gt[id].at<unsigned char>(j, i) <=255))
			{
				elements++;
			}
		}
	}
	m_gtElements[id] = elements;
}

void DataMemory::clearData()
{
	#ifdef DEBUG
	Logger->debug("DataMemory::clearData()");
	#endif
	m_gt.clear();
	m_clean.clear();

	m_gtTrain.clear();
	m_cleanTrain.clear();

	m_gtTest.clear();
	m_cleanTest.clear();

	m_names.clear();
	m_filenames.clear();
	m_paths.clear();
	m_gtPaths.clear();
	m_gtElements.clear();
}

void DataMemory::loadConfig(QJsonObject const& a_config)
{
	#ifdef DEBUG
	Logger->debug("DataMemory::loadConfig()");
	qDebug() << "DataMemory::loadConfig() config:"<< a_config;
	#endif
	clearData();

	#ifdef _WIN32
	QJsonObject jDataset{ a_config[DATASET_WIN32].toObject() };
	#endif // _WIN32
	#ifdef __linux__ 
	QJsonObject jDataset{ a_config[DATASET_UNIX].toObject() };
	#endif // _UNIX
	
	QString configName = jDataset[CONFIG_NAME].toString();
	m_configPath = jDataset[PATH_TO_DATASET].toString();

	auto cR = std::make_unique<ConfigReader>();
	if (!cR->readConfig(m_configPath + configName, m_datasetConfig))
	{
		Logger->error("DataMemory::configure() File {} not readed", (m_configPath + configName).toStdString());
	}

	m_cleanPath = m_datasetConfig[DIR_CLEAN].toString();
	m_gtPath = m_datasetConfig[DIR_GT].toString();

	m_roiPath = m_datasetConfig[DIR_ROI].toString();
	
	m_inputType = m_datasetConfig[INPUT_TYPE].toString();
	m_outputType = m_datasetConfig[OUTPUT_TYPE].toString();

	m_cleanTrainPath = m_datasetConfig[DIR_CLEAN_TRAIN].toString();
	m_gtTrainPath = m_datasetConfig[DIR_GT_TRAIN].toString();

	m_cleanTestPath = m_datasetConfig[DIR_CLEANT_TEST].toString();
	m_gtTestPath = m_datasetConfig[DIR_GT_TEST].toString();

	m_allFrames = m_datasetConfig[ALL_FRAMES].toInt();
	m_fromVideo = m_datasetConfig[FROM_VIDEO].toBool();
	m_videoName = m_datasetConfig[VIDEO_NAME].toString();

	m_zeroPadding = m_datasetConfig[ZERO_PADDING].toInt();

	m_height = m_datasetConfig[HEIGHT].toInt();
	m_width = m_datasetConfig[WIDTH].toInt();

	checkAndCreateFolder(m_configPath + m_cleanPath);
	checkAndCreateFolder(m_configPath + m_gtPath);
	checkAndCreateFolder(m_configPath + m_roiPath);
	checkAndCreateFolder(m_configPath + m_cleanTrainPath);
	checkAndCreateFolder(m_configPath + m_gtTrainPath);
	checkAndCreateFolder(m_configPath + m_cleanTestPath);
	checkAndCreateFolder(m_configPath + m_gtTestPath);

	m_startTrain = m_datasetConfig[START_TRAIN].toInt();
	m_stopTrain = m_datasetConfig[STOP_TRAIN].toInt();
	m_startTest = m_datasetConfig[START_TEST].toInt();
	m_stopTest = m_datasetConfig[STOP_TEST].toInt();

	#ifdef DEBUG
	Logger->debug("DataMemory::loadConfig() m_zeroPadding:{}", m_zeroPadding);
	Logger->debug("DataMemory::loadConfig() m_allFrames:{}", m_allFrames);
	Logger->debug("DataMemory::loadConfig() m_fromVideo:{}", m_fromVideo);
	Logger->debug("DataMemory::loadConfig() done");
	
	#endif
}

bool DataMemory::loadNamesOfFile()
{
	m_imageInfoTest.clear();
	m_imageInfoTrain.clear();
	
	QVector<QString> m_imgList = scanAllImages(m_configPath+m_cleanPath);
	std::sort(m_imgList.begin(), m_imgList.end());
	#ifdef DEBUG
	Logger->debug("m_imgList:{}", m_imgList.size());
	#endif
	if (m_imgList.size() > 0)
	{
		int counterTrain{0};
		int counterTest{0};
		
		for (qint32 iteration = 0; iteration < m_imgList.size(); iteration++)
		{
			QString name = m_configPath + m_cleanPath + m_split + m_imgList[iteration] + m_inputType;
			QString gt = m_configPath + m_gtTrainPath + m_split + m_imgList[iteration] + m_inputType;
			if (iteration % 100 == 0)
			{
				Logger->debug("DataMemory::loadNamesOfFile() file loading:{}/{}...", iteration, m_imgList.size());
				Logger->debug("DataMemory::loadNamesOfFile() name:{}", name.toStdString());
				Logger->debug("DataMemory::loadNamesOfFile() gt:{}", gt.toStdString());
			}
			if (iteration >= m_startTrain && iteration < m_stopTrain)
			{
				m_imageInfoTrain.push_back({ name.toStdString(), gt.toStdString() });
				counterTrain++;
			}
			else if (iteration >= m_startTest && iteration < m_stopTest)
			{
				m_imageInfoTest.push_back({ name.toStdString(), gt.toStdString() });
				counterTest++;
			}
		}
		Logger->info("DataMemory::loadNamesOfFile() file loaded:{}, train:{} test:{}", m_imgList.size(), counterTrain, counterTest);
	}
	else
	{
		Logger->warn("DataMemory::loadNamesOfFile() file non loaded{}");
		return false;
	}
	return true;
}

bool DataMemory::configure(QJsonObject const& a_config)
{
	#ifdef DEBUG
	Logger->debug("DataMemory::configure()");
	#endif
	DataMemory::loadConfig(a_config);

	if(m_fromVideo)
	{
		if(!loadDataFromStream(m_clean, m_gt))
		{
			return false;
		}
		saveCleanData(m_clean);
		saveEmptyGt();
		return true;
	}
	else
	{
		return loadDataFromPath(m_clean, m_gt);
	}
	#ifdef DEBUG
	Logger->debug("DataMemory::configure() done");
	#endif
	return false;
}


bool DataMemory::loadStream(cv::VideoCapture videoFromFile, std::vector<cv::Mat> &data, int framesNumber)
{
	#ifdef DEBUG
	Logger->debug("DataMemory::loadStream()");
	#endif
	int iter{ 0 };
	while (videoFromFile.isOpened())
	{
		cv::Mat inputMat;
		videoFromFile >> inputMat;
		if (inputMat.channels() > 1)
		{
			cv::cvtColor(inputMat, inputMat, 6);
		}

		iter++;
		if (iter > framesNumber)
		{
			#ifdef DEBUG
			Logger->debug("push_back {} images end", iter);
			#endif
			break;
		}
		if (inputMat.empty())
		{
			Logger->error("DataMemory::loadStream() failed loading stream");
			return false;
		}
		data.push_back(inputMat);
	}
	return true;
}

bool DataMemory::loadDataFromStream(std::vector<cv::Mat> &data, std::vector<cv::Mat> &gt)
{
	const QString m_data = m_configPath + m_videoName;

	#ifdef DEBUG
		Logger->debug("m_data:{}", m_data.toStdString());
	#endif
	const int ret = m_videoFromFile.open(m_data.toStdString());
	if (ret < 0)
	{
		Logger->error("input data failed to open:{}", (m_data).toStdString());
	}
	return loadStream(m_videoFromFile, data, m_allFrames);
}

bool DataMemory::loadCleanData(QString path, std::vector<cv::Mat> &data, int framesNumber)
{
	QVector<QString> m_imgList = scanAllImagesNames(path);
	std::sort(m_imgList.begin(), m_imgList.end());
	#ifdef DEBUG
		Logger->debug("m_imgList:{}", m_imgList.size());
	#endif

	if (m_imgList.size() > 0)
	{
		for (qint32 iteration = 0; iteration < m_imgList.size(); iteration++)
		{
			if (iteration % 100 == 0)
			{
				Logger->debug("loadCleanData() loaded frames:{}", iteration);
			}
			const QString name = path +  m_split + m_imgList[iteration] + m_inputType;
			const cv::Mat inputMat = cv::imread((name).toStdString(), cv::IMREAD_GRAYSCALE);
			
			if(iteration > framesNumber)
			{
				Logger->debug("DataMemory::loadCleanData() stop loading on:{} frame", iteration);
				break;
			}
			if (inputMat.empty())
			{
				return false;
			}
			data.push_back(inputMat);
			m_paths.push_back(path);
			m_names.push_back(m_imgList[iteration] );
			m_filenames.push_back(m_imgList[iteration] + m_inputType);
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool DataMemory::loadPath(QString path, std::vector<cv::Mat> &data, int framesNumber)
{
	QVector<QString> m_imgList = scanAllImagesNames(path);
	std::sort(m_imgList.begin(), m_imgList.end());
	#ifdef DEBUG
		Logger->debug("m_imgList:{}", m_imgList.size());
	#endif

	if (m_imgList.size() > 0)
	{
		for (qint32 iteration = 0; iteration < m_imgList.size(); iteration++)
		{
			if (iteration % 100 == 0)
			{
				Logger->debug("loadPath() loaded frames:{}", iteration);
			}
			const QString name = path +  m_split + m_imgList[iteration] + m_inputType;
			const cv::Mat inputMat = cv::imread((name).toStdString(), cv::IMREAD_GRAYSCALE);
			
			if(iteration > framesNumber)
			{
				Logger->debug("DataMemory::loadPath() stop loading on:{} frame", iteration);
				break;
			}
			if (inputMat.empty())
			{
				return false;
			}
			int elements{0};
			for (int i = 0; i < inputMat.cols; i++)
			{
				for (int j = 0; j < inputMat.rows; j++)
				{
					if ((inputMat.at<unsigned char>(j, i) > 0 ) && (inputMat.at<unsigned char>(j, i) <=255))
					{
						elements++;
					}
				}
			}
			m_gtElements.push_back(elements);
			m_gtPaths.push_back(path);
			data.push_back(inputMat);
			m_roiPaths.push_back(m_configPath + m_roiPath);
			m_roiElements.push_back(0);
			m_roiNames.push_back(m_imgList[iteration]+".json");
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool DataMemory::loadDataFromPath(std::vector<cv::Mat> &data, std::vector<cv::Mat> &gt)
{
	QString m_data = m_configPath + m_cleanPath + m_split;
	QString m_gt = m_configPath + m_gtPath + m_split;

	#ifdef DEBUG
		Logger->debug("DataMemory::loadDataFromPath() m_data:{}", m_data.toStdString());
		Logger->debug("DataMemory::loadDataFromPath() m_gt:{}", m_gt.toStdString());
	#endif

	if (!loadCleanData(m_data, data, m_allFrames))
	{
		Logger->error("DataMemory::loadDataFromPath() {} not loaded", m_data.toStdString());
		return false;
	}
	if (!loadPath(m_gt, gt, m_allFrames))
	{
		Logger->info("DataMemory::loadDataFromPath() {} not loaded", m_gt.toStdString());
		saveEmptyGt();
	}

	return true;
}

bool DataMemory::saveCleanData(std::vector<cv::Mat> &data)
{
	if (data.size() < m_allFrames)
	{
		Logger->error("DataMemory::saveCleanData() data.size():{} < m_allFrames:{}", data.size(), m_allFrames);
		return false;
	}
	if (data.size() > 0)
	{
		for (int i = 0; i < data.size(); i++)
		{
			const QString number = QString("%1").arg(i, m_zeroPadding, 10, QChar('0')); 
			const QString pathToSaveClean = m_configPath + m_cleanPath +  m_split + number + m_outputType;
			//Logger->trace("DataMemory::preprocess() pathToSaveClean:{}", pathToSaveClean.toStdString());
			cv::imwrite(pathToSaveClean.toStdString(), data[i]);
		}
	}
	return true;
}

bool DataMemory::saveEmptyGt()
{
	QVector<QString> m_imgList = scanAllImagesNames(m_configPath + m_cleanPath);
	std::sort(m_imgList.begin(), m_imgList.end());
	#ifdef DEBUG
		Logger->debug("DataMemory::saveEmptyGt()");
		Logger->debug("m_imgList:{}", m_imgList.size());
	#endif

	if (m_imgList.size() > 0)
	{
		for (qint32 iteration = 0; iteration < m_imgList.size(); iteration++)
		{
			if (iteration % 100 == 0)
			{
				Logger->debug("DataMemory::saveEmptyGt() loaded frames:{}", iteration);
			}
			QString name = m_configPath + m_gtPath +  m_split + m_imgList[iteration] + m_inputType;
			
			// TODO add colors later:
			cv::Mat inputMat = cv::Mat(m_height, m_width, CV_8UC1, cv::Scalar(0));
			#ifdef DEBUG
				//Logger->debug("DataMemory::saveEmptyGt() name:{}", name.toStdString().c_str());
			#endif
			cv::imwrite(name.toStdString(), inputMat);


			if(iteration > m_allFrames)
			{
				Logger->debug("DataMemory::saveEmptyGt() stop loading on:{} frame", iteration);
				break;
			}
			if (inputMat.empty())
			{
				return false;
			}
			m_gtPaths.push_back(m_configPath + m_gtPath);
			m_gt.push_back(inputMat);
			m_gtElements.push_back(0);
			m_roiPaths.push_back(m_configPath + m_roiPath);
			m_roiElements.push_back(0);
			m_roiNames.push_back(m_imgList[iteration]+".json");
		}
	}
	else
	{
		return false;
	}
}
