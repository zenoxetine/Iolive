#pragma once

#include <GL/glew.h>
#include <CubismFramework.hpp>
#include <Model/CubismUserModel.hpp>
#include <ICubismModelSetting.hpp>
#include <CubismModelSettingJson.hpp>
#include <Math/CubismMatrix44.hpp>
#include <Rendering/CubismRenderer.hpp>
#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>
#include <Id/CubismIdManager.hpp>
#include "Utility.hpp"
#include "Component/TextureManager.hpp"
#include <string>
#include <future>

using namespace Csm;

class Model2D : public CubismUserModel
{
public:
	Model2D(ICubismModelSetting* modelSetting, const wchar_t* modelDir, const wchar_t* modelFilename)
	  : m_ModelSetting(nullptr),
		m_ProjectionMatrix(CubismMatrix44()),
		m_ModelDir(modelDir),
		m_ModelFileName(modelFilename)
	{
		_initialized = false;

		if (SetupModelSetting(modelSetting))
		{
			_initialized = true;
		}
	}

	~Model2D()
	{
		if (m_ModelSetting != nullptr)
			delete m_ModelSetting;
	}

	void OnUpdate(float deltaTime)
	{
		if (!_initialized || _model == NULL) return;

		// load saved parameters, ex: updated binded parameters will loaded here
		_model->LoadParameters();

		if (_pose)
		{
			_pose->UpdateParameters(_model, deltaTime);
		}

		if (_physics)
		{
			_physics->Evaluate(_model, deltaTime);
		}

		_model->Update();
	}

	void OnDraw(int width, int height)
	{
		if (!_initialized || _model == NULL) return;
		if (width < 1 || height < 1) return;

		CubismMatrix44* projectionMatrix = GetProjectionMatrix();
		projectionMatrix->Scale(
			(static_cast<float>(height) / static_cast<float>(width)),
			1
		);

		projectionMatrix->MultiplyByMatrix(_modelMatrix);

		GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->SetMvpMatrix(projectionMatrix);
		GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->DrawModel();
	}

public:
	CubismMatrix44* GetProjectionMatrix()
	{
		return &m_ProjectionMatrix;
	}

private:
	bool SetupModelSetting(ICubismModelSetting* modelSetting)
	{
		_updating = true;

		// set modeSetting as class member
		m_ModelSetting = modelSetting;

		std::future<bool> loadModel = std::async(std::launch::async, [this]() -> bool {
			// load .moc3
			wchar_t* moc3Filename = Utility::NewWideChar(m_ModelSetting->GetModelFileName());
			auto [buffer, fileSize] = Utility::CreateBufferFromFile((m_ModelDir + moc3Filename).data());
			delete[] moc3Filename;
			if (buffer)
			{
				LoadModel(reinterpret_cast<csmByte*>(buffer), fileSize);
				delete[] buffer;
				return true;
			}
			return false;
		});

		std::future<bool> loadPhysics = std::async(std::launch::async, [this]() -> bool {
			// load physics
			if (strlen(m_ModelSetting->GetPhysicsFileName()) > 0)
			{
				wchar_t* physicsFilename = Utility::NewWideChar(m_ModelSetting->GetPhysicsFileName());
				auto [buffer, fileSize] = Utility::CreateBufferFromFile((m_ModelDir + physicsFilename).data());
				delete[] physicsFilename;
				if (buffer)
				{
					LoadPhysics(reinterpret_cast<csmByte*>(buffer), fileSize);
					delete[] buffer;
					return true;
				}
			}
			return false;
		});

		std::future<bool> loadPose = std::async(std::launch::async, [this]() -> bool {
			// Load pose
			if (strlen(m_ModelSetting->GetPoseFileName()) > 0)
			{
				wchar_t* poseFilename = Utility::NewWideChar(m_ModelSetting->GetPoseFileName());
				auto [buffer, fileSize] = Utility::CreateBufferFromFile((m_ModelDir + poseFilename).data());
				delete[] poseFilename;
				if (buffer)
				{
					LoadPose(reinterpret_cast<csmByte*>(buffer), fileSize);
					delete[] buffer;
					return true;
				}
			}
			return false;
		});

		// load the texture
		bool textureErrFlags = false;
		for (csmInt32 modelTexCount = 0; modelTexCount < m_ModelSetting->GetTextureCount(); modelTexCount++)
		{
			wchar_t* textureFilename = Utility::NewWideChar(m_ModelSetting->GetTextureFileName(modelTexCount));
			if (wcslen(textureFilename) == 0)
			{
				delete[] textureFilename;
				continue;
			}

			std::wstring texturePath = m_ModelDir + textureFilename;
			delete[] textureFilename;

			if (!m_TextureManager.CreateTextureFromPngFile(texturePath.data()))
			{
				// some texture file may not found
				textureErrFlags = true;
				// breaakk, because the model will not showing or looks weird
				break;
			}
		}

		// wait until .moc3 loaded
		if (loadModel.get() != true)
		{
			// error while loading .moc3, nande? wakannai.
			return false;
		}

		if (textureErrFlags)
		{
			return false;
		}

		// create renderer first
		CreateRenderer();

		// then bind texture into model
		for (csmInt32 modelTexCount = 0; modelTexCount < m_ModelSetting->GetTextureCount(); modelTexCount++)
		{
			const GLuint textureId = m_TextureManager.GetTextureAt(modelTexCount);
			GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->BindTexture(modelTexCount, textureId);
		}
		GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(false);

		csmMap<csmString, csmFloat32> modelLayout;
		m_ModelSetting->GetLayoutMap(modelLayout);
		_modelMatrix->SetupFromLayout(modelLayout);

		loadPhysics.wait();
		loadPose.wait();

		_updating = false;

		return true;
	}

private:
	std::wstring m_ModelDir;		// absolute model path
	std::wstring m_ModelFileName;	// model file name

	ICubismModelSetting* m_ModelSetting;
	CubismMatrix44 m_ProjectionMatrix;
	TextureManager m_TextureManager;
};