#ifndef LIBPAPRIKA_EXPORT_HPP
#define LIBPAPRIKA_EXPORT_HPP

#ifdef WIN32
# define DECL_EXPORT __declspec(dllexport)
# define DECL_IMPORT __declspec(dllimport)
#else
# define DECL_EXPORT
# define DECL_IMPORT
#endif

#ifdef LIBPAPRIKA_EXPORTS
# define LIBPAPRIKA_EXPORT DECL_EXPORT
#else
# define LIBPAPRIKA_EXPORT DECL_IMPORT
#endif

#endif	/* LIBPAPRIKA_EXPORT_HPP */
