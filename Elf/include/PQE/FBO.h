#ifndef __FBO_H__
#define __FBO_H__

#include "fantastic.h"
#include "Texture.h"

namespace PQE
{
	class FBO
	{
	public:
		enum PQE_FBO_TYPE
		{
			PQE_FBO_TYPE_BASIC,
			PQE_FBO_TYPE_RGBF1_DEPTH,
			PQE_FBO_TYPE_RGBF2_DEPTH,
			PQE_FBO_TYPE_RGBF3_DEPTH,
			PQE_FBO_TYPE_MSAA,
			PQE_FBO_TYPE_COLOR,
			PQE_FBO_TYPE_RED,
			PQE_FBO_TYPE_COLOR_FLOAT,
			PQE_FBO_TYPE_DEPTH,
			PQE_FBO_TYPE_CUBE_DEPTH,
			PQE_FBO_TYPE_GBUFFER,
			PQE_FBO_TYPE_RAYTRACING,
			PQE_FBO_TYPE_RTX,
		};
		enum PQE_FBO_PASS_TYPE
		{
			PQE_FBO_PASS_COLOR,
			PQE_FBO_PASS_DEPTH,
		};
	public:
		FBO(size_t width, size_t height, PQE_FBO_TYPE type);

		bool GenFBO_BASIC(size_t width, size_t height);
		bool GenFBO_MSAA(size_t width, size_t height);
		bool GenFBO_COLOR(size_t width, size_t height, bool isFloat);
		bool GenFBO_RTX(size_t width, size_t height);
		bool GenFBO_RED(size_t width, size_t height);
		bool GenFBO_DEPTH(size_t width, size_t height);
		bool GenFBO_CUBE_DEPTH(size_t width, size_t height);
		bool GenFBO_GBUFFER(size_t width, size_t height);
		bool GenFBO_RAYTRACING(size_t width, size_t height);
		bool GenFBO_RGBF_DEPTH(size_t width, size_t height, size_t colorBufferNum);
		bool PassTo(const FBO & fbo, PQE_FBO_PASS_TYPE passType) const;
		bool PassTo(size_t fboID, size_t width, size_t height, PQE_FBO_PASS_TYPE passType) const;
		bool IsValid() const;
		bool Use()const;
		bool IsComplete() const;

		const Texture * GetDepthTexture() const;
		const Texture * GetColorTexture(size_t idx) const;

		size_t GetID() const;
		size_t PassType2GL(PQE_FBO_PASS_TYPE passType)const;

		void UseDefault();

	private:
		PQE_FBO_TYPE			type;
		size_t					id;
		size_t					width;
		size_t					height;
		std::vector<Texture>	colorTextures;
		Texture					depthTexture;
		bool					isValid;

	};
};
#endif // !__FBO_H__
