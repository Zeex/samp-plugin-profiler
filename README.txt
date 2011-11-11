You can configure profiler via server.cfg, by changing the following options:

* profile_gamemode <0/1>

  If set to 1, profiler will profile currently running game mode. 
  Default value is 0.

* profile_filterscripts <fs1> <fs2> ...

  A list of filter scripts to be profiled.

* profiler_substract_children <0/1>

  If set to 1, callee's time is substracted from the total time of its caller(s). 
  Default value is 1.
