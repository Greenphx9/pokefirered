/* Macros to aid with metaprogramming. */
#ifndef METAPROGRAM_H
#define METAPROGRAM_H

/* Returns the number of arguments passed to it (up to 8). */
#define NARG_8(...) NARG_8_(_, ##__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define NARG_8_(_, a, b, c, d, e, f, g, h, N, ...) N

/* Expands 'a' and 'b' and then concatenates them. */
#define CAT(a, b) CAT_(a, b)
#define CAT_(a, b) a ## b

/* Expands to the first/second/third/fourth argument. */
#define FIRST(a, ...) a
#define SECOND(a, ...) __VA_OPT__(FIRST(__VA_ARGS__))
#define THIRD(a, ...) __VA_OPT__(SECOND(__VA_ARGS__))
#define FOURTH(a, ...) __VA_OPT__(THIRD(__VA_ARGS__))

/* Picks the xth VA_ARG if it exists, otherwise returns a default value */
#define DEFAULT(_default, ...) FIRST(__VA_OPT__(__VA_ARGS__, ) _default)
#define DEFAULT_2(_default, ...) DEFAULT(_default __VA_OPT__(, SECOND(__VA_ARGS__)))
#define DEFAULT_3(_default, ...) DEFAULT(_default __VA_OPT__(, THIRD(__VA_ARGS__)))
#define DEFAULT_4(_default, ...) DEFAULT(_default __VA_OPT__(, FOURTH(__VA_ARGS__)))

/* Converts a string to a compound literal, essentially making it a pointer to const u8 */
#define COMPOUND_STRING(str) (const u8[]) _(str)

#endif