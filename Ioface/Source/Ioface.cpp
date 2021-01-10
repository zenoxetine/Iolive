#include "Ioface/Ioface.hpp"
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>
#include <iostream>

#define L2Norm(p1, p2) std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2))
#define INGFO std::cout << "[IOFACE][DEBUG] "

Ioface::Ioface()
	: m_Initialized(false), m_IsDetected(false), m_DoDisplayErrors(true), m_WaitingFaceHasPrinted(false), TrackingDelay(0)
{
}

Ioface::~Ioface()
{
	CloseCamera();
}

void Ioface::Init()
{
	const char* fa_detection_model = "./Assets/models/DetectionModel-v1.5.bin";
	const char* fa_tracking_model = "./Assets/models/TrackingModel-v1.10.bin";

	m_XXD = std::make_unique<INTRAFACE::XXDescriptor>(4);
	m_FaceAlignment = std::make_unique<INTRAFACE::FaceAlignment>(
		fa_detection_model,
		fa_tracking_model,
		m_XXD.get()
	);

	bool error = false;

	if (m_FaceAlignment->Initialized())
	{
		LoggingFunction("[Ioface][I] IntraFace initialized\n");
	}
	else
	{
		LoggingFunction("[Ioface][E] Can't initialize IntraFace!\n");
		error = true;
	}

	m_FaceCascade = cv::CascadeClassifier("./Assets/models/haarcascade_frontalface_alt2.xml");
	if (m_FaceCascade.empty())
	{
		error = true;
		LoggingFunction("[Ioface][E] Can't load face detection model!\n");
	}
	else
	{
		LoggingFunction("[Ioface][I] Face detection model loaded\n");
	}

	m_Initialized = !error;
}

bool Ioface::OpenCamera(int deviceId)
{
	m_DoDisplayErrors = true;
	m_WaitingFaceHasPrinted = false;

	m_Cap.open(deviceId);

	cv::Mat testFrame;
	bool testSuccess = m_Cap.read(testFrame);

	if (!testSuccess || testFrame.empty())
	{
		LoggingFunction("[Ioface][E] Can't read new frame from camera!\n");

		m_Cap.release();
		return false;
	}
	else
	{
		return m_Cap.isOpened();
	}
}

void Ioface::CloseCamera()
{
	m_DoDisplayErrors = true;

	CloseAllFrame();
	if (m_Cap.isOpened())
		m_Cap.release();

	if (!m_Frame.empty())
		m_Frame.release();
}

void Ioface::PrintIofaceStatus()
{
	LoggingFunction("Status:\n\tCamera opened: %d\n", m_Cap.isOpened());
	LoggingFunction("\tFrame not empty: %d\n", !m_Frame.empty());
	LoggingFunction("\tIntraface initialized: %d\n", m_FaceAlignment->Initialized());
	LoggingFunction("\tFace detection model loaded: %d\n", !m_FaceCascade.empty());
}

void Ioface::UpdateAll()
{
	UpdateFrame();
	UpdateParameters();
}

void Ioface::UpdateFrame()
{
	if (m_Cap.isOpened())
	{
		m_Cap.read(m_Frame);
	}
	else
	{
		if (m_DoDisplayErrors)
			LoggingFunction("[Ioface][E] Can't take new frame from camera because it's not opened\n");
	}
}

void Ioface::UpdateParameters()
{
	bool errStatus = (!m_Cap.isOpened() || !m_Initialized || m_Frame.empty());
	if (errStatus)
	{
		if (m_DoDisplayErrors)
		{
			m_DoDisplayErrors = false;
			LoggingFunction("[Ioface][E] Block updating parameters!\n");
			PrintIofaceStatus();
		}
		return;
	}

	float score = 0.0f;
	static bool doTrackLandmarks = false;

	if (doTrackLandmarks)
	{
		// trying to track new accurate landmarks based on previous landmarks
		cv::Mat trackedLandmarks;
		if (m_FaceAlignment->Track(m_Frame, m_Landmarks, trackedLandmarks, score) == INTRAFACE::IF_OK)
		{
			m_Landmarks = trackedLandmarks;

			if (TrackingDelay > 0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(TrackingDelay));
			}
		}
	}
	else
	{
		if (!m_WaitingFaceHasPrinted)
		{
			LoggingFunction("[Ioface][I] Waiting for a face ...\n");
			m_WaitingFaceHasPrinted = true;
		}

		auto faceRect = DetectFirstFace(m_Frame);
		if (!faceRect.has_value()) // check is there's a face in the frame
		{
			// sleep a little bit, because detecting face again and again, is heavy.
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			return;
		}

		// detect face landmarks
		if (m_FaceAlignment->Detect(m_Frame, faceRect.value(), m_Landmarks, score) == INTRAFACE::IF_OK)
		{
			// got accurate Landmarks, do follow them in the next frame
			doTrackLandmarks = true;
		
			if (score > 0.5)
			{
				// log
				LoggingFunction("[Ioface][I] Found face!\n\n");
				m_WaitingFaceHasPrinted = false;
			}
		}
	}

	if (score > 0.5)
	{
		m_IsDetected = true;
		DoUpdateParameters();
	}
	else
	{
		// don't track, because the landmarks is not reliable
		m_IsDetected = false;
		doTrackLandmarks = false;
	}
}

void Ioface::DoUpdateParameters()
{
	// head pose estimation
	m_FaceAlignment->EstimateHeadPose(m_Landmarks, m_HeadPose);
	EstimateHeadPose(m_HeadPose);

	EstimateFeatureDistance(m_Landmarks);

	/* // face tracking experiment
	if (AngleZ > -10.0f && AngleZ < 10.0f)
	{
		if (LeftEAR > 0.21f)
		{
			int l_eye_hlx = m_Landmarks.at<float>(0, 19) * 0.99f;
			int l_eye_hrx = m_Landmarks.at<float>(0, 22) * 1.01f;
			int l_eye_top_ly = m_Landmarks.at<float>(1, 20) * 0.98f;
			int l_eye_bottom_ly = m_Landmarks.at<float>(1, 24) * 1.02f;

			cv::Mat leftEyeColored = cv::Mat(m_Frame, cv::Rect(l_eye_hlx, l_eye_top_ly, l_eye_hrx - l_eye_hlx, l_eye_bottom_ly - l_eye_top_ly)).clone();
			cv::resize(leftEyeColored, leftEyeColored, cv::Size(115*2.0f, 50*2.0f));

			leftEyeColored = cv::Mat(leftEyeColored, cv::Rect(26*2, 5*2, (115-32)*2, (50-8)*2));

			cv::Mat leftEyeGray;
			cv::cvtColor(leftEyeColored, leftEyeGray, cv::COLOR_BGR2GRAY);

			cv::threshold(leftEyeGray, leftEyeGray, 24, 255, cv::THRESH_BINARY_INV);
			cv::erode(leftEyeGray, leftEyeGray, 0, cv::Point(-1, -1), 2);
			cv::dilate(leftEyeGray, leftEyeGray, 0, cv::Point(-1, -1), 4);
			cv::medianBlur(leftEyeGray, leftEyeGray, 3);

			std::vector<std::vector<cv::Point>> contours;
			std::vector<cv::Vec4i> hierarchy;
			cv::findContours(leftEyeGray, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
			if (contours.size() > 0)
			{
				double maxArea = 0;
				int maxAreaContourId = -1;
				for (int j = 0; j < contours.size(); j++)
				{
					double newArea = cv::contourArea(contours.at(j));
					if (newArea > maxArea) {
						maxArea = newArea;
						maxAreaContourId = j;
					}
				}

				drawContours(leftEyeColored, std::vector<std::vector<cv::Point>>(1, contours[maxAreaContourId]), -1, cv::Scalar(0, 255, 0), 1, cv::LINE_8);

				cv::imshow("Left eye colored", leftEyeColored);
				cv::imshow("Left eye gray", leftEyeGray);
			}
		}
	}*/
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
	// scale landmark size based on nose height
	cv::Point pTopNose = cv::Point(landmarks.at<float>(0, 10), landmarks.at<float>(1, 10));
	cv::Point pBottomNose = cv::Point(landmarks.at<float>(0, 16), landmarks.at<float>(1, 16));
	float noseHeight = L2Norm(pTopNose, pBottomNose);
	this->DistScale = noseHeight / 80.f;
	if (DistScale > 1.0f)
	{
		DistScale = 1.f - (DistScale - 1.f);
	}
	else if (DistScale < 1.0f)
	{
		DistScale = 1.f + (1.0f - DistScale);
		if (DistScale > 1.5f)
			DistScale = 1.5f;
	}

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

	// eye brow (distance between brow & top nose)
	cv::Point pBrowLeft = cv::Point(landmarks.at<float>(0, 4), landmarks.at<float>(1, 4));
	cv::Point pBrowRight = cv::Point(landmarks.at<float>(0, 5), landmarks.at<float>(1, 5));
	this->EyeBrowLY = L2Norm(pTopNose, pBrowLeft);
	this->EyeBrowRY = L2Norm(pTopNose, pBrowRight);
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

void Ioface::ShowFrame(bool showFace)
{
	if (!m_Cap.isOpened() || m_Frame.empty()) return;
	
	cv::Mat showedFrame;

	if (showFace)
	{
		showedFrame = m_Frame;
	}
	else
	{
		showedFrame = cv::Mat(cv::Size(m_Frame.cols, m_Frame.rows), CV_8UC1, cv::Scalar(255, 255, 255));
	}
	
	DrawLandmarks(showedFrame,
		showFace ? cv::Scalar(0, 255, 0) // green
				 : cv::Scalar(0, 0, 0)   // black
	);

	cv::imshow("Showing Frame", showedFrame);
	cv::waitKey(1); // one milliseconds
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

void Ioface::DrawLandmarks(cv::Mat& frame, const cv::Scalar& pointColor)
{
	if (!m_Landmarks.empty() && m_IsDetected)
	{
		// plot facial landmarks
		for (int i = 0; i < m_Landmarks.cols; i++)
		{
			cv::circle(frame,
				cv::Point((int)m_Landmarks.at<float>(0, i), (int)m_Landmarks.at<float>(1, i)),
				1, pointColor, -1
			);
		}
	}
}

void Ioface::DrawPose(float lineL)
{
	if (!m_IsDetected && m_Landmarks.empty() && m_Frame.empty()) return;
	if (m_HeadPose.rot.empty()) return;

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