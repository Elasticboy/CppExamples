#include <iostream>

class Shape {
public:
	Shape() {
		std::cout << "Building Shape" << std::endl;
	}
	void draw() {
		std::cout << "Shape::draw" << std::endl;
		do_draw();
	}
	virtual void virtual_draw() = 0;

private:
	Shape& operator=(const Shape&);

	virtual void do_draw() {
		std::cout << "Shape::do_draw" << std::endl;
	}
};

class Square : public Shape {
public:
	Square() {
		std::cout << "Building Square" << std::endl;
	}

	virtual void virtual_draw() override {
		std::cout << "Square::virtual_draw" << std::endl;
	}

	Square get() {
		return Square(*this);
	}

private:
	virtual void do_draw() {
		std::cout << "Square::do_draw" << std::endl;
	}
};


class Triangle : public Shape {
public:
	Triangle() {
		std::cout << "Building Triangle" << std::endl;
	}

	virtual void virtual_draw() override { 
		std::cout << "Triangle::virtual_draw" << std::endl;
	}

private:
	virtual void do_draw() {
		std::cout << "Triangle::do_draw" << std::endl;
	}
};

class WeirdShape : public Square, public Triangle {
public:
	WeirdShape() {
		std::cout << "Building WeirdShape" << std::endl;
	}

	virtual void virtual_draw() override {
		std::cout << "WeirdShape::virtual_draw" << std::endl;
	}

private:
	virtual void do_draw() {
		std::cout << "WeirdShape::do_draw" << std::endl;
	}
};

int main() {
	Square square;
	square.draw();
	square.virtual_draw();
	std::cout << std::endl;

	Shape* triangle = new Triangle();
	triangle->draw();
	triangle->virtual_draw();
	std::cout << std::endl;

	WeirdShape weird;
	weird.virtual_draw();


	system("PAUSE");
	return EXIT_SUCCESS;
}