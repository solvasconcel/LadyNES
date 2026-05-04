# Bug Report: Application Deadlock on Fullscreen Stretch Toggle

**ID:** BUG-001  
**Severity:** CRITICAL (Application becomes unresponsive)  
**Priority:** HIGH  
**Status:** OPEN  

---

## 1. Description
When the user attempts to toggle the "Stretch Aspect Ratio" setting while the emulator is in Fullscreen mode, the application enters a deadlock state. The screen freezes with extreme zoom distortion, and the process must be killed manually via Task Manager/Terminal.

## 2. Environment
*   **Version:** LadyNES v0.3.9 (Bento Interface)
*   **OS:** Linux Ubuntu 22.04
*   **Renderer:** SDL2_Renderer
*   **Scaling:** 2x Default

## 3. Steps to Reproduce
1. Launch LadyNES.
2. Load any valid ".nes" or use the settings from the launcher menu.
3. Enter **Fullscreen Mode** via the Settings menu or shortcut.
4. Open the Pause Menu (ENTER).
5. Toggle the **Stretch Aspect Ratio** button to "ON", then "OFF".
6. Observe the immediate freeze and visual distortion.

## 4. Expected Result
The emulator should update the `SDL_RenderSetLogicalSize` to (0,0) to fill the screen without interrupting the main execution loop or causing rendering deadlocks.

## 5. Actual Result
*   Visual: Extreme zoom-in effect (only a few pixels visible).
*   System: Main thread hangs (Deadlock); no input is accepted.
*   Stability: Application crash/freeze requires a forced process termination.

## 6. Technical Hypotheses (QA Analysis)
The issue likely stems from a race condition or an unhandled exception during the `SDL_RenderSetLogicalSize` recalculation while the SDL_Window is in an exclusive fullscreen state. The renderer fails to reset the viewport coordinates correctly, leading to an out-of-bounds scaling factor.
