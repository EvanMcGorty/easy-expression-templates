# easy-expression-templates
a set of functions that make it incredibly easy to use expression templates

there is not much to say about this project. currently, the only two functions are "call" and "take".
call takes a callable object and arguments to call it with. the result should be stored with auto, as it represents a theoretical concept for an object containing a result.
"take" will cause the expression template to be evaluated, and is the transition from expression-templaty code into normal, stateful code. use it when a value must be stored.
take will also work on a plain value, so it can be used with generic code when an argument can be either a raw value or an expression template.
