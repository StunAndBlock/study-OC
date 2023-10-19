#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
typedef struct FileInfo {
  char* name;
  unsigned long modif_time;
} FileInfo;

int scanfolder(DIR*,FileInfo**,size_t*);
void diag(const char*);


int compare_for_qsort(const void * a, const void * b){
  FileInfo* data_1 = (FileInfo *)a;
  FileInfo* data_2 = (FileInfo *)b;
  return ( data_1->modif_time - data_2->modif_time);
}

void diag(const char* mes){
    write(1, mes, strlen(mes));
    write(1, ": ", 2);
    write(1, strerror(errno), strlen(strerror(errno)));
    write(1, "\n", 1);
}

int scanfolder(DIR* folder_ptr,FileInfo** folder_contents,size_t* contents_count){
  struct dirent* folder;
  struct stat sbuf;
  (*folder_contents) = (FileInfo*)malloc(sizeof(FileInfo) * (*contents_count));
  FileInfo* temp_ptr = NULL;
  while((folder = readdir(folder_ptr)) != NULL) { 

    stat(folder->d_name, &sbuf);
    switch (sbuf.st_mode & __S_IFMT)
    {
    case __S_IFDIR:
      (*folder_contents)[(*contents_count)-1].name = (char*)malloc(sizeof(char) * strlen(folder->d_name)+2);  
      memcpy((*folder_contents)[(*contents_count)-1].name, folder->d_name, strlen(folder->d_name));
      (*folder_contents)[(*contents_count)-1].name[strlen(folder->d_name)]='/';
      (*folder_contents)[(*contents_count)-1].name[strlen(folder->d_name)+1]='\0';
      break;
    default:
      (*folder_contents)[(*contents_count)-1].name = (char*)malloc(sizeof(char) * strlen(folder->d_name)+1);  
      memcpy((*folder_contents)[(*contents_count)-1].name, folder->d_name, strlen(folder->d_name)+1);
      break;
    }
  
    (*folder_contents)[(*contents_count)-1].modif_time=sbuf.st_mtim.tv_sec;
    (*contents_count) += 1;
    if ((temp_ptr = (FileInfo*)realloc((*folder_contents),(*contents_count) * sizeof(FileInfo))) != NULL){
      (*folder_contents) = temp_ptr;
    } else {
      return 127;
    }
  }

  return(0);
}


char* get_folder_name(int argc, char* argv[]){
  char* fold_name = NULL;
  const char* default_fold_name = ".";
  size_t fold_name_len;
  switch (argc)
  {
  case 1:
    fold_name_len=strlen(default_fold_name)+1;
    fold_name = (char*)malloc(fold_name_len*(sizeof(char)));
    memcpy(fold_name,default_fold_name,fold_name_len);
    break;
  default:
    fold_name_len=strlen(argv[1])+1;
    fold_name = (char*)malloc(fold_name_len*(sizeof(char)));
    memcpy(fold_name,argv[1],fold_name_len);
    break;
  }
  return fold_name;
}

void output(FileInfo* folder_contents, size_t contents_count){
  size_t blanketing = 20;
  for(size_t i = 0; i < contents_count; i++){
    write(1,folder_contents[i].name,strlen(folder_contents[i].name));
    for(size_t whitespace_count=0; whitespace_count < blanketing-strlen(folder_contents[i].name); whitespace_count++){
      write(1," ",1);
    }
    //write(1,&folder_contents[i].modif_time,sizeof(folder_contents[i].modif_time));
    fprintf(stderr,"%ld",folder_contents[i].modif_time);
    write(1,"\n",1);
  }
}


int main(int argc, char* argv[]) {
  char* folder_name = get_folder_name(argc,argv);
  DIR* folder_ptr = NULL;
  FileInfo* folder_contents = NULL;
  size_t contents_count=1;
  if((folder_ptr = opendir(folder_name)) != NULL){
    scanfolder(folder_ptr,&folder_contents,&contents_count);
    closedir(folder_ptr);
    qsort(folder_contents, contents_count - 1, sizeof(FileInfo), compare_for_qsort);
    output(folder_contents, contents_count - 1);
    for (size_t i = 0; i < contents_count - 1; i++)
    {
      free(folder_contents[i].name);
    }
    free(folder_contents);
    free(folder_name);
    
  } else {
    diag(folder_name);
  }
  
  return 0;
}

