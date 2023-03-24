#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>

#include "words.h"
#include "common.h"

void words_init(){
	struct stat statbuf;
	int fd;

	if ((fd = open("data/words_formatted.txt", O_RDONLY)) == -1){
		perror("open words_formatted.txt");
		exit(EXIT_FAILURE);
	}
	fstat(fd, &statbuf);

	if ((word_map = mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0)) == (void*) -1){
		perror("mmap words_formatted.txt");
		exit(EXIT_FAILURE);
	}
}