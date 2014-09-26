[Profiler plugin][github]
=========================

[![Donate][donate_button]][donate]
[![Build Status][build_status]][build]

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

	NOTE for `html`: it is possible to sort stats by clicking on column names!!

*	`call_graph <0|1>`

	Toggle call graph generation. Default is `0`.

*	`call_graph_format <format>`

	Set call graph format. Currently only `dot` is supported (can be viewed
	in [GraphViz][graphviz]).

License
-------

Licensed under the 2-clause BSD license. See the LICENSE.txt file.

[github]: https://github.com/Zeex/samp-plugin-profiler
[donate]: http://pledgie.com/campaigns/19751
[donate_button]: http://www.pledgie.com/campaigns/19751.png
[build]: https://travis-ci.org/Zeex/samp-plugin-profiler
[build_status]: https://travis-ci.org/Zeex/samp-plugin-profiler.png?branch=master
[download]: https://github.com/Zeex/samp-plugin-profiler/releases 
[graphviz]: http://www.graphviz.org
