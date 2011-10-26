// VFSTest.cpp : Defines the entry point for the console application.
//

#include <stdio.h>

#include <direct.h>

#include "virtualFileSystem.h"

void printWorkingDir()
{
  char *path=0;
  size_t size;
  path=_getcwd(path,size);
  printf(path);
}

bool file_exists(const char * filename)
{
  if (FILE * file = fopen(filename, "r"))
  {
    fclose(file);
    return true;
  }
  return false;
}

int main(int argc, char* argv[])
{
  printWorkingDir();

  char* fileName = "..\\..\\..\\..\\data\\tests\\testVFS.p3d";

  if( !file_exists(fileName) )
  {
    printf("Multifile could not be found\n");
    return -1;
  }

  VirtualFileSystem *vfs = VirtualFileSystem::get_global_ptr();
  bool bResult = vfs->mount(fileName, ".", VirtualFileSystem::MF_read_only);
  if( !bResult )
  {
    printf("Could not mount multifile\n");
    return -1;
  }
  else
  {
    printf("Multifile mounted\n");
  }

  PT(VirtualFileList) vfl = vfs->scan_directory(Filename("."));
  if( vfl == 0 ) printf("Scanning failed\n");

  int numFiles = vfl->get_num_files();
  printf("Found %d files\n",numFiles);

  for(int i=0; i<numFiles; i++ )
  {
    VirtualFile* vf = vfl->get_file(i);
    Filename fn = vf->get_filename();
    std:string str = fn.get_basename();
    printf( "%d: %s\n", i, str.c_str() );
  }

  //vfs->mount("./foo.mf", ".", VirtualFileSystem::MF_read_only);
  //istream* is = vfs.open_read_file("test",true);
  //cout << vfs;
  printf("Test\n");
	return 0;
}

