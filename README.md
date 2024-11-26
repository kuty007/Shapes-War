# Shapes-War
Shapes War is a C++ project that blends SFML and ImGui to create a fast-paced action game. Players take control of a character who shoots at dynamically moving shapes to score points and compete for the highest score.
[![Demo Video](./Demo%20video/gif.gif)](./Demo%20video/game.mp4)





## Features

- **Player Interaction**: Control a shooter to target and destroy shapes.
- **Scoring System**: Earn points by successfully hitting and destroying shapes.
- **ImGui Interface**: Intuitive UI that allows players to customize gameplay settings.
- **Configuration Files**: Load and set shape properties from external files.
## Requirements
- **C++ Compiler**: Supporting C++17 or later.
- **SFML**: Version 2.5 or later.
- **ImGui**: Integrated with SFML.

### On Windows
Install SFML and ensure proper library linking in your IDE or build environment.

### On Linux
Ensure the following dependencies are installed:
- `g++` or another C++ compiler
- SFML development libraries (`libsfml-dev` or equivalent)
- A compatible Makefile build system

## Running on Linux
1. Clone the repository:
   ```bash
   git clone https://github.com/kuty007/Shapes-War.git
   cd Shapes-War
   ``
### Makefile
The project includes a `Makefile` for building on Linux. To compile and run:
1. Ensure SFML is installed:
   ```bash
   sudo apt update
   sudo apt install libsfml-dev
   ```
2. Compile using `make`:
   ```bash
   make
   ```
3. Execute the binary:
   ```bash
   cd bin
   ./GAME
   ```


---

## Adding Music to the Game

The game includes a music file that can be loaded during gameplay. You can download free music assets from the following link:

[OpenGameArt's Advanced Search for Game Assets](https://opengameart.org/art-search-advanced?keys=sample&title=&field_art_tags_tid_op=or&field_art_tags_tid=&name=&field_art_type_tid%5B%5D=9&field_art_type_tid%5B%5D=10&field_art_type_tid%5B%5D=7273&field_art_type_tid%5B%5D=14&field_art_type_tid%5B%5D=12&field_art_type_tid%5B%5D=13&field_art_type_tid%5B%5D=11&sort_by=score&sort_order=DESC&items_per_page=24&Collection=)

After downloading the music file, follow these steps:

1. Create a folder named `music` inside the `bin` directory of your project.
2. Place the downloaded music file inside the `bin/music` folder.
3. The game will automatically load the music without any further changes needed to the code.

Alternatively, if you don't want to use music, you can remove the music loading section from the `Game.cpp` file to disable it.

### Cleaning Up
To clean the build directory:
```bash
make clean
```
## Contributing

Contributions are welcome! Fork the repository, create a feature branch, and submit a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.

---

Enjoy exploring and customizing game settings in **Shapes War**! Feedback and ideas are always appreciated. 😊
