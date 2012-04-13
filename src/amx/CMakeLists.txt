cmake_minimum_required(VERSION 2.8)

add_library(amx STATIC
	"amx.h"
	"amxaux.c"
	"amxaux.h"
	"amxdbg.c"
	"amxdbg.h"
	"getch.h"
	"osdefs.h"
	"sclinux.h"
)
