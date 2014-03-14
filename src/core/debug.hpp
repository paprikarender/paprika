#ifndef INC_DEBUG_H
#define INC_DEBUG_H

#include <libpaprika_export.hpp>
#include <assert.h>

namespace paprika {
namespace core {

// Setup printf format
#ifdef __GNUG__
# define GNUG_PRINTF_FUNC __attribute__ ((__format__ (__printf__, 1, 2)))
#else
# define GNUG_PRINTF_FUNC
#endif // __GNUG__

extern LIBPAPRIKA_EXPORT void Info(const char *, ...) GNUG_PRINTF_FUNC;
extern LIBPAPRIKA_EXPORT void Warning(const char *, ...) GNUG_PRINTF_FUNC;
extern LIBPAPRIKA_EXPORT void Error(const char *, ...) GNUG_PRINTF_FUNC;
extern LIBPAPRIKA_EXPORT void Severe(const char *, ...) GNUG_PRINTF_FUNC;

#ifdef NDEBUG
#define Assert(expr) ((void)0)
#else
//#define Assert(expr) ((expr) ? (void)0 : core::Severe("Assertion \"%s\" failed in %s, line %d", #expr, __FILE__, __LINE__))
#define Assert(expr) assert(expr)
#endif /* NDEBUG */

}		// util
}		// paprika
#endif	// DEBUG_H
