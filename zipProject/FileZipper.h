#ifndef FILEZIPPER_HPP
#define FILEZIPPER_HPP

#include <vector>
#include <string>


#include "contrib/minizip/zip.h"
#include "contrib/minizip/iowin32.h"

#define WRITEBUFFERSIZE (16384)
#define MAXFILENAME (256)
class FileZipper
{
public:
	FileZipper();
	~FileZipper();
	int ZipFiles(std::string &zipFileName, std::vector<std::string> files, bool bAppendFile = false, int nCompressLevel = 0);

private:
	int isLargeFile(std::string filename);

	//char *f;                /* name of file to get info on */
	//tm_zip *tmzip;             /* return value: access, modific. and creation times */
	//uLong *dt;             /* dostime */
	uLong filetime(const char* f, tm_zip *tmzip, uLong *dt);
};
#endif

