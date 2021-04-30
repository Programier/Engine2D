# Flappy Bird (Linux)

## Compile from source
### Using terminal
1. Clone or download the repository in a directory
2. Open terminal
3. Installing the required packages (Debian based operating system)
```
sudo apt update
sudo apt install libglfw3-dev libpng-dev libopengl-dev libglew-dev libalut-dev libopenal-dev libaudio-dev libglm-dev
```
4. Compile source
```
cd source_directory
chmod +x compile.sh
chmod +x Engine2D/compileEngine.sh
./compile.sh
```
## Run game
### Using terminal
1. Open terminal and run command
```
./source_directory/Release/FlappyBird
```
### Using File Manager
1. Open source_directory/Release folder in file manager and run binary file FlappyBird
