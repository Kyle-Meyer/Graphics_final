///============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  Brian Russin
//	File:	 preprocessor.hpp
//	Purpose: Preprocessor definitions
//============================================================================

#ifndef __FILESYSTEM_SUPPORT_PREPROCESSOR_HPP__
#define __FILESYSTEM_SUPPORT_PREPROCESSOR_HPP__

namespace cg
{

#define STRINGIFY(x) #x
#define C_STRING(x) STRINGIFY(x)
#define STD_STRING(x) std::string(STRINGIFY(x))

} // namespace cg

#endif
