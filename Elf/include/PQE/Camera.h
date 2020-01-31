#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <Windows.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera()
	{
		position = glm::vec3(0.0, 0.0, 10.0);
		view = glm::vec3(0.0, 0.0, 0.0);
		up = glm::vec3(0.0, 1.0, 0.0);
		speed = 0.1f;
	}

	~Camera();

	void init(glm::vec3 mposition = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 meye = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 mup = glm::vec3(0.0f, 1.0f, 0.0f))
	{
		position = mposition;
		view = meye;
		up = mup;
		speed = 5.0f;
	}

	glm::mat4 getViewMat()
	{
		return glm::lookAt(position, view,up);
	}

	void rotateView(float angle, float x, float y, float z)
	{
		glm::vec3 newView;
		glm::vec3 mview = view - position;

		float cosTheta = (float)cos(angle);
		float sinTheta = (float)sin(angle);

		/** 计算旋转向量的x值 */
		newView.x = (cosTheta + (1 - cosTheta) * x * x)		* mview.x;
		newView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* mview.y;
		newView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* mview.z;

		/** 计算旋转向量的y值 */
		newView.y = ((1 - cosTheta) * x * y + z * sinTheta)	* mview.x;
		newView.y += (cosTheta + (1 - cosTheta) * y * y)		* mview.y;
		newView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* mview.z;

		/** 计算旋转向量的z值 */
		newView.z = ((1 - cosTheta) * x * z - y * sinTheta)	* mview.x;
		newView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* mview.y;
		newView.z += (cosTheta + (1 - cosTheta) * z * z)		* mview.z;

		view = position + newView;
	}

	void useMouse()
	{
		POINT mousePos;									  /**< 保存当前鼠标位置 */
		int middleX = GetSystemMetrics(SM_CXSCREEN) >> 1; /**< 得到屏幕宽度的一半 */
		int middleY = GetSystemMetrics(SM_CYSCREEN) >> 1; /**< 得到屏幕高度的一半 */
		float angleY = 0.0f;							  /**< 摄像机左右旋转角度 */
		float angleZ = 0.0f;		                      /**< 摄像机上下旋转角度 */
		static float currentRotX = 0.0f;

		/** 得到当前鼠标位置 */
		GetCursorPos(&mousePos);
		ShowCursor(TRUE);

		/** 如果鼠标没有移动,则不用更新 */
		if ((mousePos.x == middleX) && (mousePos.y == middleY))
			return;

		/** 设置鼠标位置在屏幕中心 */
		SetCursorPos(middleX, middleY);

		/**< 得到鼠标移动方向 */
		angleY = (float)((middleX - mousePos.x)) / 1000.0f;
		angleZ = (float)((middleY - mousePos.y)) / 1000.0f;

		static float lastRotX = 0.0f;      /**< 用于保存旋转角度 */
		lastRotX = currentRotX;

		/** 跟踪摄像机上下旋转角度 */
		currentRotX += angleZ;

		/** 如果上下旋转弧度大于1.0,我们截取到1.0并旋转 */
		if (currentRotX > 1.0f)
		{
			currentRotX = 1.0f;

			/** 根据保存的角度旋转方向 */
			if (lastRotX != 1.0f)
			{
				/** 通过叉积找到与旋转方向垂直的向量 */
				glm::vec3 vAxis = view - position;
				vAxis=glm::cross(vAxis,up);
				vAxis=glm::normalize(vAxis);
				///旋转
				rotateView(1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
			}
		}
		/** 如果旋转弧度小于-1.0,则也截取到-1.0并旋转 */
		else if (currentRotX < -1.0f)
		{
			currentRotX = -1.0f;

			if (lastRotX != -1.0f)
			{

				/** 通过叉积找到与旋转方向垂直的向量 */
				glm::vec3 vAxis = view - position;
				vAxis = glm::cross(vAxis, up);
				vAxis = glm::normalize(vAxis);

				///旋转
				rotateView(-1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
			}
		}
		/** 否则就旋转angleZ度 */
		else
		{
			/** 找到与旋转方向垂直向量 */
			glm::vec3 vAxis = view - position;
			vAxis = glm::cross(vAxis, up);
			vAxis = glm::normalize(vAxis);

			///旋转
			rotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
		}

		/** 总是左右旋转摄像机 */
		rotateView(angleY, 0, 1, 0);
	}

	void leftMove(bool right)
	{
		float mSpeed = speed;
		if (!right)
		{
			mSpeed *= -1;
		}
		glm::vec3 left = glm::normalize(glm::cross(view - position,up));

		position.x += left.x*mSpeed;
		position.z += left.z*mSpeed;

		view.x += left.x*mSpeed;
		view.z += left.z*mSpeed;
	}

	void fontMove(bool back)
	{
		float mSpeed = speed;
		if (back)
		{
			mSpeed *= -1;
		}
		glm::vec3 fong = glm::normalize(view - position);
		position.x += fong.x*mSpeed;
		position.y += fong.y*mSpeed;
		position.z += fong.z*mSpeed;

		view.x += fong.x*mSpeed;
		view.y += fong.y*mSpeed;
		view.z += fong.z*mSpeed;
	}

	glm::vec3 getPosition()
	{
		return position;
	}

	void channgeSpeed()
	{
		if (speed == 1.0f)
		{
			speed = 0.1f;
		}
		else
		{
			speed = 1.0f;
		}
	}

	glm::vec3 &getView()
	{
		return view;
	}
private:
	glm::vec3 position;
	glm::vec3 view;
	glm::vec3 up;
	float speed;
};

#endif

