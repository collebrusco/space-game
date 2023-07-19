# space-game
Untitled experimental overhead space game written with OpenGL & C++
This game uses my [graphics submodule](https://github.com/collebrusco/flgl) 
My internal working name for it is 'risklike', if you see that in the source or docs that's why
## Build
Currently the makefile is macos only, though it will build on linux with a few tweaks.
```bash
git clone https://github.com/collebrusco/space-game
git submodule update --remote --init --recursive
make all
./bin/risklike
```
