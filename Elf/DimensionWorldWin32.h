/*!
@file
@author		Albert Semenov
@date		08/2008
*/

#ifndef __DIMENSIONWORLD_H__
#define __DIMENSIONWORLD_H__

#include "fantastic.h"
#include "include/pde/PurpleDreamElf.h"
#if MYGUI_PLATFORM == MYGUI_PLATFORM_WIN32
#	include <windows.h>
#	include <winuser.h>
#endif

namespace DW
{

	class DimensionWorld 
	{
	public://公共

		
		DimensionWorld();
		void			_windowResized();
		virtual			~DimensionWorld();
		virtual void	prepare();
		int				getWidth();
		int				getHeight();

	public://系统
		static DimensionWorld * Create(const TCHAR * class_name);	/**< 创建子类 */
		bool			create(const TCHAR * window_title, const TCHAR * class_name, bool fullscreen, HINSTANCE h_instance, LPVOID lpParam);
		bool			ChangeScreenSetting();
		void			reshapeGL();
		void			run();
		virtual void	draw() {}
		void			quit();
		void			setWindowCaption(const std::wstring& _text);
	private:
		void			updateFPS();
		void			drawOneFrame();
		void			drawSkyBox();

	public://初始化
		bool			initSystem();
		bool			initGlewAndOther();
		bool			createGui();
		virtual void	createScene() { }
		virtual bool	init() = 0;


	private://卸载
		void			destroyAll();
		virtual void	destroyScene() { }
		void			destroyWindow();
		//void destroyRender();

	public://主系统
		friend int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
		int				Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

		/** 消息处理回调函数 */
		friend LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT			Message(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	public:
		float getMouseX()
		{
			return mouse_x;
		}

		float getMouseY()
		{
			return mouse_y;
		}

		
		HWND getHwnd()
		{
			return hWnd;
		}
	private:

		TCHAR* m_ClassName;
		bool	m_IsFullScreen;
		int		m_WindowPosX;												/**< 窗口的左上角的X位置 */
		int		m_WindowPosY;												/**< 窗口的左上角的Y位置 */
		int		m_WindowWidth;												/**< 窗口的宽度 */
		int		m_WindowHeight;												/**< 窗口的高度 */
		int		m_ScreenWidth;												/**< 全屏的宽度 */
		int		m_ScreenHeight;												/**< 全屏的高度 */
		int		m_BitsPerPixel;

		

		HWND hWnd;
		HDC hDC;
		HGLRC hRC;
		HINSTANCE hInstance;

		bool mExit;
		bool isEnableSkyBox;
		bool isEnableUI;

		float mouse_x, mouse_y;

		std::string mRootMedia;
		std::string mResourceFileName;



	};

} // namespace base

#endif // __BASE_MANAGER_H__
