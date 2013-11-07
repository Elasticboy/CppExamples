#include <iostream>
#include <string>
#include <utility>

template <typename T> class Shared_pointer
{
private:
	T* data_;
	int* refCount_;
	
	/** Increment and return the refCount. */
	inline int incrementRef() { return ++(*refCount_); }
	
	/** Decrement and return the refCount. */
	inline int decrementRef() { return --(*refCount_); }
	
	/** Default constructor. */
	Shared_pointer(T* data) : data_(data), refCount_(new int())
	{
		refCount_ = new int(1);
	}
	
public:

	/** make_shared using variadic templates. */
	template <typename... Args>
	static inline Shared_pointer<T> make_shared(Args... args)
	{
		return Shared_pointer<T>(new T(std::forward<Args>(args)...));
	}

	/** Copy constructor. */
	Shared_pointer(const Shared_pointer<T>& source) : data_(source.data_), refCount_(source.refCount_)
	{
		incrementRef();
		std::cout << "  Shared_pointer(copy) : refCount is " << *refCount_ << std::endl;
	}

	/** Destructor. */
	~Shared_pointer()
	{
		std::cerr << "  ~Shared_pointer() : refCount is " << *refCount_ << std::endl;
		
		if (decrementRef() <= 0) {
			std::cerr << "  ~Shared_pointer() : Destruction of the data." << std::endl;
			delete data_;
			delete refCount_;
		}
	}

	Shared_pointer<T>& operator =(const Shared_pointer<T>& source)
	{
		// Decrement the current ref
		decrementRef();
		
		data_ = source.data_;
		refCount_ = source.refCount_;
		
		// Increment the new ref
		incrementRef();
		
		std::cout << "  operator = : Refcount is " << *refCount_ << std::endl;
		
		return *this;
	}
	
	// Overload pointer operators
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
		std::cout << name_ << " : Hi, I'm a new robot." << std::endl;
	}

	~Robot()
	{
		std::cout << name_ << " : Goodbye." << std::endl;
	}
};

int main()
{
	Robot basicRobot = Robot("BasicRobot");
	
	Shared_pointer<Robot> nao = Shared_pointer<Robot>::make_shared("Nao");
	Shared_pointer<Robot> romeo = Shared_pointer<Robot>::make_shared("Romeo");
	
	Shared_pointer<Robot> romeoCopy = romeo;
	Shared_pointer<Robot> naoCopy(nao);
	
	romeo = nao;
	
	return 0;
}
