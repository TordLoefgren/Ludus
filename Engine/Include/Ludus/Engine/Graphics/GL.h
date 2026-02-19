#pragma once

// Use only lean WIN32 macros import to reduce risk of macro redefinition warnings.
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

// Fix macro redefinition warnings.
#ifdef APIENTRY
#undef APIENTRY
#endif
#endif

#include <glad/glad.h>
