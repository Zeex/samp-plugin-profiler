Profiler plugin for SA-MP server
================================

https://github.com/Zeex/samp-profiler-plugin

Profiler provides detailed information about how long each function takes to execute and how many times
it is called. This can be helpful in finding bottlenecks or "hot spots" in code for further optimization.

Profiler Settings
-----------------

Profiler recognizes the following `server.cfg` options:

*	`profile_gamemode <0|1>`

	If set to 1, profiler will profile currently running game mode. Default value is 0.

*	`profile_filterscripts <name1> <name2> ...`

	A list of filter scripts to be profiled. Can be empty.

*	`profile_format <xml|html|txt>`

	Set statistics output format. Default is html. 
	Note for HTML: it is possible to sort stats by clicking on column names!!

*	`call_graph <0|1>`

	Set this option to 1 if you want profiler to generate a call graph.
	The graph is generated as a [GraphViz](http://www.graphviz.org GraphViz) dot file.

*	`call_graph_format <format>`

	If specified, profiler will try to convert the .gv file into one of the formats supported by `dot`.
	with the following command:

	`<install_path/>dot -T<format> <some_script>-calls.gv -O`

	NOTE: On Windows, *install_path* is looked up in registry first, then read from `GV_HOME` variable
	(as on Linux). If GraphViz is not found it is assumed to be in `PATH`.

