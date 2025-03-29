# Bézier surfaces and curves
A simple tool for rendering Bézier surfaces and curves.
## Set-up
Make sure to have the correct files for your machine's architecture chosen if you're using Visual Studio.\
If not, go to `Project` > `Properties` > `Configuration Properites` > `Linker`
- `General` > `Additional Library Directories`\
![linker general](https://github.com/user-attachments/assets/304774a3-caa3-431c-9f52-fb540186200a)\
and change the `x64` to your architecture
- `Input` > `Additional Dependencies`\
![linker input](https://github.com/user-attachments/assets/dbacce95-d4e4-4731-8a48-1c4be1221392)\
and change the `x64` to your architecture
- All the SDL3 and SDL3_ttf (for helper box font) should go to the `ThirdParty` directory
- All the object control point files should go to the `Objects` directory
## Example object file for Bézier surfaces 
`teapotCGA.txt`\
**Note:** the text after the `<-` symbol explains the file structure.
```
32 <- This is the total numer of Bézier surfaces in the file
3 3 <- Size of the control point matrix
1.4 0.0 2.4 <- X, Y, Z coordinates for the first control point of the surface.
1.4 -0.784 2.4
...
0.0 -1.5 2.4 <- X, Y, Z coordinates for the last control point of the surface
3 3 <- next surface
0.0 -1.4 2.4
...
```
This patter repeats for all the control points defying a surface.\
**Note:** The program currently only works on 3x3 point matrices.
## Example object file for Bézier curves
Currently the Bézier curves are static and can be only defined inside of the program.
## Functionalities
### Bézier surfaces
The user is able to:
- move the object in 2D
- offset the object in the Z axis
- change the color of the object
- change the FOV
- rotate around X, Y and Z axes
### Bézier curves
The user is able to:
- move the object in 2D
## Rendering
- The surfaces are rendered using the perspective projection method.
- The curves are plotted onto the X, Y plane with appropriate scaling factor so that they are easily visible.\
**For the best results while also maintaining decent performance `px_density` value is recommended to be set to `0.01f` or below** 
## Example Bézier surfaces rendering
![initial surfaces view](https://github.com/user-attachments/assets/e579b9ce-e226-41e7-a3cb-fcbe94bfbc09)
## Example Bézier surfaces rendering while rotating around all 3 axes
![rotating surfaces](https://github.com/user-attachments/assets/faadf94f-c4a2-4398-9090-bd1a075e1d4d)
## Example Bézier curves rendering
![example curves](https://github.com/user-attachments/assets/a15eb368-7cb8-449d-b4e2-fdb8580766e1)
# Technologies
- Visual Studio 2022
- [SDL3-devel-3.2.8-VC](https://github.com/libsdl-org/SDL/releases/tag/release-3.2.8)
- [SDL3_ttf-devel-3.2.0-VC](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-3.2.0)
# Sources
[Object files](http://www.holmes3d.net/graphics/teapot/)
# License
[MIT](LICENSE.txt)

