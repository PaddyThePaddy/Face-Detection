#include <iostream>
#include <dirent.h>

using namespace std;

int main(void)
{
	char srcName[256] = { 0 }, dstName[256] = { 0 }, stageDirName[256] = "F:\\not_face\\stage00", tmpStr[256] = { 0 };
	char sysRename[256] = { 0 };
	int stageNum = 1;
	DIR *dir = NULL;
	dirent *entry = NULL;

	
	while (1){
		sprintf_s(tmpStr, "%02d", stageNum++);
		stageDirName[strlen(stageDirName) - 2] = '\0';
		strcat_s(stageDirName, tmpStr);

		if ((dir = opendir(stageDirName)) == NULL)
			break;

		sprintf_s(dstName, stageDirName);
		int index = 0;
		while (entry = readdir(dir)){
			if (strstr(entry->d_name, ".bmp")){
				sprintf_s(srcName, "%s\\%s", stageDirName, entry->d_name);
				entry = NULL;
			}
			else{
				entry = NULL;
				continue;
			}

			sprintf_s(sysRename, "rename %s not_face_No_%06d.bmp", srcName, index++);
			system(sysRename);
		}
		printf("%s complete.\n", stageDirName);
	}
	return 0;
}