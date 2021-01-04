#pragma once

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/calib3d.hpp>
#include "intraface/FaceAlignment.h"
#include "intraface/XXDescriptor.h"
#include <memory>
#include <optional>
#include <tuple>

class Ioface
{
public:
	Ioface();
	~Ioface();

	void Init();

	/*
	* Open camera
	* return true when success
	* return false when failed
	*/
	bool OpenCamera(int deviceId = 0);
	bool IsCameraOpened() const { return m_Cap.isOpened(); }

	void CloseCamera();

	bool IsFrameEmpty() const { return m_Frame.empty(); }
	bool IsDetected() const { return m_IsDetected; }

	void UpdateAll();

	void ShowFrame();
	void CloseAllFrame();

	void SetLogFunction(void(*fpLog)(const char*, ...)) { if (fpLog) m_LogFunc = fpLog; }
	void PrintIofaceStatus();

private:
	void UpdateFrame();
	void UpdateParameters();
	void DoUpdateParameters();
	
	void DrawPose(float lineL);
	void DrawLandmarks(cv::Mat& frame);

	std::optional<cv::Rect> DetectFirstFace(const cv::Mat& image);
	void EstimateHeadPose(const INTRAFACE::HeadPose& headPose);
	void EstimateFeatureDistance(const cv::Mat& landmarks);
	std::tuple<float, float> GetEyeAspectRatio(const cv::Mat& landmarks);

public:
	// parameter properties
	float DistScale = 1.f;
	float AngleX = 0.0f;
	float AngleY = 0.0f;
	float AngleZ = 0.0f;
	float LeftEAR = 0.0f;
	float RightEAR = 0.0f;
	float EAR = 0.0f; // average of left & right EAR
	float MouthOpenY = 0.0f;
	float MouthForm = 1.0f;
	float EyeBrowLY = 0.0f;
	float EyeBrowRY = 0.0f;

private:
	bool m_Initialized;

	cv::VideoCapture m_Cap;
	cv::Mat m_Frame;
	
	cv::CascadeClassifier m_FaceCascade;
	
	std::unique_ptr<INTRAFACE::XXDescriptor> m_XXD;
	std::unique_ptr<INTRAFACE::FaceAlignment> m_FaceAlignment;
	INTRAFACE::HeadPose m_HeadPose;

	cv::Mat m_Landmarks; // 49 facial landmarks

	// log function
	void(*m_LogFunc)(const char*, ...) = [](const char*, ...) { };

	// flags
	bool m_IsDetected;
	bool m_DoDisplayErrors;
	bool m_WaitingFaceHasPrinted;
};