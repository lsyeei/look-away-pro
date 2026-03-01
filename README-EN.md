# Look Away Pro

[English Version](./README-EN.md) | [中文版](./README-CN.md)

A desktop eye-care reminder application developed with Qt C++, designed to help users prevent eye strain from prolonged computer use.

## 📖 Overview

Look Away Pro is an intelligent eye-care reminder tool that helps alleviate eye fatigue and prevent myopia and dry eye syndrome by reminding users to take regular breaks. The program runs minimized in the system tray without interfering with daily work.

## ✨ Key Features

### 🎯 Smart Timing Reminders
- **20-20-20 Rule**: Default setting reminds users every 20 minutes to look at something 20 feet away for 20 seconds
- **Customizable Durations**: Users can freely set work and break times
- **Forced Break Mode**: Optional forced mode prevents closing the reminder during break time

### ⏱️ Tools Feature
- **Timer**: Precise timing display (HH:MM:SS format)
  - Play/Pause/Stop functionality
  - Draggable transparent window
  - Frameless design, always on top
  - Customizable font, size, and color

- **Countdown**: Customizable duration countdown reminder
  - Flexible setting of hours, minutes, and seconds
  - Flashing alert and sound notification when complete
  - Draggable transparent window
  - Frameless design, always on top
  - Customizable font, size, color, and alert sound

### 🖼️ Personalized Reminder Interface
- **Full-screen Alerts**: Pops up full-screen to ensure users notice the break reminder
- **Customizable Messages**: Supports custom text with `{time}` and `{break}` placeholders
- **Multiple Background Options**:
  - Default black background
  - Custom solid color background
  - Custom image background (supports BMP, JPG, PNG, GIF)
  - Image drawing modes: Fill, Fit, Stretch, Tile
- **Font Customization**: Adjustable font family and size for reminder text

### 📊 Data Statistics & Analysis
- **Multi-dimensional Statistics**: Provides daily, weekly, and monthly time statistics
- **Custom Date Range**: Supports querying custom time periods (defaults to one year)
- **Local Data Storage**: Uses SQLite database to ensure data persistence
- **Interface Size**: Statistics window defaults to 1/4 of screen size

### ⚙️ Flexible Configuration Options
Configuration page divided into four main tabs:

**Basic Settings**:
- Work time setting (minutes)
- Break time setting (seconds)
- Startup with system option
- Force break option
- Screensaver/Lock screen behavior: Pause or Restart timer

**Reminder Interface Settings**:
- Custom message editing
- Background type selection: Image or Solid Color
- Image preview thumbnail
- Image drawing mode selection
- Sound notification settings (supports WAV, MP3, OGG)
- Font and size settings
- Real-time preview (ESC to exit preview)

**Tools Settings**:
- Timer appearance settings
  - Font, size, color configuration
- Countdown appearance settings
  - Font, size, color configuration
  - Alert sound file selection

**About**:
- Program version information
- Author information

### 💡 Convenient Operations
- **System Tray Integration**: Automatically minimizes to system tray on startup
- **Context Menu**:
  - 🛠️ Tools
    - ⏱️ Timer: Open timer window
    - ⏳ Countdown: Open countdown input dialog
  - 📈 Statistics: View work and break statistics
  - ⚙️ Settings: Open configuration page
  - ⏯️ Pause/Resume: Pause or resume timing
  - ℹ️ About: Show program information
  - 🚪 Exit: Safely quit the program
- **Smart Pause**: Automatically pauses timer when screensaver runs or system is locked
- **Status Synchronization**: Menu icons and text dynamically update based on state

## 🛠️ Technical Implementation

### Development Environment
- **Language**: C++17
- **Framework**: Qt 6.8.3
- **Build Tool**: qmake
- **Database**: SQLite3
- **Platforms**: Windows, Linux, macOS

### Core Components
- **ConfigManager**: Singleton configuration manager for XML file read/write
- **TimerManager**: Precise control of work and break timing
- **ScreenStateMonitor**: Cross-platform screen state monitoring (Windows WTSAPI/Linux DBus/macOS Cocoa)
- **DatabaseManager**: SQLite operations for recording statistics
- **ReminderDialog**: Full-screen reminder dialog with multiple background modes
- **SystemTrayIcon**: System tray integration with complete context menu
- **TimerWindow**: Timer window with play/pause/stop functionality
- **CountdownWindow**: Countdown window with flashing alert and sound notification
- **CountdownInputDialog**: Countdown time input dialog

### Configuration Storage
- Config file: `config.xml` (stored in application directory)
- Database: `LookAwayPro.db` (SQLite format)
- Supports startup with system (Windows registry)

## 📦 Build & Installation

### Prerequisites
- Qt 6.x development environment
- C++17 compatible compiler
- Windows: Visual Studio 2019+ or MinGW
- Linux: GCC 7+ or Clang
- macOS: Xcode 10+

### Build Steps
```bash
# Clone repository
git clone https://github.com/lsyeei/look-away.git
cd look-away

# Generate Makefile
qmake LookAwayPro.pro

# Compile
make  # Linux/macOS
# or
nmake  # Windows MSVC
# or
mingw32-make  # Windows MinGW

# Run
./release/LookAwayPro
```

### Installation Notes
Windows users can directly run the compiled `.exe` file. The program will automatically create necessary configuration files and database.

## 🎯 Health Tips

Prolonged computer use can cause:
- Eye strain and dryness
- Vision deterioration
- Cervical spine issues
- Loss of concentration

Use Look Away Pro and follow the 20-20-20 rule to give your eyes proper rest!

## 🔮 Future Plans

- [ ] Multi-language support
- [ ] Cloud data synchronization
- [ ] Richer statistical charts
- [ ] Smart break suggestions
- [ ] Pomodoro technique integration
- [ ] Mobile companion app

## 🤝 Contributing

Issues and Pull Requests are welcome!

## 📢 Acknowledgments

Thanks to the Qt framework for providing cross-platform solutions, and to all open-source community contributors.

---

**Reminder**: Protect your vision, start now!
