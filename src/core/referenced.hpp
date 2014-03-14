#ifndef REFERENCED_HPP
#define REFERENCED_HPP

#include <core/debug.hpp>

namespace paprika {
namespace core {

class Referenced
{
public:
	void ref()
	{
		++refcount_;
	}

	void unref()
	{
		Assert(refcount_ > 0);

		if (--refcount_ == 0)
		{
			refcount_ = 1;
			delete this;
		}
	}

	int refCount() const
	{
		return refcount_;
	}

protected:
	Referenced() : refcount_(1)
	{
	}
	
	virtual ~Referenced()
	{
		Assert(refcount_ == 1);
	}
	
	int refcount_;

private:
	Referenced(const Referenced&);
	Referenced &operator=(const Referenced&);
};

}		/* base */
}		/* paprika */
#endif	/* REFERENCED_HPP */
