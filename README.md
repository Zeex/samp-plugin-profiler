[Profiler plugin][github]
=========================

[![Version][version_badge]][version]
[![Build Status][build_status]][build]
[![Build Status - Windows][build_status_win]][build_win]

Profiler provides detailed information about how long each function takes to
execute and how many times it is called. Additionally it can build a call
graph based on the collected information.

Download
--------

Get latest binaries for Windows and Linux [here][download].

Settings
--------

Profiler settings can be changed via the following `server.cfg` options:

*	`profile_gamemode <0|1>`

	Toggle gamemode profiling. Default is `0`.

*	`profile_filterscripts <name1> <name2> ...`

	A list of filter scripts to be profiled.

*	`profile_format <format>`

	Set statistics output format. This can be one of: `html` (default), `xml`,
	`txt`.

*	`call_graph <0|1>`

	Toggle call graph generation. Default is `0`.

*	`call_graph_format <format>`

	Set call graph format. Currently only `dot` is supported (can be viewed
	in [GraphViz][graphviz]).

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
