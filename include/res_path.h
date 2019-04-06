#ifndef RES_PATH_H
#define RES_PATH_H

#include <iostream>
#include <string>
#include <SDL.h>

std::string getResourcePath(const std::string& subDir = "") {

#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif

	static std::string baseRes; //space for the static variable is allocated only once and the value of variable in the previous call gets carried through the next function call
	if (baseRes.empty()) {
		char* basePath = SDL_GetBasePath();
		if (basePath) {
			baseRes = basePath;
			SDL_free(basePath); // dellocates
		}
		else {
			std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
			return ""; 
		}
		size_t pos = baseRes.rfind("Debug");
		baseRes = baseRes.substr(0, pos) + "res" + PATH_SEP;


	}
	return subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;
}

#endif