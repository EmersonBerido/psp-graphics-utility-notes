#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>

// PSP Module Info
PSP_MODULE_INFO("Context Sample", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

// Define PSP Width and Height
#define PSP_BUF_WIDTH (512)
#define PSP_SCR_WIDTH (480)
#define PSP_SCR_HEIGHT (272)

// Global Variable
int running = 1;

int exit_callback(int arg1, int arg2, void* common) {
  sceKernelExitGame();
}
void setupCallbacks() {
  
}

int main () {

  SetupCallbacks();

  return 0;
}
