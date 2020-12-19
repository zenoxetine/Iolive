#include "Ioface/Ioface.hpp"
#include <vector>

Ioface::Ioface()
  :	m_Initialized(false),
	angleX(0), angleY(0), angleZ(0)
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

	CloseCamera();
	
	m_Cap.open(deviceId);
	return m_Cap.isOpened();
}

//
// close the camera if opened
//
void Ioface::CloseCamera()
{
	cv::destroyAllWindows();
	if (m_Cap.isOpened())
		m_Cap.release();
}

void Ioface::UpdateFrame()
{
	if (!m_Cap.isOpened()) return;
	m_Cap >> m_Frame;
}

void Ioface::ShowFrame()
{
	if (!m_Cap.isOpened() || m_Frame.empty()) return;
	cv::imshow("Ioface frame", m_Frame);
}

void Ioface::UpdateParameters()
{
	if (!m_Cap.isOpened() || !m_Initialized) return;

	UpdateFrame();
	if (m_Frame.empty()) return;

	float score = 0.0f;
	static cv::Mat landmarks;
	static bool doTrackLandmarks = false;

	if (doTrackLandmarks)
	{
		// trying to track new accurate landmarks based on previous landmarks
		cv::Mat trackedLandmarks;
		if (m_FaceAlignment->Track(m_Frame, landmarks, trackedLandmarks, score) == INTRAFACE::IF_OK)
		{
			landmarks = trackedLandmarks;
		}
	}
	else
	{
		auto faceRect = DetectFirstFace(m_Frame);
		if (!faceRect.has_value()) return; // check if there's a face in the frame

		// detect face landmarks
		if (m_FaceAlignment->Detect(m_Frame, faceRect.value(), landmarks, score) == INTRAFACE::IF_OK)
		{
			// got accurated landmarks, do follow them in the next frame
			doTrackLandmarks = true;
		}
	}

	if (score > 0.5)
	{
		// draw facial landmarks to the frame
		for (int i = 0; i < landmarks.cols; i++)
		{
			cv::circle(m_Frame,
				cv::Point((int)landmarks.at<float>(0, i), (int)landmarks.at<float>(1, i)),
				1, cv::Scalar(0, 255, 0), -1
			);
		}

		// head pose estimation
		INTRAFACE::HeadPose headPose;
		m_FaceAlignment->EstimateHeadPose(landmarks, headPose);
		cv::Vec3d eav = DrawPose(m_Frame, headPose.rot, 50);
		angleY = -eav[0];
		angleX = eav[1];
		angleZ = -eav[2];
	}
	else
	{
		// don't track, because the landmarks is not reliable
		doTrackLandmarks = false;
	}
}

std::optional<cv::Rect> Ioface::DetectFirstFace(const cv::Mat& image)
{
	std::vector<cv::Rect> facesRect;
	m_FaceCascade.detectMultiScale(image, facesRect,
		1.2,
		2,
		0,
		cv::Size(169, 169)
	);

	if (facesRect.size() > 0)
		return facesRect[0];
	else
		return std::nullopt;
}

cv::Vec3d Ioface::DrawPose(cv::Mat& img, const cv::Mat& rot, float lineL)
{
	int loc[2] = { 70, 70 };
	int thickness = 2;
	int lineType = 8;

	cv::Mat P = (cv::Mat_<float>(3, 4) <<
		0, lineL, 0, 0,
		0, 0, -lineL, 0,
		0, 0, 0, -lineL);
	P = rot.rowRange(0, 2) * P;
	P.row(0) += loc[0];
	P.row(1) += loc[1];
	cv::Point p0(P.at<float>(0, 0), P.at<float>(1, 0));

	line(img, p0, cv::Point(P.at<float>(0, 1), P.at<float>(1, 1)), cv::Scalar(255, 0, 0), thickness, lineType);
	line(img, p0, cv::Point(P.at<float>(0, 2), P.at<float>(1, 2)), cv::Scalar(0, 255, 0), thickness, lineType);
	line(img, p0, cv::Point(P.at<float>(0, 3), P.at<float>(1, 3)), cv::Scalar(0, 0, 255), thickness, lineType);

	cv::Vec3d eav;
	cv::Mat tmp, tmp1, tmp2, tmp3, tmp4, tmp5;
	double _pm[12] = { rot.at<float>(0, 0), rot.at<float>(0, 1),rot.at<float>(0, 2), 0,
						rot.at<float>(1, 0), rot.at<float>(1, 1),rot.at<float>(1, 2),0,
						rot.at<float>(2, 0),rot.at<float>(2, 1),rot.at<float>(2, 2),0 };
	cv::decomposeProjectionMatrix(cv::Mat(3, 4, CV_64FC1, _pm), tmp, tmp1, tmp2, tmp3, tmp4, tmp5, eav);

	/* eav[0] : Pitch
	   eav[1] : Yaw
	   eav[2] : Roll
	*/
	return eav;
}