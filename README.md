## cinema4d-scripts
Cinema 4D - Useful scripts and plugins with scene setup (Python, Xpresso, Coffee Script)

This repository is divided on three main categories:
* [python](/python) - Python based scripts which are able to run from version R13 - up
* [xpresso](/xpresso) - Xpresso "scripts"
* [cpp](/cpp) - C++ based plugins, scripts which are CINEMA 4D R20 dependent, but some of the solutions will contain "multi-version" directories.

# Python-based scripts

* [Mopy_c4d](/python/mopy_c4d) & [Mopy_rpi](/python/mopy_rpi) - Credits to [pushbuttons.io](https://pushbuttons.io/) you can see the plugin in the action [here](https://vimeo.com/145118655) 
* [Geometry script](/python/geometry_script.py) & [Splines script](/python/splines_script.py) - Credits to [pushbuttons.io](https://pushbuttons.io/) 
* [Ray-Bounce-refract](/python/ray_bounce_refract.py) - Is a script for demonstrating purposes for rendering a method called raytracing
* [Menger Sponge](/python/mengersponge.py) - Is a simple Menger sponge fractal generator.

# Xpresso

* Shader-Vertex mapping - brick_sv_maps.c4d

# C++ (R20 solutions)

Before compiling Cinema 4D R20 plugins please follow a instructions on [developers.maxon.net](https://developers.maxon.net/?page_id=1108)
 , [Development for Microsoft Windows](https://developers.maxon.net/docs/Cinema4DCPPSDK/html/page_maxonapi_dev_windows.html) and [Project Tool](https://developers.maxon.net/docs/Cinema4DCPPSDK/html/page_maxonapi_projecttool.html)

* [c4dIrrlichtDemo](/cpp/R20/c4dIrrlichtDemo) - Is a plugin "demo/port" of utilizing [Irrlicht 3D engine](http://irrlicht.sourceforge.net/) for viewing, modyfiing 3D BSP map in Cinema 4D environment. - for a proper run you need to copy [irrlicht.dll](/cpp/R20/c4dIrrlichtDemo/externs/irrlicht/1.8.4/bin/Win64-VisualStudio) into Cinema 4D R20 root directory
