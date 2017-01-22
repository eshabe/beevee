BeeVee2 README.TXT

This file contains:
1. Project Development
2. Versioning

1. Project Development
1.1 Developer Tools
	IDE: Code::Block 8.02
	Compiler: MingW32 Compiler 4.2.1
	
	Pls search the web how to set your IDE configuration to use it...!

1.2 Directories and Files
	1. Directory's Structure
	
	+ root
	|/BV2
	|/bv2data
	|/sdata_c
	|/steks_c
	|/slog_c
	
	2. Files inside directory
	
	+ BV2
	|- Documentations: ChangesLog*.txt, README.TXT, BeeVee.htm
	|- License: COPYING.TXT
	|- Code::Block 8.02's workspace: BeeVee.workspace
	
	+ bv2data
	|- Core: sboard.c
	|- File / board data manipulators
	
	+ sdata_c
	|- Core: sdata.c
	|- Data manipulators
	
	+ steks_c
	|- Text manipulators
	
	+ slog_c
	|- Log file manipulators

1.3 SVN
	1. Download BeeVee SVN for developers
	$ cd beevee-svn-dir
	$ svn checkout https://beevee.googlecode.com/svn/trunk/ beevee --username eshabe

	2. upload BeeVee SVN for developers
	$ cd beevee-svn-dir
	$ svn commit beevee --username eshabe
	
		
2. Versioning
	BeeVee2's versioning is similar with other Windows program's.
	Since version 2.1.1, PrivateBuild & SpecialBuild set to null, and
	version info do not show revision number (4th digit) except in
	StringFileInfo & ProductVersion.
	
	About box display build info in format rev.yyyymmdd.build_count
