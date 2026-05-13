// Example_10_VisitorVariant.ino
// visitor and visitable for C++11 to be like variant for C++17
// Extend to use variant for which I have not found documentation....

// Example code from the documentation https://www.etlcpp.com/visitor.html
// Eventually it works.....

// I did eventually find the documentation
// https://www.etlcpp.com/visitor_tutorial.html

// make sure that we do not rely on the STL

// Variant documentation is here.
//https://www.etlcpp.com/variant_variadic.html
// I have my first success.
// I now have a way to visit a variant.

// This will enable me to implement some things on the UNO R3 with only C++11.

// The code does not run on an R4.

#define ETL_NO_STL

#include "Embedded_Template_Library.h"  // This is required for any more etl import when using Arduino IDE
#include "etl/visitor.h"
#include "etl/variant.h"

String board(ARDUINO_BOARD);

// Events are part of the user code.
struct EventA {
	const char* msg{nullptr};
};

struct EventB {
	int number{0};
};

using vars = etl::variant<int,bool>;
using Event = etl::variant<EventA, EventB>;
using EventVisitor = etl::visitor<EventA&, EventB&>;

class Square;
class Circle;
class Triangle;

using ShapeVariant = etl::variant<Square& , Circle&, const Triangle&>;
using ShapeVisitor = etl::visitor<Square& , Circle&, const Triangle&>;

class Shape : public etl::visitable<ShapeVisitor>//, etl::visitable<ShapeVariant>
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
  //void accept(ShapeVariant& variant) { variant.visit(*this); }

};

class Circle : public Shape
{
  public:
  void accept(ShapeVisitor& visitor) { visitor.visit(*this); }
  //void accept(ShapeVariant& variant) { variant.visit(*this); }

};

class Triangle : public Shape
{
  public:
  void accept(ShapeVisitor& visitor) { visitor.visit(*this); }
  //void accept(ShapeVariant& variant) { variant.visit(*this); }

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

//ShapeVariant shapeVariant;

Visitor visitor;

// Syntax not clear
// ShapeVariant::accept<ShapeVisitor>(visitor);

// Note that square has another meaning in Arduino code...
Square   square_;
Circle   circle;
Triangle triangle;

class EVisitor : public EventVisitor {

public:
    void visit(EventA &) { Serial.println("EventA");}
    void visit(EventB &) { Serial.println("EventB");}

};

EVisitor eventVisitor;

void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println("Embedded Template Library Example 9 with etl/visitor and etl/variant");
  Serial.print("running on ");
  Serial.println(board);
  
  square_.accept(visitor);   // visitor's visit(Square) is called.
  circle.accept(visitor);   // visitor's visit(Circle&) is called.
  triangle.accept(visitor); // visitor's visit(const Triangle&) is called.
  Serial.println("tests of variant");
  Event events[] = {EventB{},EventB{}};
  //Serial.println(events.get(0).number);
  auto evt = EventB{};
  Serial.print("evt holds ");
  Serial.println(evt.number);
  Serial.println("events[0] holds not yet found");
  //  Serial.println(events.get<0>.number);
  Serial.println("Variant experiments");
  Serial.print("events[0].accept(eventVisitor) = ");
  events[0].accept(eventVisitor);
  // I have not declared the types allowed.
  auto one = etl::variant<int>(1);
  one = 2;
  Serial.print ("one = "); Serial.println(etl::get<int>(one));
  // Here I am using a list of allowed types.
  auto what = vars(1);

  Serial.print ("vars(1) = "); Serial.println(etl::get<int>(what));
  Serial.println("End of setup");
}


void loop()
{
  
}

