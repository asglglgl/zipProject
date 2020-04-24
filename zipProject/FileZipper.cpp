#include "FileZipper.h"
FileZipper::FileZipper()
{
}


FileZipper::~FileZipper()
{
}

int FileZipper::ZipFiles(std::string & zipFileName, std::vector<std::string> files, bool bAppendFile, int nCompressLevel)
{
	int size_buf = WRITEBUFFERSIZE;
	void* buf = NULL;
	buf = (void*)malloc(size_buf);

	if (buf == NULL)
	{
		printf("Error allocating memory\n");
		return ZIP_INTERNALERROR;
	}

	zipFile zf;
	int err, errclose;
	zlib_filefunc64_def ffunc;
	fill_win32_filefunc64A(&ffunc);
	zf = zipOpen2_64(zipFileName.c_str(), (bAppendFile) ? 2 : 0, NULL, &ffunc);

	if (zf == NULL)
	{
		printf("error opening %s\n", zipFileName.c_str());
		err = ZIP_ERRNO;
	}
	else
	{
		printf("creating %s\n", zipFileName.c_str());
		err = ZIP_OK;
	}

	int i = 0;
	for (; i < files.size() && (err == ZIP_OK); i++)
	{
		FILE* fin = NULL;
		int size_read;
		std::string filenameinzip = files[i];
		std::string savefilenameinzip;
		zip_fileinfo zi;
		unsigned long crcFile = 0;
		int zip64 = 0;

		zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
			zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
		zi.dosDate = 0;
		zi.internal_fa = 0;
		zi.external_fa = 0;
		filetime(filenameinzip.c_str(), &zi.tmz_date, &zi.dosDate);

		zip64 = isLargeFile(filenameinzip);

		savefilenameinzip = filenameinzip.substr((filenameinzip.rfind('\\') + 1));

		err = zipOpenNewFileInZip3_64(zf, savefilenameinzip.c_str(), &zi,
			NULL, 0, NULL, 0, NULL /* comment*/,
			(nCompressLevel != 0) ? Z_DEFLATED : 0,
			nCompressLevel, 0,
			/* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
			-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
			NULL, crcFile, zip64);

		if (err != ZIP_OK)
			printf("error in opening %s in zipfile\n", filenameinzip.c_str());
		else
		{
			//fin = fopen64(filenameinzip.c_str(), "rb");
			fopen_s(&fin, filenameinzip.c_str(), "rb");
			if (fin == NULL)
			{
				err = ZIP_ERRNO;
				printf("error in opening %s for reading\n", filenameinzip.c_str());
			}
		}

		if (err == ZIP_OK)
		{
			do
			{
				err = ZIP_OK;
				size_read = (int)fread(buf, 1, size_buf, fin);
				if (size_read < size_buf)
					if (feof(fin) == 0)
					{
						printf("error in reading %s\n", filenameinzip.c_str());
						err = ZIP_ERRNO;
					}

				if (size_read > 0)
				{
					err = zipWriteInFileInZip(zf, buf, size_read);
					if (err < 0)
					{
						printf("error in writing %s in the zipfile\n",
							filenameinzip.c_str());
					}

				}
			} while ((err == ZIP_OK) && (size_read > 0));
		}

		if (fin)
			fclose(fin);

		if (err < 0)
			err = ZIP_ERRNO;
		else
		{
			err = zipCloseFileInZip(zf);
			if (err != ZIP_OK)
				printf("error in closing %s in the zipfile\n",
					filenameinzip.c_str());
		}
	}

	errclose = zipClose(zf, NULL);
	if (errclose != ZIP_OK)
		printf("error in closing %s\n", zipFileName.c_str());

	if (buf != NULL)
		free(buf);

	return err;
}

int FileZipper::isLargeFile(std::string filename)
{
	int largeFile = 0;
	ZPOS64_T pos = 0;
	FILE* pFile = NULL;
	//pFile = fopen64(filename.c_str(), "rb");
	fopen_s(&pFile, filename.c_str(), "rb");

	if (pFile != NULL)
	{
		int n = fseeko64(pFile, 0, SEEK_END);
		pos = ftello64(pFile);

		printf("File : %s is %lld bytes\n", filename.c_str(), pos);

		if (pos >= 0xffffffff)
			largeFile = 1;

		fclose(pFile);
	}

	return largeFile;
}

uLong FileZipper::filetime(const char* f, tm_zip *tmzip, uLong *dt)
{
	int ret = 0;
	{
		FILETIME ftLocal;
		HANDLE hFind;
		WIN32_FIND_DATAA ff32;

		hFind = FindFirstFileA(f, &ff32);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			FileTimeToLocalFileTime(&(ff32.ftLastWriteTime), &ftLocal);
			FileTimeToDosDateTime(&ftLocal, ((LPWORD)dt) + 1, ((LPWORD)dt) + 0);
			FindClose(hFind);
			ret = 1;
		}
	}
	return ret;
}