#include "Input/InputDevice.h"

#include "Engine/Engine.h"

namespace DAVA
{
InputDevice::InputDevice(uint32 id)
    : id(id)
{
    Engine* engine = Engine::Instance();

    Vector<Window*> activeWindows = engine->GetWindows();
    for (Vector<Window*>::iterator it = activeWindows.begin(); it != activeWindows.end(); ++it)
    {
        OnWindowCreated(*it);
    }

    engine->windowCreated.Connect(this, &InputDevice::OnWindowCreated);
    engine->windowDestroyed.Connect(this, &InputDevice::OnWindowDestroyed);
}

InputDevice::~InputDevice()
{
    Engine* engine = Engine::Instance();

    engine->windowCreated.Disconnect(this);
    engine->windowDestroyed.Disconnect(this);

    for (Vector<Window*>::iterator it = windows.begin(); it != windows.end(); ++it)
    {
        Window* window = *it;

        DVASSERT(window != nullptr);

        window->focusChanged.Disconnect(this);
        window->sizeChanged.Disconnect(this);
    }
}

void InputDevice::OnWindowCreated(Window* window)
{
    DVASSERT(window != nullptr);

    DVASSERT(std::find(windows.begin(), windows.end(), window) == windows.end());

    windows.push_back(window);

    window->focusChanged.Connect(this, &InputDevice::OnWindowFocusChanged);
    window->sizeChanged.Connect(this, &InputDevice::OnWindowSizeChanged);
}

void InputDevice::OnWindowDestroyed(Window* window)
{
    DVASSERT(window != nullptr);

    Vector<Window*>::iterator it = std::find(windows.begin(), windows.end(), window);

    if (it != windows.end())
    {
        window->focusChanged.Disconnect(this);
        window->sizeChanged.Disconnect(this);

        windows.erase(it);
    }
}

void InputDevice::OnWindowFocusChanged(Window* window, bool focused)
{
    // Reset device state when window is unfocused
    if (!focused)
    {
        this->ResetState(window);
    }
}

void InputDevice::OnWindowSizeChanged(Window* window, Size2f, Size2f)
{
    // Reset device state when window size changes
    // To workaround cases when input events are not generated while window is changint its size
    // (e.g. when maximizing window in macOS)
    this->ResetState(window);
}

uint32 InputDevice::GetId() const
{
    return id;
}
} // namespace DAVA