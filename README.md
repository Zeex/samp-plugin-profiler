[Profiler plugin][github]
=========================

[![Version][version_badge]][version]
[![Build Status][build_status]][build]
[![Build Status - Windows][build_status_win]][build_win]

Profiler provides detailed information about how long each function takes to
execute and how many times it is called. Additionally it can build a call
graph based on the collected information.

Installing
----------

1. Download a compiled plugin form the [Releases][download] page on Github or
build it yourself from source code (see below).
2. Extract/copy `profiler.so` or `profiler.dll` to `<sever>/plugins/`.
3. Add `profiler` (Windows) or `profiler.so` (Linux) to the `plugins` line of your server.cfg.

Building on Linux
-----------------

Install gcc and g++, make and cmake. On Ubuntu you would do that with:

```
sudo apt-get install gcc g++ make cmake
```

If you're building on a 64-bit system you'll need multilib packages for gcc and g++:

```
sudo apt-get install gcc-multilib g++-multilib
```

If you're building on CentOS, install the following packages:

```
yum install gcc gcc-c++ cmake28 make
```

Now you're ready to build Profiler:

```
cd profiler
mkdir build && cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release
make
```

Building on Windows
-------------------

You'll need to install CMake and Visual Studio (Express edition will suffice).
After that, either run cmake from the command line:

```
cd profiler
mkdir build && cd build
path/to/cmake.exe ../
```

or do the same from cmake-gui. This will generate a Visual Studio project in
the build folder.

To build the project:

```
path/to/cmake.exe --build . --config Release
```

You can also build it from within Visual Studio: open build/profiler.sln and
go to menu -> Build -> Build Solution (or just press F7).

Configuration
-------------

Profiler reads settings from server.cfg, the server configuration file. Below is
the list of available settings:

*   `profiler_gamemodes <gm_name1> <gm_name2> ...`

    Specify which gamemodes should be profiled.

    Names are either gamemodes' file names or paths relative to the `gamemodes/`
    directory in case if the AMX file is not a direct child of `gamemodes/`.

*   `profile_filterscripts <fs_name1> <fs_name2> ...`

    Specify which filterscripts should be profiled.

*   `profiler_outputformat <format>`

    Set statistics output format. This can be one of: `html` (default), `xml`,
    `txt`.

*   `profiler_callgraph <0|1>`

    Enable or disable call graph generation. Default is `0`.

*   `profiler_callgraphformat <format>`

    Set call graph format. Currently only `dot` is supported (can be viewed
    in [GraphViz][graphviz]).

### Old (deprecated) config variables

*	`profile_gamemode <0|1>`

	Toggle gamemode profiling. Default is `0`.

    Same as setting `profiler_gamemodes` to the name of the current gamemode.

*	`profile_filterscripts <fs_name1> <fs_name2> ...`

	Same as `profiler_filterscripts`.

*	`profile_format <format>`

	Same as `profiler_outputformat`.

*	`call_graph <0|1>`

	Same as `profiler_callgraph`.

*	`call_graph_format <format>`

	Same as `profiler_callgraphformat`.

License
-------

Licensed under the 2-clause BSD license. See the LICENSE.txt file.

[github]: https://github.com/Zeex/samp-plugin-profiler
[version]: http://badge.fury.io/gh/Zeex%2Fsamp-plugin-profiler
[version_badge]: https://badge.fury.io/gh/Zeex%2Fsamp-plugin-profiler.svg
[build]: https://travis-ci.org/Zeex/samp-plugin-profiler
[build_status]: https://travis-ci.org/Zeex/samp-plugin-profiler.svg?branch=master
[build_win]: https://ci.appveyor.com/project/Zeex/samp-plugin-profiler/branch/master
[build_status_win]: https://ci.appveyor.com/api/projects/status/kmv39b0awryjvykq/branch/master?svg=true
[download]: https://github.com/Zeex/samp-plugin-profiler/releases
[graphviz]: http://www.graphviz.org
