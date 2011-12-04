************************************
Profiler plugin for the SA:MP server
************************************

Copyright (c) 2011 Zeex.
See LICENSE.txt for license details.

Source code can be obtained here:
	https://github.com/Zeex/profiler

Latest binaries can be downloaded from:
	https://github.com/Zeex/profiler/downloads

--------------------
Plugin configuration
--------------------

You can configure profiler via server.cfg, by changing the following options:

* profile_gamemode <0/1>

  If set to 1, profiler will profile currently running game mode. 
  Default value is 0.

* profile_filterscripts <fs1> <fs2> ...

  A list of filter scripts to be profiled.

* profiler_output_format <format>

  Set output format. Currently supported formats: html, text.
  Default is html.

  Note for html: it is possible to sort stats by clicking on column names.

* profiler_sort_output_by <what>
  
  Set output sort mode. Supported values are: calls, time, time_per_call.
  By default output sorted by time.
