#include "Texture.h"
namespace PQE
{
	Texture::Texture(size_t ID, PQE_TEXTURE_TYPE type) : id(ID), type(type){}
	Texture::Texture(PQE_TEXTURE_TYPE type) : Texture(0, type) { }
	Texture::Texture(size_t width, size_t height, const float * data, size_t dataType, size_t srcFormat, size_t internalFormat)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, srcFormat, dataType, data);
		//glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		type = PQE_TEXTURE_TYPE_2D;
	}
	Texture::Texture(const std::vector<std::string> & skybox)
	{
		if (skybox.size() != 6)
		{
			id = 0;
			type = PQE_TEXTURE_TYPE_NOT_VALID;
			return;
		}
		Load(skybox);
	}
	Texture::Texture(const std::string & path, bool flip, bool gammaCorrection )
	{
		type = PQE_TEXTURE_TYPE_NOT_VALID;
		Load(path, flip, gammaCorrection);
	}
	
	size_t Texture::GetID() const
	{
		return id;
	}
	size_t Texture::Type2GL(PQE_TEXTURE_TYPE type) const
	{
		switch (type)
		{
		case PQE_TEXTURE_TYPE_NOT_VALID:
			return 0;
		case PQE_TEXTURE_TYPE_2D:
		case PQE_TEXTURE_TYPE_2D_DYNAMIC:
			return GL_TEXTURE_2D;
		case PQE_TEXTURE_TYPE_CUBE_MAP:
			return GL_TEXTURE_CUBE_MAP;
		default:
			return 0;
		}
	}

	bool Texture::Load(const std::vector<std::string> & skybox)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < skybox.size(); i++)
		{
			unsigned char *data = stbi_load(skybox[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << skybox[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		type = PQE_TEXTURE_TYPE_CUBE_MAP;
		UnBind();
		return true;
	}
	bool Texture::Load(const std::string & path, bool flip , bool gammaCorrection )
	{
		if (path == "")
			return false;
		int width, height, nrComponents;
		unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}
		type = PQE_TEXTURE_TYPE_2D;
		UnBind();
		return true;
	}
	bool Texture::IsValid() const
	{
		return id != 0 && type != PQE_TEXTURE_TYPE_NOT_VALID;
	}
	bool Texture::Use(size_t ID) const
	{
		if (!IsValid())
			return false;

		glActiveTexture(GL_TEXTURE0 + ID);
		glBindTexture(Type2GL(type), id);
		return true;
	}

	void Texture::UnBind() const
	{
		if (!IsValid())
			return;

		glBindTexture(Type2GL(type), 0);
	}

}