#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>

class Renderer
{
public:
	// deleting copy constructor
	Renderer(const Renderer& obj)
		= delete;

	// returns the single instance of this class
	static Renderer* getInstance()
	{
		if (instance_ptr_ == NULL)
		{
			instance_ptr_ = new Renderer();
			return instance_ptr_;
		}
		else
		{
			return instance_ptr_;
		}
	}



private:
	static Renderer* instance_ptr_;
	Renderer() {}
};

#endif // !RENDERER_H
