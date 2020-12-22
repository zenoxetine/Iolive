#include "Ioface/Ioface.hpp"
#include <iostream>
#include <vector>
#include <cmath>

#define INGFO std::cout << "[IOFACE][INFO] "
#define L2Norm(p1, p2) std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));

Ioface::Ioface()
  :	m_Initialized(false)
{
	const char* fa_detection_model = "./Assets/models/DetectionModel-v1.5.bin";
	const char* fa_tracking_model = "./Assets/models/TrackingModel-v1.10.bin";

	m_XXD = std::make_unique<INTRAFACE::XXDescriptor>(4);
	m_FaceAlignment = std::make_unique<INTRAFACE::FaceAlignment>(
		fa_detection_model,
		fa_tracking_model,
		m_XXD.get()
	);

	if (!m_FaceAlignment->Initialized())
		return;

	m_FaceCascade = cv::CascadeClassifier("./Assets/models/haarcascade_frontalface_alt2.xml");
	if (m_FaceCascade.empty())
		return;
	
	m_Initialized = true;
}

Ioface::~Ioface()
{
	CloseCamera();
}

//
// open camera
// return true if success
// return false if failed
//
bool Ioface::OpenCamera(int deviceId)
{
	if (!m_Initialized)
		return false;

	return m_Cap.open(deviceId);
}

void Ioface::CloseCamera()
{
	CloseAllFrame();
	if (m_Cap.isOpened())
		m_Cap.release();
}

void Ioface::UpdateAll()
{
	Ioface::UpdateFrame();
	Ioface::UpdateParameters();
}

void Ioface::UpdateFrame()
{
	if (m_Cap.isOpened())
		m_Cap.read(m_Frame);
}

void Ioface::UpdateParameters()
{
	if (!m_Cap.isOpened() || !m_Initialized) return;

	// UpdateFrame();
	if (m_Frame.empty()) return;

	float score = 0.0f;
	static bool doTrackLandmarks = false;

	if (doTrackLandmarks)
	{
		// trying to track new accurate landmarks based on previous landmarks
		cv::Mat trackedLandmarks;
		if (m_FaceAlignment->Track(m_Frame, m_Landmarks, trackedLandmarks, score) == INTRAFACE::IF_OK)
		{
			m_Landmarks = trackedLandmarks;
		}
	}
	else
	{
		auto faceRect = DetectFirstFace(m_Frame);
		if (!faceRect.has_value()) return; // check if there's a face in the frame

		// detect face landmarks
		if (m_FaceAlignment->Detect(m_Frame, faceRect.value(), m_Landmarks, score) == INTRAFACE::IF_OK)
		{
			// got accurate Landmarks, do follow them in the next frame
			doTrackLandmarks = true;
		}
	}

	if (score > 0.5)
	{
		DoUpdateParameters();
	}
	else
	{
		// don't track, because the landmarks is not reliable
		doTrackLandmarks = false;
	}
}

void Ioface::DoUpdateParameters()
{
	// head pose estimation
	m_FaceAlignment->EstimateHeadPose(m_Landmarks, m_HeadPose);
	EstimateHeadPose(m_HeadPose);

	EstimateFeatureDistance(m_Landmarks);
}

void Ioface::EstimateHeadPose(const INTRAFACE::HeadPose& headPose)
{
	cv::Vec3d eav;
	cv::Mat tmp, tmp1, tmp2, tmp3, tmp4, tmp5;
	double _pm[12] = { headPose.rot.at<float>(0, 0), headPose.rot.at<float>(0, 1),headPose.rot.at<float>(0, 2), 0,
						headPose.rot.at<float>(1, 0), headPose.rot.at<float>(1, 1),headPose.rot.at<float>(1, 2), 0,
						headPose.rot.at<float>(2, 0),headPose.rot.at<float>(2, 1),headPose.rot.at<float>(2, 2), 0 };
	cv::decomposeProjectionMatrix(cv::Mat(3, 4, CV_64FC1, _pm), tmp, tmp1, tmp2, tmp3, tmp4, tmp5, eav);

	/*
		eav[0] : Pitch
		eav[1] : Yaw
		eav[2] : Roll
	*/

	this->AngleY = -eav[0];
	this->AngleX = eav[1];
	this->AngleZ = -eav[2];
}

void Ioface::EstimateFeatureDistance(const cv::Mat& landmarks)
{
	// calculate eye aspect ratio
	auto [_leftEAR, _rightEAR] = GetEyeAspectRatio(landmarks);
	this->LeftEAR = _leftEAR;
	this->RightEAR = _rightEAR;
	this->EAR = (LeftEAR + RightEAR) / 2.0f;

	// mouth open y (distance between point 44 & 47 (top & bottom mouth))
	cv::Point pCenterMouthTop = cv::Point(landmarks.at<float>(0, 44), landmarks.at<float>(1, 44));
	cv::Point pCenterMouthBottom = cv::Point(landmarks.at<float>(0, 47), landmarks.at<float>(1, 47));
	this->MouthOpenY = L2Norm(pCenterMouthTop, pCenterMouthBottom);

	// mouth form (distance between point 31 & 37 (left & right mouth))
	cv::Point pMouthLeft = cv::Point(landmarks.at<float>(0, 31), landmarks.at<float>(1, 31));
	cv::Point pMouthRight = cv::Point(landmarks.at<float>(0, 37), landmarks.at<float>(1, 37));
	this->MouthForm = L2Norm(pMouthLeft, pMouthRight);


}


std::tuple<float, float> Ioface::GetEyeAspectRatio(const cv::Mat& landmarks)
{
	/* Reference: https://www.pyimagesearch.com/2017/04/24/eye-blink-detection-opencv-python-dlib/
	*/

	// left eye
	cv::Point l_eye_top_v1 = cv::Point(landmarks.at<float>(0, 20), landmarks.at<float>(1, 20)); // vertical 1
	cv::Point l_eye_bottom_v1 = cv::Point(landmarks.at<float>(0, 24), landmarks.at<float>(1, 24));
	cv::Point l_eye_top_v2 = cv::Point(landmarks.at<float>(0, 21), landmarks.at<float>(1, 21)); // vertical 2
	cv::Point l_eye_bottom_v2 = cv::Point(landmarks.at<float>(0, 23), landmarks.at<float>(1, 23));
	cv::Point l_eye_hl = cv::Point(landmarks.at<float>(0, 19), landmarks.at<float>(1, 19)); // horizontal left
	cv::Point l_eye_hr = cv::Point(landmarks.at<float>(0, 22), landmarks.at<float>(1, 22)); // horizontal right
	float l_eye_v1_range = L2Norm(l_eye_top_v1, l_eye_bottom_v1);
	float l_eye_v2_range = L2Norm(l_eye_top_v2, l_eye_bottom_v2);
	float l_eye_h_range = L2Norm(l_eye_hl, l_eye_hr);
	float _leftEAR = (l_eye_v1_range + l_eye_v2_range) / (2.0f * l_eye_h_range);

	// right eye
	cv::Point r_eye_top_v1 = cv::Point(landmarks.at<float>(0, 26), landmarks.at<float>(1, 26));
	cv::Point r_eye_bottom_v1 = cv::Point(landmarks.at<float>(0, 30), landmarks.at<float>(1, 30));
	cv::Point r_eye_top_v2 = cv::Point(landmarks.at<float>(0, 27), landmarks.at<float>(1, 27));
	cv::Point r_eye_bottom_v2 = cv::Point(landmarks.at<float>(0, 29), landmarks.at<float>(1, 29));
	cv::Point r_eye_hl = cv::Point(landmarks.at<float>(0, 25), landmarks.at<float>(1, 25));
	cv::Point r_eye_hr = cv::Point(landmarks.at<float>(0, 28), landmarks.at<float>(1, 28));
	float r_eye_v1_range = L2Norm(r_eye_top_v1, r_eye_bottom_v1);
	float r_eye_v2_range = L2Norm(r_eye_top_v2, r_eye_bottom_v2);
	float r_eye_h_range = L2Norm(r_eye_hl, r_eye_hr);
	float _rightEAR = (r_eye_v1_range + r_eye_v2_range) / (2.0f * r_eye_h_range);

	return { _leftEAR, _rightEAR };
}

void Ioface::ShowFrame()
{
	if (!m_Cap.isOpened() || m_Frame.empty()) return;
	// cv::imshow("Ioface frame", m_Frame);
}

void Ioface::CloseAllFrame()
{
	cv::destroyAllWindows();
}

std::optional<cv::Rect> Ioface::DetectFirstFace(const cv::Mat& image)
{
	std::vector<cv::Rect> facesRect;
	m_FaceCascade.detectMultiScale(image, facesRect,
		1.2,
		2,
		0,
		cv::Size(150, 150) // the bigger the lighter, but can't see smoll face
	);

	if (facesRect.size() > 0)
		return facesRect[0];
	else
		return std::nullopt;
}

void Ioface::DrawPose(float lineL)
{
	int loc[2] = { 70, 70 };
	int thickness = 2;
	int lineType = 8;

	cv::Mat P = (cv::Mat_<float>(3, 4) <<
		0, lineL, 0, 0,
		0, 0, -lineL, 0,
		0, 0, 0, -lineL);
	P = m_HeadPose.rot.rowRange(0, 2) * P;
	P.row(0) += loc[0];
	P.row(1) += loc[1];
	cv::Point p0(P.at<float>(0, 0), P.at<float>(1, 0));

	line(m_Frame, p0, cv::Point(P.at<float>(0, 1), P.at<float>(1, 1)), cv::Scalar(255, 0, 0), thickness, lineType);
	line(m_Frame, p0, cv::Point(P.at<float>(0, 2), P.at<float>(1, 2)), cv::Scalar(0, 255, 0), thickness, lineType);
	line(m_Frame, p0, cv::Point(P.at<float>(0, 3), P.at<float>(1, 3)), cv::Scalar(0, 0, 255), thickness, lineType);
}