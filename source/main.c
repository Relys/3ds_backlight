
#include "global.h"


#define REG(x)   (*(volatile u32*)(x))
#define REG8(x)  (*(volatile  u8*)(x))
#define REG16(x) (*(volatile u16*)(x))
#define SW(addr, data)  *(u32*)(addr) = data

Handle fsUserHandle;
FS_archive sdmcArchive = {0x9, (FS_path){PATH_EMPTY, 1, (u8*)""}};

static int bklightValue = 8;

void updateBklight() {
	u32 t;
	t = bklightValue * 8;
	// http://3dbrew.org/wiki/LCD_Registers
	*(vu8*)(0xfffd6240) = t;
	*(vu8*)(0xfffd6a40) = t;
}

void adjustBklight(int adj) {
	bklightValue += adj;
	if (bklightValue < 0) {
		bklightValue = 0;
	}
	if (bklightValue > 31) {
		bklightValue = 31;
	}
	updateBklight();
}

u32 controlBacklightUi() {
	u8 buf[200];
	acquireVideo();
	while(1) {
		blank(0, 0, 320, 240);
		xsprintf(buf, "backlight: %02d", bklightValue);
		print(buf, 10, 10, 255, 0, 0);
		updateScreen();
		u32 key = waitKey();
		if (key == BUTTON_DU) {
			adjustBklight(1);
		}
		if (key == BUTTON_DD) {
			adjustBklight(-1);
		}
		if (key == BUTTON_B) {
			break;
		}
	}
	releaseVideo();
	return 0;
}

int main() {
	u32 retv;


	initSharedFunc();
	nsDbgPrint("initializing backlight plugin\n");
	plgRegisterMenuEntry(1, "Backlight Control", controlBacklightUi);
	plgGetSharedServiceHandle("fs:USER", &fsUserHandle);
	nsDbgPrint("fsUserHandle: %08x\n", fsUserHandle);
}

