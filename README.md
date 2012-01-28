Profiler plugin for SA-MP server
================================

Profiler can measure how long each function executes and how often it is called.

Profiler Settings
-----------------

New `server.cfg` options:

*	profile_gamemode &lt;0|1&gt;

	If set to 1, profiler will profile currently running game mode. Default value is 0.

*	profile_filterscripts &lt;fs1&gt; &lt;fs2&gt; ...

	A list of filter scripts to be profiled. Can be empty.

*	profile_format &lt;xml|html|txt&gt;

	Set statistics output format. Default is html. 
	Note for HTML: it is possible to sort stats by clicking on column names!!

*	call_graph &lt;0|1&gt;

	Set this option to 1 if you want profiler to generate a call graph.
	The graph is generated as a [GraphViz](http://www.graphviz.org GraphViz) dot file.

*	call_graph_format &lt;format&gt;

	If specified, profiler will try to convert the .gv file into one of the formats supported by dot. 

	The `dot` program is invoked as follows:

	`<install_path>/dot -T<format> <some_script-calls.gv> -O`

	On Windows, *install_path* is looked up in registry first, then in GV_HOME variable (as on *nix).
	If GraphViz install path is not found it is assumed that it's already in PATH.

License
-------

Licensed under the Apache License version 2.0, see LICENSE.txt.

