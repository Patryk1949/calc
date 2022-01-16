# calc
simple command line calculator


# program expects string as an input
# example:
$ calc "x = 21   2 + sin x - 3"
# it can also execute text files:
$ calc -f math_expression.txt

# syntax
spaces, tabs and newlines have no meaning
commas are optional, but sometimes they are required

build in functions can be called with or without pharenthesis
user defined functions must be called with pharenthesis

variable definition syntax:
  x1 = pi / 2
 
function definition syntax:
  fib = (n){ [n<=2] 1 fib(n-1)+fib(n-2) }
  hypot = (x y){ sqrt(x^2 + y^2) }
  
sum, product and integral syntax:
  sum[i=1 10] 1/x^2
  prod[i=1 10] 1/x^2
  int[x=1 10] 1/x^2

To make an use of the recursion there is also a conditional operator
syntax: [condition] on_true on_false
Conditin is evaluated using comparison operators: == != < > <= >=
Condiational operators return boolean value.
Opeartors + an * applied to boolean values work as or and and respectively.



# unfinished (barely started) features
plotting with command "plot[x=0 10 0 10] f(x)" using sfml or sdl library
