#include "App.h"

#include <functional>
#include <ppltasks.h>
#include <wrl.h>

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

// The DirectX 12 Application template is documented at http://go.microsoft.com/fwlink/?LinkID=613670&clcid=0x409

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
    auto app = ref new TakoyakiView();
    CoreApplication::Run(app);

    return 0;
}

IFrameworkView^ TakoyakiView::CreateView()
{
    return ref new TakoyakiApp::App();
}

namespace TakoyakiApp
{
    App::App() :
        mWindowClosed(false),
        mWindowVisible(true)
    {
    }

    Takoyaki::EDisplayOrientation App::DisplayOrientationsToTakoyaki(Windows::Graphics::Display::DisplayOrientations orientation)
    {
        switch (orientation) {
            case Windows::Graphics::Display::DisplayOrientations::Landscape:
                return Takoyaki::EDisplayOrientation::LANDSCAPE;
            case Windows::Graphics::Display::DisplayOrientations::Portrait:
                return Takoyaki::EDisplayOrientation::PORTRAIT;
            case Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped:
                return Takoyaki::EDisplayOrientation::LANDSCAPE_FLIPPED;
            case Windows::Graphics::Display::DisplayOrientations::PortraitFlipped:
                return Takoyaki::EDisplayOrientation::PORTRAIT_FLIPPED;
            default:
                throw std::runtime_error("DisplayOrientationsToTakoyaki");
                break;
        }

        return Takoyaki::EDisplayOrientation::UNKNOWN;
    }

    // The first method called when the IFrameworkView is being created.
    void App::Initialize(CoreApplicationView^ applicationView)
    {
        // Register event handlers for app life cycle. This example includes Activated, so that we
        // can make the CoreWindow active and start rendering on the window.
        applicationView->Activated += ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);
        CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);
        CoreApplication::Resuming += ref new EventHandler<Platform::Object^>(this, &App::OnResuming);
    }

    // Called when the CoreWindow object is created (or re-created).
    void App::SetWindow(CoreWindow^ window)
    {
        window->SizeChanged += ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);
        window->VisibilityChanged += ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);
        window->Closed += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

        DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

        currentDisplayInformation->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDpiChanged);
        currentDisplayInformation->OrientationChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);
        DisplayInformation::DisplayContentsInvalidated += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDisplayContentsInvalidated);
    }

    // Initializes scene resources, or loads a previously saved app state.
    void App::Load(Platform::String^ entryPoint)
    {
        framework_.reset(new Takoyaki::Framework());

        auto window = CoreWindow::GetForCurrentThread();
        DisplayInformation^ disp = DisplayInformation::GetForCurrentView();

        Takoyaki::FrameworkDesc desc = { 0 };

        desc.bufferCount = 3;
        desc.nativeOrientation = DisplayOrientationsToTakoyaki(disp->NativeOrientation);
        desc.currentOrientation = DisplayOrientationsToTakoyaki(disp->CurrentOrientation);
        desc.numWorkerThreads = 4;
        desc.type = Takoyaki::EDeviceType::DX12;
        desc.windowHandle = reinterpret_cast<void*>(window);
        desc.windowSize.x = window->Bounds.Width;
        desc.windowSize.y = window->Bounds.Height;
        desc.windowDpi = disp->LogicalDpi;

        desc.loadAsyncFunc = std::bind<void>([this](const std::wstring& filename)
        {
            auto createVSTask = loadFileAsync(filename).then([=, this](std::vector<byte>& data)
            {
                framework_->loadAsyncFileResult(filename, data);
            });
        }, std::placeholders::_1);

        framework_->initialize(desc);
    }

    // This method is called after the window becomes active.
    void App::Run()
    {
        while (!mWindowClosed) {
            if (mWindowVisible) {
                CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

                framework_->present();
            } else {
                CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
            }
        }
    }

    // Required for IFrameworkView.
    // Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
    // class is torn down while the app is in the foreground.
    void App::Uninitialize()
    {
    }

    // Application lifecycle event handlers.

    void App::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
    {
        // Run() won't start until the CoreWindow is activated.
        CoreWindow::GetForCurrentThread()->Activate();
    }

    void App::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
    {
        // Save app state asynchronously after requesting a deferral. Holding a deferral
        // indicates that the application is busy performing suspending operations. Be
        // aware that a deferral may not be held indefinitely. After about five seconds,
        // the app will be forced to exit.
        SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

        create_task([this, deferral]()
        {
            // WinRT doesn't call destructor upon quitting but will call suspend 
            // Since this is probably a bad habit for other platforms, use suspend to simulate resource
            framework_.reset();
            deferral->Complete();
        });
    }

    void App::OnResuming(Platform::Object^ sender, Platform::Object^ args)
    {
        // Restore any data or state that was unloaded on suspend. By default, data
        // and state are persisted when resuming from suspend. Note that this event
        // does not occur if the app was previously terminated.

        // TODO: Insert your code here.
        //m_main->OnResuming();
    }

    // Window event handlers.

    void App::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
    {
        glm::vec2 size(sender->Bounds.Width, sender->Bounds.Height);

        framework_->setProperty(Takoyaki::EPropertyID::WINDOW_SIZE, size);
    }

    void App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
    {
        mWindowVisible = args->Visible;
    }

    void App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
    {
        mWindowClosed = true;
    }

    void App::OnDpiChanged(DisplayInformation^ sender, Object^ args)
    {
        framework_->setProperty(Takoyaki::EPropertyID::WINDOW_DPI, sender->LogicalDpi);
    }

    void App::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
    {
        framework_->setProperty(Takoyaki::EPropertyID::WINDOW_ORIENTATION, sender->CurrentOrientation);
    }

    void App::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
    {
        framework_->validateDevice();
    }

    Concurrency::task<std::vector<byte>> App::loadFileAsync(const std::wstring& filename)
    {
        using namespace Windows::Storage;
        using namespace Concurrency;

        auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation;

        return create_task(folder->GetFileAsync(Platform::StringReference(filename.c_str()))).then([](StorageFile^ file)
        {
            return FileIO::ReadBufferAsync(file);
        }).then([](Streams::IBuffer^ fileBuffer) -> std::vector<byte>
        {
            std::vector<byte> returnBuffer;
            returnBuffer.resize(fileBuffer->Length);
            Streams::DataReader::FromBuffer(fileBuffer)->ReadBytes(Platform::ArrayReference<byte>(returnBuffer.data(), fileBuffer->Length));
            return returnBuffer;
        });
    }
}