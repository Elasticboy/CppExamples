#include <iostream>

class Shape {
public:
	Shape() {
		std::cout << "Building Shape" << std::endl;
	}

	void draw() {
		std::cout << "Shape::draw" << std::endl;
	}
	virtual void virtual_draw() = 0;
};

class Square : public Shape {
public:
	Square() {
		std::cout << "Building Square" << std::endl;
	}

	void draw() {
		std::cout << "Square::draw" << std::endl;
	}
	virtual void virtual_draw() override { draw(); }
};


class Triangle : public Shape {
public:
	Triangle() {
		std::cout << "Building Triangle" << std::endl;
	}

	void draw() {
		std::cout << "Triangle::draw" << std::endl;
	}
	virtual void virtual_draw() override { draw(); }
};

class WeirdShape : public Square, public Triangle {
public:
	WeirdShape() {
		std::cout << "Building WeirdShape" << std::endl;
	}

	void draw() {
		std::cout << "WeirdShape::draw" << std::endl;
	}
	virtual void virtual_draw() override { draw(); }
};

int main() {
	Square square;
	square.virtual_draw();
	std::cout << std::endl;

	Shape* triangle = new Triangle();
	triangle->virtual_draw();
	std::cout << std::endl;

	WeirdShape weird;
	weird.draw();
	weird.virtual_draw();


	system("PAUSE");
	return EXIT_SUCCESS;
}