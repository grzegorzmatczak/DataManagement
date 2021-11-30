#ifndef DATAMEMORY_H
#define DATAMEMORY_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio.hpp"

//#include "processing.h"
//#include "loaddata.h"
//#include "graph.h"
#include "foldermanagement.h"

#include "includespdlog.h"
#include "configreader.h"

struct image_info
{
	std::string image_filename;
	std::string gt_filename;
};

class DataMemory : public QObject
{
	Q_OBJECT

	public:
		explicit DataMemory();
		~DataMemory();
		bool configure(QJsonObject const& a_config);
		bool loadNamesOfFile();
		void countGtElements(int id);

	public: // get/set
		std::vector<struct image_info> get_imageInfoTrain() { return m_imageInfoTrain; }
		std::vector<struct image_info> get_imageInfoTest() { return m_imageInfoTest; }

		size_t getSizeClean() { return m_clean.size(); }
		size_t getSizeGt() { return m_gt.size(); }

		size_t getSizeCleanTrain() { return m_cleanTrain.size(); }
		size_t getSizeGtTrain() { return m_gtTrain.size(); }

		size_t getSizeCleanTest() { return m_cleanTest.size(); }
		size_t getSizeGtTest() { return m_gtTest.size(); }
		
		bool getLoad() { return m_loaded; };
		cv::Mat gt(int id);
		
		cv::Mat clean(int i) { return m_clean[i]; }

		cv::Mat gtTrain(int i) { return m_gtTrain[i]; }
		cv::Mat cleanTrain(int i) { return m_cleanTrain[i]; }

		cv::Mat gtTest(int i) { return m_gtTest[i]; }
		cv::Mat cleanTest(int i) { return m_cleanTest[i]; }

		std::vector<QString> names() {return m_names;}
		QString names(int i) {return m_names[i];}
		
		std::vector<QString> filenames() {return m_filenames;}
		QString filenames(int i) {return m_filenames[i];}

		std::vector<QString> paths() {return m_paths;}
		QString paths(int i) {return m_paths[i];}

		std::vector<QString> gtPaths() {return m_gtPaths;}
		QString gtPaths(int i) {return m_gtPaths[i];}

		std::vector<int> gtElements() {return m_gtElements;}
		int gtElements(int i) {return m_gtElements[i];}

	private:
		void loadConfig(QJsonObject const& a_config);
		void createSplit();
		void clearData();
		bool saveCleanData(std::vector<cv::Mat> &data);
		bool saveEmptyGt();

		bool loadStream(cv::VideoCapture videoFromFile, std::vector<cv::Mat> &data, int framesNumber);
		bool loadDataFromStream(std::vector<cv::Mat> &data, std::vector<cv::Mat> &gt);
		bool loadDataFromPath(std::vector<cv::Mat> &data, std::vector<cv::Mat> &gt);


		bool loadPath(QString path, std::vector<cv::Mat> &data, int framesNumber);
		bool loadCleanData(QString path, std::vector<cv::Mat> &data, int framesNumber);
		bool loadDataWindows(std::vector<cv::Mat> &data, std::vector<cv::Mat> &gt);


	signals:
		void memoryLoaded();


	public:
		std::vector<QString> m_names;
		std::vector<QString> m_filenames;
		std::vector<QString> m_paths;
		std::vector<QString> m_gtPaths;
		std::vector<int> m_gtElements;
		std::vector<cv::Mat> m_clean;
		std::vector<cv::Mat> m_gt;
		std::vector<cv::Mat> m_cleanTrain;
		std::vector<cv::Mat> m_cleanTest;
		std::vector<cv::Mat> m_gtTrain;
		std::vector<cv::Mat> m_gtTest;

	private:
		QString m_folderInputPath{};
		QString m_configPath{};
		QString m_cleanPath{};
		QString m_gtPath{};
		QString m_cleanTrainPath{};
		QString m_gtTrainPath{};
		QString m_cleanTestPath{};
		QString m_gtTestPath{};
		QString m_inputType{};
		QString m_outputType{};
		QString m_split{};
		QString m_videoName{};
		QJsonObject m_datasetConfig{};

	private:
		std::vector<struct image_info>  m_imageInfoTrain;
		std::vector<struct image_info>  m_imageInfoTest;
		
	private:
		int m_width{};
		int m_height{};
		bool m_loaded{};
		bool m_resize{};
		bool m_savePreprocessingDataset{};
		bool m_fromVideo{};
		int m_initFrames{};
		int m_zeroPadding{};
		int m_allFrames{};
		int m_startTrain{};
		int m_stopTrain{};
		int m_startTest{};
		int m_stopTest{};

	private:
		cv::VideoCapture m_videoFromFile;
		cv::VideoCapture m_videoFromFileGT;

};

#endif // DATAMEMORY_H
