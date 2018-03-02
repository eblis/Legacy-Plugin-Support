Legacy Plugin Support plugin v.0.0.0.1
Copyright © 2008 Cristian Libotean

This plugin acts as a bridge between old style plugins for Miranda 0.7 or earlier and new Miranda cores, 0.8 or later.
Thanks to Joe for the initial Bridge plugin and idea.

Please backup your database before using the plugin.

The plugin renames legacy plugins inside miranda's folder, adding .legacy to the file extension once a legacy
plugin has been bridged and removes the .legacy extension if the bridge is removed.
Be careful which plugins you enable and disable from the GUI as the management plugin will try to rename the files when you press Apply.

The bridge plugin will not load legacy plugins that do not have their UUID and interfaces specified in the configuration (LPS.ini) file.
Check the configuration file to find the format you need to add UUIDs in. UUIDs are valid only if they're added in the correct format!

If you add UUID information and interfaces for other plugins please submit them to me so I can update the configuration file.

Changes
=======
+ : new feature
* : changed
! : bufgix
- : feature removed or disabled because of pending bugs

v. 0.0.0.1 - 2008/02/04
	+ Initial release.