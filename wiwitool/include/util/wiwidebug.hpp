// Debug macro

#pragma once

#if !defined(WIWIRELEASE) && defined(WIWIDEBUG)
#  define wiwidebug if (true)
#else
#  define wiwidebug if (false)
#endif
