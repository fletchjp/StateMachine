// Example_11_Overload.ino
// Extending to test out Overload ideas which I am using in the State Machine.
// Copying some code from StateMachine_R3_A which is not working at the moment.
// I am having success in getting etl::overload to work although the documentation
// says it needs C++17 to work. See the test example at the end of this code.

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

//#include <Arduino.h>
#include "Embedded_Template_Library.h"  // This is required for any more etl import when using Arduino IDE
#include "etl/visitor.h"
#include "etl/variant.h"
#include "etl/tuple.h"

#include "Overload.h"

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

// ETL documentation says this needs C++17. I have it working!!
auto eventOverload = etl::make_overload(
  [](EventA &a) { Serial.print("EventA : "); Serial.println(a.msg); },
  [](EventB &b) { Serial.print("EventB : "); Serial.println(b.number); }
);

using TypeVisitor = etl::visitor<int& , const char *>;

class TestVisitor : public TypeVisitor {

public:
    void visit(int &i) { Serial.println(i);}
    void visit(const char * s) { Serial.println(s);}

};

TestVisitor testVisitor;

// ETL documentation says this needs C++17. I have it working!!
auto testOverload = etl::make_overload(
  [](int i)         { Serial.println(i); return i; },
  [](const char* s) { Serial.println(s); return s; }
);

// Using an earlier stage
struct PrintInt { //(1)
    void operator() (int i) {
        Serial.println(i);
    }
} printInt;

struct PrintCString { // (2)
    void operator () (const char* str) {
        Serial.println(str);
    }
} printCString;

struct MyPrint : PrintInt, PrintCString { // (3)
    using PrintInt::operator();
    using PrintCString::operator();
};

void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println("Embedded Template Library Example 11 with etl/visitor and etl/variant");
  Serial.print("running on ");
  Serial.println(board);
  
  square_.accept(visitor);   // visitor's visit(Square) is called.
  circle.accept(visitor);   // visitor's visit(Circle&) is called.
  triangle.accept(visitor); // visitor's visit(const Triangle&) is called.
  Serial.println("tests of variant");
  Event events[] = {EventB{},EventB{},EventA{}};
  //Serial.println(events.get(0).number);
  for (auto evt : events) {
		   // This unpacks the std::pair returned.
		   // etl::tie(state, ctx) = state(ctx, evt);
       Serial.print("found an event: ");
       evt.accept(eventVisitor);
       evt.accept(eventOverload);
	}
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
  Serial.println("Explore etl::tuple");

  auto example = etl::make_tuple(1,2.2);
  Serial.println("auto example = etl::make_tuple(1,2.2)");
  int i1; double d2;
  etl::tie(i1,d2) = example;
  Serial.print("etl::tie(i1,d2) = example gives "); Serial.print(i1); Serial.print(", "); Serial.println(d2);

// Overload example
  Serial.println("Working towards overload implementation in C++11");
  using test_t = etl::variant<int, const char*>;

//  struct test_v {
//     test_t t;
//  };
 // This does not work.
 //etl::vector<test_t,10> tests; // It does do a vector of variant. I have to give it a SIZE!

  int t1 = 1;
  test_t test1 = t1;
  const char* ts = "test";
  test_t teststring = ts;
  test_t testv[] = {test1,teststring,2}; // This works

  Serial.println(etl::get<int>(test1));
  Serial.println(etl::get<int>(testv[0]));
  Serial.println(etl::get<const char*>(testv[1]));
  Serial.println(etl::get<int>(testv[2]));
  Serial.println("Version using visitor now works.");
  for (auto atest : testv) {
		   // This unpacks the std::pair returned.
		   // etl::tie(state, ctx) = state(ctx, evt);
       Serial.print("found a test using visitor: ");
       atest.accept(testVisitor);
       Serial.print("found a test using overload: ");
       atest.accept(testOverload);
	}
/* These crash it
  tests[0] = test1;
  tests[1] = teststring;
 */
//  using testOverload = makeOverload<int, const char*>; // (EventA ea, EventB eb ); // -> overloaded<EventA, T2>
// auto myOverload = makeOverload(t1,ts){}
// auto myOverload = makeOverload(printInt,printCString);

  Serial.println("End of setup");
}


void loop()
{
  
}

