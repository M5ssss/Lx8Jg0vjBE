#pragma once
#ifndef __LAB_WORK_3_HPP__
#define __LAB_WORK_3_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include <vector>
#include "common/camera.hpp"

namespace M3D_ISICG
{
	class LabWork3 : public BaseLabWork
	{
	  public:
		LabWork3() : BaseLabWork() {}
		~LabWork3();

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
		Mesh					  _cube;
		Camera					  _camera;
		Mesh					  _createCube();
		void					  _updateViewMatrix();
		void					  _updateProjectionMatrix();
		void					  _updateClipSpaceMatrix();
		void					  _initCamera();
		// ================

		// ================ GL data.
		GLuint idProgram = 0;

		GLint idTranslationX	  = 0;
		GLint idTranslationY	  = 0;
		GLint idBrightness		  = 0;
		GLint idMtxTransformation = 0;
		GLint idMtxView			  = 0;
		GLint idMtxProj			  = 0;
		GLint idMtxClip			  = 0;
		// ================

		// ================ Settings.
		Vec4f _bgColor		  = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		float _time			  = 0.0f;
		float _anim_frequency = 1.0f;
		float _anim_amplitude = 0.5f;
		float _brightess	  = 1.0f;
		float _cameraSpeed		 = 0.1f;
		float _cameraSensitivity = 0.1f;
		float _fov = 60.0f;
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_3_HPP__
