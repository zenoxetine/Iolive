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

class Ioface
{
public:
	Ioface();
	~Ioface();

	bool OpenCamera(int deviceId = 0);
	void CloseCamera();
	bool IsCameraOpened() const { return m_Cap.isOpened(); }

	void ShowFrame();

	void UpdateParameters();

private:
	void UpdateFrame();
	std::optional<cv::Rect> DetectFirstFace(const cv::Mat& image);
	cv::Vec3d DrawPose(cv::Mat& img, const cv::Mat& rot, float lineL);

public:
	// parameter properties
	float angleX;
	float angleY;
	float angleZ;

private:
	bool m_Initialized;
	cv::VideoCapture m_Cap;
	cv::Mat m_Frame;

	cv::CascadeClassifier m_FaceCascade;
	
	std::unique_ptr<INTRAFACE::XXDescriptor> m_XXD;
	std::unique_ptr<INTRAFACE::FaceAlignment> m_FaceAlignment;

};