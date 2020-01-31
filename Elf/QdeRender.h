#ifndef __QDERENDER_H__
#define __QDERENDER_H__

#include <vector>

class QdeRender
{
public:
	enum RENDERATTR{FRAME,MODEL,SHADER};
public:
	QdeRender();
	~QdeRender();
	virtual void render()
	{
	
	}

public:
	std::string name;
	RENDERATTR rootType;
};
#endif