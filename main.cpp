#include "Ini.hpp"

int main() {
    Ini::Ini ini;
    ini.load("example.ini");

    int width = ini.get<int>("settings", "width", 1920);
    float volume = ini.get<float>("audio", "volume", 0.8f);
    bool fullscreen = ini.get<bool>("settings", "gullscreen", true);
    std::string title = ini.get<std::string>("App", "Title", "My App");

    ini.set<int>("settings", "width", 2560);
    width = ini.get<int>("settings", "width", 1920);
    
    std::cout << "Width: " << width << "\nVolume: " << volume << "\nFullscreen: " << fullscreen << "\nTitle: " << title << '\n';

    ini.save("example.ini");
    
    return 0;
}