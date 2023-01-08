#pragma once
#pragma once
#pragma once
#ifndef __LAB_WORK_6_HPP__
#define __LAB_WORK_6_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork6 : public BaseLabWork
	{
	  public:
		LabWork6() : BaseLabWork() {}
		~LabWork6();

		bool init() override;
		bool initMainProg();

		void prepareGBuffer();
		bool initGPassProg();
		bool initSPassProg();
		bool initPostProcessingProg();

		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

		struct Mesh
		{
			std::vector<Vec3f>		  Vertices;
			std::vector<Vec3f>		  VertexColor;
			std::vector<unsigned int> Faces;
			Mat4f					  MatTransformation;
			GLuint					  idVBO			= 0;
			GLuint					  idColorVBO	= 0;
			GLuint					  idVAO			= 0;
			GLuint					  idEBO			= 0;
		};

	  private:
		// ================ Scene data.
		Camera			  _camera;
		void			  _updateClipSpaceMatrix();
		void			  _initCamera();
		TriangleMeshModel _sponzaModel;
		Vec3f			  _lightpos;
		std::vector<Vec2f> _frameCoord = { {1.f,1.f}, {-1.f,1.f}, {-1.f,-1.f}, {1.f,-1.f} };
		std::vector<unsigned int> _vertexIndc = { 0, 1, 2, 2, 3, 0 };
		// ================

		// ================ GL data.
		GLuint idProgram			= 0;
		GLuint _geometryPassProgram = 0;
		GLuint _shadingPassProgram	= 0;
		GLuint _postProcessingProgram = 0;

		GLuint idMMatrix	= 0;
		GLuint idVMatrix	= 0;
		GLuint idMVMatrix	= 0;
		GLuint idMVPMatrix	= 0;
		GLuint idNormMatrix = 0;
		GLuint idVLightPos	= 0;
		GLuint idFLightPos	= 0;

		GLuint _frameVBO	= 0;
		GLuint _frameVAO	= 0;
		GLuint _frameEBO	= 0;

		GLuint _gBufferTextures[6] = {0,0,0,0,0,0};
		GLuint idFBO = 0;
		// ================

		// ================ Settings.
		Vec4f _bgColor			 = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		float _time				 = 0.0f;
		float _anim_frequency	 = 1.0f;
		float _anim_amplitude	 = 0.5f;
		float _brightess		 = 1.0f;
		float _cameraSpeed		 = 0.1f;
		float _cameraSensitivity = 0.1f;
		float _fov				 = 60.0f;
		int _selectedBuffer		 = 0;
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif
