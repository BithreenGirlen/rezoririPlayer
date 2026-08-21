# rezoririPlayer

某寝室用

## How to play

First, prepare the files, commented below, with proper directory.

<pre>
assets
  ├ bundles
  │  ├ ...
  │  ├ hcg // Scene spine folder
  │  │  ├ ...
  │  │  ├ 204101
  │  │  │  ├ 2041_CG.atlas
  │  │  │  ├ 2041_CG.skel
  │  │  │  ├ 2041_CG.png
  │  │  │  ├ 2041_CG_2.png
  │  │  │  ├ 2041_CG_3.png
  │  │  │  ├ 2041_CG_4.png
  │  │  │  ├ 2041_CG_5.png
  │  │  │  └ ...
  │  │  ├ ...
  │  └ ...
  ├ ...
  ├ dialogvoice
  │  ├ ...
  │  ├ 204161 // Scene voice folder
  │  │  ├ 204161005.m4a
  │  │  ├ 204161010.m4a
  │  │  └ ...
  │  └ ...
  ├ dialogxml // Scene dialogue folder
  │  └ jp
  │     ├ ...
  │     ├ 204161
  │     │  ├ dialog_show.xml
  │     │  └ dialog_text.xml
  │     └ ...
  └ ...
</pre>

Then, select `bundles/hcg/XXXXXX` folder from the application file menu `File->Open folder`.  
The scene will be set up based on `dialog_show.xml` and `dialog_text.xml`.

## Preferences

The following preferences can be configured through `setting.txt` in the same directory of the executable file.
- Font file with which the scene texts will be drawn.
  - Mind that Raylib cannot load `.ttc` file.
- Text to be shown; those in `dialog_show.xml` or in `dialog_text.xml`. 

## External libraries

- [lz4 v1.10.0](https://github.com/lz4/lz4)
- [raylib v6.0](https://github.com/raysan5/raylib)
- [spine-cpp-4.2](https://github.com/EsotericSoftware/spine-runtimes/tree/4.2)
- [Dear ImGui v1.92.7](https://github.com/ocornut/imgui)
- [rlImGui Raylib 6.0](https://github.com/raylib-extras/rlImGui)

## Build

Visual Studio is required.

1. Open `rezoririPlayer` directory with Visual Studio.
2. Wait for downloading external libraries to be done.
3. Build all. 
