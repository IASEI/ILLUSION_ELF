#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "QdeRender.h"
#include "PurpleDreamElf.h"

namespace PQE 
{
	class Texture :public QdeRender
	{
	public:
		enum PQE_TEXTURE_TYPE
		{
			PQE_TEXTURE_TYPE_NOT_VALID,
			PQE_TEXTURE_TYPE_2D,
			PQE_TEXTURE_TYPE_CUBE_MAP,
			PQE_TEXTURE_TYPE_2D_DYNAMIC,
		};
	public:
		Texture(size_t ID = 0, PQE_TEXTURE_TYPE type = PQE_TEXTURE_TYPE_2D);
		Texture(PQE_TEXTURE_TYPE type);
		Texture(size_t width, size_t height, const float * data, size_t dataType, size_t srcFormat, size_t internalFormat);
		Texture(const std::vector<std::string> & skybox);
		Texture(const std::string & path, bool flip = false, bool gammaCorrection = false);

		bool Load(const std::vector<std::string> & skybox);
		bool Load(const std::string & path, bool flip = false, bool gammaCorrection = false);

		bool Use(size_t ID = 0) const;

		size_t Type2GL(PQE_TEXTURE_TYPE type) const;
		void UnBind() const;

		size_t GetID() const;
		bool IsValid() const;

	private:
		size_t id;
		PQE_TEXTURE_TYPE type;
	};
}

#endif // !_QDETEXTURE_H__


