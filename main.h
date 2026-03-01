#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
	char* name;
	char* text;
//	Texture2D media;
	uint8_t uid;
	int8_t cid;
	uint16_t id;
	uint16_t did;
	uint16_t iid;
	uint16_t rid;
} Message;

typedef struct {
	char* name;
	uint8_t uid;
//	Texture2D ava;
	bool ver;	
} User;


typedef struct {
	char* name;
	uint8_t uid;
//	Texture2D ava;
	bool ver;	
	char* pass;
	bool in_voice;
	bool regged;
	bool logged;	
} me;


typedef struct {
	char* name;
//	Texture2D ava;
	int8_t cid;
	uint16_t um;
} Chat;

typedef struct {
	uint8_t uid;
	uint8_t* ava;
} Ava;
