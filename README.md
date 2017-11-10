Compiler for some statically typed language.

extern def print(s: String): None

``` scala
def main(): None = {
   var arr = Array[String](4);
   arr(0) = "a mulatto";
   arr(1) = "an albino";
   arr(2) = "a mosquito";
   arr(3) = "my libido";

   var reversed = map[String, String](arr, reverse);

   foreach[String](reversed, printLine);
}

def map[A, B](xs: Array[A], f: (A) => B): Array[B] = {
   var result = Array[B](size(xs));
   var i = 0;
   while (i < size(xs)) {
      result(i) = f(xs(i));
      i = i + 1;
   }

   return result;
}

def foreach[T](xs: Array[T], f: (T) => None): None = {
   for (var i = 0; i < size(xs); i = i + 1)
      f(xs(i));
}

```