Kopete Scripting Plugin

The Kopete scripting plugin does use the Kross Scripting Framework (http://kross.dipe.org)
included in kdelibs to allow to write Kopete plugins using the Python, Ruby and JavaScript
scripting languages.

Notes;
* so far this is experimental since the scripting API is work under progress and may
  change depending on the requirments.
* while JavaScript got shipped with kdelibs as well and therefore should be available per
  default, the other scripting backends are optional and need to be installed if you like
  to use them.
    * http://websvn.kde.org/trunk/KDE/kdelibs/kross/kjs is the JavaScript-backend
    * http://websvn.kde.org/trunk/KDE/kdebindings/python/krosspython/ is the Python-backend
    * http://websvn.kde.org/trunk/KDE/kdebindings/ruby/krossruby/ is the Ruby-backend.
    * http://websvn.kde.org/trunk/playground/bindings/krossjava/ is the Java-backend.
    * http://websvn.kde.org/trunk/playground/bindings/krossfalcon/ is the Falcon-backend.
* please provide feedback if you wrote a useful script or discover problems. Thank you!
