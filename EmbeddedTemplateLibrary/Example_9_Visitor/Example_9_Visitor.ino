// Example_9_Visitor.ino
// visitor and visitable for C++11 to be like variant for C++17

// Example code from the documentation https://www.etlcpp.com/visitor.html
// Eventually it works.....

// I did eventually find the documentation
// https://www.etlcpp.com/visitor_tutorial.html

// make sure that we do not rely on the STL
#define ETL_NO_STL

#include "Embedded_Template_Library.h"  // This is required for any more etl import when using Arduino IDE
#include "etl/visitor.h"
//#include "etl/variant.h"

String board(ARDUINO_BOARD);

class Square;
class Circle;
class Triangle;

using ShapeVisitor = etl::visitor<Square& , Circle&, const Triangle&>;

class Shape : public etl::visitable<ShapeVisitor>
{
  // Shape based objects are visitable by a ShapeVisitor
  // The example did NOT tell me that definitions of accept are needed.
  // public:
  //void accept(ShapeVisitor& visitor) { visitor.visit(*this); }
};

class Square : public Shape
{
public:
  void accept(ShapeVisitor& visitor) { visitor.visit(*this); }

};

class Circle : public Shape
{
  public:
  void accept(ShapeVisitor& visitor) { visitor.visit(*this); }

};

class Triangle : public Shape
{
  public:
  void accept(ShapeVisitor& visitor) { visitor.visit(*this); }

};

class Visitor : public ShapeVisitor
{
public:

  void visit(Square& s) override
  {
    // Square passed by value
    Serial.println("Square");
  }
  
  void visit(Circle& c) override
  {
    // Square passed by reference
    Serial.println("Circle");
  }

  void visit(const Triangle& t) override
  {
    // Square passed by const reference
    Serial.println("Triangle");
  }

};

Visitor visitor;

// Note that square has another meaning in Arduino code...
Square   square_;
Circle   circle;
Triangle triangle;


void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println("Embedded Template Library Example 9 with etl/visitor");
  Serial.print("running on ");
  Serial.println(board);
  
  square_.accept(visitor);   // visitor's visit(Square) is called.
  circle.accept(visitor);   // visitor's visit(Circle&) is called.
  triangle.accept(visitor); // visitor's visit(const Triangle&) is called.

  Serial.println("End of setup");
}


void loop()
{
  
}

