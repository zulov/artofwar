//
// Copyright (c) 2008-2022 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

/// \file

#pragma once

#include "../Container/FlagSet.h"
#include "../Container/HashSet.h"
#include "../Core/Mutex.h"
#include "../Core/Object.h"
#include "../Container/List.h"
#include "../Input/InputEvents.h"
#include "../UI/Cursor.h"

namespace Urho3D
{

/// %Input Mouse Modes.
enum MouseMode
{
    MM_ABSOLUTE = 0,
    MM_RELATIVE,
    MM_WRAP,
    MM_FREE,
    MM_INVALID
};

class Deserializer;
class Graphics;
class Serializer;
class UIElement;
class XMLFile;

const IntVector2 MOUSE_POSITION_OFFSCREEN = IntVector2(M_MIN_INT, M_MIN_INT);

/// %Input state for a finger touch.
/// @nocount
struct TouchState
{
    /// Return last touched UI element, used by scripting integration.
    /// @property
    UIElement* GetTouchedElement();

    /// Touch (finger) ID.
    int touchID_;
    /// Position in screen coordinates.
    IntVector2 position_;
    /// Last position in screen coordinates.
    IntVector2 lastPosition_;
    /// Movement since last frame.
    IntVector2 delta_;
    /// Finger pressure.
    float pressure_;
    /// Last touched UI element from screen joystick.
    WeakPtr<UIElement> touchedElement_;
};

#ifdef __EMSCRIPTEN__
class EmscriptenInput;
#endif

/// %Input subsystem. Converts operating system window messages to input state and events.
class URHO3D_API Input : public Object
{
    URHO3D_OBJECT(Input, Object);

#ifdef __EMSCRIPTEN__
    friend class EmscriptenInput;
#endif

public:
    /// Construct.
    explicit Input(Context* context);
    /// Destruct.
    ~Input() override;

    /// Poll for window messages. Called by HandleBeginFrame().
    void Update();
    /// Set whether ALT-ENTER fullscreen toggle is enabled.
    /// @property
    void SetToggleFullscreen(bool enable);
    /// Set whether the operating system mouse cursor is visible. When not visible (default), is kept centered to prevent leaving the window. Mouse visibility event can be suppressed-- this also recalls any unsuppressed SetMouseVisible which can be returned by ResetMouseVisible().
    void SetMouseVisible(bool enable, bool suppressEvent = false);
    /// Reset last mouse visibility that was not suppressed in SetMouseVisible.
    void ResetMouseVisible();
    /// Set whether the mouse is currently being grabbed by an operation.
    void SetMouseGrabbed(bool grab, bool suppressEvent = false);
    /// Reset the mouse grabbed to the last unsuppressed SetMouseGrabbed call.
    void ResetMouseGrabbed();
    /// Set the mouse mode.
    /** Set the mouse mode behaviour.
     *  MM_ABSOLUTE is the default behaviour, allowing the toggling of operating system cursor visibility and allowing the cursor to escape the window when visible.
     *  When the operating system cursor is invisible in absolute mouse mode, the mouse is confined to the window.
     *  If the operating system and UI cursors are both invisible, interaction with the Urho UI will be limited (eg: drag move / drag end events will not trigger).
     *  SetMouseMode(MM_ABSOLUTE) will call SetMouseGrabbed(false).
     *
     *  MM_RELATIVE sets the operating system cursor to invisible and confines the cursor to the window.
     *  The operating system cursor cannot be set to be visible in this mode via SetMouseVisible(), however changes are tracked and will be restored when another mouse mode is set.
     *  When the virtual cursor is also invisible, UI interaction will still function as normal (eg: drag events will trigger).
     *  SetMouseMode(MM_RELATIVE) will call SetMouseGrabbed(true).
     *
     *  MM_WRAP grabs the mouse from the operating system and confines the operating system cursor to the window, wrapping the cursor when it is near the edges.
     *  SetMouseMode(MM_WRAP) will call SetMouseGrabbed(true).
     *
     *  MM_FREE does not grab/confine the mouse cursor even when it is hidden. This can be used for cases where the cursor should render using the operating system
     *  outside the window, and perform custom rendering (with SetMouseVisible(false)) inside.
    */
    void SetMouseMode(MouseMode mode, bool suppressEvent = false);
    /// Reset the last mouse mode that wasn't suppressed in SetMouseMode.
    void ResetMouseMode();
    /// Show or hide on-screen keyboard on platforms that support it. When shown, keypresses from it are delivered as key events.
    /// @property
    void SetScreenKeyboardVisible(bool enable);
    /// Set touch emulation by mouse. Only available on desktop platforms. When enabled, actual mouse events are no longer sent and the mouse cursor is forced visible.
    /// @property
    void SetTouchEmulation(bool enable);
    /// Begin recording a touch gesture. Return true if successful. The E_GESTURERECORDED event (which contains the ID for the new gesture) will be sent when recording finishes.
    bool RecordGesture();
    /// Save all in-memory touch gestures. Return true if successful.
    bool SaveGestures(Serializer& dest);
    /// Save a specific in-memory touch gesture to a file. Return true if successful.
    bool SaveGesture(Serializer& dest, unsigned gestureID);
    /// Load touch gestures from a file. Return number of loaded gestures, or 0 on failure.
    unsigned LoadGestures(Deserializer& source);
    /// Remove an in-memory gesture by ID. Return true if was found.
    bool RemoveGesture(unsigned gestureID);
    /// Remove all in-memory gestures.
    void RemoveAllGestures();
    /// Set the mouse cursor position. Uses the backbuffer (Graphics width/height) coordinates.
    /// @property
    void SetMousePosition(const IntVector2& position);
    /// Center the mouse position.
    void CenterMousePosition();

    /// Return keycode from key name.
    Key GetKeyFromName(const String& name) const;
    /// Return keycode from scancode.
    Key GetKeyFromScancode(Scancode scancode) const;
    /// Return name of key from keycode.
    String GetKeyName(Key key) const;
    /// Return scancode from keycode.
    Scancode GetScancodeFromKey(Key key) const;
    /// Return scancode from key name.
    Scancode GetScancodeFromName(const String& name) const;
    /// Return name of key from scancode.
    String GetScancodeName(Scancode scancode) const;
    /// Check if a key is held down.
    /// @property
    bool GetKeyDown(Key key) const;
    /// Check if a key has been pressed on this frame.
    /// @property
    bool GetKeyPress(Key key) const;
    /// Check if a key is held down by scancode.
    /// @property
    bool GetScancodeDown(Scancode scancode) const;
    /// Check if a key has been pressed on this frame by scancode.
    /// @property
    bool GetScancodePress(Scancode scancode) const;
    /// Check if a mouse button is held down.
    /// @property
    bool GetMouseButtonDown(MouseButtonFlags button) const;
    /// Check if a mouse button has been pressed on this frame.
    /// @property
    bool GetMouseButtonPress(MouseButtonFlags button) const;
    /// Check if a qualifier key is held down.
    /// @property
    bool GetQualifierDown(Qualifier qualifier) const;
    /// Check if a qualifier key has been pressed on this frame.
    /// @property
    bool GetQualifierPress(Qualifier qualifier) const;
    /// Return the currently held down qualifiers.
    /// @property
    QualifierFlags GetQualifiers() const;
    /// Return mouse position within window. Should only be used with a visible mouse cursor. Uses the backbuffer (Graphics width/height) coordinates.
    /// @property
    IntVector2 GetMousePosition() const;
    /// Return mouse movement since last frame.
    /// @property
    IntVector2 GetMouseMove() const;
    /// Return horizontal mouse movement since last frame.
    /// @property
    int GetMouseMoveX() const;
    /// Return vertical mouse movement since last frame.
    /// @property
    int GetMouseMoveY() const;
    /// Return mouse wheel movement since last frame.
    /// @property
    int GetMouseMoveWheel() const { return mouseMoveWheel_; }
    /// Return input coordinate scaling. Should return non-unity on High DPI display.
    /// @property
    Vector2 GetInputScale() const { return inputScale_; }

    /// Return number of active finger touches.
    /// @property
    unsigned GetNumTouches() const { return touches_.Size(); }
    /// Return active finger touch by index.
    /// @property{get_touches}
    TouchState* GetTouch(unsigned index) const;

    /// Return whether fullscreen toggle is enabled.
    /// @property
    bool GetToggleFullscreen() const { return toggleFullscreen_; }

    /// Return whether on-screen keyboard is supported.
    /// @property
    bool GetScreenKeyboardSupport() const;
    /// Return whether on-screen keyboard is being shown.
    /// @property
    bool IsScreenKeyboardVisible() const;

    /// Return whether touch emulation is enabled.
    /// @property
    bool GetTouchEmulation() const { return touchEmulation_; }

    /// Return whether the operating system mouse cursor is visible.
    /// @property
    bool IsMouseVisible() const { return mouseVisible_; }
    /// Return whether the mouse is currently being grabbed by an operation.
    /// @property
    bool IsMouseGrabbed() const { return mouseGrabbed_; }
    /// Return whether the mouse is locked to the window.
    /// @property
    bool IsMouseLocked() const;

    /// Return the mouse mode.
    /// @property
    MouseMode GetMouseMode() const { return mouseMode_; }

    /// Return whether application window has input focus.
    /// @property{get_focus}
    bool HasFocus() { return inputFocus_; }

    /// Return whether application window is minimized.
    /// @property
    bool IsMinimized() const;

private:
    /// Initialize when screen mode initially set.
    void Initialize();

    /// Prepare input state for application gaining input focus.
    void GainFocus();
    /// Prepare input state for application losing input focus.
    void LoseFocus();
    /// Clear input state.
    void ResetState();
    /// Clear touch states and send touch end events.
    void ResetTouches();
    /// Reset input accumulation.
    void ResetInputAccumulation();
    /// Get the index of a touch based on the touch ID.
    unsigned GetTouchIndexFromID(int touchID);
    /// Used internally to return and remove the next available touch index.
    unsigned PopTouchIndex();
    /// Push a touch index back into the list of available when finished with it.
    void PushTouchIndex(int touchID);
    /// Send an input focus or window minimization change event.
    void SendInputFocusEvent();
    /// Handle a mouse button change.
    void SetMouseButton(MouseButton button, bool newState, int clicks);
    /// Handle a key change.
    void SetKey(Key key, Scancode scancode, bool newState);
    /// Handle mouse wheel change.
    void SetMouseWheel(int delta);
    /// Suppress next mouse movement.
    void SuppressNextMouseMove();
    /// Unsuppress mouse movement.
    void UnsuppressMouseMove();
    /// Handle screen mode event.
    void HandleScreenMode(StringHash eventType, VariantMap& eventData);
    /// Handle frame start event.
    void HandleBeginFrame(StringHash eventType, VariantMap& eventData);
    /// Handle touch events from the controls of screen joystick(s).
    void HandleScreenJoystickTouch(StringHash eventType, VariantMap& eventData);
    /// Handle SDL event.
    void HandleSDLEvent(void* sdlEvent);

#ifndef __EMSCRIPTEN__
    /// Set SDL mouse mode relative.
    void SetMouseModeRelative(SDL_bool enable);
    /// Set SDL mouse mode absolute.
    void SetMouseModeAbsolute(SDL_bool enable);
#else
    /// Set whether the operating system mouse cursor is visible (Emscripten platform only).
    void SetMouseVisibleEmscripten(bool enable, bool suppressEvent = false);
    /// Set mouse mode final resolution (Emscripten platform only).
    void SetMouseModeEmscriptenFinal(MouseMode mode, bool suppressEvent = false);
    /// SetMouseMode  (Emscripten platform only).
    void SetMouseModeEmscripten(MouseMode mode, bool suppressEvent);
    /// Handle frame end event.
    void HandleEndFrame(StringHash eventType, VariantMap& eventData);
#endif

    /// Graphics subsystem.
    WeakPtr<Graphics> graphics_;
    /// Key down state.
    HashSet<int> keyDown_;
    /// Key pressed state.
    HashSet<int> keyPress_;
    /// Key down state by scancode.
    HashSet<int> scancodeDown_;
    /// Key pressed state by scancode.
    HashSet<int> scancodePress_;
    /// Active finger touches.
    HashMap<int, TouchState> touches_;
    /// List that maps between event touch IDs and normalised touch IDs.
    List<int> availableTouchIDs_;
    /// Mapping of touch indices.
    HashMap<int, int> touchIDMap_;
    /// String for text input.
    String textInput_;
    /// Mouse buttons' down state.
    MouseButtonFlags mouseButtonDown_;
    /// Mouse buttons' pressed state.
    MouseButtonFlags mouseButtonPress_;
    /// Last mouse position for calculating movement.
    IntVector2 lastMousePosition_;
    /// Last mouse position before being set to not visible.
    IntVector2 lastVisibleMousePosition_;
    /// Mouse movement since last frame.
    IntVector2 mouseMove_;
    /// Mouse wheel movement since last frame.
    int mouseMoveWheel_;
    /// Input coordinate scaling. Non-unity when window and backbuffer have different sizes (e.g. Retina display).
    Vector2 inputScale_;
    /// SDL window ID.
    unsigned windowID_;
    /// Fullscreen toggle flag.
    bool toggleFullscreen_;
    /// Operating system mouse cursor visible flag.
    bool mouseVisible_;
    /// The last operating system mouse cursor visible flag set by end use call to SetMouseVisible.
    bool lastMouseVisible_;
    /// Flag to indicate the mouse is being grabbed by an operation. Subsystems like UI that uses mouse should temporarily ignore the mouse hover or click events.
    bool mouseGrabbed_;
    /// The last mouse grabbed set by SetMouseGrabbed.
    bool lastMouseGrabbed_;
    /// Determines the mode of mouse behaviour.
    MouseMode mouseMode_;
    /// The last mouse mode set by SetMouseMode.
    MouseMode lastMouseMode_;
#ifndef __EMSCRIPTEN__
    /// Flag to determine whether SDL mouse relative was used.
    bool sdlMouseRelative_;
#endif
    /// Touch emulation mode flag.
    bool touchEmulation_;
    /// Input focus flag.
    bool inputFocus_;
    /// Minimized flag.
    bool minimized_;
    /// Gained focus on this frame flag.
    bool focusedThisFrame_;
    /// Next mouse move suppress flag.
    bool suppressNextMouseMove_;
    /// Whether mouse move is accumulated in backbuffer scale or not (when using events directly).
    bool mouseMoveScaled_;
    /// Initialized flag.
    bool initialized_;

#ifdef __EMSCRIPTEN__
    /// Emscripten Input glue instance.
    UniquePtr<EmscriptenInput> emscriptenInput_;
    /// Flag used to detect mouse jump when exiting pointer-lock.
    bool emscriptenExitingPointerLock_;
    /// Flag used to detect mouse jump on initial mouse click when entering pointer-lock.
    bool emscriptenEnteredPointerLock_;
    /// Flag indicating current pointer-lock status.
    bool emscriptenPointerLock_;
#endif
};

}