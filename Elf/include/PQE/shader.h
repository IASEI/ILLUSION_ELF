#ifndef __SHADER_H__
#define __SHADER_H__


#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "QdeRender.h"

class shader:public QdeRender
{
public:
	enum SHADERATTR {NONE,INT,FLOAT,BOOL,VEC2, VEC3, VEC4,MAT2,MAT3,MAT4};

	struct material
	{
		glm::vec3 diffuseVec3;
		glm::vec3 specularVec3;
		unsigned int diffuse;
		unsigned int specular;
		float shininess;
		material()
		{
			specularVec3 = glm::vec3(0.00f);
			diffuse = 0;
			specular = 1;
			shininess = 2;
		}
	};

	struct dirLight
	{
		glm::vec3 direction;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		dirLight()
		{
			direction = glm::vec3(-0.5f, -1.0f, -0.8f);
			ambient = glm::vec3(0.6f);
			diffuse = glm::vec3(0.6f);
			specular = glm::vec3(0.0f);
		}
	};

	struct pointLight {
		glm::vec3 position;

		float constant;
		float linear;
		float quadratic;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		pointLight()
		{
			position = glm::vec3(0.7f, 100.2f, 2.0f);

			constant = 1.0f;
			linear = 0.0;
			quadratic = 0.0;

			ambient = glm::vec3(0.5f);
			diffuse = glm::vec3(0.6f);
			specular = glm::vec3(0.5f);
		}
	};

	struct spotLight {
		glm::vec3 position;
		glm::vec3 direction;
		float cutOff;
		float outerCutOff;

		float constant;
		float linear;
		float quadratic;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		spotLight()
		{
		
		}
	};

	struct config
	{
		bool diffuseVec3;
		bool specularVec3;
		bool dirLight;
		int dirLightNum;
		int PointLightNum;
		int SpotLightNum;

		config()
		{
			diffuseVec3 = false;
			specularVec3 = true;
			dirLight = false;
			dirLightNum = 1;
			PointLightNum = 0;
			SpotLightNum = 0;
		}
	};

	struct shaderData
	{
		SHADERATTR type;
		std::string name;

		int *int_date;
		float *float_date;
		bool *bool_date;
		glm::vec2 *vec2_date;
		glm::vec3 *vec3_date;
		glm::vec4 *vec4_date;
		glm::mat2 *mat2_date;
		glm::mat3 *mat3_date;
		glm::mat4 *mat4_date;
	};
public:
	// ------------------------------------------------------------------------
	shader()
	{
		rootType = SHADER;
		name = "shader";
	}
	// ------------------------------------------------------------------------
	~shader()
	{
	}
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	shader(const char* vertexPath, const char* fragmentPath)
	{
		ID=createShader(vertexPath, fragmentPath);
	}
	// activate the shader
	// ------------------------------------------------------------------------
	void initShader(const char* vertexPath, const char* fragmentPath)
	{
		ID = createShader(vertexPath, fragmentPath);
	}
	// ------------------------------------------------------------------------
	unsigned int getInitId()const
	{
		return ID;
	}
	// ------------------------------------------------------------------------
	unsigned int createShader(const char* vertexPath, const char* fragmentPath)
	{
		unsigned int id=0;
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char * fShaderCode = fragmentCode.c_str();
		// 2. compile shaders
		unsigned int vertex, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		// shader Program
		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		glLinkProgram(id);
		checkCompileErrors(id, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		return id;
	}
	// ------------------------------------------------------------------------
	void use() const
	{
		glUseProgram(ID);
	}
	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string &name, bool *value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setBool(const unsigned int id, const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const std::string &name, int *value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), *value);
	}
	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setInt(const unsigned int id, const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const std::string &name, float *value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), *value);
	}
	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const unsigned int id, const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	

	void setVec2(const std::string &name, const glm::vec2 *value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &(*value)[0]);
	}
	void setVec2(const std::string &name, const glm::vec2 value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	void setVec2(const unsigned int id, const std::string &name, const glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void setVec2(const unsigned int id, const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string &name, const glm::vec3 *value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &(*value)[0]);
	}
	void setVec3(const std::string &name, const glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string &name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	void setVec3(const unsigned int id,const std::string &name, const glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void setVec3(const unsigned int id,const std::string &name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string &name, const glm::vec4 *value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &(*value)[0]);
	}
	void setVec4(const std::string &name, const glm::vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string &name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	void setVec4(const unsigned int id, const std::string &name, const glm::vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void setVec4(const unsigned int id, const std::string &name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const std::string &name, const glm::mat2 *mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &(*mat)[0][0]);
	}
	void setMat2(const std::string &name, const glm::mat2 &mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat2(const unsigned int id, const std::string &name, const glm::mat2 &mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat3(const std::string &name, const glm::mat3 *mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &(*mat)[0][0]);
	}
	void setMat3(const std::string &name, const glm::mat3 &mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat3(const unsigned int id, const std::string &name, const glm::mat3 &mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat4(const std::string &name, const glm::mat4 *ma) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &((*ma)[0][0]));
	}
	void setMat4(const std::string &name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat4(const unsigned int id, const std::string &name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMVC(const glm::mat4 *model, const glm::mat4 *view, const glm::vec3 crame)
	{
		setMat4("view", view);
		setVec3("carmePos", crame);
		setMat4("model", model);
	}
	// ------------------------------------------------------------------------
	//ssbo
	unsigned int createShaderStorageBufferObject(unsigned int bind, GLsizeiptr size, const void *data, GLenum usage)
	{
		static GLuint ssbo = 0;
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bind, ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		return ssbo;
	}

	void prr(std::string t)
	{
		printf("%s===", t.c_str());
		switch (glGetError())
		{
		case GL_NO_ERROR:
			printf("ok\n");
			break;
		default:
			printf("错误===\n");
			break;
		}
	}

	//更新数据1
	void setShaderStorageBufferObjectData(unsigned int id, GLsizeiptr size, void *date)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
		GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		memcpy(p, date, size);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
	
	//更新数据2
	void setShaderStorageBufferObjectData(unsigned int bind,unsigned int id, void *date, GLsizeiptr size, GLsizeiptr sizeType, GLenum usage)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, date, usage);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, bind, sizeType, date);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	// ------------------------------------------------------------------------
	void setUBO(const std::string &name, const glm::mat4 &mat,int i,int bindId,unsigned int ubo) const
	{
		glUniformBlockBinding(ID, glGetUniformBlockIndex(ID, name.c_str()), bindId);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*i, sizeof(glm::mat4), &mat);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void setUBOs(const std::string &name, void *mat, unsigned __int64 size, int bindId, unsigned int ubo) const
	{
		glUniformBlockBinding(ID, glGetUniformBlockIndex(ID, name.c_str()), bindId);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, mat);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	unsigned int GenUBOBuf(unsigned __int64 size)
	{
		unsigned int ubo;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		return ubo;
	}

	
	// ------------------------------------------------------------------------
	
	virtual void render()
	{
		glUseProgram(ID);
		for (int i = 0;i < data.size();i++)
		{
			switch (data[i].type)
			{
			case NONE:
				return;
			case INT:
				setInt(data[i].name,data[i].int_date);
				break;
			case FLOAT:
				setFloat(data[i].name, data[i].float_date);
				break;
			case BOOL:
				setBool(data[i].name, data[i].bool_date);
				break;
			case VEC2:
				setVec2(data[i].name, data[i].vec2_date);
				break;
			case VEC3:
				setVec3(data[i].name, data[i].vec3_date);
				break;
			case VEC4:
				setVec4(data[i].name, data[i].vec4_date);
				break;
			case MAT2:
				setMat2(data[i].name, data[i].mat2_date);
				break;
			case MAT3:
				setMat3(data[i].name, data[i].mat3_date);
				break;
			case MAT4:
				setMat4(data[i].name, data[i].mat4_date);
				break;
			default:
				break;
			}
		}
	}

	void appendData(std::string name, glm::mat4 *mat)
	{
		shaderData sDate;
		sDate.type = MAT4;
		sDate.name = name;
		sDate.mat4_date = mat;
		data.push_back(sDate);
	}

	void appendData(std::string name, glm::vec3 *vec)
	{
		shaderData sDate;
		sDate.type = VEC3;
		sDate.name = name;
		sDate.vec3_date = vec;
		data.push_back(sDate);
	}

	// ------------------------------------------------------------------------
	void setMaterial(material &mMaterial)
	{
		setVec3("mMaterial.diffuseVec3", mMaterial.diffuseVec3);
		setVec3("mMaterial.specularVec3", mMaterial.specularVec3);
		setInt("mMaterial.diffuse", mMaterial.diffuse);
		setInt("mMaterial.specular", mMaterial.specular);
		setFloat("mMaterial.shininess", mMaterial.shininess);
	}

	void setDirLight(dirLight &mDirLight)
	{
		setVec3("mDirLight.direction", mDirLight.direction);
		setVec3("mDirLight.ambient", mDirLight.ambient);
		setVec3("mDirLight.diffuse", mDirLight.diffuse);
		setVec3("mDirLight.specular", mDirLight.specular);
	}

	void setPointLight(pointLight &mPointLight,std::string id)
	{
		std::string name;
		name = "mPointLight[" + id + "]";
		setVec3(std::string(name) + ".position", mPointLight.position);
		setVec3(std::string(name) + ".ambient", mPointLight.ambient);
		setVec3(std::string(name) + ".diffuse", mPointLight.diffuse);
		setVec3(std::string(name) + ".specular", mPointLight.specular);
		setFloat(std::string(name) + ".constant", mPointLight.constant);
		setFloat(std::string(name) + ".linear", mPointLight.linear);
		setFloat(std::string(name) + ".quadratic", mPointLight.quadratic);

		setVec3("mPointLight[0].position", mPointLight.position);
		setVec3("mPointLight[0].ambient", mPointLight.ambient);
		setVec3("mPointLight[0].diffuse", mPointLight.diffuse);
		setVec3("mPointLight[0].specular", mPointLight.specular);
		setFloat("mPointLight[0].constant", mPointLight.constant);
		setFloat("mPointLight[0].linear", mPointLight.linear);
		setFloat("mPointLight[0].quadratic", mPointLight.quadratic);
		
	}

	void setSpotLight(spotLight &mSpotLight)
	{
		setVec3("mSpotLight.position", mSpotLight.position);
		setVec3("mSpotLight.ambient", mSpotLight.ambient);
		setVec3("mSpotLight.diffuse", mSpotLight.diffuse);
		setVec3("mSpotLight.specular", mSpotLight.specular);
		setFloat("mSpotLight.constant", mSpotLight.constant);
		setFloat("mSpotLight.linear", mSpotLight.linear);
		setFloat("mSpotLight.quadratic", mSpotLight.quadratic);
		setFloat("mSpotLight.cutOff", mSpotLight.cutOff);
		setFloat("mSpotLight.outerCutOff", mSpotLight.outerCutOff);
	}

	void setConfig(config &mConfig)
	{
		setBool("mConfig.diffuseVec3", mConfig.diffuseVec3);
		setBool("mConfig.specularVec3", mConfig.specularVec3);
		setInt("mConfig.dirLightNum", mConfig.dirLightNum);
		setInt("mConfig.PointLightNum", mConfig.PointLightNum);
		setInt("mConfig.SpotLightNum", mConfig.SpotLightNum);
	}
private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void checkCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
public:
	unsigned int ID;
	std::vector<shaderData> data;
};
#endif