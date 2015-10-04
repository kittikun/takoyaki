#include "main.h"

#include <functional>
#include <ppltasks.h>
#include <wrl.h>

#include <takoyaki.h>

#include "app.h"

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
    auto app = ref new AppMainView();
    CoreApplication::Run(app);

    return 0;
}

IFrameworkView^ AppMainView::CreateView()
{
    return ref new AppMain::Main();
}

namespace AppMain
{
    Main::Main()
        : app_(std::make_unique<App>())
        , framework_{std::make_shared<Takoyaki::Framework >()}
        , mWindowClosed{ false }
        , mWindowVisible{ true }
    {
    }

    Takoyaki::EDisplayOrientation Main::DisplayOrientationsToTakoyaki(Windows::Graphics::Display::DisplayOrientations orientation)
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
                throw std::runtime_error{ "DisplayOrientationsToTakoyaki" };
                break;
        }

        return Takoyaki::EDisplayOrientation::UNKNOWN;
    }

    // The first method called when the IFrameworkView is being created.
    void Main::Initialize(CoreApplicationView^ applicationView)
    {
        // Register event handlers for app life cycle. This example includes Activated, so that we
        // can make the CoreWindow active and start rendering on the window.
        applicationView->Activated += ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &Main::OnActivated);
        CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs^>(this, &Main::OnSuspending);
        CoreApplication::Resuming += ref new EventHandler<Platform::Object^>(this, &Main::OnResuming);
    }

    // Called when the CoreWindow object is created (or re-created).
    void Main::SetWindow(CoreWindow^ window)
    {
        window->SizeChanged += ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &Main::OnWindowSizeChanged);
        window->VisibilityChanged += ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &Main::OnVisibilityChanged);
        window->Closed += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &Main::OnWindowClosed);

        DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

        currentDisplayInformation->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Main::OnDpiChanged);
        currentDisplayInformation->OrientationChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Main::OnOrientationChanged);
        DisplayInformation::DisplayContentsInvalidated += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Main::OnDisplayContentsInvalidated);
    }

    // Initializes scene resources, or loads a previously saved app state.
    void Main::Load(Platform::String^ entryPoint)
    {
        // configure Takoyaki framework
        auto window = CoreWindow::GetForCurrentThread();
        DisplayInformation^ disp = DisplayInformation::GetForCurrentView();

        Takoyaki::FrameworkDesc desc = { 0 };

        desc.bufferCount = 3;
        desc.nativeOrientation = DisplayOrientationsToTakoyaki(disp->NativeOrientation);
        desc.currentOrientation = DisplayOrientationsToTakoyaki(disp->CurrentOrientation);
        desc.numWorkerThreads = 4; // std::thread::hardware_concurrency();
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

        // initalize app
        app_->initialize(framework_);
    }

    // This method is called after the window becomes active.
    void Main::Run()
    {
        auto renderer = framework_->getRenderer();

        while (!mWindowClosed) {
            if (mWindowVisible) {
                CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

                app_->render(renderer.get());
                framework_->present();
            } else {
                CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
            }
        }
    }

    // Required for IFrameworkView.
    // Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
    // class is torn down while the app is in the foreground.
    void Main::Uninitialize()
    {
    }

    // Application lifecycle event handlers.

    void Main::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
    {
        // Run() won't start until the CoreWindow is activated.
        CoreWindow::GetForCurrentThread()->Activate();
    }

    void Main::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
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

    void Main::OnResuming(Platform::Object^ sender, Platform::Object^ args)
    {
        // Restore any data or state that was unloaded on suspend. By default, data
        // and state are persisted when resuming from suspend. Note that this event
        // does not occur if the app was previously terminated.

        // TODO: Insert your code here.
        //m_main->OnResuming();
    }

    // Window event handlers.

    void Main::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
    {
        glm::vec2 size(sender->Bounds.Width, sender->Bounds.Height);

        framework_->setProperty(Takoyaki::EPropertyID::WINDOW_SIZE, size);
    }

    void Main::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
    {
        mWindowVisible = args->Visible;
    }

    void Main::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
    {
        mWindowClosed = true;
    }

    void Main::OnDpiChanged(DisplayInformation^ sender, Object^ args)
    {
        framework_->setProperty(Takoyaki::EPropertyID::WINDOW_DPI, sender->LogicalDpi);
    }

    void Main::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
    {
        framework_->setProperty(Takoyaki::EPropertyID::WINDOW_ORIENTATION, sender->CurrentOrientation);
    }

    void Main::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
    {
        framework_->validateDevice();
    }

    Concurrency::task<std::vector<byte>> Main::loadFileAsync(const std::wstring& filename)
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