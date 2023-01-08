#pragma once
#pragma once
#pragma once
#ifndef __LAB_WORK_5_HPP__
#define __LAB_WORK_5_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork5 : public BaseLabWork
	{
	  public:
		LabWork5() : BaseLabWork() {}
		~LabWork5();

		bool init() override;

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
			GLuint					  idVBO		 = 0;
			GLuint					  idColorVBO = 0;
			GLuint					  idVAO		 = 0;
			GLuint					  idEBO		 = 0;
		};

	  private:
		// ================ Scene data.
		Camera			  _camera;
		void			  _updateClipSpaceMatrix();
		void			  _initCamera();
		TriangleMeshModel _sponzaModel;
		Vec3f			  _lightpos;
		// ================

		// ================ GL data.
		GLuint idProgram = 0;

		GLuint idMMatrix	= 0;
		GLuint idVMatrix	= 0;
		GLuint idMVMatrix	= 0;
		GLuint idMVPMatrix	= 0;
		GLuint idNormMatrix = 0;
		GLuint idVLightPos	= 0;
		GLuint idFLightPos	= 0;
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
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif
