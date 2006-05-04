#include "pathsearch.h"
#include "robstring1.2.h"
#include "profile.h"

#include <map>
#include <vector>
#include <string>
#include "stdio.h"
/// TODO : check platform ifdefs ?

#ifdef WIN32
// TODO : #elseif MAC or something like that
// TODO : #elseif LINUX or something like that
#else
	// this is the linux code
	#include <dirent.h>
#endif
std::map<std::string,std::vector<std::string>* > gpDirCache_Dirs;
std::map<std::string,std::vector<std::string>* > gpDirCache_Files;


/// attempts to search for an equivalent path on case-sensitive file systems
std::string rob_pathsearch (const std::string& sOldPath) { PROFILE
	//printf("pathsearch(%s)\n",sOldPath.c_str());
	
	std::string res;
	std::string test;
	std::string pathpart;
	std::vector<std::string> pathparts;
	std::vector<std::string> contents;
	explodestr("/",sOldPath.c_str(),pathparts);

	unsigned int i;
	for (i=0;i<pathparts.size();++i) {
		pathpart = pathparts[i];
		test = strprintf("%s/%s",res.c_str(),pathpart.c_str());
		if (i==pathparts.size()-1) {
			// last part of path is filename 
			if (rob_fileexists(test.c_str())) {
				return test;
			} else {
				test = rob_dirfindi(res.c_str(),pathpart,false,true);
				if (test.size() == 0) return ""; // nothing found
				return strprintf("%s/%s",res.c_str(),test.c_str()); // success !
			}
		} else {
			// directory
			if (rob_direxists(test.c_str())) {
				res = test;
			} else {
				// quick search : capitalize first letter
				pathpart[0] = toupper(pathpart[0]);
				test = strprintf("%s/%s",res.c_str(),pathpart.c_str());
				if (rob_direxists(test.c_str())) {
					// quick search success
					res = test;
				} else {
					// long search : list directory contents and compare case insensitive
					test = rob_dirfindi(res.c_str(),pathpart,true,false);
					if (test.size() == 0) return ""; // nothing found
					res = strprintf("%s/%s",res.c_str(),test.c_str()); // success !
				}
			}
		}
	}
	return "";
}

bool		rob_direxists		(const char* path) { PROFILE
	#ifdef WIN32
	// TODO : #elseif MAC or something like that
	// TODO : #elseif LINUX or something like that
	#else
	// this is the linux code
		DIR* d = opendir(path);
		if (d) { closedir(d); return true; }
	#endif
	return false;
}

bool		rob_fileexists		(const char* path) { PROFILE
	#ifdef WIN32
	// TODO : #elseif MAC or something like that
	// TODO : #elseif LINUX or something like that
	#else
	// this is the linux code
		FILE* f = fopen(path,"r");
		if (f) { fclose(f); return true; }
	#endif
	return false;
}

void			rob_dirlist			(const char* path,std::vector<std::string>& res,const bool bDirs,const bool bFiles) { PROFILE
	#ifdef WIN32
	// TODO : #elseif MAC or something like that
	// TODO : #elseif LINUX or something like that
	#else
		// this is the linux code
		DIR *d = opendir(path);
		if (!d) return;
		struct dirent *e;
		e = readdir(d);
		while (e != NULL) {
			if ((bDirs && e->d_type == DT_DIR) ||
				(bFiles && e->d_type != DT_DIR)) {
				res.push_back(std::string(e->d_name));
			}
			e = readdir(d);
		}
		closedir(d);
	#endif
}

/*
// cached variant, maybe unsave ??
std::string		rob_dirfindi		(const char* path,const std::string& name,const bool bDirs,const bool bFiles) { PROFILE
	std::string spath(path);
	std::vector<std::string>* cache;
	std::string lowername = strtolower(name.c_str());
	unsigned int i;
	if (bDirs) {
		cache = gpDirCache_Dirs[spath];
		if (!cache) {
			cache = new std::vector<std::string>();
			rob_dirlist(path,*cache,true,false);
			gpDirCache_Dirs[spath] = cache;
		}
		for (i=0;i<cache->size();++i) if (strtolower((*cache)[i].c_str()) == lowername) return (*cache)[i];
	}
	if (bFiles) {
		cache = gpDirCache_Files[spath];
		if (!cache) {
			cache = new std::vector<std::string>();
			rob_dirlist(path,*cache,false,true);
			gpDirCache_Files[spath] = cache;
		}
		for (i=0;i<cache->size();++i) if (strtolower((*cache)[i].c_str()) == lowername) return (*cache)[i];
	}
	return "";
}
*/

std::string		rob_dirfindi		(const char* path,const std::string& name,const bool bDirs,const bool bFiles) { PROFILE
	std::vector<std::string> filelist;
	std::string lowername = strtolower(name.c_str());
	rob_dirlist(path,filelist,bDirs,bFiles);
	unsigned int i;
	for (i=0;i<filelist.size();++i) if (strtolower(filelist[i].c_str()) == lowername) return filelist[i];
	return "";
}
