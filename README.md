Compiler for some statically typed language backed by LLVM.

``` scala
extern def allocate(size: Int): Ref
extern def allocateArray(count: Int, elemSize: Int): Ref
def Array[T](size: Int): Array[T] = {
   return allocateArray(size, sizeof[T]) as Array[T];
}

extern def concat(s1: String, s2: String): String
extern def print(s: String): None
extern def readline(): String
extern def getchar(): Char
extern def len(s: String): Int
extern def compare(s1: String, s2: String): Int
extern def int2string(i: Int): String
extern def double2string(d: Double): String
extern def char2string(ch: Char): String
extern def size(xs: Array[_]): Int
extern def string2int(s: String): Int
extern def strcmp(s1: String, s2: String): Int

struct Employee(name: String, salary: Double, yearsInCompany: Int, perfRate: Double)

def main(): None = {
   var emps = getEmployees();

   var toBeFired = filter(emps, e => e.perfRate < 0.8);
   // generic parameters, type of lambda and return type are infered
   var promotionFund = reduce(toBeFired, 0.0, (z, x) => z + x.salary); 
   var empsLeft = filter(emps, e => e.perfRate >= 0.8);
   var overallPerf = reduce(empsLeft, 0.0, (z, x) => z + x.perfRate);

   printLine("To be fired:");
   foreach(toBeFired, e => printLine(e.name));

   printLine("To be promoted:");
   for (var i = 0; i < size(empsLeft); ++i) {
      var emp = empsLeft(i);
      printLine(emp.name + "\t\t " + promotionFund * emp.perfRate / overallPerf);
   }

}

def getEmployees(): Array[Employee] = {
   var employees = Array[Employee](10);
   employees(0) = Employee("Jack Mulatto", 46000.0, 3, 1.1);
   employees(1) = Employee("John Albino", 39000.0, 1, 1.4);
   employees(2) = Employee("Simon Mosquito", 35000.0, 4, 1.7);
   employees(3) = Employee("Jane Libido", 56000.0, 3, 1.0);
   employees(4) = Employee("Chris Corrado", 58000.0, 2, 0.7);
   employees(5) = Employee("Brian Placebo", 50000.0, 3, 1.1);
   employees(6) = Employee("Anna Mario", 45000.0, 3, 1.3);
   employees(7) = Employee("Harry Potato", 60000.0, 2, 1.2); 
   employees(8) = Employee("Albert Relatio", 49000.0, 2, 0.6);
   employees(9) = Employee("Bad Tattoo", 46000.0, 3, 1.1);

   return employees;
}

def printLine(s: String): None = {
   print(s);
   print("\n");
}

def foreach[T](xs: Array[T], f: T => None): None = {
   for (var i = 0; i < size(xs); ++i)
      f(xs(i));
}

def reduce[T, R](xs: Array[T], z: R, f: (R, T) => R): R = {
   var result = z;
   for (var i = 0; i < size(xs); ++i)
      result = f(result, xs(i));   

   return result;
}

def count[T](xs: Array[T], f: T => Boolean): Int = {
   var result = 0;

   for (var i = 0; i < size(xs); ++i)
      if (f(xs(i)))
         ++result;   

   return result;
}

def filter[T](xs: Array[T], f: T => Boolean): Array[T] = {
   var result = Array[T](count[T](xs, f));

   var i = 0;
   var j = 0;
   while (i < size(xs)) {
      if (f(xs(i))) {
         result(j) = xs(i);
         ++j;
      }
      ++i;
   }

   return result;
}

def map[A, B](xs: Array[A], f: A => B): Array[B] = {
   var result = Array[B](size(xs));
   for (var i = 0; i < size(xs); ++i)
      result(i) = f(xs(i));   

   return result;
}
```
