#include "PurpleDreamElf.h"

namespace PDE
{
	bool	arbMultisampleSupported = false;
	int		arbMultisampleFormat = 0;

	//==================================================================================
	// WGLisExtensionSupported: This Is A Form Of The Extension For WGL
	bool WGLisExtensionSupported(const char *extension)
	{
		const size_t extlen = strlen(extension);
		const char *supported = NULL;

		// Try To Use wglGetExtensionStringARB On Current DC, If Possible
		PROC wglGetExtString = wglGetProcAddress("wglGetExtensionsStringARB");

		if (wglGetExtString)
			supported = ((char*(__stdcall*)(HDC))wglGetExtString)(wglGetCurrentDC());

		// If That Failed, Try Standard Opengl Extensions String
		if (supported == NULL)
			supported = (char*)glGetString(GL_EXTENSIONS);

		// If That Failed Too, Must Be No Extensions Supported
		if (supported == NULL)
			return false;

		// Begin Examination At Start Of String, Increment By 1 On False Match
		for (const char* p = supported; ; p++)
		{
			// Advance p Up To The Next Possible Match
			p = strstr(p, extension);

			if (p == NULL)
				return false;															// No Match

																						// Make Sure That Match Is At The Start Of The String Or That
																						// The Previous Char Is A Space, Or Else We Could Accidentally
																						// Match "wglFunkywglExtension" With "wglExtension"

																						// Also, Make Sure That The Following Character Is Space Or NULL
																						// Or Else "wglExtensionTwo" Might Match "wglExtension"
			if ((p == supported || p[-1] == ' ') && (p[extlen] == '\0' || p[extlen] == ' '))
				return true;															// Match
		}
	}
	/*
	// InitMultisample: Used To Query The Multisample Frequencies
	bool initMultisample(HINSTANCE hInstance, HWND hWnd, PIXELFORMATDESCRIPTOR pfd)
	{
		// See If The String Exists In WGL!
		if (!WGLisExtensionSupported("WGL_ARB_multisample"))
		{
			arbMultisampleSupported = false;
			return false;
		}

		// Get Our Pixel Format
		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
		if (!wglChoosePixelFormatARB)
		{
			arbMultisampleSupported = false;
			return false;
		}

		// Get Our Current Device Context
		HDC hDC = GetDC(hWnd);

		int		pixelFormat;
		int		valid;
		UINT	numFormats;
		float	fAttributes[] = { 0,0 };

		// These Attributes Are The Bits We Want To Test For In Our Sample
		// Everything Is Pretty Standard, The Only One We Want To 
		// Really Focus On Is The SAMPLE BUFFERS ARB And WGL SAMPLES
		// These Two Are Going To Do The Main Testing For Whether Or Not
		// We Support Multisampling On This Hardware.
		int iAttributes[] =
		{
			WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
			WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB,24,
			WGL_ALPHA_BITS_ARB,8,
			WGL_DEPTH_BITS_ARB,16,
			WGL_STENCIL_BITS_ARB,0,
			WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
			WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
			WGL_SAMPLES_ARB,4,
			0,0
		};

		// First We Check To See If We Can Get A Pixel Format For 4 Samples
		valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);

		// If We Returned True, And Our Format Count Is Greater Than 1
		if (valid && numFormats >= 1)
		{
			arbMultisampleSupported = true;
			arbMultisampleFormat = pixelFormat;
			return arbMultisampleSupported;
		}

		// Our Pixel Format With 4 Samples Failed, Test For 2 Samples
		iAttributes[19] = 2;
		valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
		if (valid && numFormats >= 1)
		{
			arbMultisampleSupported = true;
			arbMultisampleFormat = pixelFormat;
			return arbMultisampleSupported;
		}

		// Return The Valid Format
		return  arbMultisampleSupported;
	}
	*/
	char *tcharTochar(const TCHAR *tchar, char *_char)
	{
		int iLength;
		//获取字节长度   
		iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
		//将tchar值赋给_char    
		WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
		return _char;

	}

	char *tcharTochar(const TCHAR *tchar)
	{
		char _char[255];
		int iLength;
		//获取字节长度   
		iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
		//将tchar值赋给_char    
		WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
		return _char;

	}

	TCHAR *charToTchar(const char *_char, TCHAR *tchar)
	{

		int iLength;
		iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
		MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
		return tchar;

	}

	TCHAR *charToTchar(const char *_char)
	{
		TCHAR tchar[255];
		int iLength;
		iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
		MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
		return tchar;

	}


	int createBitTextures(const char *path)
	{
		
		GLuint id;
		float height=0, width=0;
		BYTE *pixels=NULL;
		FILE *pFile = 0; /**< 文件指针 */

						 /** 创建位图文件信息和位图文件头结构 */
		BITMAPINFOHEADER bitmapInfoHeader;
		BITMAPFILEHEADER header;

		unsigned char textureColors = 0;/**< 用于将图像颜色从BGR变换到RGB */

										/** 打开文件,并检查错误 */
		pFile = fopen(path, "rb");
		if (pFile == 0) return false;

		/** 读入位图文件头信息 */
		fread(&header, sizeof(BITMAPFILEHEADER), 1, pFile);

		/** 检查该文件是否为位图文件 */
		if (header.bfType != BITMAP_ID)
		{
			fclose(pFile);             /**< 若不是位图文件,则关闭文件并返回 */
			return false;
		}

		/** 读入位图文件信息 */
		fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);

		/** 保存图像的宽度和高度 */
		width = bitmapInfoHeader.biWidth;
		height = bitmapInfoHeader.biHeight;

		/** 确保读取数据的大小 */
		if (bitmapInfoHeader.biSizeImage == 0)
			bitmapInfoHeader.biSizeImage = bitmapInfoHeader.biWidth *
			bitmapInfoHeader.biHeight * 3;

		/** 将指针移到数据开始位置 */
		fseek(pFile, header.bfOffBits, SEEK_SET);

		/** 分配内存 */
		pixels = new unsigned char[bitmapInfoHeader.biSizeImage];

		/** 检查内存分配是否成功 */
		if (!pixels)                        /**< 若分配内存失败则返回 */
		{
			delete[] pixels;
			fclose(pFile);
			return false;
		}

		/** 读取图像数据 */
		fread(pixels, 1, bitmapInfoHeader.biSizeImage, pFile);

		/** 将图像颜色数据格式进行交换,由BGR转换为RGB */
		for (int index = 0; index < (int)bitmapInfoHeader.biSizeImage; index += 3)
		{
			textureColors = pixels[index];
			pixels[index] = pixels[index + 2];
			pixels[index + 2] = textureColors;
		}

		fclose(pFile);       /**< 关闭文件 */

		/** 生成纹理对象名称 */
		glGenTextures(1, &id);

		/** 创建纹理对象 */
		glBindTexture(GL_TEXTURE_2D, id);

		/** 控制滤波 */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		/** 创建纹理 */
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width,
			height, GL_RGB, GL_UNSIGNED_BYTE,
			pixels);
		return id;

	}

	int createBitTextures(const TCHAR *path)
	{
		char paths[255];
		PDE::tcharTochar(path, paths);
		return createBitTextures(paths);

	}

	unsigned int TextureFromFile(const char *path)
	{
		if (strcmp(path, "") == 0)
			return 0;
		string filename = string(path);
		// filename = directory + '/' + filename;
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		//GaussianBlur(data,width,height, nrComponents,4);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
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

		return textureID;
	}

	unsigned int loadCubemap(vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}


	glm::mat4x4 aiMatrix4x4ToMat4x4(const aiMatrix4x4& AssimpMatrix)
	{
		return glm::mat4x4(AssimpMatrix.a1, AssimpMatrix.a2, AssimpMatrix.a3, AssimpMatrix.a4,
			AssimpMatrix.b1, AssimpMatrix.b2, AssimpMatrix.b3, AssimpMatrix.b4,
			AssimpMatrix.c1, AssimpMatrix.c2, AssimpMatrix.c3, AssimpMatrix.c4,
			AssimpMatrix.d1, AssimpMatrix.d2, AssimpMatrix.d3, AssimpMatrix.d4);
	}

	SpecialID generateSpecialID(unsigned long int indexsize, unsigned long int datasieze,const unsigned int *index, const float *data)
	{

		SpecialID sid;
		glGenVertexArrays(1, &sid.vao);
		glGenBuffers(1, &sid.vbo);
		glGenBuffers(1, &sid.ebo);

		glBindVertexArray(sid.vao);

		glBindBuffer(GL_ARRAY_BUFFER, sid.vbo);
		glBufferData(GL_ARRAY_BUFFER, datasieze, data, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sid.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexsize, index, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// Color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		// TexCoord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0); // Unbind VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return sid;
	}

	SpecialID generateSpecialIDReturnVAOVBO(unsigned long int datasieze,float *data)
	{
		SpecialID sid;
		glGenVertexArrays(1, &sid.vao);
		glGenBuffers(1, &sid.vbo);

		glBindVertexArray(sid.vao);

		glBindBuffer(GL_ARRAY_BUFFER, sid.vbo);
		glBufferData(GL_ARRAY_BUFFER, datasieze, data, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// Color attribute
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		// TexCoord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		
		glBindVertexArray(0); // Unbind VAO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return sid;
	}

	long long GetCurrentTimeMillis()
	{
		#ifdef WIN32    
				return GetTickCount();
		#else
				timeval t;
				gettimeofday(&t, NULL);

				long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
				return ret;
		#endif    
	}

	void GaussianBlur(unsigned char* img, unsigned int width, unsigned int height, unsigned int channels, unsigned int radius)
	{
		
		radius = min(max(1, int(radius)), 248);
		unsigned int kernelSize = 1 + radius * 2;
		unsigned int* kernel = (unsigned int*)malloc(kernelSize* sizeof(unsigned int));
		memset(kernel, 0, kernelSize* sizeof(unsigned int));
		int(*mult)[256] = (int(*)[256])malloc(kernelSize * 256 * sizeof(int));
		memset(mult, 0, kernelSize * 256 * sizeof(int));

		int xStart = 0;
		int yStart = 0;
		width = xStart + width - max(0, int((xStart + width) - width));
		height = yStart + height - max(0, int((yStart + height) - height));
		int imageSize = width*height;
		int widthstep = width*channels;
		if (channels == 3 || channels == 4)
		{
			unsigned char *    CacheImg = nullptr;
			CacheImg = (unsigned char *)malloc(sizeof(unsigned char) * imageSize * 6);
			if (CacheImg == nullptr) return;
			unsigned char *    rCache = CacheImg;
			unsigned char *    gCache = CacheImg + imageSize;
			unsigned char *    bCache = CacheImg + imageSize * 2;
			unsigned char *    r2Cache = CacheImg + imageSize * 3;
			unsigned char *    g2Cache = CacheImg + imageSize * 4;
			unsigned char *    b2Cache = CacheImg + imageSize * 5;
			int sum = 0;
			for (int K = 1; K < radius; K++) {
				unsigned int szi = radius - K;
				kernel[radius + K] = kernel[szi] = szi*szi;
				sum += kernel[szi] + kernel[szi];
				for (int j = 0; j < 256; j++) {
					mult[radius + K][j] = mult[szi][j] = kernel[szi] * j;
				}
			}
			kernel[radius] = radius*radius;
			sum += kernel[radius];
			for (int j = 0; j < 256; j++) {
				mult[radius][j] = kernel[radius] * j;
			}
			for (int Y = 0; Y < height; ++Y) {
				unsigned char*     LinePS = img + Y*widthstep;
				unsigned char*     LinePR = rCache + Y*width;
				unsigned char*     LinePG = gCache + Y*width;
				unsigned char*     LinePB = bCache + Y*width;
				for (int X = 0; X < width; ++X) {
					int     p2 = X*channels;
					LinePR[X] = LinePS[p2];
					LinePG[X] = LinePS[p2 + 1];
					LinePB[X] = LinePS[p2 + 2];
				}
			}
			int kernelsum = 0;
			for (int K = 0; K < kernelSize; K++) {
				kernelsum += kernel[K];
			}
			float fkernelsum = 1.0f / kernelsum;
			for (int Y = yStart; Y < height; Y++) {
				int heightStep = Y * width;
				unsigned char*     LinePR = rCache + heightStep;
				unsigned char*     LinePG = gCache + heightStep;
				unsigned char*     LinePB = bCache + heightStep;
				for (int X = xStart; X < width; X++) {
					int cb = 0;
					int cg = 0;
					int cr = 0;
					for (int K = 0; K < kernelSize; K++) {
						unsigned    int     readPos = ((X - radius + K + width) % width);
						int * pmult = mult[K];
						cr += pmult[LinePR[readPos]];
						cg += pmult[LinePG[readPos]];
						cb += pmult[LinePB[readPos]];
					}
					unsigned int p = heightStep + X;
					r2Cache[p] = cr* fkernelsum;
					g2Cache[p] = cg* fkernelsum;
					b2Cache[p] = cb* fkernelsum;
				}
			}
			for (int X = xStart; X < width; X++) {
				int WidthComp = X*channels;
				int WidthStep = width*channels;
				unsigned char*     LinePS = img + X*channels;
				unsigned char*     LinePR = r2Cache + X;
				unsigned char*     LinePG = g2Cache + X;
				unsigned char*     LinePB = b2Cache + X;
				for (int Y = yStart; Y < height; Y++) {
					int cb = 0;
					int cg = 0;
					int cr = 0;
					for (int K = 0; K < kernelSize; K++) {
						unsigned int   readPos = ((Y - radius + K + height) % height) * width;
						int * pmult = mult[K];
						cr += pmult[LinePR[readPos]];
						cg += pmult[LinePG[readPos]];
						cb += pmult[LinePB[readPos]];
					}
					int    p = Y*WidthStep;
					LinePS[p] = (unsigned char)(cr * fkernelsum);
					LinePS[p + 1] = (unsigned char)(cg * fkernelsum);
					LinePS[p + 2] = (unsigned char)(cb* fkernelsum);


				}
			}
			free(CacheImg);
		}
		else if (channels == 1)
		{
			unsigned char *    CacheImg = nullptr;
			CacheImg = (unsigned char *)malloc(sizeof(unsigned char) * imageSize * 2);
			if (CacheImg == nullptr) return;
			unsigned char *    rCache = CacheImg;
			unsigned char *    r2Cache = CacheImg + imageSize;

			int sum = 0;
			for (int K = 1; K < radius; K++) {
				unsigned int szi = radius - K;
				kernel[radius + K] = kernel[szi] = szi*szi;
				sum += kernel[szi] + kernel[szi];
				for (int j = 0; j < 256; j++) {
					mult[radius + K][j] = mult[szi][j] = kernel[szi] * j;
				}
			}
			kernel[radius] = radius*radius;
			sum += kernel[radius];
			for (int j = 0; j < 256; j++) {
				mult[radius][j] = kernel[radius] * j;
			}
			for (int Y = 0; Y < height; ++Y) {
				unsigned char*     LinePS = img + Y*widthstep;
				unsigned char*     LinePR = rCache + Y*width;
				for (int X = 0; X < width; ++X) {
					LinePR[X] = LinePS[X];
				}
			}
			int kernelsum = 0;
			for (int K = 0; K < kernelSize; K++) {
				kernelsum += kernel[K];
			}
			float fkernelsum = 1.0f / kernelsum;
			for (int Y = yStart; Y < height; Y++) {
				int heightStep = Y * width;
				unsigned char*     LinePR = rCache + heightStep;
				for (int X = xStart; X < width; X++) {
					int cb = 0;
					int cg = 0;
					int cr = 0;
					for (int K = 0; K < kernelSize; K++) {
						unsigned    int     readPos = ((X - radius + K + width) % width);
						int * pmult = mult[K];
						cr += pmult[LinePR[readPos]];
					}
					unsigned int p = heightStep + X;
					r2Cache[p] = cr * fkernelsum;
				}
			}
			for (int X = xStart; X < width; X++) {
				int WidthComp = X*channels;
				int WidthStep = width*channels;
				unsigned char*     LinePS = img + X*channels;
				unsigned char*     LinePR = r2Cache + X;
				for (int Y = yStart; Y < height; Y++) {
					int cb = 0;
					int cg = 0;
					int cr = 0;
					for (int K = 0; K < kernelSize; K++) {
						unsigned int   readPos = ((Y - radius + K + height) % height) * width;
						int * pmult = mult[K];
						cr += pmult[LinePR[readPos]];
					}
					int    p = Y*WidthStep;
					LinePS[p] = (unsigned char)(cr* fkernelsum);
				}
			}
			free(CacheImg);
		}
		free(kernel);
		free(mult);
	}

	//求向量n1，n2夹角
	float ComputeAngleUseVector(glm::vec3 n1, glm::vec3 n2)
	{
		float length = glm::length(n1 - n2);
		if (length == 0)return 0;
		return glm::dot(n1, n2) / length;
	}
}
