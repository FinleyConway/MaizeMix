# MaizeMix
`MaizeMix` is an ECS oriented audio engine using SFML.


## Features
### AudioEngine
- Handles the audio state and attributes 
	- Audio clip management
	- Play, Pause, UnPause and Stop Audio
	- Alter the functionality of the playing audio
	- Spatialization 
	- Callbacks for finished audio
	- Audio listener position
	- Audio listener volume (global volume change)


### AudioClip
- Information about the imported audio clip
	- Number of channels
	- Duration of clip
	- Sample rate of the clip
	- stream audio clip / load clip into memory


### Sandbox
- Graphical user interface using imgui
- Demo on how you might integrate this with ecs (example using [flecs](https://github.com/SanderMertens/flecs))


## Building


## Giving Feedback
Please file an issue.


## License
MaizeMix is under the [MIT license](https://github.com/FinleyConway/MaizeMix/blob/master/license.md).


## External libraries used by MaizeMix
- [SFML](https://github.com/SFML/SFML) is under the [zLib license](https://github.com/SFML/SFML/blob/master/license.md) (MaizeMix)
- [Catch2](https://github.com/catchorg/Catch2/tree/devel) is under the [BSL-1.0 license](https://github.com/catchorg/Catch2/blob/devel/LICENSE.txt) (test)
- [flecs](https://github.com/SanderMertens/flecs) is under the [MIT license](https://github.com/SanderMertens/flecs/blob/master/LICENSE) (sandbox)
- [imgui](https://github.com/ocornut/imgui) is under the [MIT license](https://github.com/ocornut/imgui/blob/master/LICENSE.txt) (sandbox)
- [imgui-sfml](https://github.com/SFML/imgui-sfml) is under the [MIT license](https://github.com/SFML/imgui-sfml/blob/master/LICENSE) (sandbox)
