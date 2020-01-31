#include "DimensionWorldWin32.h"

namespace DW
{
	ULONG_PTR gdiplusToken;

	/*其它*/



	/*公共*/
	void DimensionWorld::_windowResized()
	{
		RECT rect = { 0, 0, 0, 0 };
		GetClientRect(hWnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		//resizeRender(width, height)
	}

	DimensionWorld::DimensionWorld():
		hWnd(0),
		hDC(0),
		hRC(0),
		mExit(true),
		mResourceFileName("MyGUI_Core.xml"),
		m_ClassName(_T("asf")),
		m_WindowPosX(0),
		m_WindowPosY(0),
		m_IsFullScreen(false),
		m_BitsPerPixel(16),
		isEnableSkyBox(false),
		isEnableUI(true)

	{
		//Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		//Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		m_ScreenWidth = GetSystemMetrics(SM_CXSCREEN);		/**< 获取当前屏幕宽 */
		m_ScreenHeight = GetSystemMetrics(SM_CYSCREEN);		/**< 获取当前屏幕高 */
		m_WindowWidth = m_ScreenWidth / 2;
		m_WindowHeight = m_ScreenHeight / 2;
	
	}

	DimensionWorld::~DimensionWorld()
	{
		
	}

	void DimensionWorld::prepare()
	{
	}

	int DimensionWorld::getWidth()
	{
		if (m_IsFullScreen == true)
		{
			return m_ScreenWidth;
		}
		else
		{
			return m_WindowWidth;
		}
	}

	int DimensionWorld::getHeight()
	{
		if (m_IsFullScreen == true)
		{
			return m_ScreenHeight;
		}
		else
		{
			return m_WindowHeight;
		}
	}

	void DimensionWorld::destroyWindow()
	{
		if (hWnd != 0)										/**< 窗口句柄是否存在 */
		{
			if (hDC != 0)										/**< 窗口设备描述表是否存在 */
			{
				wglMakeCurrent(hDC, 0);						/**< 设置当前的OpenGL的渲染对象为空NULL */
				if (hRC != 0)									/**< OpenGL的渲染描述表是否存在 */
				{
					wglDeleteContext(hRC);					/**< 释放OpenGL的渲染描述表 */
					hRC = 0;									/**< 设置OpenGL的渲染描述表为0 */
				}
				ReleaseDC(hWnd, hDC);						/**< 释放窗口的设备描述表 */
				hDC = 0;										/**< 设置窗口的设备描述表为0 */
			}
			DestroyWindow(hWnd);								/**< 删除窗口 */
			hWnd = 0;											/**< 设置窗口句柄为0 */
		}

		if (m_IsFullScreen)										/**< 如果为全屏模式，在程序结束后，变换到窗口模式，并显示鼠标 */
		{
			ChangeDisplaySettings(NULL, 0);						/**< 变换到窗口模式 */
			ShowCursor(true);									/**< 显示鼠标 */
		}
	}

	

	bool DimensionWorld::create(const TCHAR * window_title, const TCHAR * class_name, bool fullscreen, HINSTANCE h_instance, LPVOID lpParam)
	{
		m_IsFullScreen = fullscreen;
		int nX = 0;
		int nY = 0;
		
		PIXELFORMATDESCRIPTOR pfd =											/**< 设置像素描述结构 */
		{
			sizeof(PIXELFORMATDESCRIPTOR),									/**< 像素描述结构的大小 */
			1,																/**< 版本号 */
			PFD_DRAW_TO_WINDOW |											/**< 缓存区的输出显示在一个窗口中 */
			PFD_SUPPORT_OPENGL |											/**< 缓存区支持OpenGL绘图 */
			PFD_STEREO |											/**< 颜色缓存区是立体缓存 */
			PFD_DOUBLEBUFFER,												/**< 颜色缓存区是双缓存 */
			PFD_TYPE_RGBA,													/**< 使用RGBA颜色格式 */
			16,													/**< 颜色缓存区中颜色值所占的位深 */
			0, 0, 0, 0, 0, 0,												/**< 使用默认的颜色设置 */
			0,																/**< 无Alpha缓存 */
			0,																/**< 颜色缓存区中alpha成分的移位计数 */
			0,																/**< 无累计缓存区 */
			0, 0, 0, 0,														/**< 累计缓存区无移位 */
			32,																/**< 32位深度缓存 */
			0,																/**< 无蒙版缓存 */
			0,																/**< 无辅助缓存区 */
			PFD_MAIN_PLANE,													/**< 必须为PFD_MAIN_PLANE，设置为主绘图层 */
			0,																/**< 表示OpenGL实现所支持的上层或下层平面的数量 */
			0, 0, 0															/**< 过时，已不再使用 */
		};

		DWORD windowStyle = WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX&~WS_MINIMIZE;	/**< 定义我们窗口类型，使用常规设定，去掉最大化按钮，并不能改变窗体大小 */
		DWORD windowExtendedStyle = WS_EX_APPWINDOW;
		/// 调整我们窗口的大小，使其客户区的大小为我们设置的大小
		if (m_IsFullScreen == true)											/**< 如果为全屏模式，尝试转化为全屏模式 */
		{
			ShowCursor(true);											/**< 隐藏鼠标 */
			windowStyle = WS_POPUP;										/**< 设置窗口模式为顶层窗口 */
			windowExtendedStyle |= WS_EX_TOPMOST;
			/// 调整我们窗口的大小，使其客户区的大小为我们设置的大小
			
		}
		RECT windowRect = { m_WindowPosX, m_WindowPosY, m_WindowPosX + +getWidth(), m_WindowPosY + getHeight() };

		if (m_IsFullScreen == false)										/**< 在窗口模式下使用 */
		{
			windowExtendedStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	/**< 使窗口具有3D外观 */
			nX = (m_ScreenWidth - m_WindowWidth) / 2;                    /**< 计算窗口居中用 */
			nY = (m_ScreenHeight - m_WindowHeight) / 2;
			/// 调整我们窗口的大小，使其客户区的大小为我们设置的大小
			AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);
			/// 判断窗口的左上角是否隐藏在桌面外
			if (windowRect.left < 0)										/**< 如果窗口X坐标为负，移动坐标到0处，并调整窗口的位置 */
			{
				windowRect.right -= windowRect.left;
				windowRect.left = 0;
			}
			if (windowRect.top < 0)											/**< 如果窗口Y坐标为负，移动坐标到0处，并调整窗口的位置 */
			{
				windowRect.bottom -= windowRect.top;
				windowRect.top = 0;
			}
		}

		/// 创建窗口
		hWnd = CreateWindowEx(windowExtendedStyle,						/**< 窗口的扩展风格 */
			class_name,									/**< 窗口的类名 */
			window_title,								/**< 窗口标题 */
			windowStyle,								/**< 窗口的风格 */
			nX, nY,                                      /**< 窗口的左上角位置 */
			windowRect.right - windowRect.left,			/**< 窗口的宽度 */
			windowRect.bottom - windowRect.top,			/**< 窗口的高度 */
			NULL,								/**< 窗口的父窗口为桌面 */
			0,											/**< 无菜单 */
			h_instance,									/**< 传入窗口的实例句柄 */
			lpParam);									/**< 传入程序类参数 */

		while (hWnd != NULL)													/**< 窗口是否创建成功 */
		{
			hDC = GetDC(hWnd);											/**< 返回窗口的设备描述表 */
			if (hDC == 0)													/**< 如果为空 */
			{																/**< 失败 */
				break;
			}

			GLuint PixelFormat;
			if (!PDE::arbMultisampleSupported)
			{
				PixelFormat = ChoosePixelFormat(hDC, &pfd);				// Find A Compatible Pixel Format
				if (PixelFormat == 0)												// Did We Find A Compatible Format?
				{
					// Failed
					ReleaseDC(hWnd, hDC);							// Release Our Device Context
					hDC = 0;												// Zero The Device Context
					DestroyWindow(hWnd);									// Destroy The Window
					hWnd = 0;												// Zero The Window Handle
					break;												// Return False
				}

			}
			else
			{
				PixelFormat = PDE::arbMultisampleFormat;
			}

			if (SetPixelFormat(hDC, PixelFormat, &pfd) == false)			/**< 设置像素格式 */
			{																/**< 失败 */
				break;
			}
			hRC = wglCreateContext(hDC);								/**< 创建OpenGL的渲染描述表 */
			if (hRC == 0)													/**< 如果为空 */
			{																/**< 失败 */
				break;
			}
			if (wglMakeCurrent(hDC, hRC) == false)						/**< 设置当前的OpenGL的渲染对象为当前的窗口 */
			{																/**< 失败 */
				break;
			}

			if (!PDE::arbMultisampleSupported && CHECK_FOR_MULTISAMPLE)
			{

				if (PDE::initMultisample(h_instance, hWnd, pfd))
				{

					destroyWindow();
					return create(window_title, class_name, fullscreen, h_instance, lpParam);
				}
			}

			ShowWindow(hWnd, SW_NORMAL);									/**< 显示窗口 */
			reshapeGL();													/**< 告诉OpenGL调整窗口大小 */
			return true;													/**< 成功返回 */
		}

		destroyWindow();															/**< 释放资源 */
		return false;														/**< 返回失败 */
	}

	bool DimensionWorld::ChangeScreenSetting()
	{
		DEVMODE dmScreenSettings;											/**< 设备模式 */	
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));/**< 清零结构 */
		dmScreenSettings.dmSize = sizeof(DEVMODE);					/**< 结构大小 */
		dmScreenSettings.dmPelsWidth = getWidth();						/**< 设置宽度 */
		dmScreenSettings.dmPelsHeight = getHeight();						/**< 设置高度 */
		dmScreenSettings.dmBitsPerPel = m_BitsPerPixel;					/**< 设置位深度 */
																		//dmScreenSettings.dmDisplayFrequency = 75;                           /**< 设置屏幕刷新率 */
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;// | DM_DISPLAYFREQUENCY;

																				 /// 改变显示模式
		if (ChangeDisplaySettings(&dmScreenSettings, 0) != DISP_CHANGE_SUCCESSFUL)
		{
			return false;													/**< 如果失败，返回false */
		}

		return true;														/**< 成功返回 */
	}

	void DimensionWorld::reshapeGL()
	{
		GLsizei width = getWidth();
		GLsizei height = getHeight();
		glViewport(0, 0, width, height);									/**< 重新设置视口 */
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100000.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void DimensionWorld::run()
	{
		// First we clear the screen and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Then we reset the modelview matrix
		glLoadIdentity();
		drawSkyBox();
		draw();
	}

	void DimensionWorld::setWindowCaption(const std::wstring& _text)
	{
		SetWindowTextW(hWnd, _text.c_str());
	}

	void DimensionWorld::updateFPS()
	{
		
	}

	void DimensionWorld::drawOneFrame()
	{
		
	}

	void DimensionWorld::drawSkyBox()
	{
		if (isEnableSkyBox)
		{
			glTranslatef(0, -25, 0);
			initSkyBox.drawAllEBO();
			glLoadIdentity();
		}
	}

	/*初始化*/

	bool DimensionWorld::initSystem()
	{
		initGlewAndOther();
		createScene();
		_windowResized();
		return true;
	}

	bool DimensionWorld::initGlewAndOther()
	{
		glewInit();
		glEnable(GL_TEXTURE_2D);
		if (isEnableSkyBox)
		{
			initSkyBox.importFile("Elf/magic/skybox/skybox.pde");
			initSkyBox.generationAllEBO();
		}
		
		return true;
	}

	/*卸载*/


	void DimensionWorld::destroyAll()
	{
		destroyScene();

	

		destroyWindow();

		//UnregisterClass(WND_CLASS_NAME, hInstance);
	}
	/*主系统*/

	/** 消息循环 */
	LRESULT DimensionWorld::Message(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)														/**< 处理不同的Windows消息 */
		{
		case WM_SYSCOMMAND:												/**< 截获系统命令 */
			switch (wParam)
			{
			case SC_SCREENSAVE:										/**< 截获屏幕保护启动命令 */
			case SC_MONITORPOWER:									/**< 截获显示其省电模式启动命令 */
				return 0;											/**< 不启用这两个命令 */
				break;
			}
			break;															/**< 退出 */

		case WM_CLOSE:													/**< 关闭窗口 */
			//TerminateApplication();										/**< 调用TerminateApplication函数 */
			return 0;
			break;

		case WM_EXITMENULOOP:
		case WM_EXITSIZEMOVE:
			//m_LastTickCount = GetTickCount();							/**< 更新计数器的值 */
			return 0;
			break;

		case WM_MOVE:
			//m_Window.SetPosX(LOWORD(lParam));							/**< 更新鼠标的坐标 */
		//	m_Window.SetPosY(HIWORD(lParam));
			return 0;
			break;

		case WM_PAINT:
			//if (m_ResizeDraw == true)									/**< 如果需要重绘 */
			//{
			//	m_Window.ReshapeGL();									/**< 重新设置窗口的大小 */
			//	Draw();													/**< 重新绘制 */
			//	m_Window.SwapBuffers();									/**< 交换前后帧缓存 */
			//}
			break;

		case WM_SIZING:													/**< 窗口正在改变大小 */
		{
			RECT * rect = (RECT *)lParam;
			//m_Window.SetWidth(rect->right - rect->left);				/**< 设置窗口宽度 */
			//m_Window.SetHeight(rect->bottom - rect->top);				/**< 设置窗口高度 */
			return TRUE;
		}
		break;

		case WM_SIZE:													/**< 窗口改变大小后 */
			switch (wParam)												/**< 处理不同的窗口状态 */
			{
			case SIZE_MINIMIZED:									/**< 是否最小化? */
				//m_IsVisible = false;								/**< 如果是，则设置不可见 */
				return 0;
				break;

			case SIZE_MAXIMIZED:									/**< 窗口是否最大化? */
			case SIZE_RESTORED:										/**< 窗口被还原? */
				//m_IsVisible = true;									/**< 设置为可见 */
				//m_Window.SetWidth(LOWORD(lParam));					/**< 设置窗口宽度 */
				//m_Window.SetHeight(HIWORD(lParam));					/**< 设置窗口高度 */
				//m_Window.ReshapeGL();								/**< 改变窗口大小 */
				//m_LastTickCount = GetTickCount();					/**< 更新计数器的值 */
				return 0;
				break;
			}
			break;

		case WM_KEYDOWN:												/**< 更新按键信息 */
		
			return 0;
			break;

		case WM_KEYUP:													/**< 更新释放键信息 */
			
			return 0;
			break;
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);					/**< 调用默认的窗口消息处理函数 */
	}

	/** 程序的主循环 */
	int DimensionWorld::Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
		/// 注册一个窗口
		WNDCLASSEX windowClass;												/**< 窗口类 */
		ZeroMemory(&windowClass, sizeof(WNDCLASSEX));						/**< 清空结构为0 */
		windowClass.cbSize = sizeof(WNDCLASSEX);					/**< 窗口结构的大小 */
		windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	/**< 设置窗口类型为，移动时重画，并为窗口取得DC */
		windowClass.lpfnWndProc = (WNDPROC)(WindowProc);				/**< WndProc处理消息 */
		windowClass.hInstance = hInstance;							/**< 设置实例 */
		windowClass.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE);			/**< 设置背景 */
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);			/**< 载入光标 */
		windowClass.lpszClassName = m_ClassName;							/**< 设置类名 */
		if (RegisterClassEx(&windowClass) == 0)								/**< 尝试注册窗口类 */
		{																	/**< NOTE: Failure, Should Never Happen */
			MessageBox(HWND_DESKTOP, _T("注册窗口失败!"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
			return -1;														/**< 退出并返回FALSE */
		}

		while (mExit)											/**< 循环直道WM_QUIT退出程序 */
		{
			/// 创建一个窗口
			if (create(_T("次元"), m_ClassName, m_IsFullScreen, hInstance, this))
			{

				// 如果初始化失败，则退出
				if (!(init()&&initSystem()))									/**< 调用自定义的初始化函数 */
				{															/**< 失败 */
					//TerminateApplication();									/**< 关闭窗口退出程序 */
				}
				else														/**< 成功开始消息循环 */
				{
					MSG msg;												/**< Window消息结构 */
					bool isMessagePumpActive = true;						/**< 当消息不为空时，处理消息循环 */
					while (isMessagePumpActive == true)						/**< 当消息不为空时，处理消息循环 */
					{														/**< 成功创建窗口，检测窗口消息 */
						if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE) != 0)
						{
							/// 检测是否为WM_QUIT消息
							if (msg.message != WM_QUIT)
							{
								DispatchMessage(&msg);						/**< 如果不是发送消息到消息回调函数中处理 */
							}
							else
							{
								isMessagePumpActive = false;				/**< 如果是，则退出 */
							}
						}
						/// 如果没有消息
						else
						{
							//if (m_IsVisible == false)						/**< 如果窗口不可见 */
							//{
								//WaitMessage();								/**< 暂停程序，等待消息 */
							//}
							//else											/**< 如果窗口可见 */
							//{
								/**< 执行消息循环 */
								DWORD tickCount = GetTickCount();			/**< 返回计时器的当前值 */
								updateFPS();
								run();										/**< 调用用户自定义的绘制函数 */
								SwapBuffers(hDC);					/**< 交换前后帧缓存 */
							//}
								if (mExit == false)
									break;
						}
					}														/**< 如果isMessagePumpActive == true，则循环 */
				}
				destroyAll();											/**< 删除窗口 */
			}
			else															/**< 如果创建窗口失败 */
			{
				MessageBox(HWND_DESKTOP, _T("创建OpenGL窗口错误"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
				mExit = false;									/**< 停止程序循环 */
			}
		}

		UnregisterClass(m_ClassName, hInstance);							/**< 取消注册的窗口 */
		return 0;
	}

	/** 处理窗口消息 */
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LONG user_data = GetWindowLong(hWnd, GWL_USERDATA);					/**< 返回用户自定义的32位的程序附加值 */
		if (user_data == 0)
		{
			/// 如果程序第一次运行
			if (uMsg == WM_CREATE)											/**< 处理窗口创建消息 */
			{
				/// 返回窗口结构的指针，它保存刚创建的程序实例的类
				CREATESTRUCT * creation = reinterpret_cast<CREATESTRUCT *>(lParam);
				/// 获得程序实例的指针
				DimensionWorld * dimension = reinterpret_cast<DimensionWorld *>(creation->lpCreateParams);
				/// 保存程序实例的指针为用户自定义的程序附加值
				SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast<LONG>(dimension));							/**< 设置程序可见 */
				return 0;													/**< 返回 */
			}
		}
		else
		{	/// 如果不是第一次窗口，返回程序实例的指针
			DimensionWorld * appl = reinterpret_cast<DimensionWorld *>(user_data);
			return appl->Message(hWnd, uMsg, wParam, lParam);				/**< 调用程序实例自己的消息处理函数 */
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);					/**< 调用默认的窗口消息处理函数 */
	}

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int ret = -1;
	DW::DimensionWorld * dimension = DW::DimensionWorld::Create(_T("DimensionWorld"));			/**< 创建程序类 */
	if (dimension != 0)
	{
		ret = dimension->Main(hInstance, hPrevInstance, lpCmdLine, nCmdShow);/**< 执行程序主循环 */
		delete dimension;													/**< 删除程序类（在继承类中，使用GL_Example * example = new GL_Example(class_name);分配了一块内存）*/
	}
	else
	{																	/**< 创建程序出错 */
		MessageBox(HWND_DESKTOP, _T("创建程序出错"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
	}
	return ret;
}