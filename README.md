# Infinity Mirror - Interactive Live Desktop Wallpaper

## Description
Infinity Mirror is a live and interactive desktop wallpaper application that creates an infinite mirror effect on your screen. The wallpaper is fully customizable and responds to user input, allowing for a unique, personalized experience. 

## Features
- 🌌 Infinite mirror effect with smooth, real-time rendering
- 🖱️ Interactive wallpaper that responds to user actions
- ⚙️ Fully customizable settings via `settings.json`
- 🚀 High-performance OpenGL rendering
- 💻 Developed in C, C++, and Assembly (primarily C++)

## Requirements
- OS: Windows 10/11
- GPU: OpenGL-compatible
- Compiler: Visual Studio 2022 (recommended)

## Installation
1. **Download**: Download the latest release of Infinity Mirror from the [Releases](https://github.com/erfan-ops/infinity-mirror/releases) page.
2. **Launch**: Once installed, launch `infinity_mirror.exe`.

## Building
1. Clone the repository:
   ```bash
   git clone https://github.com/erfan-ops/infinity-mirror.git
   ```
2. Open the project in Visual Studio 2022.
3. Build the solution.
4. Run the executable from the output directory.

## Configuration
Customize your wallpaper by editing `settings.json`:
```json
{
  "fps": 60,
  "colors": [
    [ 1, 0, 0, 1 ],
    [ 1, 1, 0, 1 ],
    [ 0, 1, 0, 1 ],
    [ 0, 1, 1, 1 ],
    [ 0, 0, 1, 1 ],
    [ 1, 0, 1, 1 ]
  ],
  "color-speed": 0.7,
  "iteration-limit": 12,
  "distance-interval-factor": 0.85,
  "border-width": 4.0,
  "background-color": [ 0, 0, 0, 1 ]
}

```
- `fps`: Frames per second limit.
- `colors`: Array of colors in RGBA format that will be used in the mirror effect.
- `color-speed`: Speed of color changes.
- `iteration-limit`: Number of iterations to render in the mirror effect.
- `distance-interval-factor`: Factor determining how the mirror effect's distance changes.
- `border-width`: Width of the borders in the effect.
- `background-color`: Background color in RGBA format.

## Contribution
Pull requests are welcome! For major changes or feature requests, please open an issue to discuss your ideas.

## License
This project is licensed under the [MIT License](LICENSE.txt).

## Acknowledgments
- OpenGL for rendering
- Windows API for managing the desktop wallpaper and tray icon
- Visual Studio for development

💫 Enjoy your infinite mirror experience with Infinity Mirror! ✨
