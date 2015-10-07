﻿#pragma once

#include <memory>
#include <ppltasks.h>
#include <synchapi.h>

#include <framework.h>
#include <fwd.h>

class App;

namespace AppMain
{
	// Main entry point for our app. Connects the app with the Windows shell and handles application lifecycle events.
	ref class Main sealed : public Windows::ApplicationModel::Core::IFrameworkView
	{
	public:
		Main();

		// IFrameworkView methods.
		virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
		virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
		virtual void Load(Platform::String^ entryPoint);
		virtual void Run();
		virtual void Uninitialize();

	protected:
		// Application lifecycle event handlers.
		void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
		void OnResuming(Platform::Object^ sender, Platform::Object^ args);

		// Window event handlers.
		void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
		void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
		void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);

		// DisplayInformation event handlers.
		void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
		void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
		void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);

    private:
        Takoyaki::EDisplayOrientation DisplayOrientationsToTakoyaki(Windows::Graphics::Display::DisplayOrientations);
        Concurrency::task<std::vector<byte>> loadFileAsync(const std::wstring&);

	private:
        std::unique_ptr<App> app_;
        std::unique_ptr<Takoyaki::Framework> framework_;
		bool mWindowClosed;
		bool mWindowVisible;
	};
}

ref class AppMainView sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
};
