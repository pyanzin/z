Compiler for some statically typed language.

``` scala
extern def print(s: String): None
extern def char2string(ch: Char): String
extern def getchar(): Char
extern def len(s: String): Int
extern def concat(s1: String, s2: String): String
extern def Array[T](size: Int): Array[T]
extern def size(xs: Array[_]): Int

def main(): None = {
   var arr = Array[String](4);
   arr(0) = "a mulatto";
   arr(1) = "an albino";
   arr(2) = "a mosquito";
   arr(3) = "my libido";
   
   var filtered = filter(arr, x => len(x) == 9);

   var reversed = map(filtered, reverse);

   foreach(reversed, printLine);
}


def printLine(s: String): None = {
   print(s);
   print("\n");
}

def reverse(x: String): String = {
   var result = "";
   for (var i = len(x) - 1; i >= 0; --i)
      result = result + char2string(x(i));

   return result;
}

def map[A, B](xs: Array[A], f: A => B): Array[B] = {
   var result = Array[B](size(xs));
   for (var i = 0; i < size(xs); ++i)
      result(i) = f(xs(i));
   return result;
}

def foreach[T](xs: Array[T], f: T => None): None = {
   for (var i = 0; i < size(xs); ++i)
      f(xs(i));
}

def filter[T](xs: Array[T], f: T => Boolean): Array[T] = {
   var result = Array[T](count(xs, f));

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

def count[T](xs: Array[T], f: T => Boolean): Int = {
   var result = 0;
   for (var i = 0; i < size(xs); ++i)
      if (f(xs(i)))
         result = result + 1;   
   return result;
}

```
