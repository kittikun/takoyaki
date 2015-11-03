// Copyright(c) 2015 Kitti Vongsay
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sub license, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "main.h"

#include <iostream>
#include <memory>
#include <takoyaki.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/program_options.hpp>

#include "tests/01_simple_cube.h"

int main(int ac, char** av)
{
    std::cout << "Takoyaki Unit Tests" << std::endl << "Copyright (C) 2015 Kitti Vongsay" << std::endl << std::endl;
    std::cout << "MIT License, <http://opensource.org/licenses/MIT>" << std::endl;
    std::cout << "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR" << std::endl << "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY," << std::endl;
    std::cout << "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE" << std::endl << "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER" << std::endl;
    std::cout << "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM," << std::endl << "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN" << std::endl;
    std::cout << "THE SOFTWARE." << std::endl << std::endl;

    auto ret = ParseOptions(ac, av);

    if (!ret.first)
        return 1;

    // create window
    auto hWnd = MakeWindow(ret.second);

    // takoyaki
    std::unique_ptr<Takoyaki::Framework> framework = std::make_unique<Takoyaki::Framework>();
    Takoyaki::FrameworkDesc desc = {};

    desc.type = Takoyaki::EDeviceType::DX12_WIN_32;
    desc.windowHandle = hWnd;
    desc.windowSize.x = (float)ret.second.width;
    desc.windowSize.y = (float)ret.second.height;
    desc.loadAsyncFunc = std::bind<void>([&framework](const std::wstring& filename) {
        // read file lambda
        // TODO: make async for real
        auto basePath(boost::filesystem::current_path());

        if (IsDebuggerPresent())
            basePath = boost::filesystem::system_complete("..");

        auto path = basePath / filename;
        boost::filesystem::ifstream stream;
        std::vector<uint8_t> res;

        stream.open(path);
        
        if (stream) {
            // get length of file
            stream.seekg(0, stream.end);

            int length = (int)stream.tellg();

            stream.seekg(0, stream.beg);
            res.resize(length);
            stream.read(reinterpret_cast<char*>(&res.front()), length);
            stream.close();

            framework->loadAsyncFileResult(filename, res);
        } else {
            throw new std::runtime_error("Could not open file");
        }
    }, std::placeholders::_1);

    framework->initialize(desc);

    // main loop
    MSG msg;
    auto tests = GetTests();
    auto renderer = framework->getRenderer();

    // test need to load various resources so better start tasks before main loop
    for (auto& test : tests)
        test->initialize(framework.get());

    while (true) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
            break;

        tests[0]->update(renderer.get());
        tests[0]->render(renderer.get());
        framework->present();
    }
    return 0;
}

std::vector<std::unique_ptr<ITest>> GetTests()
{
    std::vector<std::unique_ptr<ITest>> list;

    list.push_back(std::make_unique<Test01>());

    return list;
}

HWND MakeWindow(const Options& options)
{
    WNDCLASSEX wcex = {};
    HINSTANCE hInst = GetModuleHandle(0);

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInst;
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "CUnitTests";
    wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    RegisterClassEx(&wcex);

    auto hWnd = CreateWindowEx( NULL, "CUnitTests", "Takoyaki Unit Tests", WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, CW_USEDEFAULT, options.width, options.height,
                                NULL, NULL, hInst, NULL);

    if (hWnd == nullptr)
        throw new std::runtime_error("Could not create window");

    ShowWindow(hWnd, SW_SHOWDEFAULT);

    return hWnd;
}

std::pair<bool, Options> ParseOptions(int ac, char** av)
{
    // define options
    boost::program_options::options_description generic("Available options");
    boost::program_options::options_description window("Window options");
    boost::program_options::options_description takoyaki("Takoyaki options");

    generic.add_options()
        ("help", "produce help message");

    window.add_options()
        ("width,w", boost::program_options::value<uint_fast32_t>()->default_value(1280), "Width of the window")
        ("height,h", boost::program_options::value<uint_fast32_t>()->default_value(768), "Height of the window");

    takoyaki.add_options()
        ("numThreads,t", boost::program_options::value<uint_fast32_t>()->default_value(4), "Number of workers");

    // parse options
    boost::program_options::variables_map vm;
    boost::program_options::options_description cmdOptions;

    cmdOptions.add(generic).add(window).add(takoyaki);

    boost::program_options::store(boost::program_options::parse_command_line(ac, av, cmdOptions), vm);
    boost::program_options::notify(vm);

    // process options
    Options res = {};

    if (vm.count("help")) {
        std::cout << cmdOptions << std::endl;
        return std::make_pair(false, res);
    }

    res.width = vm["width"].as<uint_fast32_t>();
    res.height = vm["height"].as<uint_fast32_t>();
    res.numThreads = vm["numThreads"].as<uint_fast32_t>();

    return std::make_pair(true, res);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // sort through and find what code to run for the message given
    switch (message) {
        // this message is read when the window is closed
        case WM_DESTROY:
        {
            // close the application entirely
            PostQuitMessage(0);
            return 0;
        } break;
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc(hWnd, message, wParam, lParam);
}