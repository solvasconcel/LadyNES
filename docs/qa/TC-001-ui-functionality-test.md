# Test Case Report: UI Options Functionality

**Project:** SosiNES Emulator  
**Module:** Bento Interface System  
**Category:** Black Box / Regression Testing  

---

## Overview
This test plan was developed to apply Black Box and Regression Testing methodologies to the SosiNES Bento interface system. The primary focus is the validation of User Experience (UX) and the stability of memory management during Input/Output (I/O) operations.

---

## Test Case Details: TC-001

*   **ID:** TC-001
*   **Title:** Functionality Test for New Options Menu (Fullscreen, Stretch Aspect Ratio, Bilinear Filtering)
*   **Priority:** High
*   **Status:** FAILED / REPROVED
*   **Severity:** CRITICAL (Fullscreen bug renders the software unusable)

### Pre-conditions
*   Application launched.
*   ROM file loaded to verify changes during active gameplay.
*   Configuration files present to verify persistence.

### Execution Steps
*   Launch the application.
*   Navigate to the Bento Options Menu.
*   Interact with all functional toggles (Fullscreen, Stretch, Bilinear).
*   Verify real-time application of settings during active gameplay.
*   Restart the application to verify data persistence.

### Expected Result
*   The emulator must correctly update and apply all visual options in real-time.
*   The system must maintain user choices after the application is closed and reopened.

---

## Actual Results

*   **Bilinear Filtering:** PASSED. Functioned as expected, applying real-time texture smoothing without performance degradation.
*   **Persistence:** PASSED. All configurations were successfully saved and restored after a full restart.
*   **Fullscreen + Stretch (Critical Regression):** FAILED. Activating "Stretch Aspect Ratio" while "Fullscreen" is enabled caused a Deadlock (system freeze) and extreme zoom distortion. The application became completely unresponsive.
*   **UI Layout:** FAILED. Identified inconsistency in the initial Launcher resolution. Bento menu elements appear distorted/misaligned upon first boot, correcting only after the first ROM load.

---

## Technical Analysis
*   The Launcher UI failure appears to be a Race Condition between the SDL window creation and the Dear ImGui Viewport initialization. This results in incorrect rendering coordinates during the initial boot sequence. 
*   The Fullscreen/Stretch deadlock suggests a state-sync conflict within the SDL Renderer's logical size recalculation when attempting to override display scaling in exclusive mode.

---

## Next Steps
*   Escalate Fullscreen Deadlock to development with Critical Priority.
*   Implement a mandatory viewport refresh sequence on the first frame to fix Launcher distortion.
*   Schedule a full retest of TC-001 once the fix is merged into the main branch.
