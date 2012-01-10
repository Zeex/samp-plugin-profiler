Profiler plugin for SA-MP server
================================

Profiler can measure how long each function executes and how often it is called.

Configuration
-------------

You can configure profiler via `server.cfg`, by setting the following options:

* profile_gamemode <0|1>

  If set to 1, profiler will profile currently running game mode. Default value is 0.

* profile_filterscripts <fs1> <fs2> ...

  A list of filter scripts to be profiled. Can be empty.

* profile_format <xml|html|text>

  Set statistics output format. Default is html. 
  Note for HTML: it is possible to sort stats by clicking on column names!!

Output explanation
------------------

The output consists of a table with these data:

*	Function Type

	The type of a function, can be one of: public, native, normal.
	*Publics functions are the ones denoted by the `public` keyword. 
	*Native* functions are native code procedures exported by the server or a plugin.
	*Nommal* functions are the rest.

*	Function Name

	The function name. Since normal functions' names are not usually exported the profied script
	must be compiled with debug info, otherwise you will see something like `unknown_function@123456`
	in place of the actual name.

*	Calls

	The number of times the function has been called. 

*	Self Time

	The total amount of time spent inside the function.

*	Total Time

	This Self Time + Total Time of function's callees.

License
-------

Licensed under the Apache License version 2.0, see LICENSE.txt.

