# easy-expression-templates
a set of functions that make it incredibly easy to use expression templates

There is not much to say about this project. It is an attempt to allow the creation of generic expression templates.
Currently, the only functions are "call", "take", "ref", and "use".
They all produce an object that, when invoked without any arguments, will return a value. Their return types all match an imaginary "expression template" concept that should be implemented when concepts are out. 
Their results should, for now, be taken with auto.

"call" will take an invokable object, followed by arguments wrapped by "take", "ref", "use", and "call" itself.
"take" is like passing by value. It will take (via move construction) and store the passed object until the template is evaluated, at which point it is moved into the function call.
"ref" is like passing by reference. It keeps a pointer to the passed object and dereferences it when the expression template is evaluated.
"use" is like passing by r-value reference, with a very similar use case to take. It keeps a pointer to the passed object, dereferences it, and then std::moves it when the template is evaluated.
