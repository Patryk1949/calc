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

variable definition:
  x1 = pi / 2
 
function definition:
  func = (arg_x arg_y){ (arg_x + 1) / arg(z) }
  
sum, product and integral syntax:
  sum[i=1 10] 1/x^2
  prod[i=1 10] 1/x^2
  int[x=1 10] 1/x^2

To make use of the recursion there is also a conditional operator
With syntax: [condition] on_true on_false
Conditin is evaluated using comparison operators: == != < > <= >=
Condiational operators return boolean value.
Opeartors + an * applied to boolean values work as or and and respectively.



# unfinished (barely started) features
plotting with command "plot[x=0 10 0 10] f(x)" using sfml or sdl library
