# Battleships - Windows API Laboratory

This is a sketch for a Battleships game, developed as a laboratory task for Windows API. The application consists of three windows: two boards (for the player and the PC) and a statistics window.

## Main Window

The main window, titled 'BATTLESHIPS - STATISTICS', displays the elapsed time from the start of the game and the current game statistics. It has a menu that allows the user to change the grid size, offering three difficulty levels: easy (10x10 grid), medium (15x15 grid), and hard (20x20 grid).

## Board Windows

The board windows, titled 'BATTLESHIPS - MY' and 'BATTLESHIPS - PC' respectively, display the game boards. The size of the board windows is computed automatically based on the selected grid size. Each grid cell is 30px in size, with a margin of 5px from the edge of the window and a 3px margin between grid cells.

## Game Play

The game is playable, with the player and the PC taking turns to hit each other's ships. When a cell is hit, it is marked accordingly: a hit ship cell is marked with an `X` and a red background, a missed shot is marked with a `.` and a blue background. When a ship is completely destroyed, all surrounding ship fields that are not already marked as a miss are marked as neutral (yellow background). When the game is over, a green overlay with a congratulatory message is displayed.

## Additional Features

The application stores the chosen difficulty level in an ini file and reads it back when opened another time. The PC randomly selects cells to hit, following the same rules as the player for marking cells.

## Building and Running the Project

The project is built using Visual Studio and the Windows API. To build and run the project, open the solution file `lab1_1.sln` in Visual Studio, select the desired configuration and platform, and then build and run the project.

## Implementation Details

The application uses the Windows API for creating and managing windows, GDI for drawing, and the standard C++ library for other functionality. The main window and board windows are created using the `CreateWindowEx` function, and messages are processed in the `WndProc` function. The game logic is implemented in the `board` class, which is defined in `board.h` and implemented in `board.cpp`. The application also uses the `std::chrono` library for timing.

## Update

The project now replaces classic symbols like `X` and `.` with appropriate Unicode emojis. The game is now more visually appealing and fun to play. Other minor improvements have been made to the code and the user interface.
Functionality remains the same.