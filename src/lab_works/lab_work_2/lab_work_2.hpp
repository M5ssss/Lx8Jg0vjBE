#ifndef __LAB_WORK_2_HPP__
#define __LAB_WORK_2_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork2 : public BaseLabWork
	{
	  public:
		LabWork2() : BaseLabWork() {}
		~LabWork2();

		bool init() override;

		void init_circle(Vec2f C, unsigned int N, float R);
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

	  private:
		// ================ Scene data.
		std::vector<Vec2f> Vertices;
		std::vector<unsigned int> Faces;
		std::vector<Vec4f> VertexColor;
		// ================

		// ================ GL data.
		GLuint idProgram  = 0;
		GLuint idVBO	  = 0;
		GLuint idColorVBO = 0;
		GLuint idVAO	  = 0;
		GLuint idEBO	  = 0;
		
		GLint idTranslationX = 0;
		GLint idTranslationY = 0;
		GLint idBrightness   = 0;
		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		float _time	   = 0.0f;
		float _anim_frequency = 1.0f;
		float _anim_amplitude = 0.5f;
		float _brightess	  = 1.0f;
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_2_HPP__
