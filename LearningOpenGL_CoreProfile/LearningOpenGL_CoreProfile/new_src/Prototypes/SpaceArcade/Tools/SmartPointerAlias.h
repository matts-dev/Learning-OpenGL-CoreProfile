#pragma once

#include <memory>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Usage example
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
int main(){

	//shared pointer
	sp<EG_Class> ptr2 = new_sp<EG_Class>();
	ptr2->Foo();

	//weak pointer
	wp<EG_Class> wp1 = ptr2;
	if (sp<EG_Class> aquired = wp1.lock())
	{
		aquired->Foo();
	}

	//unique pointer
	up<EG_Class> up1 = new_up<EG_Class>();
	up1->Foo();
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//shared pointers
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
using sp = std::shared_ptr<T>;

//template<typename T>
//const auto new_sp = std::make_shared<T>; //this has issues with arguments, perhaps need fptr with all signatures?

template<typename T, typename... Args>
sp<T> new_sp(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//weak pointers
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
using wp = std::weak_ptr<T>;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//unique pointers
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
using up = std::unique_ptr<T>;

template<typename T, typename... Args>
up<T> new_up(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//shared_from_this
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//template<typename T>
//sp<T> sp_this()
//{
//	return shared_from_this();
//}
//const auto s_this = std::shared_from_this; //untested, may need 