option(ENABLE_DEBUG_LOGGING "Enable the 'debug' and 'all' log-channels" TRUE)

if (HAVE_GIT_SHA1)
	define_module(util OBJECT LINKS git_sha1 boost-thread INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/..)
else()
	define_module(util OBJECT LINKS boost-thread INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/..)
endif()
