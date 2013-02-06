[Profiler plugin][github]
=========================

Profiler provides detailed information about how long each function takes to execute and how many times
it is called. This can be helpful in finding bottlenecks or "hot spots" in code for further optimization.

This plugin can recognize the following `server.cfg` options:

*	`profile_gamemode <0|1>`

	Toggle gamemode profiling. Default is `0`.

*	`profile_filterscripts <name1> <name2> ...`

	A list of filter scripts to be profiled.

*	`profile_format <format>`

	Set statistics output format. This can be one of: `html` (default), `xml`, `txt`.

	NOTE for `html`: it is possible to sort stats by clicking on column names!!

*	`call_graph <0|1>`

	Toggle call graph generation. Default is `0`.

*	`call_graph_format <format>`

	Set call graph format. Currently only `dot` is supported (can be viewed in [GraphViz][graphviz]).

[github]: https://github.com/Zeex/samp-plugin-profiler
[graphviz]: http://www.graphviz.org