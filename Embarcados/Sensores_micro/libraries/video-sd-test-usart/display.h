#ifndef DISPLAYH
#define DISPLAYH

#ifdef __cplusplus
extern "C" {
#endif

// screen size
// screen 
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128
void UpdateDisplay(void);
void DisplayInit(void);
void BeginUpdateDisplay(void);
void EndUpdateDisplay(void);
void SelectDisplay(void);

#ifdef __cplusplus
}
#endif

#endif
