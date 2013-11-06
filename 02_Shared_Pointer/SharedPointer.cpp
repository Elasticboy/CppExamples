#include <iostream>
#include <string>
#include <utility>

template <typename T> class Shared_pointer
{
private:
	T* data_;
	int* refCount_;

public:

	template <typename T, typename... Args>
	static inline Shared_pointer<T> make_shared(Args... args) {
		return Shared_pointer<T>(new T(std::forward<Args>(args)...));
	}

	Shared_pointer(T* data) : data_(data), refCount_(new int())
	{
		std::cout << "Refcount is null => assign" << *refCount_ << std::endl;
		refCount_ = new int(1);
		std::cout << "Shared_pointer() : Refcount is " << *refCount_ << std::endl;
	}

	Shared_pointer(const Shared_pointer<T>& source) : data_(source.data_), refCount_(source.refCount_)
	{
		(*refCount_)++;
		std::cout << "Shared_pointer(copy) : Refcount is " << *refCount_ << std::endl;
	}

	~Shared_pointer()
	{
		if (!refCount_) {
			std::cerr << "~Shared_pointer() : Refcount is null." << std::endl;
			delete data_;
			return;
		}

		std::cerr << "~Shared_pointer() : Refcount is " << *refCount_ << std::endl;
		if (--(*refCount_) <= 0) {
			delete data_;
			delete refCount_;
		}
	}

	T& operator *() { return *data_; }

	T* operator ->() { return data_; }

};

class Robot
{
private:
	std::string name_;
	int batterie_;

public:

	Robot(const std::string& name) : name_(name)
	{
		std::cout << "Hi, I'm a new robot. My name is " << name_ << std::endl;
	}

	~Robot()
	{
		std::cout << "Goodbye " << name_ << std::endl;
	}
};

int main()
{
	Robot nao = Robot("Nao");
	Shared_pointer<Robot> spRomeo = Shared_pointer<Robot>(new Robot("Romeo"));
	Shared_pointer<Robot> spRomeoCopy = spRomeo;

	//Shared_pointer<Robot> spRobot = Shared_pointer<Robot>::make_shared("SP_Robot");

	return 0;
}
