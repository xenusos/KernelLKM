/*
    Purpose: 
    Author: Reece W.
    License: All Rights Reserved J. Reece Wilson
*/

#pragma once

void * file_open(const char *path, int flags, int rights);

void * file_open_readonly(const char *path);
void * file_open_readwrite(const char *path);
void * file_open_append(const char *path);

int file_read(void *file, unsigned long long offset, unsigned char *data, unsigned int size);
int file_write(void *file, unsigned long long offset, unsigned char *data, unsigned int size);

long file_mode(const char * file);
long file_length(const char * file);

unsigned long long file_mt(const char * file);
unsigned long long file_ct(const char * file);
unsigned long long file_at(const char * file);

int file_sync(void *file);
int file_close(void *file);