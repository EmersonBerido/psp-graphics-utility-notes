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

// Callback initialize
int exit_callback(int arg1, int arg2, void* common) {
  sceKernelExitGame();
  return 0;
}
int CallbackThread(SceSize args, void* argp) {
  int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
  sceKernelRegisterExitCallback(cbid);
  sceKernelSleepThreadCB();
  return 0;
}
int SetupCallbacks() {
  int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
  if (thid >= 0) {
    sceKernelStartThread(thid, 0, 0);
  }
  return thid;
}

// GE LIST
static unsigned int __attribute__((aligned(16))) list[262144];

// Calcs the requried bytes for a VRam buffer
static unsigned int getMemorySize(unsigned int width, unsigned int height, unsigned int psm) {
  unsigned int size = width * height;

  //specified number of pits per color channel (RGBA)
  switch (psm) {
    case GU_PSM_T4: 
      return size / 2; // 2pixels per byte
    return GU_PSM_T8;
      return size;
    
      case GU_PSM_5650:
      case GU_PSM_5551:
      case GU_PSM_4444:
      case GU_PSM_T16:
        return size * 2;
      
      case GU_PSM_8888:
      case GU_PSM_T32:
        // one byte for each color pannel of a pixel
        return size * 4;

      default:
        return 0;
  }
}

// Returns buffer 
void* getStaticVramBuffer(unsigned int width, unsigned int height, unsigned int psm) {
  static unsigned int staticOffset = 0;

  unsigned int memSize = getMemorySize(width, height, psm);

  // Returns buffer relative to 0;
  void* result = (void*)staticOffset;
  staticOffset += memSize;

  return result;
}

// Takes buffer from above function and setting it to beginning of where Vram is; used for setting textures
void* getStaticVramTexture(unsigned int width, unsigned int height, unsigned int psm) {
  void* result = getStaticVramBuffer(width, height, psm);
  return (void*)((unsigned int)(result) + ((unsigned int)sceGeEdramGetAddr()));
}

void initGraphics() {
  // frame buffers
  void* fbp0 = getStaticVramBuffer(PSP_BUF_WIDTH, PSP_SCR_HEIGHT, GU_PSM_8888); // draw buffer
  void* fbp1 = getStaticVramBuffer(PSP_BUF_WIDTH, PSP_SCR_HEIGHT, GU_PSM_8888); // display buffer
  void* zbp = getStaticVramBuffer(PSP_BUF_WIDTH, PSP_SCR_HEIGHT, GU_PSM_4444); // Z buffer

  sceGuInit();

  sceGuStart(GU_DIRECT, list); // start list so commands are recorded & sent to graphics engine

  // Double buffered rendering (prevents visual artifacting)
  sceGuDrawBuffer(GU_PSM_8888, fbp0, PSP_BUF_WIDTH);
  sceGuDispBuffer(PSP_SCR_WIDTH, PSP_SCR_HEIGHT, fbp1, PSP_BUF_WIDTH);
  sceGuDepthBuffer(zbp, PSP_BUF_WIDTH);

  //Center the virtual coord space (4096 x 4096)
  sceGuOffset( 2048 - (PSP_SCR_WIDTH / 2), 2048 - (PSP_SCR_HEIGHT / 2)); // says to draw in the middle of coord space
  sceGuViewport(2048, 2048, PSP_SCR_WIDTH, PSP_SCR_HEIGHT); // where we're going to draw

  // near, far (inversed)
  sceGuDepthRange(65535, 0);

  sceGuEnable(GU_SCISSOR_TEST);
  sceGuScissor(0, 0, PSP_SCR_WIDTH, PSP_SCR_HEIGHT); // scissors entire screen so everything is wrapped within the PSPs dimensions

  // Depth testing function:
  sceGuEnable(GU_DEPTH_TEST); 
  sceGuDepthFunc(GU_GEQUAL); // things far from you are clipped

  // whatever the front face will be wind in clock wise order
  // winding: direction u specifcy individual points on a vertex that creates a normal plane
  sceGuFrontFace(GU_CW);
  sceGuEnable(GU_CULL_FACE); // prevents drawing planes facing away from player

  sceGuShadeModel(GU_SMOOTH);

  sceGuEnable(GU_TEXTURE_2D);
  sceGuEnable(GU_CLIP_PLANES);

  sceGuFinish();
  sceGuSync(0,0); // makes program wait for GPU to finish intializing

  sceDisplayWaitVblankStart(); // waits for next available time to get a Vblank

  sceGuDisplay(GU_TRUE);

}

void termGraphics() {
  sceGuTerm();
}

void startFrame() {
  sceGuStart(GU_DIRECT, list);
}

void endFrame() {
  sceGuFinish();
  sceGuSync(0,0);
  sceDisplayWaitVblankStart();
  sceGuSwapBuffers(); // swaps the display buffer with the updated draw buffer
}

int main () {

  SetupCallbacks(); //Boilerplate

  initGraphics();

  while (running) {
      startFrame();
      sceGuClearColor(0xFFAAAA00); //abgr order (not rgba)
      sceGuClear(GU_COLOR_BUFFER_BIT);
      endFrame();
  }

  termGraphics();

  sceKernelExitGame();

  return 0;
}
